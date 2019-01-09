//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "utils.h"

void computeNodeType(int *SOURCE, int *INTERM, int *SINK, int neighCount, const int *outConnections,
                     const int *inConnections, const int rank,
                     const int *neighbors) {
    *SOURCE = 1;
    *SINK = 1;
    if (rank == LOG_ID)
        printf("[%d]", rank);
    for (int i = 0; i < neighCount; i++) {
        if (outConnections[i]) {
            if (rank == LOG_ID)
                printf("O%d,", neighbors[i]);
            *SINK = 0;
            continue;
        }
        if (inConnections[i]) {
            if (rank == LOG_ID)
                printf("I%d,", neighbors[i]);
            *SOURCE = 0;
        }
    }
    if (rank == LOG_ID)
        printf("\n");

    *INTERM = (*SOURCE == 0 && *SINK == 0);
}

void logSend(const int currentRank, const int destNeigh, const int messageTag) {
    if (messageTag == YO_) {
        printf("[%d]Send "YO_S" to %d\n", currentRank, destNeigh);
    }
    if (messageTag == _YO) {
        printf("[%d]Send "_YO_S" to %d\n", currentRank, destNeigh);
    }
}

void logForward(const int currentRank, const int destNeigh, const int messageTag, const int value) {
    if (messageTag == YO_) {
        printf("[%d]Forward "YO_S" to %d value %d\n", currentRank, destNeigh, value);
    }
    if (messageTag == _YO) {
        printf("[%d]Send "_YO_S" to %d value %d\n", currentRank, destNeigh, value);
    }
}

void broadcastOutConnections(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *outConnections,
        const int *message,
        const int messageTag,
        const int type
) {
    for (int i = 0; i < neighCount; i++) {
        if (!outConnections[i]) {
            continue;
        }
        int destNeigh = neighbors[i];
        MPI_Send(message, 1, MPI_INT, destNeigh, messageTag, newComm);
        switch (type) {
            case SNK: {
                break;
            }
            case FWD: {
                logForward(currentRank, destNeigh, messageTag, *message);
                break;
            }
            case SND:
            default:
                logSend(currentRank, destNeigh, messageTag);
        }

    }
}