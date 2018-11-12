// AlgoLib.cpp : Defines the exported functions for the DLL application.
//

#include "include_global.h"
#include "MathAlgoLib.h"
#include <math.h>
//
//// This is an example of an exported variable
//ALGOLIB_API int nAlgoLib=0;
//
//// This is an example of an exported function.
//ALGOLIB_API int fnAlgoLib(void)
//{
//	return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see AlgoLib.h for the class definition
//CAlgoLib::CAlgoLib()
//{
//	return;
//}

void MatrixTranspose( double* pdMatrix, long m, long n, double* pdMatrixT )
{
	int i,j;
	for (i = 0; i<m; i++)
	{
		for ( j=0; j<n; j++)
		{
			pdMatrixT[j*n + i] = pdMatrix[i*m + j];
		}
	}
}

int iJcobiMatrixCharacterValue( double * pdblCof, long lChannelCount, double * pdblVects, double dblEps/*=0.01*/,long ljt/*=4000*/ )
{
	long i,j,p,q,u,w,t,s,l;
	double fm,cn,sn,omega,x,y,d;
	l = 1;

	for(i = 0; i < lChannelCount; i ++)
	{    
		pdblVects[i * lChannelCount + i] = 1.0;

		for(j = 0; j < lChannelCount; j ++)
			if(i != j) pdblVects[i * lChannelCount + j] = 0.0;
	}

	while(1)
	{
		fm = 0.0;
		for(i = 0; i < lChannelCount; i ++)
			for(j = 0; j < lChannelCount; j ++)
			{  
				d = fabs(pdblCof[i * lChannelCount + j]);
				if((i != j)&&(d > fm))
				{ fm = d; p = i;  q = j;}
			}

			if(fm<dblEps) return 1;
			if(l>ljt)   return 0;
			l+=1;
			u= p*lChannelCount+q;   w= p*lChannelCount+p; t= q*lChannelCount+p;  s= q*lChannelCount+q;
			x= -pdblCof[u];    y= (pdblCof[s]-pdblCof[w])/2.0;
			omega= x/sqrt(x*x+y*y);
			if(y< 0) omega=-omega;
			sn=1.0+sqrt(1.0-omega*omega);
			sn=omega/sqrt(2.0*sn);
			cn=sqrt(1.0-sn*sn);
			fm=pdblCof[w];
			pdblCof[w]=fm*cn*cn+pdblCof[s]*sn*sn+pdblCof[u]*omega;
			pdblCof[s]=fm*sn*sn+pdblCof[s]*cn*cn-pdblCof[u]*omega;
			pdblCof[u]=0.0;   pdblCof[t]=0.0;

			for(j=0;j< lChannelCount ; j++)
				if((j!=p)&&(j!=q))
				{
					u=p*lChannelCount+j;w=q*lChannelCount+j;
					fm=pdblCof[u];
					pdblCof[u]=fm*cn+pdblCof[w]*sn;
					pdblCof[w]=-fm*sn+pdblCof[w]*cn;
				}

				for(i=0;i<lChannelCount;i++)
					if((i!=p)&&(i!=q))
					{  
						u=i*lChannelCount+p;w=i*lChannelCount+q;
						fm=pdblCof[u];
						pdblCof[u]=fm*cn+pdblCof[w]*sn;
						pdblCof[w]=-fm*sn+pdblCof[w]*cn;
					}

					for(i=0;i<lChannelCount;i++)
					{  
						u=i*lChannelCount+p;w=i*lChannelCount+q;
						fm=pdblVects[u];
						pdblVects[u]= fm*cn+pdblVects[w]*sn;
						pdblVects[w]=-fm*sn+pdblVects[w]*cn;
					}
	}
	return 1;
}

void swap( double &a,double &b )
{
	double temp=a;
	a=b;
	b=temp;
}

