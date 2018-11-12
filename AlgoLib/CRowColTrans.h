#ifndef CROWCOLTRANS_H_
#define CROWCOLTRANS_H_

#include "Comlib.h"

///////////////////////////////////////////////////////////////////////////////////////////
// first: run Init()
// then: run GetRowCol() to get row and col in Level1 
///////////////////////////////////////////////////////////////////////////////////////////

class ALGOLIB_API CRowColTrans
{
public:
	CRowColTrans();
	virtual ~CRowColTrans();
	
	//workPath:  workdir path, example: "/dps/workdir/CCD1/LELEL2/12345/" 
	int Init(char *workPath, int jbTag=0);
	
	//input: xpL2, ypL2,  output: xpL1, ypL1
	//xp: col,  yp: row
	//return code:    0: success,  1: fail (and set: xpL1=-1, ypL1=-1)
	int GetRowCol(float xpL2, float ypL2, float *xpL1, float *ypL1);

	
	
private:

	//中间结果文件
	char *cTempFile2;	//三角形剖分后的控制点文件
	char *cTempFile3;	//三角形剖分后的三角形文件
	
	float *xu, *xv, *xw, *xuv, *xvw, *xwu, *yu, *yv, *yw, *yuv, *yvw, *ywu;
	
	int *list;
	int xosize,yosize,xmin,ymin,nx,ny,tn,pn,xnsize,ynsize;

	float **f, *x, *y, *xold, *yold;
	
	void One(float x1, float y1, float x2, float y2, float x3, float y3,
			 float u1, float v1, float u2, float v2, float u3, float v3,
			 float *a1,float *b1,float *c1,float *a2,float *b2,float *c2);
	int post(float x, float y);
	float **FloatTwoArrayAlloc(int r, int c);
	void FloatTwoArrayFree(float **x);
	float samp(float a, float b, float c, float x, float y);
};

#endif /*CROWCOLTRANS_H_*/
