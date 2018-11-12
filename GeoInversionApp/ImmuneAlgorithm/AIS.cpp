#include<iostream>
#include "AIS.h"
using namespace std;
#define MUL 1
 double Pow(double x,int n)//乘方
{
	if (n>0)
	{
		for (int i=1;i<n;i++)
		{
			x*=x;
		}
	}
	if (n==0)
		return 1;
	if (n<0)
	{
		for (int i=1;i<n;i++)
		{
			x*=x;
		}
		return 1/x;
	}
}
bool PgSelect(double Pg)//轮盘机制
{
	if((double)rand()/RAND_MAX<=Pg)
		return true;
	else
		return false;
}
ais::ais(int n)
{
	int temp;
	ab=new double[n];
	for (int i=0;i<n;i++)
	{
		ab[i]=i+1;
		ab[i]=MUL*(double)rand()/RAND_MAX;
		/*temp=10*(double)rand()/RAND_MAX;
		ab[i]=temp;*/
	}
	size=n;
	af=0;
}

ais::~ais()
{
	if (ab!=NULL)
	{
		delete []ab;
	}
}


double &ais::operator[](int n)
{
	if(n<0||n>size-1)
	{
		cout<<"下标溢出！"<<endl;
		exit(1);
	}
	return ab[n];
}
void ais::Print()
{
	for (int i=0;i<size;i++)
	{
		cout<<ab[i]<<"    ";
	}
	cout<<endl;
}
ais& ais::operator =(const ais &rhs)
{
	int n=rhs.size;//如果本对象中的数组大小和rhs不同，则删除数组原有的内存，然后重新分配
	af=rhs.af;
	if (size!=n)
	{
		delete []ab;
		ab=new double[n];
		size=n;
	}
	//从rhs向本对象复制元素
	double* destptr=ab;
	double* srcptr=rhs.ab;
	while(n--)
	{
		*destptr++=*srcptr++;//只复制数值，地址不复制
	}
	return *this;//返回当前对象的引用
}
void ais::CalculateAf(double **ag,int rows,int cols)
{
	int NumX=(size-1)/(cols-1);//阶数
	double TempJ=1;
	af=0;
	for(int i=0;i<(int)rows*0.6;i++/*i+=2*rand()/RAND_MAX*/)//计算亲和力时，随机1或者2为步长，随机选取抗原进行计算，只用抗原的前60%做模型计算，后40%做验证。
	{
		/*af+=ab[0];*/
		TempJ=ab[0];
		for(int j=0;j<cols-1;j++)
		{
			for (int n=1;n<=NumX;n++)
			{	
				double a=ab[n+j*NumX];
				double b=ag[i][j];
				TempJ+=(ab[n+j*NumX]*pow(ag[i][j],n));
				/*af+=(ab[n+j*NumX]*pow(ag[i][j],n));*/
			}
		}

		TempJ=sqrt(pow((ag[i][cols-1]-TempJ),2));
		af+=TempJ;
	}
	af=1.0/(1+af);
}
double ais::GetAf()
{
	return af;
}
void ais::Variation(double Pg)
{
	for (int i=0;i<size;i++)
	{
		if(PgSelect(Pg))
			ab[i]=MUL*(double)rand()/RAND_MAX;
	}
}
int ais::GetClasses(double *NewAg,int cows,int classes)
{
	int NumX=(size-1)/cows;//阶数
	double Distance=9999999;
	double Temp=ab[0];
	int myclass;
	for(int i=0;i<cows;i++)
	{
		for (int n=1;n<=NumX;n++)
		{	
			//double a=ab[n+j*NumX];
			//double b=ag[i][j];
			Temp+=(ab[n+i*NumX]*pow(NewAg[i],n));
			/*af+=(ab[n+j*NumX]*pow(ag[i][j],n));*/
		}
	}
	for (int i=1;i<=classes;i++)
	{
		if(sqrt((Temp-i)*(Temp-i))<Distance)
			myclass=i;
		Distance=sqrt((Temp-i)*(Temp-i));
	}
	return myclass;
}
double ais::Verification(double **ag,int rows,int cols,int classes)
{
	int Flat=0;
	int n=0;
	double a;
	double b;
	for (int i=0/*+(int)rows*0.6*/;i<rows;i++)
	{
		n++;
		a=ag[i][cols-1];
		b=GetClasses(ag[i],cols-1,classes);
		if (ag[i][cols-1]==GetClasses(ag[i],cols-1,classes))
		{
			Flat++;
		}
	}
	Ver=(double)Flat/n;
	return Ver;
}
double ais::GetVerification()
{
	return Ver;
}

