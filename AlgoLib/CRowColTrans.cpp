#include "CRowColTrans.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>



CRowColTrans::CRowColTrans()
{
	cTempFile2 = NULL;
	cTempFile3 = NULL;
	xu = NULL;
	xv = NULL;
	xw = NULL;
	xuv = NULL;
	xvw = NULL;
	xwu = NULL;
	yu = NULL;
	yv = NULL;
	yw = NULL;
	yuv = NULL;
	yvw = NULL;
	ywu = NULL;
		
	list = NULL;
	f = NULL;
	x = NULL;
	y = NULL;
	xold = NULL;
	yold = NULL;
	

}

CRowColTrans::~CRowColTrans()
{
	if(cTempFile2 != NULL) delete[]cTempFile2;
	if(cTempFile3 != NULL) delete[]cTempFile3;
	if(xu != NULL) delete[]xu;
	if(xv != NULL) delete[]xv;
	if(xw != NULL) delete[]xw;
	if(xuv != NULL) delete[]xuv;
	if(xvw != NULL) delete[]xvw;
	if(xwu != NULL) delete[]xwu;
	if(yu != NULL) delete[]yu;
	if(yv != NULL) delete[]yv;
	if(yw != NULL) delete[]yw;
	if(yuv != NULL) delete[]yuv;
	if(yvw != NULL) delete[]yvw;
	if(ywu != NULL) delete[]ywu;
	if(list != NULL) delete[]list;
	if(x != NULL) delete[]x;
	if(y != NULL) delete[]y;
	if(xold != NULL) delete[]xold;
	if(yold != NULL) delete[]yold;
	if(f != NULL) FloatTwoArrayFree(f);

}

void CRowColTrans::One(float x1, float y1, float x2, float y2, float x3, float y3,
		 float u1, float v1, float u2, float v2, float u3, float v3,
		 float *a1,float *b1,float *c1,float *a2,float *b2,float *c2)
{
	float x21,x31,y21,y31,xy23,xy13,xy12,u21,v21,u31,v31;
	float r0;
	x21=x2-x1;
	x31=x3-x1;
	y21=y2-y1;
	y31=y3-y1;
	u21=u2-u1;
	v21=v2-v1;
	u31=u3-u1;
	v31=v3-v1;
	xy23=x2*y3-x3*y2;
	xy13=x1*y3-x3*y1;
	xy12=x1*y2-x2*y1;
	r0=x21*y31-x31*y21;

//	*a1=(xy23*u1-xy13*u2+xy12*u3)/r0;    //wang hongqiang 20081008
//	*a2=(v1*xy23-v2*xy13+v3*xy12)/r0;    //the two line is error

	*b1=(u21*y31-u31*y21)/r0;
	*b2=(v21*y31-v31*y21)/r0;
	*c1=(x21*u31-x31*u21)/r0;
	*c2=(x21*v31-x31*v21)/r0;

	*a1=u1-(*b1)*x1-(*c1)*y1;          //wang hongqiang 20081008
	*a2=v1-(*b2)*x1-(*c2)*y1;          //the two line is right
}

int CRowColTrans::post(float x, float y)
{
	int kk,k,k1,n;
	float p,q;
	p=(float)(x+1);
	q=(float)(y+1);
	
	p=x;
	q=y;
	
	n=0;
	k=0;
	//while(n==0 && list[k]>0)
	while(list[k]>0)
	{

		kk=k;
		k=list[k];
		k1=k-1;   
		
		if(((p-xv[k1])*yuv[k1]-(q-yv[k1])*xuv[k1])>=0)
		{
			if(((p-xw[k1])*yvw[k1]-(q-yw[k1])*xvw[k1])>=0)
			{
				if(((p-xu[k1])*ywu[k1]-(q-yu[k1])*xwu[k1])>=0)
					n=k;
				else
					continue;
			}
			else
				continue;
		}
		else
		{
			if(((p-xw[k1])*yvw[k1]-(q-yw[k1])*xvw[k1])<=0)
			{
				if(((p-xu[k1])*ywu[k1]-(q-yu[k1])*xwu[k1])<=0)
					n=k;
				else
					continue;
			}
			else
				continue;
		}

		if(kk>0)
		{
			list[kk]=list[k];
			list[k]=list[0];
			list[0]=k;
		}
		return(n);
	}
	
	return(n);
}

