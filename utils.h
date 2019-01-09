//
// Created by Emanuel ACHIREI on 2019-01-09.
//

#ifndef FSD_UTILS_H
#define FSD_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define LOG_ID 5
#define YO_ 100
#define YO_S "YO-"
#define _YO 101
#define _YO_S "-YO"
#define LEADER 102

#define SND 777
#define FWD 888
#define SNK 999

#define TRUE 1
#define FALSE 0

void computeNodeType(int *SOURCE, int *INTERM, int *SINK, int neighCount, const int *outConnections,
                     const int *inConnections, const int rank,
                     const int *neighbors);

void broadcastOutConnections(
        const MPI_Comm newComm,
        const int currentRank,
        const int neighCount,
        const int *neighbors,
        const int *outConnections,
        const int *message,
        const int messageTag,
        const int type
);

#endif //FSD_UTILS_H
