//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#ifndef FSD_SINK_H
#define FSD_SINK_H

#include "utils.h"

void sink(
        const MPI_Comm newComm,
        const int numberOfProcesses,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections,
        int* DEAD
);

#endif //FSD_SINK_H
