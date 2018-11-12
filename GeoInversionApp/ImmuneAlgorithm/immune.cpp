#include "immune.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "../global_include.h"
using namespace std;
bool PgSelectM(double Pg)//���̻���
{
	if((double)rand()/RAND_MAX<=Pg)
		return true;
	else
		return false;
}
void Exchange(double Pg,ais *Fais,int ABNUMBER)//��������,������ڽ����������Ӧ����λ���·��Ľ���
{
	double Temp;
	for (int i=0;i<ABNUMBER;i++)
	{
		for (int j=0;j<Fais[i].size;j++)
		{
			if(PgSelectM(Pg))
			{
				if (i==ABNUMBER-1)//���һ�鿹ԭ���һ������
				{
					Temp=Fais[i][j];
					Fais[i][j]=Fais[0][j];
					Fais[0][j]=Temp;
				}
				else
				{
					Temp=Fais[i][j];
					Fais[i][j]=Fais[i+1][j];
					Fais[i+1][j]=Temp;
				}

			}

		}
	}
}
void AbSelect(ais *Fais,ais *Sais,int ABNUMBER)//����ѡ�񣬴����ѡ���׺�����Ŀ���
{
	double a,b;
	for (int i=0;i<ABNUMBER;i++)
	{
		a=Sais[i].GetAf();
		b=Fais[i].GetAf();
		if (Sais[i].GetAf()>Fais[i].GetAf()/*PgSelectM(Sais[i].GetAf()/(Fais[i].GetAf()+Sais[i].GetAf()))*/)//ѡ�����Ϊ���Ӵ��׺Ͷ�/(�����׺Ͷ�+�Ӵ��׺Ͷ�)������ѡ�����򣬸��������Ӵ�
		{
			Fais[i]=Sais[i];
			/*cout<<1<<endl;*/
		}
	}
}
int ClassifyHerd(ais *Fais,int ABNUMBER)
{
	double Vermax=0;
	int Flat;
	for (int i=0;i<ABNUMBER;i++)
	{
		if (Fais[i].GetVerification()>Vermax)
			Flat=i;
	}
	return Flat;
}
void ClassifySoft(ais *Fais,double *NewAg,int cows,int classes,double *result,int ABNUMBER)
{
	/*result=new double[classes];*/
	for (int i=0;i<classes;i++)
	{
		result[i]=0;
	}
	for (int i=0;i<ABNUMBER;i++)
	{
		result[Fais[i].GetClasses(NewAg,cows,classes)-1]++;
	}

	for (int i=0;i<classes;i++)
	{
		result[i] /= ABNUMBER;
	}
}
void GetClassifier(ais* Fais,ag& myag,int ABNUMBER,int NUMJ,int NUMAG ,int AGNUMBER,double VARIATIONPG,double EXCHANGEPG,int CLASSES,int CycleIndex)
{
	srand((unsigned)time(NULL));
	ais *Sais;
	Sais=new ais[ABNUMBER];
	for (int i=0;i<ABNUMBER;i++)
	{
		Fais[i]=ais((AGNUMBER-1)*NUMJ+1);
		Sais[i]=ais((AGNUMBER-1)*NUMJ+1);

		/*Fais[i].Print();*/
		/*cout<<endl;*/
		/*Sais[i].Print();*/
	}

#pragma omp parallel for num_threads(MAX_THREAD_NUM),schedule(dynamic)
	for(int n=0;n<CycleIndex;n++)
	{
		//cout<<"AG Starting No. "<<n+1<<" Training / "<<CycleIndex<<". "<<endl;
		global_ilog(QString("AG Starting No. %1 Training / %2...").arg(n+1).arg(CycleIndex));
		for (int i=0;i<ABNUMBER;i++)
		{
			Sais[i]=Fais[i];
			Sais[i].Variation(VARIATIONPG);
		}	
		Exchange(EXCHANGEPG,Sais,ABNUMBER);
		for (int i=0;i<ABNUMBER;i++)
		{
			Fais[i].CalculateAf(myag.myAg2,NUMAG,AGNUMBER);
			Sais[i].CalculateAf(myag.myAg2,NUMAG,AGNUMBER);
		}
		AbSelect(Fais,Sais,ABNUMBER);
	/*	for (int i=0;i<ABNUMBER;i++)
		{
			Fais[i].CalculateAf(myag.myAg2,NUMAG,AGNUMBER);
			cout<<Fais[i].GetAf();
			cout<<endl;
		}
		cout<<endl;*/
		global_ilog(QString("AG Finished No. %1 Training / %2.").arg(n+1).arg(CycleIndex));
	}
	for (int i=0;i<ABNUMBER;i++)
	{
		//Fais[i].CalculateAf(myag.myAg2,NUMAG,AGNUMBER);
		//cout<<Fais[i].GetAf();
		//cout<<endl;
		cout<<"No. "<<i+1<<" Accu = "<<Fais[i].Verification(myag.myAg2,NUMAG,AGNUMBER,CLASSES);
		cout<<endl;
	}
}
