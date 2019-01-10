//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#include "sink.h"

void sink(
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
    printf("[%d]SINK\n", currentRank);
    // initiate yo-
    // received ids
    int *YO_Values = getUndefinedArray(neighCount);
    simpleGather(newComm, neighCount, neighbors, inConnections, YO_Values, YO_);
    int min = reduceArrayMIN(YO_Values, neighCount);

    // initiate -yo
    // prune if leaf
    if (*inConnCount == 1) {
        int *messagePacket = (int *) malloc(2 * sizeof(int));
        messagePacket[0] = TRUE; // if only one neighbor, obviously send yes
        messagePacket[1] = TRUE; // prune flag

        complexBroadcast(newComm, currentRank, neighCount, neighbors, inConnections, messagePacket, _YO);

        free(messagePacket);
        free(YO_Values);
        *DEAD = TRUE;
        return;
    }

    int *OKValues = getUndefinedArray(neighCount);
    int *prunesArr = getUndefinedArray(neighCount);

    // send YES through in connections with min and NO otherwise
    prepareOKValues(neighCount, inConnections, YO_Values, min, TRUE, OKValues);

    preparePruneEdges(neighCount, inConnections, prunesArr, YO_Values);

    complexMultiBroadcast(newComm, currentRank, neighCount, neighbors, inConnections, OKValues, prunesArr, _YO);

    processPrunes(neighCount, prunesArr, inConnCount, inConnections);

    reverseEdges(neighCount, OKValues, inConnCount, inConnections, outConnCount, outConnections);

    free(OKValues);
    free(prunesArr);
    free(YO_Values);
}
