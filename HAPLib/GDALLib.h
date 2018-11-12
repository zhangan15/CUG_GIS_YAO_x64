// GDALAlgoLib.h: interface for the GDALAlgoLib class.
//
//////////////////////////////////////////////////////////////////////


#include "HAPLIB.h"
#include "GDALFileManager.h"
#include "HAPBEGTBase.h"

USING_NAMESPACE(HAPLIB)

/****************************************************************
* 经纬度和像素坐标的转变
****************************************************************/
HIPAPI int World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char *lpszSpatialReferrence);
HIPAPI int Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char *lpszSpatialReferrence);

/****************************************************************
* 大地坐标和像素坐标的转变
****************************************************************/
HIPAPI int Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform);
HIPAPI int Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform);


//获取拼合后图像的四个角点坐标
/*********************************************************************************
* poDatase[] 输入图像指针
* nCount 输入图像个数
* nIndex 标准影像，一般为0
* x1, y1 输出图像左上角地理坐标
* x2, y2 输出图像右下角地理坐标
* scaleX, scaleY xy方向上的比例尺
* mosaic_width, mosaic_height 输出图像的宽度和高度
*********************************************************************************/
//HIPAPI void GetMosaicRect(GDALDataset** poDataset, int nCount, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height);
HIPAPI void GetMosaicRect(CGDALFileManager** ppImg, int nCount, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height);
HIPAPI void GetMosaicRect(CGDALFileManager* pMgr1, CGDALFileManager* pMgr2, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height);

//判断一个大地坐标在不在图像内
//在图像内返回true，同时将像素坐标存入xy中，否则返回0，xy为-1
/******************************************************************
* gX, gY 地面坐标
* poDataset 遥感影像
* x, y 遥感影像坐标，从0开始
******************************************************************/
//HIPAPI bool IsGroudXYInImg(double gX, double gY, GDALDataset* poDataset, double& x, double& y);
HIPAPI bool IsGroudXYInImg(double gX, double gY, CGDALFileManager* pImg, double& x, double& y);

//图像的滤波函数
//HIPAPI template<class TT> double PointConvolution(HAPBEGTBase* pImgIO, int col, int row, int band, float* pMask, int masksize, float parameter = 1);
	


