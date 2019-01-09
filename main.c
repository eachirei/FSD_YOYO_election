#include "utils.h"
#include "source.h"
#include "interm.h"
#include "sink.h"


/*
+---+    +---+    +---+
| 0 |    | 2 |    | 1 |
+-+-+    +-+-+    +-+-+
  |        |        |
+-v-+    +-v-+    +-v-+
| 3 |  +-+ 4 |  +-+ 5 |
+-+-+  | +-+-+  | +-+-+
  |    |   |    |   |
+-v-+  | +-v-+  | +-v-+
| 6 <--+ | 8 <--+ |10 |
+-+-+    +-+-+    +---+
  |        |
+-v-+    +-v-+
| 7 |    | 9 |
+---+    +---+
 */

int main(int argc, char *argv[]) {
    int currentRank; /* rank of process */
    int numberOfProcesses;       /* number of processes */
    MPI_Status status;   /* return status for receive */

    /* start up MPI */

    MPI_Init(&argc, &argv);

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    if (numberOfProcesses != 11) {
        printf("Incorrect number of processes. Expected 11, got %d.", numberOfProcesses);
        MPI_Finalize();
        return 1;
    }

    MPI_Comm newComm;

    int index[] = {1, 2, 3, 5, 8, 11, 14, 15, 18, 19, 20};
    int edges[] = {
            3, // 0
            5, // 1
            4, // 2
            0, 6, // 3
            2, 6, 8, // 4
            1, 8, 10, // 5
            3, 4, 7, // 6
            6, // 7
            4, 5, 9, // 8
            8, // 9
            5 // 10
    };

    MPI_Graph_create(MPI_COMM_WORLD, 11, index, edges, 0, &newComm);

    int neighCount;
    MPI_Graph_neighbors_count(newComm, currentRank, &neighCount);
    int *neighbors = (int *) malloc(neighCount * sizeof(int));
    MPI_Graph_neighbors(newComm, currentRank, neighCount, neighbors);
    int *outConnections = (int *) malloc(neighCount * sizeof(int));
    int *inConnections = (int *) malloc(neighCount * sizeof(int));

    int INTERM, SOURCE, SINK, DEAD = 0;
    int inConnCount = 0, outConnCount = 0;

    for (int i = 0; i < neighCount; i++) {
        if (neighbors[i] > currentRank) {
            outConnections[i] = 1;
            inConnections[i] = 0;
            outConnCount++;
        } else {
            outConnections[i] = 0;
            inConnections[i] = 1;
            inConnCount++;
        }
    }

    while (1) {
        printf("[%d]New cycle...\n", currentRank);
        if (DEAD) {
            int recvVal;
            printf("[%d]Waiting for leader...\n", currentRank);
            MPI_Recv(&recvVal, 1, MPI_INT, MPI_ANY_SOURCE, LEADER, MPI_COMM_WORLD, &status);
            printf("[%d]Got leader %d\n", currentRank, status.MPI_SOURCE);
            break;
        }

        computeNodeType(&SOURCE, &INTERM, &SINK, neighCount, outConnections, inConnections, currentRank, neighbors);
//        printf("[%d]SOURCE: %d, INTERM: %d, SINK: %d\n", currentRank, SOURCE, INTERM, SINK);

        if (SOURCE) {
            if (source(newComm, numberOfProcesses, currentRank, neighCount, neighbors, &outConnCount, outConnections,
                       &inConnCount, inConnections) == FALSE) {
                break;
            }
        }

        if (INTERM) {
            interm(newComm, numberOfProcesses, currentRank, neighCount, neighbors, &outConnCount, outConnections,
                   &inConnCount, inConnections, &DEAD);
        }

        if (SINK) {
            sink(newComm, numberOfProcesses, currentRank, neighCount, neighbors, &outConnCount, outConnections,
                 &inConnCount, inConnections, &DEAD);
        }
    }

    free(outConnections);
    free(inConnections);
    free(neighbors);

    MPI_Comm_free(&newComm);

    MPI_Finalize();
    return 0;
}