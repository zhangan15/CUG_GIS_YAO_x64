#ifndef AIS_H
#define AIS_H
#include <stdio.h>
#include <math.h>
class ais//抗体类
{
public:
	ais(int n=10);//构造函数
	~ais();
	double& operator[](int i);//重载[]，符合编码习惯
	ais&operator =(const ais &rhs);//重载=号
	void CalculateAf(double **ag,int rows,int cols);//计算亲和力函数
	double GetAf();//获得亲和力
	void Variation(double Pg);//变异函数
	void Print();//输出每个基因位
	int GetClasses(double *NewAg,int cows,int classes);
	double Verification(double **ag,int rows,int cols,int classes);//验证
	double GetVerification();
	int size;//数组长度
protected:
	double *ab;//数组首地址
	double af;//亲和力
	double Ver;//精度
};
#endif