int InverseMatrix( double *matrix,const int &row )
{
	double *m=new double[row*row];
	double *ptemp,*pt=m;

	int i,j;

	ptemp=matrix;
	for (i=0;i<row;i++)
	{
		for (j=0;j<row;j++)
		{
			*pt=*ptemp;
			ptemp++;
			pt++;
		}
	}

	int k;

	int *is=new int[row],*js=new int[row];

	for (k=0;k<row;k++)
	{
		double max=0;
		//全选主元
		//寻找最大元素
		for (i=k;i<row;i++)
		{
			for (j=k;j<row;j++)
			{
				if (fabs(*(m+i*row+j))>max)
				{
					max=*(m+i*row+j);
					is[k]=i;
					js[k]=j;
				}
			}
		}

		if (0 == max)
		{
			return 1;
		}

		//行交换
		if (is[k]!=k)
		{
			for (i=0;i<row;i++)
			{
				swap(*(m+k*row+i),*(m+is[k]*row+i));
			}
		}

		//列交换
		if (js[k]!=k)
		{
			for (i=0;i<row;i++)
			{
				swap(*(m+i*row+k),*(m+i*row+js[k]));
			}
		}

		*(m+k*row+k)=1/(*(m+k*row+k));

		for (j=0;j<row;j++)
		{
			if (j!=k)
			{
				*(m+k*row+j)*=*((m+k*row+k));
			}
		}

		for (i=0;i<row;i++)
		{
			if (i!=k)
			{
				for (j=0;j<row;j++)
				{
					if(j!=k)
					{
						*(m+i*row+j)-=*(m+i*row+k)**(m+k*row+j);
					}
				}
			}
		}

		for (i=0;i<row;i++)
		{
			if(i!=k)
			{
				*(m+i*row+k)*=-(*(m+k*row+k));
			}
		}
	}

	int r;
	//恢复
	for (r=row-1;r>=0;r--)
	{
		if (js[r]!=r)
		{
			for (j=0;j<row;j++)
			{
				swap(*(m+r*row+j),*(m+js[r]*row+j));
			}
		}
		if (is[r]!=r)
		{
			for (i=0;i<row;i++)
			{
				swap(*(m+i*row+r),*(m+i*row+is[r]));
			}
		}
	}

	ptemp=matrix;
	pt=m;
	for (i=0;i<row;i++)
	{
		for (j=0;j<row;j++)
		{
			*ptemp=*pt;
			ptemp++;
			pt++;
		}
	}
	delete []is;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	delete []js;
	delete []m;

	return 0;
}

void RGB2HSL( double r, double g, double b, double& h, double& s, double& l )
{
	double v;	
	double m;	
	double vm;	
	double r2, g2, b2;	

	h = 0; 	
	s = 0;	
	l = 0;	
	v = max(r,g);
	v = max(v,b);	
	m = min(r,g);	
	m = min(m,b);	
	l = (m + v) / 2.0;	
	if (l <= 0.0) 
		return;

	vm = v - m;	
	s = vm;	
	if (s > 0.0) 

	{		
		s /= (l <= 0.5) ? (v + m ) : (2.0 - v - m) ;		
	} 

	else 

	{		
		return;		
	}

	r2 = (v - r) / vm;	
	g2 = (v - g) / vm;	
	b2 = (v - b) / vm;

	if (r == v) 
	{		
		h = (g == m ? 5.0 + b2 : 1.0 - g2);		
	}

	else if (g == v) 
	{
		h = (b == m ? 1.0 + r2 : 3.0 - b2);		
	}

	else 
	{		
		h = (r == m ? 3.0 + g2 : 5.0 - r2);		
	}

	h /= 6.0;
}

void HSL2RGB( double h, double s, double l, double& r, double& g, double& b )
{
	double v;

	r = l;   // default to gray
	g = l;
	b = l; 

	v = (l <= 0.5) ? (l * (1.0 + s)) : (l + s - l * s);

	if (v > 0) 

	{
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2; 

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = (int)h; 
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;

		switch (sextant) 
		{
		case 0: 
			r = v;
			g = mid1;
			b = m;
			break;
		case 1: 
			r = mid2;
			g = v;
			b = m; 
			break;
		case 2:
			r = m;
			g = v;
			b = mid1;
			break;
		case 3:
			r = m;
			g = mid2;
			b = v;
			break;
		case 4:
			r = mid1;
			g = m;
			b = v;
			break;
		case 5:
			r = v;
			g = m;
			b = mid2;
			break;
		}
	}
}

