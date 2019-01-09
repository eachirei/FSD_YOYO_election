//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "source.h"

int source(
        const MPI_Comm newComm,
        const int numberOfProcesses,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections
) {
    MPI_Status status;
    // initiate yo-
    // send id through out connections
    printf("[%d]SOURCE\n", currentRank);
    broadcastOutConnections(
            newComm,
            currentRank,
            neighCount,
            neighbors,
            outConnections,
            &currentRank,
            YO_,
            SND);
    // receive from out connections
    int recvOK = 1;
    for (int i = 0; i < neighCount; i++) {
        if (!outConnections[i]) {
            continue;
        }
        int recvNeigh = neighbors[i];
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        MPI_Recv(messagePacket, 2, MPI_INT, recvNeigh, _YO, newComm, &status);
//                printf("[%d]Receive -YO from %d with <%d, %d>\n", currentRank, recvNeigh, messagePacket[0],
//                       messagePacket[1]);
        if (messagePacket[1]) {
            outConnections[i] = 0;
            *outConnCount = *outConnCount - 1;
            continue;
        }
        // reverse edges
        recvOK = recvOK & messagePacket[0];
        if (messagePacket[0] == 0) {
            if (currentRank == LOG_ID) {
                printf("[%d]Reversed edge %d\n", currentRank, neighbors[i]);
            }
            inConnections[i] = 1;
            *inConnCount = *inConnCount + 1;
            outConnections[i] = 0;
            *outConnCount = *outConnCount - 1;
        }
        free(messagePacket);
    }
    if (recvOK && (*outConnCount) == 0) {
        printf("[%d]Me leader!\n", currentRank);
        for (int i = 0; i < numberOfProcesses; i++) {
            if (i == currentRank) {
                continue;
            }
            MPI_Send(&currentRank, 1, MPI_INT, i, LEADER, MPI_COMM_WORLD);
        }
        return FALSE;
    }
    return TRUE;
}
