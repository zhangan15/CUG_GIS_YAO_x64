// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ALGOLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ALGOLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
/************************************************************************/
/* MathAlgoLib.h                                                                     */
/************************************************************************/
#ifndef MATH_ALGOLIB_H_
#define MATH_ALGOLIB_H_


#include "Comlib.h"
//// This class is exported from the AlgoLib.dll
//class ALGOLIB_API CAlgoLib {
//public:
//	CAlgoLib(void);
//	// TODO: add your methods here.
//};
//
//extern ALGOLIB_API int nAlgoLib;

//ALGOLIB_API int fnAlgoLib(void);

//����ת��
ALGOLIB_API void MatrixTranspose(double* pdMatrix, long m, long n, double* pdMatrixT);

//�Ÿ���㷨������ֵ����������
/************************************************************************/
/* double dblEps: �������ޣ�������+0��ȡ0.01
long ljt��ѭ���������ޣ�ȡ4000
	/************************************************************************/
ALGOLIB_API int iJcobiMatrixCharacterValue(double * pdblCof, long lChannelCount, double * pdblVects, double dblEps=0.01,long ljt=4000);


//�������
/**********************************************
*��������InverseMatrix       
*�������ܣ�������󣨸�˹��Լ������ 
*��������������׵�ַ����ά���飩matrix������row
*���������matrixԭ����������
*����ֵ���ɹ���0��ʧ�ܣ�1
*���ú�����swap(double &a,double &b)
**********************************************/
void swap(double &a,double &b);
ALGOLIB_API int InverseMatrix(double *matrix,const int &row);

/************************************************************************
* RGBת��ΪHSL��RGB��Χ��[0, 1]��HSL��Χ��[0, 1]
************************************************************************/
ALGOLIB_API void RGB2HSL(double r, double g, double b, double& h, double& s, double& l);

/************************************************************************
* HSLת��ΪRGB��RGB��Χ��[0, 1]��HSL��Χ��[0, 1]
************************************************************************/
ALGOLIB_API void HSL2RGB(double h, double s, double l, double& r, double& g, double& b);

/************************************************************************
* ����Ѱ��������ֵ����
************************************************************************/
ALGOLIB_API int FindArrMid(int arr[], int length);


ALGOLIB_API int LeastSquares2Element(double *x1, double *x2, double *y, double *a, int m, int n);

ALGOLIB_API double GetValueFromEquation(double x1, double x2, const double *a, int n);

ALGOLIB_API double **DoubleTwoArrayAlloc(int r, int c);

ALGOLIB_API void DoubleTwoArrayFree(double **x);

ALGOLIB_API int PivotingGaussJordanEliminatingForLinearEquations(double *a, double *x, int n);

ALGOLIB_API double lag(double t, double t0, double dt, double e0, double e1,double e2);	//Ҫ��ʱ�����̶�

ALGOLIB_API double lag2(double x, double x0, double x1, double x2, double y0, double y1, double y2);//��Ҫ��ʱ�����̶�


#endif /*MATH_ALGOLIB_H_*/
