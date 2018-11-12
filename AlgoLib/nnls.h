
#ifndef _NNLS_H
#define _NNLS_H

#include "Comlib.h"
/*****************************************************************************/
extern int NNLS_TEST;
/*****************************************************************************/
ALGOLIB_API int nnls(double **a, int m, int n, double *b, double *x,
          double *rnorm, double *w, double *zz, int *index);
ALGOLIB_API int nnlsWght(int N, int M, double **A, double *b, double *weight);
ALGOLIB_API int nnlsWghtSquared(int N, int M, double **A, double *b, double *sweight);
/*****************************************************************************/
#endif
