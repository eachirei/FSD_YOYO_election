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

void logComplex(const int currentRank, const int destNeigh, const int messageTag, const int *message) {
    if (messageTag == YO_) {
        printf("[%d]Forward "YO_S" to %d value <%d,%d>\n", currentRank, destNeigh, message[0], message[1]);
    }
    if (messageTag == _YO) {
        printf("[%d]Send "_YO_S" to %d value <%d,%d>\n", currentRank, destNeigh, message[0], message[1]);
    }
}

void simpleBroadcast(
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
        if (outConnections[i] != TRUE) {
            continue;
        }
        int destNeigh = neighbors[i];
        MPI_Send(message, 1, MPI_INT, destNeigh, messageTag, newComm);
        switch (type) {
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

void complexBroadcast(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        const int *message,
        const int messageTag
) {
    for (int i = 0; i < neighCount; i++) {
        if (connections[i] != TRUE) {
            continue;
        }
        int destNeigh = neighbors[i];
        MPI_Send(message, 2, MPI_INT, destNeigh, messageTag, newComm);
        logComplex(currentRank, destNeigh, messageTag, message);
    }
}

void complexMultiBroadcast(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        const int *valuesArr,
        const int *prunesArr,
        const int messageTag
) {
    for (int i = 0; i < neighCount; i++) {
        if (connections[i] != TRUE) {
            continue;
        }
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        messagePacket[0] = valuesArr[i];
        messagePacket[1] = prunesArr[i];
        int destNeigh = neighbors[i];
        MPI_Send(messagePacket, 2, MPI_INT, destNeigh, messageTag, newComm);
        logComplex(currentRank, destNeigh, messageTag, messagePacket);
        free(messagePacket);
    }
}


void simpleGather(
        const MPI_Comm newComm,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        int *valuesArr,
        const int messageTag
) {
    MPI_Status status;
    for (int i = 0; i < neighCount; i++) {
        if (connections[i] != TRUE) {
            continue;
        }
        int recvNeigh = neighbors[i];
        MPI_Recv(&valuesArr[i], 1, MPI_INT, recvNeigh, messageTag, newComm, &status);
    }
}

void complexGatherOutConnections(
        const MPI_Comm newComm,
        const int neighCount,
        const int *neighbors,
        const int *outConnections,
        int *valuesArr,
        int *prunesArr,
        const int messageTag
) {
    MPI_Status status;
    for (int i = 0; i < neighCount; i++) {
        if (outConnections[i] != TRUE) {
            continue;
        }
        int recvNeigh = neighbors[i];
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        MPI_Recv(messagePacket, 2, MPI_INT, recvNeigh, messageTag, newComm, &status);
//                printf("[%d]Receive -YO from %d with <%d, %d>\n", currentRank, recvNeigh, messagePacket[0],
//                       messagePacket[1]);
        valuesArr[i] = messagePacket[0];
        prunesArr[i] = messagePacket[1];
        free(messagePacket);
    }
}

int reduceArrayAND(
        const int *arr,
        const int count
) {
    int initial = arr[0];
    for (int i = 1; i < count; i++) {
        if (arr[i] == UNDEFINED) {
            continue;
        }
        initial = initial & arr[i];
    }
    return initial;
}

int reduceArrayMIN(
        const int *arr,
        const int count
) {
    int initial = arr[0];
    for (int i = 1; i < count; i++) {
        if (arr[i] == UNDEFINED) {
            continue;
        }
        if (initial == UNDEFINED) {
            initial = arr[i];
            continue;
        }
        if (initial > arr[i]) {
            initial = arr[i];
        }
    }
    return initial;
}

void processPrunes(
        const int neighCount,
        const int *prunesArr,
        int *connCount,
        int *connections
) {
    for (int i = 0; i < neighCount; i++) {
        if (prunesArr[i] == TRUE && connections[i] == TRUE) {
            connections[i] = FALSE;
            *connCount = *connCount - 1;
        }
    }
}

void reverseEdges(
        const int neighCount,
        const int *valuesArr,
        int *connToRevCount,
        int *connToReverse,
        int *complemConnCount,
        int *complementaryConnections
) {
    for (int i = 0; i < neighCount; i++) {
        if (valuesArr[i] == FALSE && connToReverse[i] == TRUE) {
            connToReverse[i] = FALSE;
            *connToRevCount = *connToRevCount - 1;
            complementaryConnections[i] = TRUE;
            *complemConnCount = *complemConnCount + 1;
        }
    }
}

int *getUndefinedArray(const int count) {
    int *returnArr = (int *) malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        returnArr[i] = UNDEFINED;
    }
    return returnArr;
}

void preparePruneEdges(
        const int neighCount,
        int *inConnections,
        int *prunesArr,
        const int *minRecvValues
) {
    for (int i = 0; i < neighCount; i++) {
        if (inConnections[i] != TRUE && prunesArr[i] != TRUE) {
            continue;
        }
        for (int j = i + 1; j < neighCount; j++) {
            if (inConnections[j] != TRUE && prunesArr[i] != TRUE) {
                continue;
            }
            prunesArr[j] = minRecvValues[i] == minRecvValues[j];
        }
    }
}

void prepareOKValues(
        const int neighCount,
        const int *inConnections,
        const int *recvValues,
        const int min,
        const int passForwardYO,
        int *OKValues
) {
    for (int i = 0; i < neighCount; i++) {
        if (inConnections[i] != TRUE) {
            continue;
        }
        OKValues[i] = passForwardYO && recvValues[i] == min;
    }
}

void pruneAll(
        const int neighCount,
        const int *connections,
        int *prunesArr
) {
    for (int i = 0; i < neighCount; i++) {
        if (connections[i] != TRUE) {
            continue;
        }
        prunesArr[i] = TRUE;
    }
}

void preparePruneNode(
        const int neighCount,
        const int *outConnCount,
        const int *inConnCount,
        const int *inConnections,
        int *prunesArr,
        int *DEAD
) {
    if (*outConnCount > 0) {
        return;
    }
    if (*inConnCount == 1) {
        pruneAll(neighCount, inConnections, prunesArr);
        *DEAD = TRUE;
        return;
    }
    int inConnCountWithoutPruned = 0;
    for (int i = 0; i < neighCount; i++) {
        if (inConnections[i] != TRUE || prunesArr[i] == TRUE) {
            continue;
        }
        inConnCountWithoutPruned++;
    }
    if (inConnCountWithoutPruned == 1) {
        pruneAll(neighCount, inConnections, prunesArr);
        *DEAD = TRUE;
    }
}
