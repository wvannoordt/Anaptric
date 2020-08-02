#ifndef UTILS_H
#define UTILS_H

#include "Config.h"
#include <cstdlib>
#include <ctime>

static inline void Dim2Idx(int idx, int* dims, int* ijk)
{
    ijk[0] = idx%(dims[0]);
    ijk[1] = ((idx-ijk[0]) / dims[0])%dims[1];
#if(IS3D)
    ijk[2] = ((idx-ijk[0]-dims[0]*ijk[1])/dims[1]) % dims[2];
#endif
}

static inline int CharBit(char v, int dim)
{
    return 1&(v>>dim);
}

static inline int Idx2Dim(int* dims, int* ijk)
{
#if(IS3D)
    return ijk[0] + ijk[1]*dims[0] + ijk[2]*dims[1]*dims[0];
#else
    return ijk[0] + ijk[1]*dims[0];
#endif
}

static inline char GetOctant(double* bounds, double* coords)
{
#if(IS3D)
    return ((2.0*(coords[0]-bounds[0])>=(bounds[1]-bounds[0]))?1:0)
        +  ((2.0*(coords[1]-bounds[2])>=(bounds[3]-bounds[2]))?2:0)
        +  ((2.0*(coords[2]-bounds[4])>=(bounds[5]-bounds[4]))?4:0);
#else    
    return ((2.0*(coords[0]-bounds[0])>=(bounds[1]-bounds[0]))?1:0)+((2.0*(coords[1]-bounds[2])>=(bounds[3]-bounds[2]))?2:0);
#endif
}

static inline int RandomInt(int bound)
{
    double r = (double)(rand()%0x000fffff)/((double)0x000fffff);
    return (int)(r*bound)%bound;
}

#endif