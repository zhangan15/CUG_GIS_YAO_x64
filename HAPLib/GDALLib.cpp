// GDALAlgoLib.cpp: implementation of the GDALAlgoLib class.
//
//////////////////////////////////////////////////////////////////////

#include "GDALLib.h"
#include <math.h>
#include <vector>

#include "gdal_priv.h"
#include "ogr_core.h"
#include "ogr_spatialref.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char *lpszSpatialReferrence)
{
	OGRSpatialReferenceH   srcSR = OSRNewSpatialReference(lpszSpatialReferrence); // ground
	OGRSpatialReferenceH   latLongSR=OSRCloneGeogCS(srcSR);  //geo
	
	OGRCoordinateTransformationH poTransform =OCTNewCoordinateTransformation(latLongSR, srcSR);
	double height;
	OCTTransform(poTransform,1, &lon, &lat, &height);
	
	double  adfInverseGeoTransform[6];
	
	GDALInvGeoTransform(geoTransform, adfInverseGeoTransform);
	GDALApplyGeoTransform(adfInverseGeoTransform, lon,lat, x, y);
	
	
	OSRDestroySpatialReference(srcSR);
	OSRDestroySpatialReference(latLongSR);
	OCTDestroyCoordinateTransformation(poTransform);
	
	return 0;
}

int Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char *lpszSpatialReferrence)
{
	GDALApplyGeoTransform(geoTransform, x, y, lon, lat);
	
	OGRSpatialReferenceH   srcSR = OSRNewSpatialReference(lpszSpatialReferrence); // ground
	OGRSpatialReferenceH   latLongSR=OSRCloneGeogCS(srcSR);  //geo
	
	OGRCoordinateTransformationH poTransform =OCTNewCoordinateTransformation(srcSR, latLongSR);
	double height;
	OCTTransform(poTransform,1, lon, lat, &height);
	
	OSRDestroySpatialReference(srcSR);
	OSRDestroySpatialReference(latLongSR);
	OCTDestroyCoordinateTransformation(poTransform);
	
	return 0;
}

int Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform)
{
	GDALApplyGeoTransform(geoTransform, x, y, pX, pY);
	
	return 0;
}

int Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform)
{
	double  adfInverseGeoTransform[6];
	
	GDALInvGeoTransform(geoTransform, adfInverseGeoTransform);
	GDALApplyGeoTransform(adfInverseGeoTransform, X, Y, px, py);
	
	return 0;
}

// void GetMosaicRect( GDALDataset** poDataset, int nCount, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height )
// {
// 	vector<double> vX,vY;
// 	vX.clear(); vY.clear();
// 
// 	int i=0;
// 	double _x1, _y1, _x2, _y2;
// 	double pGeoTransform[6];
// 	poDataset[nIndex]->GetGeoTransform(pGeoTransform);
// 	scaleX = pGeoTransform[1];
// 	scaleY = pGeoTransform[5];
// 
// 	//计算4个点坐标
// 	for (i=0; i<nCount; i++)
// 	{
// 		poDataset[i]->GetGeoTransform(pGeoTransform);
// 		Pixel2Ground(0, 0, &_x1, &_y1, pGeoTransform);
// 		Pixel2Ground(poDataset[i]->GetRasterXSize(), poDataset[i]->GetRasterYSize(), &_x2, &_y2, pGeoTransform);
// 		vX.push_back(_x1); vX.push_back(_x2);
// 		vY.push_back(_y1); vY.push_back(_y2);
// 	}
// 
// 	x1 = x2 = vX[0];
// 	y1 = y2 = vY[0];
// 
// 	for (i=0; i<vX.size(); i++)
// 	{
// 		if (vX[i]<x1) x1=vX[i];
// 		if (vX[i]>x2) x2=vX[i];
// 
// 		if (vY[i]>y1) y1=vY[i];
// 		if (vY[i]<y2) y2=vY[i];
// 	}
// 
// 	mosaic_width = (x2-x1)/scaleX;
// 	mosaic_height = (y2-y1)/scaleY;
// 
// }

