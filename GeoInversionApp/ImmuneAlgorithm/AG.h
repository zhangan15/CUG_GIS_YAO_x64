#ifndef AG_H
#define AG_H
#include <stdio.h>
class ag//��ԭ��
{
public:
	int rows;
	int cols;
	ag(int n,int m);//���캯��
	ag(int n);
	ag(double **a,int n,int m);//���캯��
	ag(double *a,int n);//���캯��
	double *myAg1;//һά�����׵�ַ
	double **myAg2;//��ά�����׵�ַ

public:
	~ag();
	ag();
	void setAg(double **a,int row,int col);
};
#endif