int FindArrMid( int arr[], int length )
{
	int max_count = 0, min_count = 0;
	int i = 0;
	int median = 0;

	for (i=0; i<length; i++)
		median += arr[i];
	median /= length;

	// 计算大于和小于median的数值
	while (1)
	{
		max_count=min_count=0;
		for (i=0; i<length; i++)
		{
			if (arr[i]>median)	max_count++;
			if (arr[i]<median)	min_count++;
		}

		if (max_count==min_count || max_count-min_count==1 || min_count-max_count==1)
			return median;
		else
			median = median*(1.0-double(min_count-max_count)/double(min_count));	//经验公式
	}

	return median;
}


double GetValueFromEquation(double x1, double x2, const double *a, int n)
{
	double dResult;
	switch(n)
	{
	case 1:
		dResult = a[0] + a[1]*x1 + a[2]*x2;
		break;
	case 2:
		dResult = a[0] + a[1]*x1 + a[2]*x2 
			+ a[3]*x1*x1 + a[4]*x1*x2 + a[5]*x2*x2;
		break;
	case 3:
		dResult = a[0] + a[1]*x1 + a[2]*x2 
			+ a[3]*x1*x1 + a[4]*x1*x2 + a[5]*x2*x2 
			+ a[6]*x1*x1*x1 + a[7]*x1*x1*x2 + a[8]*x1*x2*x2 + a[9]*x2*x2*x2;
		break;
	case 4:
		dResult = a[0] + a[1]*x1 + a[2]*x2 
			+ a[3]*x1*x1 + a[4]*x1*x2 + a[5]*x2*x2 
			+ a[6]*x1*x1*x1 + a[7]*x1*x1*x2 + a[8]*x1*x2*x2 + a[9]*x2*x2*x2
			+ a[10]*x1*x1*x1*x1 + a[11]*x1*x1*x1*x2 + a[12]*x1*x1*x2*x2 + a[13]*x1*x2*x2*x2 + a[14]*x2*x2*x2*x2;
		break;
	default:
		dResult = 1.0e+30;
		break;
	}
	return dResult;
}



double **DoubleTwoArrayAlloc(int r, int c)
{
	double *x, **y;
	int n;
	x=(double *)calloc(r*c, sizeof(double));
	y=(double **)calloc(r, sizeof(double *));
	for(n=0; n<r; ++n)
		y[n]=&x[c*n];
	return (y);
}

void DoubleTwoArrayFree(double **x)
{
	free(x[0]);
	free(x);
}

int PivotingGaussJordanEliminatingForLinearEquations(double *a, double *x, int n)
{
	int k, i, j, p;
	double c;
	double temp;
	int n1 = n+1;
	//int n_1 = n-1;

	for(k=0; k<n; k++)
	{
		c=a[k*n1+k];
		p = k;
		for(i = k+1; i<n; i++)
		{
			if(fabs(a[i*n1+k]) > c)
			{	
				c = a[i*n1+k];
				p = i;
			}
		}
		if(fabs(c) < 1e-9) return 1;

		for(j=k; j<n+1; j++)
		{
			temp = a[k*n1+j];
			a[k*n1+j] = a[p*n1+j];
			a[p*n1+j] = temp;
		}


		for(i=0; i<n; i++)
		{
			if(i==k) continue;
			for(j=k+1; j<n+1; j++)
				a[i*n1+j] = a[i*n1+j] - a[i*n1+k]*a[k*n1+j]/a[k*n1+k];
		}
	}

	for(i=0; i<n; i++)
	{
		x[i] = a[i*n1+n]/a[i*n1+i];
	}
	return 0;
}


