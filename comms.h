//
// Created by Emanuel ACHIREI on 2019-01-10.
//

#ifndef FSD_COMMS_H
#define FSD_COMMS_H

#include "utils.h"

void simpleBroadcast(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *outConnections,
        const int *message,
        const int messageTag,
        const int type
);

void complexBroadcast(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        const int *message,
        const int messageTag
);

void complexMultiBroadcast(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        const int *valuesArr,
        const int *prunesArr,
        const int messageTag
);

void simpleGather(
        const MPI_Comm newComm,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        int *valuesArr,
        const int messageTag
);

void complexGather(
        const MPI_Comm newComm,
        const int neighCount,
        const int *neighbors,
        const int *connections,
        int *valuesArr,
        int *prunesArr,
        const int messageTag
);

#endif //FSD_COMMS_H
