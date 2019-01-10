//
// Created by Emanuel ACHIREI on 2019-01-10.
//

#include "comms.h"

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

void complexGather(
        const MPI_Comm newComm,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        int *valuesArr,
        int *prunesArr,
        const int messageTag
) {
    MPI_Status status;
    for (int i = 0; i < neighCount; i++) {
        if (connections[i] != TRUE) {
            continue;
        }
        int recvNeigh = neighbors[i];
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        MPI_Recv(messagePacket, 2, MPI_INT, recvNeigh, messageTag, newComm, &status);
        valuesArr[i] = messagePacket[0];
        prunesArr[i] = messagePacket[1];
        free(messagePacket);
    }
}

