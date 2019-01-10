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

    // receive -yo
    // receive from out connections

    int *_YOValues = getUndefinedArray(neighCount);
    int *prunesArr = getUndefinedArray(neighCount);

    complexGather(
            newComm,
            neighCount,
            neighbors,
            outConnections,
            _YOValues,
            prunesArr,
            _YO
    );

    int recvOK = reduceArrayAND(_YOValues, neighCount);

    processPrunes(neighCount, prunesArr, outConnCount, outConnections);

    reverseEdges(neighCount, _YOValues, outConnCount, outConnections, inConnCount, inConnections);

    free(_YOValues);
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
