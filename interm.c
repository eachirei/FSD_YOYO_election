//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "interm.h"


void interm(
        const MPI_Comm newComm,
        const int numberOfProcesses,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections,
        int *DEAD
) {
    MPI_Status status;
    printf("[%d]INTERM\n", currentRank);
    // receive id from in connections
    // compute min and pass through out connections
    int min = numberOfProcesses; // set as max as there cannot be a rank higher than the number of processes
    int *minRecvValues = (int *) malloc(neighCount * sizeof(int));
    for (int i = 0; i < neighCount; i++) {
        if (!inConnections[i]) {
            continue;
        }
        int recvNeigh = neighbors[i];
        MPI_Recv(&minRecvValues[i], 1, MPI_INT, recvNeigh, YO_, newComm, &status);
//                printf("[%d]Receive YO- from %d value %d\n", currentRank, recvNeigh, minRecvValues[i]);
        if (min > minRecvValues[i]) {
            min = minRecvValues[i];
        }
    }
    broadcastOutConnections(
            newComm,
            currentRank,
            neighCount,
            neighbors,
            outConnections,
            &min,
            YO_,
            FWD);
    // -yo phase
    // receive values and reverse connections if needed
    // receive values from out connections
    int passForwardYO = 1;
    int *recvValues = (int *) malloc(neighCount * sizeof(int));
    for (int i = 0; i < neighCount; i++) {
        if (!outConnections[i]) {
            continue;
        }
        int recvNeigh = neighbors[i];
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        MPI_Recv(messagePacket, 2, MPI_INT, recvNeigh, _YO, newComm, &status);
        recvValues[i] = messagePacket[0];
        passForwardYO = passForwardYO & recvValues[i];
        // prune if needed
        if (messagePacket[1]) {
            if (currentRank == LOG_ID) {
                printf("[%d]Pruned node %d\n", currentRank, neighbors[i]);
            }
            outConnections[i] = 0;
            *outConnCount = *outConnCount - 1;
        }
        free(messagePacket);
    }
    int *messagePacket = (int *) malloc(2 * sizeof(int));
    // send values to in connections
    for (int i = 0; i < neighCount; i++) {
        if (!inConnections[i]) {
            continue;
        }
        messagePacket[0] = (passForwardYO && minRecvValues[i] == min) ? 1 : 0;
        // prune edges
        for (int j = i + 1; j < neighCount; j++) {
            if (!inConnections[j]) {
                continue;
            }
            if (minRecvValues[j] != minRecvValues[i]) {
                continue;
            }
            messagePacket[1] = 1; // prune flag
            MPI_Send(messagePacket, 2, MPI_INT, neighbors[j], _YO, newComm);
            printf("[%d]Forward -YO to %d values <%d,%d>\n", currentRank, neighbors[j], messagePacket[0],
                   messagePacket[1]);
            // prune edge
            inConnections[i] = 0;
            *inConnCount = *inConnCount - 1;
        }
        // preemeptive pruning
        messagePacket[1] = ((*outConnCount == 0) && *inConnCount == 1) ? 1 : 0;
        if (messagePacket[1]) {
            *DEAD = 1;
            if (currentRank == LOG_ID)
                printf("[%d]Preemptive pruning\n", currentRank);
        }
        int destNeigh = neighbors[i];
        MPI_Send(messagePacket, 2, MPI_INT, destNeigh, _YO, newComm);
        printf("[%d]Forward -YO to %d values <%d,%d>\n", currentRank, destNeigh, messagePacket[0],
               messagePacket[1]);
    }
    // reverse edges if needed
    for (int i = 0; i < neighCount; i++) {
        // reverse out connections
        if (outConnections[i]) {
            if (recvValues[i] != 0) {
                continue;
            }
            if (currentRank == LOG_ID) {
                printf("[%d]Reversed edge %d\n", currentRank, neighbors[i]);
            }
            inConnections[i] = 1;
            *inConnCount = *inConnCount + 1;
            outConnections[i] = 0;
            *outConnCount = *outConnCount - 1;
            continue;
        }
        if (inConnections[i] && (!passForwardYO || minRecvValues[i] != min)) {
            // reverse in connections
            if (currentRank == LOG_ID) {
                printf("[%d]Reversed edge %d\n", currentRank, neighbors[i]);
            }
            inConnections[i] = 0;
            *inConnCount = *inConnCount - 1;
            outConnections[i] = 1;
            *outConnCount = *outConnCount + 1;
        }
    }
    free(messagePacket);
    free(minRecvValues);
    free(recvValues);
}