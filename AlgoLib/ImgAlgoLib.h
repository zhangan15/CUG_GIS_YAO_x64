// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ALGOLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ALGOLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

//ImgAlgoLib.h

#ifndef IMG_ALGOLIB_H_
#define IMG_ALGOLIB_H_


#include "Comlib.h"
//// This class is exported from the AlgoLib.dll
//class ALGOLIB_API CAlgoLib {
//public:
//	CAlgoLib(void);
//	// TODO: add your methods here.
//};
//
//extern ALGOLIB_API int nAlgoLib;
//
//ALGOLIB_API int fnAlgoLib(void);


//
/************************************************************************/
/*�������ܣ�ͼ��ֱ��ͼ����
float *pfHist: ����Ƶ��ֱ��ͼ����ָ�루�����ܶȣ�Ҳ���������ظ���ֱ��ͼ��(256��С)
unsigned char *pucLUT������ҶȲ��ұ� ��� LUT ����ָ��  (256��С)
bool StatEdgeValue = true : �Ƿ�ͳ�Ʊ�Եֵ����0,255�Ҷ�ֵ��Ĭ��Ϊͳ��
*/
/************************************************************************/
ALGOLIB_API BOOL Func_HistgramEqulizeEnhance (double *pfHist, unsigned char *pucLUT, bool StatEdgeValue = true);

/************************************************************************/
/* ֱ��ͼ�涨�����Ĵ��룬GML��ӳ�䷽����
float* pStandard���涨����׼ֱ��ͼ
float* pSrc��ԭֱ��ͼ
unsigned* pOutMap�����ֱ��ͼӳ���ϵ,pOutMap[i]Ϊ���ֵ
int GrayScale: �ҽ׵ȼ�����Ϊ8, 64, 256, 512, 1024
Ч����������ƽ̨������ǿ���ܲ��ԡ�
*/
/************************************************************************/
ALGOLIB_API BOOL StandardlizeHisGML(float* pStandard,float* pSrc,BYTE* pOutMap, int GrayScale = 256);

/************************************************************************/
/* ֱ��ͼ�涨�����Ĵ��룬SML��ӳ�䷽����
float* pStandard���涨����׼ֱ��ͼ
float* pSrc��ԭֱ��ͼ
BYTE* pOutMap�����ֱ��ͼ,Ч�����ѣ���ʱ���ٲ�⡣
*/
/************************************************************************/
ALGOLIB_API BOOL StandardlizeHis(float* pStandard,float* pSrc,BYTE* pOutMap);



class ALGOLIB_API Filter_IMG
{
public:
	Filter_IMG();
	virtual ~Filter_IMG();

public:
//	template<class TT> static TT MedianF(TT* pData, int Length);//����һά���飬����������ֵ
	static int MedianF(int* pData, int Length);//����һά���飬����������ֵ
};

#endif  /*IMG_ALGOLIB_H_*/