int LeastSquares2Element(double *x1, double *x2, double *y, double *a, int m, int n)
{
	double **x, **xp, **A;
	double *AA;    
	int i, j, k;
	int iNumberOfEquations;
	double dTemp;

	//support the polynomial max number 4
	if(n<1 || n > 4)
	{
		return -2;
	}

	iNumberOfEquations = (n+2)*(n+1)/2;

	///////////////////////////////////////////////////////////////////////////////
	//allock memory for x
	if((x = DoubleTwoArrayAlloc(m, iNumberOfEquations)) == NULL)
	{
		return -1;
	}

	//allock memory for xp
	if((xp = DoubleTwoArrayAlloc(iNumberOfEquations, m)) == NULL)
	{
		DoubleTwoArrayFree(x);
		return -1;
	}

	//allock memory for A
	if((A = DoubleTwoArrayAlloc(iNumberOfEquations, iNumberOfEquations+1)) == NULL)
	{
		DoubleTwoArrayFree(x);
		DoubleTwoArrayFree(xp);
		return -1;
	}

	//allock memory for AA
	if((AA = (double *)calloc(iNumberOfEquations*(iNumberOfEquations+1), sizeof(double))) == NULL)
	{
		DoubleTwoArrayFree(x);
		DoubleTwoArrayFree(xp);
		DoubleTwoArrayFree(A);
		return -1;
	}

	/////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	//construct matrix x
	for(i=0; i<m; i++)
	{
		int index=0;
		for(j=0; j<=n; j++)
		{
			for(k=0; k<=j; k++)
			{
				x[i][index] = 1*pow(x1[i], j-k)*pow(x2[i], k); 
				index++;
			}
		}
	}

	//construct matrix xp
	for(i=0; i<m; i++)
	{
		for(j=0; j<iNumberOfEquations; j++)
		{
			xp[j][i] = x[i][j];
		}
	}
	//////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////
	//construct matrix A=xp*x
	for(i=0; i<iNumberOfEquations; i++)
	{
		for(j=0; j<iNumberOfEquations; j++)
		{
			dTemp = 0.0;
			for(k=0; k<m; k++)
			{
				dTemp += (xp[i][k]*x[k][j]);
			}
			A[i][j] = dTemp;
		}
	}

	for(i=0; i<iNumberOfEquations; i++)
	{
		dTemp = 0.0;
		for(j=0; j<m; j++)
			dTemp += xp[i][j]*y[j];
		A[i][iNumberOfEquations] = dTemp; 
	}

	k=0;
	for(i=0; i<iNumberOfEquations; i++)
	{
		for(j=0; j<=iNumberOfEquations; j++)
		{
			AA[k] = A[i][j];
			k++;
		}
	}


	//release memory
	DoubleTwoArrayFree(x);
	DoubleTwoArrayFree(xp);
	DoubleTwoArrayFree(A);

	//calculate
	if(0 == PivotingGaussJordanEliminatingForLinearEquations(AA, a, iNumberOfEquations))
	{
		free(AA);
		return 0;  
	}
	else 
	{
		free(AA);
		return -3;  
	}
}


//////////////////////////////////////////////////////////
// This program execute Lagrange interpolation  //
// Input parameter:                             //
//  t: x coordinate of interpolated point       //
//  t0: x coordinate of e0                      //
//  dt: interval of known point                 //
//  e0,e1,e2: y coordinate of known four point  //
// Output parameter:                            //
//  fm: y coordinate of interpolated point      //
//  该函数要求时间间隔dt是相同的                                                             // 
/////////////////////////////////////////////////////////
double lag(double t, double t0, double dt, double e0, double e1,double e2)
{
	double f0,f1,f2,pp,q1,q2,q3,fm,pp1,pp2;
	f0=e0;
	f1=e1;
	f2=e2;

	pp=(t-t0-dt)/dt;

	pp1=0.5*pp;
	pp2=pp*pp;
	q1=0.5*pp2-pp1;
	q2=1.0-pp2;
	q3=0.5*pp2+pp1;
	fm=q1*f0+q2*f1+q3*f2;
	return(fm);
}

////////////////////////////////////////////////////////////////////////
//	函数：lag2                                                 //
//	二次的拉格朗日插值函数                                                                                                       //
//  输入参数：                                                                                                                               //
//  x:  需要插值点的自变量                                                                   //
//  x0、x1、x2:  已知的三个点的自变量                                         //
//               y0、y1、y2:  已知的三个点的因变量                                         //
//          返回值：                                                                                                                                  //
//             插值计算得到的自变量x处的因变量值                                                                       // 
//                                                                                                                                                             //
//          注：该函数不强求自变量间隔相同，这是与上一个插值函数的不同之处             //
//                  但要求自变量不能有重合                                                                                            //
//           汪红强于2003年9月1日                                                                                                        //
///////////////////////////////////////////////////////////////////////
double lag2(double x, double x0, double x1, double x2, double y0, double y1, double y2)
{
	double w0, w1, w2;

	w0 = (x-x1)*(x-x2)/((x0-x1)*(x0-x2));
	w1 = (x-x0)*(x-x2)/((x1-x0)*(x1-x2));
	w2 = (x-x0)*(x-x1)/((x2-x0)*(x2-x1));

	double y = w0*y0 + w1*y1 + w2*y2;
	return y;
}