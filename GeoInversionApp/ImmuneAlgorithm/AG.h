#ifndef AG_H
#define AG_H
#include <stdio.h>
class ag//抗原类
{
public:
	int rows;
	int cols;
	ag(int n,int m);//构造函数
	ag(int n);
	ag(double **a,int n,int m);//构造函数
	ag(double *a,int n);//构造函数
	double *myAg1;//一维数组首地址
	double **myAg2;//二维数组首地址

public:
	~ag();
	ag();
	void setAg(double **a,int row,int col);
};
#endif