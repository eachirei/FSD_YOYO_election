//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "interm.h"


void interm(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections,
        int *DEAD
) {
    printf("[%d]INTERM\n", currentRank);
    // yo- phase
    // receive id from in connections
    // compute min and pass through out connections
    int *YO_Values = getUndefinedArray(neighCount);

    simpleGather(newComm, neighCount, neighbors, inConnections, YO_Values, YO_);

    int min = reduceArrayMIN(YO_Values, neighCount);

    simpleBroadcast(
            newComm,
            currentRank,
            neighCount,
            neighbors,
            outConnections,
            &min,
            YO_,
            FWD);

    // -yo phase
    // receive values from out connections
    int *_YOValues = getUndefinedArray(neighCount);
    int *prunesArr = getUndefinedArray(neighCount);

    complexGather(newComm, neighCount, neighbors, outConnections, _YOValues, prunesArr, _YO);

    processPrunes(neighCount, prunesArr, outConnCount, outConnections);

    free(prunesArr);

    int passForwardYO = reduceArrayAND(_YOValues, neighCount);

    prunesArr = getUndefinedArray(neighCount);

    int *OKValues = getUndefinedArray(neighCount);

    prepareOKValues(neighCount, inConnections, YO_Values, min, passForwardYO, OKValues);

    preparePruneEdges(neighCount, inConnections, prunesArr, YO_Values);

    // preemptive pruning of node
    preparePruneNode(neighCount, outConnCount, inConnCount, inConnections, prunesArr, DEAD);

    complexMultiBroadcast(newComm, currentRank, neighCount, neighbors, inConnections, OKValues, prunesArr, _YO);

    free(prunesArr);

    // reverse edges if needed
    for (int i = 0; i < neighCount; i++) {
        // reverse out connections
        if (outConnections[i]) {
            if (_YOValues[i] == TRUE) {
                continue;
            }
            if (currentRank == LOG_ID) {
                printf("[%d]Reversed edge %d\n", currentRank, neighbors[i]);
            }
            inConnections[i] = TRUE;
            *inConnCount = *inConnCount + 1;
            outConnections[i] = FALSE;
            *outConnCount = *outConnCount - 1;
            continue;
        }
        // the in connection is reversed if
        // at least one NO was received in _YO phase
        // or
        // the minimum is different than the values received in yo- phase
        if (inConnections[i] && (!passForwardYO || YO_Values[i] != min)) {
            // reverse in connections
            if (currentRank == LOG_ID) {
                printf("[%d]Reversed edge %d\n", currentRank, neighbors[i]);
            }
            inConnections[i] = FALSE;
            *inConnCount = *inConnCount - 1;
            outConnections[i] = TRUE;
            *outConnCount = *outConnCount + 1;
        }
    }
    free(YO_Values);
    free(_YOValues);
}