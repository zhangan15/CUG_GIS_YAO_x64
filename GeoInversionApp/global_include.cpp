#include "global_include.h"
#include "gdalfilemanager.h"

using namespace std;

// 
// int _World2Pixel( double lat, double lon, double *x, double *y, double* geoTransform, const char* lpszSpatialReferrence )
// {
// 	OGRSpatialReferenceH   srcSR = OSRNewSpatialReference(lpszSpatialReferrence); // ground
// 	OGRSpatialReferenceH   latLongSR=OSRCloneGeogCS(srcSR);  //geo
// 	OGRCoordinateTransformationH poTransform =OCTNewCoordinateTransformation(latLongSR, srcSR);
// 
// 	if(poTransform != NULL)
// 	{
// 		double height;
// 		OCTTransform(poTransform,1, &lon, &lat, &height);
// 
// 		double  adfInverseGeoTransform[6];
// 
// 		GDALInvGeoTransform(geoTransform, adfInverseGeoTransform);
// 		GDALApplyGeoTransform(adfInverseGeoTransform, lon,lat, x, y);
// 
// 		OCTDestroyCoordinateTransformation(poTransform);
// 		OSRDestroySpatialReference(latLongSR);
// 		OSRDestroySpatialReference(srcSR);		
// 		return 0;
// 	}
// 	else
// 	{
// 		OSRDestroySpatialReference(latLongSR);
// 		OSRDestroySpatialReference(srcSR);
// 		return -1;
// 	}
// }
// 
// int _Pixel2World( double *lat, double *lon, double x, double y, double* geoTransform, const char* lpszSpatialReferrence )
// {
// 	GDALApplyGeoTransform(geoTransform, x, y, lon, lat);
// 
// 	OGRSpatialReferenceH   srcSR = OSRNewSpatialReference(lpszSpatialReferrence); // ground
// 	OGRSpatialReferenceH   latLongSR=OSRCloneGeogCS(srcSR);  //geo
// 	OGRCoordinateTransformationH poTransform =OCTNewCoordinateTransformation(srcSR, latLongSR);
// 
// 	if(poTransform != NULL)
// 	{
// 		double height;
// 		OCTTransform(poTransform,1, lon, lat, &height);
// 
// 		OSRDestroySpatialReference(srcSR);
// 		OSRDestroySpatialReference(latLongSR);
// 		OCTDestroyCoordinateTransformation(poTransform);
// 
// 		//OSRCleanup();
// 
// 		return 0;
// 	}
// 	else
// 	{
// 		OSRDestroySpatialReference(srcSR);
// 		OSRDestroySpatialReference(latLongSR);
// 
// 		//OSRCleanup();
// 		return -1;
// 	}
// }
// 
// int _Pixel2Ground( double x,double y,double* pX,double* pY,double* geoTransform )
// {
// 	GDALApplyGeoTransform(geoTransform, x, y, pX, pY);
// 
// 	return 0;
// }
// 
// int _Ground2Pixel( double X,double Y,double* px,double* py,double* geoTransform )
// {
// 	double  adfInverseGeoTransform[6];
// 
// 	GDALInvGeoTransform(geoTransform, adfInverseGeoTransform);
// 	GDALApplyGeoTransform(adfInverseGeoTransform, X, Y, px, py);
// 
// 	return 0;
// }
// 
// bool _IsGroudXYInImg( double gX, double gY, CGDALFileManager* pImg, double& x, double& y )
// {
// 	if (pImg == NULL)
// 		return false;
// 
// 	//获取poDataset的大小
// 	double gX1, gX2, gY1, gY2;
// 	//double pGeo[6];
// 	//Pixel2Ground(0, 0, &gX1, &gY1, pGeo);
// 	//Pixel2Ground(poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), &gX2, &gY2, pGeo);
// 	//
// 	MapInfo* pMap = pImg->m_header.m_MapInfo;
// 	int width = pImg->m_header.m_nSamples;
// 	int height = pImg->m_header.m_nLines;
// 
// 	gX1 = pMap->m_adfGeotransform[0]; //->m_dLeftUpper[0];
// 	gY1 = pMap->m_adfGeotransform[3];//m_dLeftUpper[1];
// 	gX2 = gX1 + pMap->m_adfGeotransform[1]*width + pMap->m_adfGeotransform[2]*height;
// 	gY2 = gY1 + pMap->m_adfGeotransform[4]*width + pMap->m_adfGeotransform[5]*height;;
// 
// 	double scaleX = pMap->m_adfGeotransform[1];
// 	double scaleY = pMap->m_adfGeotransform[5];
// 
// 	//判断是否在其中
// 	if (gX>=gX1 && gX<gX2 && gY<=gY1 && gY>gY2)
// 	{
// 		x=(gX-gX1)/scaleX;
// 		y=(gY-gY1)/scaleY;
// 		return true;
// 	}
// 	else
// 	{
// 		x = y = -1;
// 		return false;
// 	}	
// }

bool _Src2DstCoordTrans( double src_x, double src_y, double& dst_x, double& dst_y, CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg )
{
	double lat = -1, lon = -1;	
	//_Pixel2World(&lat, &lon, src_x, src_y, pSrcImg->m_header.m_MapInfo->m_adfGeotransform, pSrcImg->GetpoDataset()->GetProjectionRef());
	//_World2Pixel(lat, lon, &dst_x, &dst_y, pDstImg->m_header.m_MapInfo->m_adfGeotransform, pDstImg->GetpoDataset()->GetProjectionRef());

	pSrcImg->pixel2World(&lat, &lon, src_x, src_y);
	//pSrcImg->world2Pixel(lat, lon, &src_x, &src_y);
	pDstImg->world2Pixel(lat, lon, &dst_x, &dst_y);

	if (dst_x<0 || dst_y<0 || dst_x>=pDstImg->m_header.m_nSamples || dst_y>=pDstImg->m_header.m_nLines)
		return false;
	else
		return true;
}

double _calXPower( double x, double x_min, double x_max, double x_mean, bool isNearerBetter/*=true*/ )
{
	double _p;

	if (x<x_mean)
		_p = 0.5*(x-x_min)/(x_mean-x_min);
	else
		_p = 0.5+0.5*(x-x_mean)/(x_max-x_mean);

	if (isNearerBetter)
		return 1-_p;
	else
		return _p;
}

void global_ilog( char* sMessage, char* sInstance /*= "info"*/ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage);
	cout<<sMessage<<endl;
}

void global_ilog( QString sMessage, char* sInstance /*= "info"*/ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage.toStdString().c_str());
	cout<<sMessage.toStdString().c_str()<<endl;
}

bool str_connect_begin( char* filepath, char* filename )
{
	char _sconnect[1024] = {'\0'};
	strcat(_sconnect, filepath);
	strcat(_sconnect, "/");
	strcat(_sconnect, filename);

	strcpy(filename, _sconnect);

	return true;

}
