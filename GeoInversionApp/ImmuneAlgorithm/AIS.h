#ifndef AIS_H
#define AIS_H
#include <stdio.h>
#include <math.h>
class ais//������
{
public:
	ais(int n=10);//���캯��
	~ais();
	double& operator[](int i);//����[]�����ϱ���ϰ��
	ais&operator =(const ais &rhs);//����=��
	void CalculateAf(double **ag,int rows,int cols);//�����׺�������
	double GetAf();//����׺���
	void Variation(double Pg);//���캯��
	void Print();//���ÿ������λ
	int GetClasses(double *NewAg,int cows,int classes);
	double Verification(double **ag,int rows,int cols,int classes);//��֤
	double GetVerification();
	int size;//���鳤��
protected:
	double *ab;//�����׵�ַ
	double af;//�׺���
	double Ver;//����
};
#endif