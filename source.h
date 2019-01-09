//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#ifndef FSD_SOURCE_H
#define FSD_SOURCE_H

#include "utils.h"

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
);

#endif //FSD_SOURCE_H