int CRowColTrans::Init(char *workPath, int jbTag)
{
	FILE *fp1,*fp2;
	int i;
	int i1, i2, i3;
/*************************************************************************
*                   Read the parameters of resampling                   * 
*************************************************************************/

	cTempFile2 = new char[512];
	cTempFile3 = new char[512];
	
	if(jbTag==0)
	{
		sprintf(cTempFile2, "%s/GC_TEMP_2.txt", workPath);
		sprintf(cTempFile3, "%s/GC_TEMP_3.txt", workPath);
	}
	else
	{
		sprintf(cTempFile2, "%s/PP_TEMP_2.txt", workPath);
		sprintf(cTempFile3, "%s/PP_TEMP_3.txt", workPath);
	}
	
	if((fp1=fopen(cTempFile2,"r"))!=NULL)
	{
		if((fp2=fopen(cTempFile3,"r"))!=NULL)
		{

/*************************************************************************
*             Read number of control points                              * 
*************************************************************************/
			fscanf(fp1,"%d",&pn);
/*************************************************************************
*             Read sample and line of input image and output image       * 
*************************************************************************/
			fscanf(fp1,"%d %d %d %d",&xosize,&yosize,&xnsize,&ynsize);
					
/*************************************************************************
* Read the coordinates of control points in the input and output image   * 
*************************************************************************/
			xold = new float[pn+17];
			yold = new float[pn+17];
			x = new float[pn+17];
			y = new float[pn+17];
			for(i=0; i<pn; i++)
				fscanf(fp1,"%f %f %f %f",&xold[i],&yold[i],&x[i],&y[i]);
			fclose(fp1);
/*************************************************************************
*             Read the number of reiangle                                * 
*************************************************************************/
			fscanf(fp2,"%d",&tn);
			xu = new float[tn+1];	xv = new float[tn+1];	xw = new float[tn+1];
			xuv = new float[tn+1];xvw = new float[tn+1];xwu = new float[tn+1];
			yu = new float[tn+1];	yv = new float[tn+1];	yw = new float[tn+1];
			yuv = new float[tn+1];yvw = new float[tn+1];ywu = new float[tn+1];
			list = new int[tn+4];
			f = FloatTwoArrayAlloc(tn, 6);
/*************************************************************************
*              Find triangle interpolation parameters                    *
*************************************************************************/
				
			for(i=0; i<tn; i++)
			{
				fscanf(fp2,"%d %d %d",&i1,&i2,&i3);
				i1=i1-1;
				i2=i2-1;
				i3=i3-1;
				xu[i]=x[i1];
				xv[i]=x[i2];
				xw[i]=x[i3];
				xuv[i]=xu[i]-xv[i];
				xvw[i]=xv[i]-xw[i];
				xwu[i]=xw[i]-xu[i];
				yu[i]=y[i1];
				yv[i]=y[i2];
				yw[i]=y[i3];
				yuv[i]=yu[i]-yv[i];
				yvw[i]=yv[i]-yw[i];
				ywu[i]=yw[i]-yu[i];
				One(x[i1],y[i1],x[i2],y[i2],x[i3],y[i3],
				   xold[i1],yold[i1],xold[i2],yold[i2],xold[i3],yold[i3],
				   &f[i][0],&f[i][1],&f[i][2],&f[i][3],&f[i][4],&f[i][5]);
				list[i]=i+1;  
			}
			fclose(fp2);
			list[tn]=0;
		}
		else
		{
			fclose(fp1);
			return -1;
		}
	}
	else
	{
		return -1;
	}
	
	return 0;
}

float **CRowColTrans::FloatTwoArrayAlloc(int r, int c)
{
	float *x, **y;
	int n;
	x = (float *)calloc(r*c, sizeof(float));
	y = (float **)calloc(r, sizeof(float *));
	for(n=0; n<r; n++)
	{
		y[n] = &x[c*n];
	}
	return (y);
}
void CRowColTrans::FloatTwoArrayFree(float **x)
{
	free(x[0]);
	free(x);
}

float CRowColTrans::samp(float a, float b, float c, float x, float y)
{
	return (a+b*x+c*y);
}

int CRowColTrans::GetRowCol(float xpL2, float ypL2, float *xpL1, float*ypL1)
{
	int i = post(xpL2, ypL2);
	if(i>0)
	{
		i = i-1;
		*xpL1=samp(f[i][0],f[i][1],f[i][2],xpL2,ypL2);
		*ypL1=samp(f[i][3],f[i][4],f[i][5],xpL2,ypL2);
													
		return 0;
	}
	else
	{
		*xpL1 = -1;
		*ypL1 = -1;
		return 1;
	}
	
}
