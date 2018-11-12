// AlgoLib.cpp : Defines the exported functions for the DLL application.
//

#include "include_global.h"
#include "ImgAlgoLib.h"
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
//

BOOL Func_HistgramEqulizeEnhance (double *pfHist, unsigned char *pucLUT, bool StatEdgeValue)
{
	double hist[256];
	double max1,min1;
	long k;

	memcpy (hist, pfHist, sizeof(double) * 256);
	if (!StatEdgeValue) //��ͳ�Ʊ�Ե�Ҷ�ֵ
	{
		hist[0] = 0;
		hist[255] = 0;
	}

	//pucLUT[0] = 0;

//	hist[0] = 0;
	try
	{
		//ͳ���ۼ�ֱ��ͼ
		for(k=1;k<256;k++) 
			hist[k] = hist[k-1]+hist[k];

		//do  equalization
//		if(hist[255] != 1)
//			hist[255] = 1;

		for(k=0;k<256;k++)
		{
			hist[k] = 255.0*hist[k]/hist[255];
		}
		// linear enhancement
		max1=0;
		min1=9000;

		for(k=0;k<256;k++)
		{
			if((long)hist[k]>max1)
				max1 = hist[k];
			if((long)hist[k]<min1)
				min1 = hist[k];
		}

		if(max1 == min1)
		{
			for (k=0;k<256;k++)
				pucLUT[k] = (unsigned char)k;
			return FALSE;
		}

		double dValue;
		for(k=0;k<256;k++)
		{
			dValue = 255.0*(hist[k]-min1)/(max1-min1);
			if (dValue>255)
			{
				dValue = 255;
			}
			pucLUT[k] = (unsigned char)(dValue+0.5); //ȷ��HAP�߾��ȣ�����������
//			pucLUT[k] = (unsigned char)(pucLUT[k]*(float)(max1-min1)+min1+0.5); //Hps�ķ���������ȷ��
		}
	}
	catch (...) 
	{
		for (k=0;k<256;k++)
			pucLUT[k] = (unsigned char)k;
	}
	return TRUE;
}

// ����ֱ��ͼ�涨������ [1/12/2012 liuxiang]
/************************************************************************/
/* ֱ��ͼ�涨�����Ĵ��룬SML��ӳ�䷽����
float* pStandard���涨����׼ֱ��ͼ
float* pSrc��ԭֱ��ͼ
BYTE* pOutMap�����ֱ��ͼ,Ч�����ѣ���ʱ���ٲ�⡣
*/
/************************************************************************/
BOOL StandardlizeHis(float* pStandard,float* pSrc,BYTE* pOutMap)
{
	float bMap1[256],bMap2[256];
	memset(bMap1,0,sizeof(float)*256);
	memset(bMap2,0,sizeof(float)*256);
	float lTemp1,lTemp2;
	int i,j;
	for(i=0;i<256;i++)//�����ۼ�ֱ��ͼbMap1,bMap2
	{
		lTemp1=0;
		lTemp2=0;
		for(j=0;j<=i;j++)
		{
			lTemp1+=pStandard[j];
			lTemp2+=pSrc[j];
		}
		bMap1[i]=lTemp1;
		bMap2[i]=lTemp2;
	}
	float temp;
	float min;
	for(i=0;i<256;i++)
	{
		min=256;
		for(j=0;j<256;j++)
		{
			temp=bMap1[i]-bMap2[j];
			if(temp<0)
			{
				temp=(-1)*temp;
			}
			if(temp<min)
			{
				pOutMap[i]=(BYTE)j;
				min=temp;
			}
		}
	}

	return TRUE;
}

/************************************************************************/
/* ֱ��ͼ�涨�����Ĵ��룬GML��ӳ�䷽����
float* pStandard���涨����׼ֱ��ͼ
float* pSrc��ԭֱ��ͼ
unsigned* pOutMap�����ֱ��ͼӳ���ϵ,pOutMap[i]Ϊ���ֵ
int GrayScale: �ҽ׵ȼ�����Ϊ8, 64, 256, 512, 1024
Ч����������ƽ̨������ǿ���ܲ��ԡ�
*/
/************************************************************************/
BOOL StandardlizeHisGML(float* pStandard,float* pSrc,BYTE* pOutMap, int GrayScale)
{
	float* bMapSrc = new float[GrayScale];
	float* bMapStd = new float[GrayScale];
	memset(bMapSrc,0,sizeof(float)*GrayScale);
	memset(bMapStd,0,sizeof(float)*GrayScale); //�ۼ�ֱ��ͼ
	memset(pOutMap,0,sizeof(BYTE)*GrayScale); //�Ҷ�ӳ���

	float lTemp1,lTemp2;
	int i,j;
	for(i=0;i<GrayScale;i++)//�����ۼ�ֱ��ͼbMap1,bMap2
	{
		lTemp1=0;
		lTemp2=0;
		for(j=0;j<=i;j++)
		{
			lTemp1+=pStandard[j];
			lTemp2+=pSrc[j];
		}
		bMapStd[i]=lTemp1;
		bMapSrc[i]=lTemp2;
	}

	float temp,min;
	int l,ISrc0 = 0,ISrc1=0;
	for(l=0;l<GrayScale;l++)
	{
		if(ISrc0==0)
			min = bMapStd[l];
		else
			min = fabs(bMapStd[l]-bMapSrc[ISrc0-1]);//����ԭʼֱ��ͼΪ0�����
		for(j=ISrc0;j<GrayScale;j++)
		{
			temp = fabs(bMapSrc[j]-bMapStd[l]);
			if(temp<min)//&&bMapStd[j]>bMapStd[j-1])
			{
				ISrc1 = j+1;
				min = temp;
			}
		}
		if(ISrc1>ISrc0)
		{
			for(i=ISrc0;i<ISrc1;i++)
				pOutMap[i] = (BYTE)l;
			ISrc0 = ISrc1;
			//min = bMapStd[Il0];
		}
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Filter_IMG::Filter_IMG()
{

}
//  [3/22/2012 liuxiang]�麯�����붨�壬���򱨴�link2001��
Filter_IMG::~Filter_IMG()
{

}

/************************************************************************/
/* ͨ���������򣬷����м���ֵ                                                                     */
/************************************************************************/
//template<class TT> TT Filter_IMG::MedianF(TT* pData, int Length)//����һά���飬����������ֵ
int Filter_IMG::MedianF(int* pData, int Length)//����һά���飬����������ֵ
{
	int temp;
	for(int i=0;i<Length;i++)
	{
		for(int j=i;j<Length-1;j++)
		{
			if(pData[j] > pData[j+1])
			{
				temp = pData[j+1];
				pData[j+1] = pData[j];
				pData[j] = temp;
			}
		}
	}
	return pData[Length/2];
}