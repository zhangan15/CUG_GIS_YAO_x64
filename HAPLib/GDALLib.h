// GDALAlgoLib.h: interface for the GDALAlgoLib class.
//
//////////////////////////////////////////////////////////////////////


#include "HAPLIB.h"
#include "GDALFileManager.h"
#include "HAPBEGTBase.h"

USING_NAMESPACE(HAPLIB)

/****************************************************************
* ��γ�Ⱥ����������ת��
****************************************************************/
HIPAPI int World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char *lpszSpatialReferrence);
HIPAPI int Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char *lpszSpatialReferrence);

/****************************************************************
* �����������������ת��
****************************************************************/
HIPAPI int Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform);
HIPAPI int Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform);


//��ȡƴ�Ϻ�ͼ����ĸ��ǵ�����
/*********************************************************************************
* poDatase[] ����ͼ��ָ��
* nCount ����ͼ�����
* nIndex ��׼Ӱ��һ��Ϊ0
* x1, y1 ���ͼ�����Ͻǵ�������
* x2, y2 ���ͼ�����½ǵ�������
* scaleX, scaleY xy�����ϵı�����
* mosaic_width, mosaic_height ���ͼ��Ŀ�Ⱥ͸߶�
*********************************************************************************/
//HIPAPI void GetMosaicRect(GDALDataset** poDataset, int nCount, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height);
HIPAPI void GetMosaicRect(CGDALFileManager** ppImg, int nCount, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height);
HIPAPI void GetMosaicRect(CGDALFileManager* pMgr1, CGDALFileManager* pMgr2, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height);

//�ж�һ����������ڲ���ͼ����
//��ͼ���ڷ���true��ͬʱ�������������xy�У����򷵻�0��xyΪ-1
/******************************************************************
* gX, gY ��������
* poDataset ң��Ӱ��
* x, y ң��Ӱ�����꣬��0��ʼ
******************************************************************/
//HIPAPI bool IsGroudXYInImg(double gX, double gY, GDALDataset* poDataset, double& x, double& y);
HIPAPI bool IsGroudXYInImg(double gX, double gY, CGDALFileManager* pImg, double& x, double& y);

//ͼ����˲�����
//HIPAPI template<class TT> double PointConvolution(HAPBEGTBase* pImgIO, int col, int row, int band, float* pMask, int masksize, float parameter = 1);
	


