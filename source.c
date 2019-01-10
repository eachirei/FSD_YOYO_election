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
    // initiate yo-
    // send id through out connections
    printf("[%d]SOURCE\n", currentRank);
    simpleBroadcast(
            newComm,
            currentRank,
            neighCount,
            neighbors,
            outConnections,
            &currentRank,
            YO_,
            SND);
    // receive from out connections
    int *valuesArr = getUndefinedArray(neighCount);
    int *prunesArr = getUndefinedArray(neighCount);

    complexGatherOutConnections(
            newComm,
            neighCount,
            neighbors,
            outConnections,
            valuesArr,
            prunesArr,
            _YO
    );

    int recvOK = reduceArrayAND(valuesArr, neighCount);

    processPrunes(neighCount, prunesArr, outConnCount, outConnections);

    reverseEdges(neighCount, valuesArr, outConnCount, outConnections, inConnCount, inConnections);

    free(valuesArr);
    free(prunesArr);

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
