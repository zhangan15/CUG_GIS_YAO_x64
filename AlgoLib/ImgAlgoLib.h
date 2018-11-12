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
/*函数功能：图像直方图均衡
float *pfHist: 输入频率直方图数据指针（概率密度，也可以是像素个数直方图）(256大小)
unsigned char *pucLUT：输出灰度查找表， 输出 LUT 数据指针  (256大小)
bool StatEdgeValue = true : 是否统计边缘值，即0,255灰度值。默认为统计
*/
/************************************************************************/
ALGOLIB_API BOOL Func_HistgramEqulizeEnhance (double *pfHist, unsigned char *pucLUT, bool StatEdgeValue = true);

/************************************************************************/
/* 直方图规定化核心代码，GML组映射方法。
float* pStandard：规定化标准直方图
float* pSrc：原直方图
unsigned* pOutMap：输出直方图映射关系,pOutMap[i]为输出值
int GrayScale: 灰阶等级，可为8, 64, 256, 512, 1024
效果不错，可用平台对数增强功能测试。
*/
/************************************************************************/
ALGOLIB_API BOOL StandardlizeHisGML(float* pStandard,float* pSrc,BYTE* pOutMap, int GrayScale = 256);

/************************************************************************/
/* 直方图规定化核心代码，SML单映射方法。
float* pStandard：规定化标准直方图
float* pSrc：原直方图
BYTE* pOutMap：输出直方图,效果不佳，有时间再测测。
*/
/************************************************************************/
ALGOLIB_API BOOL StandardlizeHis(float* pStandard,float* pSrc,BYTE* pOutMap);



class ALGOLIB_API Filter_IMG
{
public:
	Filter_IMG();
	virtual ~Filter_IMG();

public:
//	template<class TT> static TT MedianF(TT* pData, int Length);//输入一维数组，返回数据中值
	static int MedianF(int* pData, int Length);//输入一维数组，返回数据中值
};

#endif  /*IMG_ALGOLIB_H_*/