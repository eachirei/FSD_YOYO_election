//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "utils.h"

int *getUndefinedArray(const int count) {
    int *returnArr = (int *) malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        returnArr[i] = UNDEFINED;
    }
    return returnArr;
}

int reduceArrayAND(
        const int *arr,
        const int count
) {
    int initial = arr[0];
    for (int i = 1; i < count; i++) {
        // short-circuit
        if (initial == FALSE) {
            return initial;
        }
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

void computeNodeType(
        int *SOURCE,
        int *INTERM,
        int *SINK,
        int neighCount,
        const int *outConnections,
        const int *inConnections,
        const int rank,
        const int *neighbors
) {
    *SOURCE = TRUE;
    *SINK = TRUE;
    if (rank == LOG_ID) {
        printf("[%d]", rank);
    }
    for (int i = 0; i < neighCount; i++) {
        if (outConnections[i]) {
            if (rank == LOG_ID) {
                printf("O%d,", neighbors[i]);
            }
            *SINK = FALSE;
            continue;
        }
        if (inConnections[i]) {
            if (rank == LOG_ID) {
                printf("I%d,", neighbors[i]);
            }
            *SOURCE = FALSE;
        }
    }
    if (rank == LOG_ID) {
        printf("\n");
    }

    *INTERM = (*SOURCE == FALSE && *SINK == FALSE);
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
