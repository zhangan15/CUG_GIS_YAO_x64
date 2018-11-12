/********************************************
 * Name  :matrix class
 * Author:Zou Tongyuan 
 * Date  :20111105
 * ALL RIGHT DESERVED
 ********************************************/
#ifndef CMATRIX_H_
#define CMATRIX_H_
#include "include_global.h"
#include "Comlib.h"
#include "basedefine_global.h"

class ALGOLIB_API CMatrix
{
protected:
	int	m_nNumColumns;			// columns of matrix
	int	m_nNumRows;				// rows of matrix
	double*	m_pData;			// matrix data

public:
	//
	// construct and distroy matrix
	//
	
	CMatrix();										// base construction
	CMatrix(int nRows, int nCols);					// construction with columns and rows, data is NULL
	CMatrix(int nRows, int nCols, double value[]);	// construction with columns and rows and data
	CMatrix(int nSize);								// construction the columns = rows, data is NULL
	CMatrix(int nSize, double value[]);				// construction the columns = rows, the data is set
	CMatrix(const CMatrix& other);					// copy construction
	BOOL	Init(int nRows, int nCols);				// init the matrix	
	BOOL	MakeUnitMatrix(int nSize);				// init the cubic matrix with 1
	virtual ~CMatrix();								// destroy matirx

	//
	// element and value process
	//
	
	BOOL	SetElement(int nRow, int nCol, double value);	// set element value
	double	GetElement(int nRow, int nCol) const;			// get element value
	void    SetData(double value[]);						// set matrix array value
	int		GetNumColumns() const;							// get matrix columns
	int		GetNumRows() const;								// get matrix rows
	int     GetRowVector(int nRow, double* pVector) const;	// get matrix row data
	int     GetColVector(int nCol, double* pVector) const;	// get matrix column data
	double* GetData() const;								// get matrix array value
	
	//
	// math operation
	//
	
	CMatrix& operator=(const CMatrix& other);
	BOOL operator==(const CMatrix& other) const;
	BOOL operator!=(const CMatrix& other) const;
	CMatrix	operator+(const CMatrix& other) const;
	CMatrix	operator-(const CMatrix& other) const;
	CMatrix	operator*(double value) const;
	CMatrix	operator*(const CMatrix& other) const;
	// complex matrix multiplication
	BOOL CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, 
		const CMatrix& BI, CMatrix& CR, CMatrix& CI) const;
	// complex matrix multiplication��dot multiplication��
	BOOL CInMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, 
		const CMatrix& BI, CMatrix& CR, CMatrix& CI) const;
	// matrix transpos
	CMatrix Transpose() const;

	BOOL Mul(const CMatrix& Amat, const CMatrix& Bmat, CMatrix& Cmat);
	// 
	BOOL InMul(const CMatrix& Amat, const CMatrix& Bmat, CMatrix& Cmat);
	
	//
	void InvTByRowVec(int* RowVec,double* Para);
	void InvTByRowVec(double* RowVec,double* Para);
	
	//
	// algorithm
	//
	
	// 
	BOOL InvertGaussJordan();                                               
	// 
	BOOL InvertGaussJordan(CMatrix& mtxImag);                                 
	// 
	BOOL InvertSsgj();                                              
	// 
	BOOL InvertTrench();                                                    
	// 
	double DetGauss();                                                              
	// 
	int RankGauss();
	// 
	BOOL DetCholesky(double* dblDet);                                                               
	// 
	BOOL SplitLU(CMatrix& mtxL, CMatrix& mtxU);                                     
	// 
	BOOL SplitQR(CMatrix& mtxQ);                                                      
	// 
	BOOL SplitUV(CMatrix& mtxU, CMatrix& mtxV, double eps = 0.000001);                                       
	// 
	BOOL GInvertUV(CMatrix& mtxAP, CMatrix& mtxU, CMatrix& mtxV, double eps = 0.000001);
	// 
	BOOL MakeSymTri(CMatrix& mtxQ, CMatrix& mtxT, double dblB[], double dblC[]);
	// 
	BOOL SymTriEigenv(double dblB[], double dblC[], CMatrix& mtxQ, int nMaxIt = 60, double eps = 0.000001);
	// 
	void MakeHberg();
	// 
	BOOL HBergEigenv(double dblU[], double dblV[], int nMaxIt = 60, double eps = 0.000001);
	// 
	BOOL JacobiEigenv(double dblEigenValue[], CMatrix& mtxEigenVector, int nMaxIt = 60, double eps = 0.000001);
	// 
	BOOL JacobiEigenv2(double dblEigenValue[], CMatrix& mtxEigenVector, double eps = 0.000001);

	//
	// inner function
	//
private:
	void ppp(double a[], double e[], double s[], double v[], int m, int n);
	void sss(double fg[2], double cs[2]);

};

#endif /*CMATRIX_H_*/
