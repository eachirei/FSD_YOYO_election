//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "sink.h"

void sink(
        const MPI_Comm newComm,
        const int numberOfProcesses,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections,
        int* DEAD
){
    MPI_Status status;
    printf("[%d]SINK\n", currentRank);
    // initiate -yo
    // received ids
    int *recvValues = (int *) malloc(neighCount * sizeof(int));
    int min = numberOfProcesses;
    for (int i = 0; i < neighCount; i++) {
        if (!inConnections[i]) {
            continue;
        }
        int recvNeigh = neighbors[i];
        MPI_Recv(&recvValues[i], 1, MPI_INT, recvNeigh, YO_, newComm, &status);
//                printf("[%d]Receive YO- from %d value %d\n", currentRank, recvNeigh, recvValues[i]);
        if (min > recvValues[i]) {
            min = recvValues[i];
        }
    }
    // prune if leaf
    if (*inConnCount == 1) {
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        messagePacket[0] = 1; // if only one neighbor, obviously send yes
        messagePacket[1] = 1; // prune flag
        for (int g = 0; g < neighCount; g++) {
            if (!inConnections[g]) {
                continue;
            }
            MPI_Send(messagePacket, 2, MPI_INT, neighbors[g], _YO, newComm);
            printf("[%d]Send -YO to %d values <%d,%d>\n", currentRank, neighbors[g], messagePacket[0],
                   messagePacket[1]);
            break;
        }
        free(messagePacket);
        *DEAD = 1;
        free(recvValues);
        return;
    }
    // compute min and send YES through in connections with min and NO otherwise
    for (int i = 0; i < neighCount; i++) {
        if (!inConnections[i]) {
            continue;
        }
        int sendValue = recvValues[i] == min ? 1 : 0;
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        messagePacket[0] = sendValue;
        // prune edges
        for (int j = i + 1; j < neighCount; j++) {
            if (!inConnections[j]) {
                continue;
            }
            if (recvValues[j] != recvValues[i]) {
                continue;
            }
            messagePacket[1] = 1; // prune flag
            MPI_Send(messagePacket, 2, MPI_INT, neighbors[j], _YO, newComm);
            printf("[%d]Send -YO to %d values <%d,%d>\n", currentRank, neighbors[j], messagePacket[0],
                   messagePacket[1]);
            // prune edge
            inConnections[i] = 0;
            *inConnCount = *inConnCount - 1;
        }
        messagePacket[1] = 0;
        int destNeigh = neighbors[i];
        MPI_Send(messagePacket, 2, MPI_INT, destNeigh, _YO, newComm);
        printf("[%d]Send -YO to %d values <%d,%d>\n", currentRank, destNeigh, messagePacket[0],
               messagePacket[1]);
        // reverse edges
        if (sendValue == 0) {
            inConnections[i] = 0;
            *inConnCount = *inConnCount - 1;
            outConnections[i] = 1;
            *outConnCount = *outConnCount + 1;
        }
        free(messagePacket);
    }
    free(recvValues);
}
