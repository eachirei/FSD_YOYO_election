//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#ifndef FSD_INTERM_H
#define FSD_INTERM_H

#include "utils.h"

void interm(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections,
        int* DEAD
        );

#endif //FSD_INTERM_H
