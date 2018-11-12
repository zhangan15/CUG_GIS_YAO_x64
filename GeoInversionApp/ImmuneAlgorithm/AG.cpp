#include<iostream>
#include "AG.h"
using namespace std;
ag::ag(int n)//一维构造函数
{
	rows=n;
	cols=0;
	myAg1=new double [n];
	myAg2=NULL;
	for (int i=0;i<n;i++)
	{
		myAg1[i]=i*10;
		cout<<myAg1[i]<<"    ";
	}
	cout<<endl;
}
ag::ag(double *a,int n)//一维构造函数
{
	rows=n;
	cols=0;
	myAg1=a;
	myAg2=NULL;
	for (int i=0;i<n;i++)
	{
		myAg1[i]=a[i];
		cout<<myAg1[i]<<"    ";
	}
	cout<<endl;
}
ag::ag(int n,int m)//二维构造函数
{
	rows=n;
	cols=m;
	myAg1 = NULL;
	myAg2=new double *[n];
	for (int i=0;i<n;i++)
	{
		myAg2[i]=new double[m];
	}
	for (int i=0;i<n;i++)
	{
		for (int j=0;j<m;j++)
		{
			myAg2[i][j]=i*10+j;
			//cout<<myAg2[i][j]<<"    ";
		}
		//cout<<endl;
	}
	//myAg2[0][3]=152;
	//myAg2[1][3]=33092;
	//myAg2[2][3]=211831;
}
ag::ag(double **a,int n,int m)//二维构造函数
{
	rows=n;
	cols=m;
	myAg1 = NULL;
	myAg2=new double *[n];
	for (int i=0;i<n;i++)
	{
		myAg2[i]=new double[m];
	}
	for (int i=0;i<n;i++)
	{
		for (int j=0;j<m;j++)
		{
			myAg2[i][j]=*((double*)a+i*m+j);
			/*cout<<*((double*)a+i*m+j)<<"\t";*/
		}
	/*	cout<<endl;*/
	}
}

ag::ag()
{
	rows = 0;
	cols = 0;
	myAg1 = NULL;
	myAg2 = NULL;
}

ag::~ag()
{
	if (myAg1!=NULL)
		delete []myAg1;
	myAg1 = NULL;

	if (myAg2!=NULL)
	{
		for (int i=0; i<rows; i++)
		{
			delete[] myAg2[i];
		}
		delete []myAg2;
		myAg2 = NULL;
	}

}

void ag::setAg( double **a,int row,int col )
{
	if (myAg1!=NULL)
		delete []myAg1;
	myAg1 = NULL;

	if (myAg2!=NULL)
	{
		for (int i=0; i<rows; i++)
		{
			delete[] myAg2[i];
		}
		delete []myAg2;
		myAg2 = NULL;
	}

	rows=row;
	cols=col;
	myAg1 = NULL;
	myAg2=new double *[rows];

	cout<<"loc 12"<<endl;
	for (int i=0;i<rows;i++)
	{
		myAg2[i]=new double[cols];
	}

	//cout<<"loc 125"<<endl;
	for (int i=0;i<rows;i++)
	{
		for (int j=0;j<cols;j++)
		{
			myAg2[i][j]=a[i][j];//*((double*)a+i*cols+j);
			/*cout<<*((double*)a+i*m+j)<<"\t";*/
		}
		/*	cout<<endl;*/
	}

	//cout<<"loc 13"<<endl;
}
