// GDALFileManager.h: interface for the CGDALFileManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GDALFILEMANAGER_H
#define GDALFILEMANAGER_H

#include "hapheader.h"
#include "gdal_priv.h"
#include "ogr_core.h"
#include "ogr_spatialref.h"
#include <string>
using namespace std;

#ifndef small char
typedef unsigned char byte;
#endif

USING_NAMESPACE(HAPLIB)

typedef enum {
	HAP_Unknown = 0,
	/*! Eight bit unsigned integer */           HAP_Byte = 1,
	/*! Sixteen bit unsigned integer */         HAP_UInt16 = 2,
	/*! Sixteen bit signed integer */           HAP_Int16 = 3,
	/*! Thirty two bit unsigned integer */      HAP_UInt32 = 4,
	/*! Thirty two bit signed integer */        HAP_Int32 = 5,
	/*! Thirty two bit floating point */        HAP_Float32 = 6,
	/*! Sixty four bit floating point */        HAP_Float64 = 7,
	/*! Complex Int16 */                        HAP_CInt16 = 8,
	/*! Complex Int32 */                        HAP_CInt32 = 9,
	/*! Complex Float32 */                      HAP_CFloat32 = 10,
	/*! Complex Float64 */                      HAP_CFloat64 = 11,
	HAP_TypeCount = 12          /* maximum type # + 1 */
} HAPDataType;



class HAPCLASS CGDALFileManager   
{
public:
	CGDALFileManager();
	virtual ~CGDALFileManager();

	//add by zouty 20121227
	void WriteColorTable(byte *rgbArray);

	void GetFileName(char filename[HAP_MAX_PATH]);
	bool LoadFrom(const char* Imfilename);
	/*
	  mode =1  BSQ; mode =2 BIP; mode 3=BIL
	*/
	bool LoadRaw(const char* Imfilename,  long width,  long height, long pntLength=1, long bandNum=1, int mode = 1, double *padfTransform=NULL, const char* szSRS_WKT=NULL);

	//read image by input parameters, add by zouty 20120814
	bool ReadImg(int srcLeft, int srcTop, int srcRight, int srcBottom, void * pBuffer, int bufferWidth, int bufferHeight, 
		int nBandCount, int *panBandCount, int nPixelSpace, int nLineSpace, int nBandSpace);

	void GetBandMaxMin(int bandID, double &minVal, double &maxVal);

public:
	static bool World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char *lpszSpatialReferrence);
	static bool Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char *lpszSpatialReferrence);
	static bool Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform);
	static bool Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform);

public:
	bool GetDIMSDataByBlockInfo(SimDIMS *pSBlockInfo);
	bool GetDIMSDataByBlockInfo(DIMS *pDims,SimDIMS *pSBlockInfo);

	HAPDataType GetHAPDataType();
	
	void GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo);

	bool HapSaveImageAs(const char *szImageName);
	bool HapSaveImageAs(const char *szImageName, const char *szDriver);
	
	bool WriteBlock(SimDIMS *pSBlockInfo);

	void Close();
	bool Open();

	GDALDataset* GetpoDataset();

	int m_HAPFlag;			
	HAPHeader m_header;   
	char pcErrMessage[256];	

protected:
	void SetFileName(const char *filename); 
	void *m_poDataset;
	char m_szFileName[HAP_MAX_PATH]; 

private:
	char *getFileExtension(const char *filename);
	const char *getGdalDriver(char *szExt);

	bool m_bCreateCopy;
	void *m_poDriver;
	char m_szTiffFileName[HAP_MAX_PATH]; 

public:
	OGRSpatialReferenceH msrcSR;
	OGRSpatialReferenceH mlatLongSR;
	OGRCoordinateTransformationH mpoTransform;
	OGRCoordinateTransformationH mpoTransformT;

public:
	bool world2Pixel(double lat, double lon, double *x, double *y);
	bool pixel2World(double *lat, double *lon, double x, double y);
	bool pixel2Ground(double x,double y,double* pX,double* pY);
	bool ground2Pixel(double X,double Y,double* px,double* py);
};

#endif //GDALFILEMANAGER_H