void GetMosaicRect( CGDALFileManager** ppImg, int nCount, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height )
{
	vector<double> vX,vY;
	vX.clear(); vY.clear();

	int i=0;
	double _x1, _y1, _x2, _y2;
//	double pGeoTransform[6];
//	poDataset[nIndex]->GetGeoTransform(pGeoTransform);
	scaleX = ppImg[nIndex]->m_header.m_MapInfo->m_adfGeotransform[1];
	scaleY = ppImg[nIndex]->m_header.m_MapInfo->m_adfGeotransform[5];

	//计算4个点坐标
	for (i=0; i<nCount; i++)
	{
		//poDataset[i]->GetGeoTransform(pGeoTransform);
		//Pixel2Ground(0, 0, &_x1, &_y1, pGeoTransform);
		//Pixel2Ground(poDataset[i]->GetRasterXSize(), poDataset[i]->GetRasterYSize(), &_x2, &_y2, pGeoTransform);
		_x1 = ppImg[i]->m_header.m_MapInfo->m_adfGeotransform[0];
		_y1 = ppImg[i]->m_header.m_MapInfo->m_adfGeotransform[3];
		_x2 = ppImg[i]->m_header.m_MapInfo->m_adfGeotransform[0]+scaleX*ppImg[i]->m_header.m_nSamples;
		_y2 = ppImg[i]->m_header.m_MapInfo->m_adfGeotransform[3]+scaleY*ppImg[i]->m_header.m_nLines;
		vX.push_back(_x1); vX.push_back(_x2);
		vY.push_back(_y1); vY.push_back(_y2);
	}

	x1 = x2 = vX[0];
	y1 = y2 = vY[0];

	for (i=0; i<vX.size(); i++)
	{
		if (vX[i]<x1) x1=vX[i];
		if (vX[i]>x2) x2=vX[i];

		if (vY[i]>y1) y1=vY[i];
		if (vY[i]<y2) y2=vY[i];
	}

	mosaic_width = (x2-x1)/scaleX;
	mosaic_height = (y2-y1)/scaleY;
}

HIPAPI void GetMosaicRect( CGDALFileManager* pMgr1, CGDALFileManager* pMgr2, int nIndex, double& x1, double& y1, double& x2, double& y2, double& scaleX, double& scaleY, double& mosaic_width, double& mosaic_height )
{
	CGDALFileManager* ppMgr[2];
	ppMgr[0] = pMgr1;
	ppMgr[1] = pMgr2;
	GetMosaicRect(ppMgr, 2, nIndex, x1, y1, x2, y2, scaleX, scaleY, mosaic_width, mosaic_height);

}

// bool IsGroudXYInImg( double gX, double gY, GDALDataset* poDataset, double& x, double& y )
// {
// 	if (poDataset == NULL)
// 		return false;
// 
// 	//获取poDataset的大小
// 	double gX1, gX2, gY1, gY2;
// 	double pGeo[6];
// 	Pixel2Ground(0, 0, &gX1, &gY1, pGeo);
// 	Pixel2Ground(poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), &gX2, &gY2, pGeo);
// 
// 	//判断是否在其中
// 	if (gX>=gX1 && gX<=gX2 && gY<=gY1 && gY>=gY2)
// 	{
// 		x=(gX-gX1)/pGeo[1];
// 		y=(gY-gY1)/pGeo[5];
// 		return true;
// 	}
// 	else
// 	{
// 		x = y = -1;
// 		return false;
// 	}
// 
// }

bool IsGroudXYInImg( double gX, double gY, CGDALFileManager* pImg, double& x, double& y )
{
	if (pImg == NULL)
		return false;

	//获取poDataset的大小
	double gX1, gX2, gY1, gY2;
	//double pGeo[6];
	//Pixel2Ground(0, 0, &gX1, &gY1, pGeo);
	//Pixel2Ground(poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), &gX2, &gY2, pGeo);
	//
	MapInfo* pMap = pImg->m_header.m_MapInfo;
	int width = pImg->m_header.m_nSamples;
	int height = pImg->m_header.m_nLines;

	gX1 = pMap->m_adfGeotransform[0]; //->m_dLeftUpper[0];
	gY1 = pMap->m_adfGeotransform[2];//m_dLeftUpper[1];
	gX2 = gX1 + pMap->m_adfGeotransform[1]*width + pMap->m_adfGeotransform[2]*height;
	gY2 = gX2 + pMap->m_adfGeotransform[4]*width + pMap->m_adfGeotransform[5]*height;;

	double scaleX = pMap->m_adfGeotransform[1];
	double scaleY = pMap->m_adfGeotransform[5];

	//判断是否在其中
	if (gX>=gX1 && gX<gX2 && gY<=gY1 && gY>gY2)
	{
		x=(gX-gX1)/scaleX;
		y=(gY-gY1)/scaleY;
		return true;
	}
	else
	{
		x = y = -1;
		return false;
	}	
}

// template<class TT> double PointConvolution( HAPBEGTBase* pImgIO, int col, int row, int band, float* pMask, int masksize, float parameter /*= 1*/ )
// {
// 	return 0;
// }

