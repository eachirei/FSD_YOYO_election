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
#define UNDEFINED -1

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

void complexGatherOutConnections(
        const MPI_Comm newComm,
        const int neighCount,
        const int *neighbors,
        const int *outConnections,
        int *valuesArr,
        int *prunesArr,
        const int messageTag
);

int reduceArrayAND(
        const int *arr,
        const int count
);

void processPrunes(
        const int neighCount,
        const int *prunesArr,
        int *outConnCount,
        int *outConnections
);

void reverseEdges(
        const int neighCount,
        const int *valuesArr,
        int *outConnCount,
        int *outConnections,
        int *inConnCount,
        int *inConnections
);

#endif //FSD_UTILS_H
