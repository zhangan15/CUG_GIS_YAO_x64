// GDALFileManager.cpp: implementation of the CGDALFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "gdalfilemanager.h"
#include "gdal.h"
#include "ogr_srs_api.h"     //spatialref
#include "cpl_string.h"
#include "cpl_conv.h"


/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGDALFileManager::CGDALFileManager()
{
	m_HAPFlag = 0;    
	m_poDataset = NULL;
	m_bCreateCopy = false;
	m_poDriver    = NULL;
	
	strcpy(m_szTiffFileName ,"");
	strcpy(m_szFileName,"");
	strcpy(pcErrMessage,"");

	msrcSR = NULL;
	mlatLongSR = NULL;
	mpoTransform = NULL;
	mpoTransformT = NULL;

}

CGDALFileManager::~CGDALFileManager()
{

	if(NULL !=m_poDataset)
	{
		GDALClose(m_poDataset);	 
		m_poDataset=NULL;
	}

	//destory
	if (mpoTransform!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransform);
	mpoTransform = NULL;

	if (mpoTransformT!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransformT);
	mpoTransformT = NULL;

	if (mlatLongSR != NULL)
		OSRDestroySpatialReference(mlatLongSR);
	mlatLongSR = NULL;

	if (msrcSR!=NULL)
		OSRDestroySpatialReference(msrcSR);
	msrcSR = NULL;

// 	mpoTransform = NULL;
// 	mpoTransformT = NULL;
// 	mlatLongSR = NULL;
// 	msrcSR = NULL;



	
}

bool CGDALFileManager::LoadFrom(const char* Imfilename)
{
	m_HAPFlag = 0; 
	if(GDALGetDriverCount() == 0)
	{
	    GDALAllRegister();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	}

	if(NULL != m_poDataset)
	{
		GDALClose(m_poDataset); m_poDataset=NULL;
	}

	//destory
	if (mpoTransform!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransform);
	mpoTransform = NULL;

	if (mpoTransformT!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransformT);
	mpoTransformT = NULL;

	if (mlatLongSR != NULL)
		OSRDestroySpatialReference(mlatLongSR);
	mlatLongSR = NULL;

	if (msrcSR!=NULL)
		OSRDestroySpatialReference(msrcSR);
	msrcSR = NULL;
	
	m_poDataset= GDALOpenShared(Imfilename,GA_ReadOnly);

	if(m_poDataset==NULL)
	{
		strcpy(pcErrMessage, "Cannot Open the File, Check the File Type!");
		return false;
	}
	
	m_header.m_nSamples=GDALGetRasterXSize(m_poDataset);     
	m_header.m_nLines  =GDALGetRasterYSize(m_poDataset);
	m_header.m_nBands  =GDALGetRasterCount(m_poDataset);     

	if(m_header.m_nSamples<=0 || m_header.m_nLines<=0||m_header.m_nBands<=0)
	{
		strcpy(pcErrMessage, "File Header Info Error");
		return false;
	}

	
	double AfTransform[6]; //Affine Transform Coefficient
	memset(AfTransform,0,sizeof(double)*6);
	CPLErr error = GDALGetGeoTransform(m_poDataset, AfTransform);

	if(error == CE_None)
	{
		MapInfo* headerMapInfo = new MapInfo;  
		memcpy(headerMapInfo->m_adfGeotransform, AfTransform, 6*sizeof(double));
		const char *pszSRS_WKT = GDALGetProjectionRef(m_poDataset);
		if(NULL != pszSRS_WKT)
			headerMapInfo->SetSpatialReference(pszSRS_WKT);
		m_header.m_MapInfo = headerMapInfo;


		//create others
		msrcSR = OSRNewSpatialReference(pszSRS_WKT); // ground
		mlatLongSR = OSRCloneGeogCS(msrcSR);  //geo
		mpoTransform =OCTNewCoordinateTransformation(msrcSR, mlatLongSR);
		//mpoTransformT = OCTNewCoordinateTransformation(msrcSR, mlatLongSR);
	}
	
	
	SetFileName(Imfilename);
	
	void *poBand;
	poBand = GDALGetRasterBand(m_poDataset, 1); 
	GDALDataType  DT =GDALGetRasterDataType(poBand);
	
	int* pHeadDT = &(m_header.m_nDataType);
	switch(DT) {
	case GDT_Byte:
		*pHeadDT = 1; break;
	case GDT_Int16:
		*pHeadDT = 2; break;
	case GDT_Int32:
		*pHeadDT = 3; break;
	case GDT_Float32:
		*pHeadDT = 4; break;
	case GDT_Float64:
		*pHeadDT = 5; break;
	case GDT_UInt16:
		*pHeadDT = 12; break;
	default: 
		strcpy(pcErrMessage, "DataType not Supported!");
		return false;
	}
	return true;
}

bool CGDALFileManager:: LoadRaw(const char* Imfilename,  long width,  long height, long pntLength, long bandNum,  int mode , double *padfTransform, const char* szSRS_WKT)
{
	m_HAPFlag = 0; 
	if(GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	}

	if(NULL != m_poDataset)
	{
		GDALClose(m_poDataset); 
		m_poDataset=NULL;
	}

	//destory
	if (mpoTransform!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransform);
	mpoTransform = NULL;

	if (mpoTransformT!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransformT);
	mpoTransformT = NULL;

	if (mlatLongSR != NULL)
		OSRDestroySpatialReference(mlatLongSR);
	mlatLongSR = NULL;

	if (msrcSR!=NULL)
		OSRDestroySpatialReference(msrcSR);
	msrcSR = NULL;

	GDALDataType et;
	switch(pntLength)
	{
		case 1:
			et = GDT_Byte;
			m_header.m_nDataType = 1;
		case 2:
			et= GDT_UInt16;
		case 3:
			et= GDT_UInt32;
		default:
			et = GDT_Byte;
	}

	GDALDriverH poDriver = GDALGetDriverByName("VRT");
	GDALDatasetH poDataset = GDALCreate(poDriver, "", width, height, bandNum, et, NULL);

	char **papszOption =NULL;
	papszOption = CSLAddNameValue(papszOption, "subclass", "VRTRawRasterBand");
	papszOption = CSLAddNameValue(papszOption, "SourceFileName", Imfilename);
	//papszOption = CSLAddNameValue(papszOption, "RelativeToVRT", "true");
	GDALAddBand(poDataset, GDT_UInt16, papszOption);
	CSLDestroy(papszOption);

	if(m_poDataset==NULL)
	{
		strcpy(pcErrMessage, "Cannot Open the File, Check the File Type!");
		return false;
	}

	if(NULL != padfTransform)
	{
		GDALSetGeoTransform(poDataset, padfTransform);
		MapInfo* headerMapInfo = new MapInfo;  
		memcpy(headerMapInfo->m_adfGeotransform, padfTransform, 6*sizeof(double));

		if(NULL != szSRS_WKT)
		{
			GDALSetProjection(poDataset, szSRS_WKT);
			headerMapInfo->SetSpatialReference(szSRS_WKT);

			//create others
			msrcSR = OSRNewSpatialReference(szSRS_WKT); // ground
			mlatLongSR = OSRCloneGeogCS(msrcSR);  //geo
			mpoTransform = OCTNewCoordinateTransformation(msrcSR, mlatLongSR);
			//mpoTransformT = OCTNewCoordinateTransformation(msrcSR, mlatLongSR);
		}

		m_header.m_MapInfo = headerMapInfo;
	}

	SetFileName(Imfilename);


	//int* pHeadDT = &(m_header.m_nDataType);
	//switch(DT) {
	//case GDT_Byte:
	//	*pHeadDT = 1; break;
	//case GDT_Int16:
	//	*pHeadDT = 2; break;
	//case GDT_Int32:
	//	*pHeadDT = 3; break;
	//case GDT_Float32:
	//	*pHeadDT = 4; break;
	//case GDT_Float64:
	//	*pHeadDT = 5; break;
	//case GDT_UInt16:
	//	*pHeadDT = 12; break;
	//default: 
	//	strcpy(pcErrMessage, "DataType not Supported!");
	//	return false;
	//}
	return true;
}

//get data from one dataset with specified area considering offset from  Real Data Image Area
bool CGDALFileManager::GetDIMSDataByBlockInfo(SimDIMS *pSBlockInfo)
{
	if (!pSBlockInfo->IsValid())
	{
		return false;
	}
	
	int bands = pSBlockInfo->GetBands();
	if(bands <=0)
		return false;
	
	int *pBandArray = pSBlockInfo->GetBandArray();
	if(pBandArray == NULL)
		return false;
	
	void* poDataset = m_poDataset;
	if (poDataset){
		GDALDatasetRasterIO(poDataset, GF_Read, \
				pSBlockInfo->ROI.Xstart, pSBlockInfo->ROI.Ystart, pSBlockInfo->GetWidth(), pSBlockInfo->GetHeight(), 
				pSBlockInfo->pData,pSBlockInfo->GetWidth(), pSBlockInfo->GetHeight(),\
				GDALGetRasterDataType(GDALGetRasterBand(poDataset,1)), \
				bands, pBandArray, 0, 0, 0);
		delete []pBandArray;
		return true;
	}

	delete []pBandArray;
	return false;
}
bool CGDALFileManager::GetDIMSDataByBlockInfo(DIMS *pDims,SimDIMS *pSBlockInfo)
{
	if (!pSBlockInfo->IsValid() || !pDims->IsValid())
	{
		return false;
	}
	
	if(!pDims->IsContain(pSBlockInfo))
	{
		return false;
	}
		
	int bands = pSBlockInfo->GetBands();
	if(bands <=0)
		return false;
	
	int *pBandArray = pSBlockInfo->GetBandArray();
	if(pBandArray == NULL)
		return false;
	
	void* poDataset = m_poDataset;
	if (poDataset)
	{
		GDALDatasetRasterIO(poDataset, GF_Read, \
				pSBlockInfo->ROI.Xstart, pSBlockInfo->ROI.Ystart, pSBlockInfo->GetWidth(), pSBlockInfo->GetHeight(), 
				pSBlockInfo->pData,pSBlockInfo->GetWidth(), pSBlockInfo->GetHeight(),\
				GDALGetRasterDataType(GDALGetRasterBand(poDataset,1)), \
				bands, pBandArray, 0, 0, 0);
		delete []pBandArray;
		return true;
	}

	delete []pBandArray;
	return false;
}


HAPDataType CGDALFileManager::GetHAPDataType()
{
	switch(m_header.m_nDataType)
	{
	case 1:
		return HAP_Byte;
	case 2:
		return HAP_Int16;
	case 3:
		return HAP_Int32;
	case 4:
		return HAP_Float32;
	case 5:
		return HAP_Float64;
	case 6:
		return HAP_Unknown;
	case 12:
		return HAP_UInt16;
	default:
		return HAP_Unknown;
	}
}	

void CGDALFileManager::GetFileName(char filename[])
{
	if(filename)
	{
		memcpy(filename,m_szFileName, HAP_MAX_PATH);
	}
	return;
}

void CGDALFileManager::SetFileName(const char *filename)
{
	strcpy(m_szFileName, filename);
}

bool CGDALFileManager::HapSaveImageAs(const char *szImageName, const char *szDriver)
{
		if(GDALGetDriverCount() == 0)
		{
			GDALAllRegister();
			CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
		}

	    void *poDriver = GDALGetDriverByName(szDriver);
		if( poDriver == NULL )
			return false;
		
		void *poDstDS = NULL;

		
#ifdef WINDOWS
		if(_stricmp(szDriver, "ENVI")==0)
#else   //for linux
		if(strcasecmp(szDriver, "ENVI")==0)
#endif
		{
			char *szRealImageName=strdup(szImageName);
			char *pExt=NULL;
			pExt = strrchr(szRealImageName, '.');
			
			if(pExt == NULL)
				return false;
			int checkNum = strlen(szRealImageName) - (pExt-szRealImageName);
			if(checkNum >=4)
			{
				strcpy(pExt,".dat");
			}
			else
			{
				return false;
			}
			
			poDstDS = GDALCreate(poDriver,szRealImageName, m_header.m_nSamples, m_header.m_nLines, m_header.m_nBands, (GDALDataType)GetHAPDataType(),
												      NULL );
			free(szRealImageName);szRealImageName=NULL;
		}
		else
		{
			char **papszMetadata;
			papszMetadata = GDALGetMetadata(poDriver, NULL);
			if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
			{
		
				poDstDS = GDALCreate(poDriver,szImageName, m_header.m_nSamples, m_header.m_nLines, m_header.m_nBands, (GDALDataType)GetHAPDataType(),
													      NULL );
				 m_bCreateCopy = false;
				 m_poDriver    = NULL;
			}
			else if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
			{
				strcpy(m_szTiffFileName, szImageName);
				strcat(m_szTiffFileName,".tif");
				
				 void *poTiffDriver = GDALGetDriverByName("GTiff");
				 poDstDS = GDALCreate(poTiffDriver,m_szTiffFileName, m_header.m_nSamples, m_header.m_nLines, m_header.m_nBands, (GDALDataType)GetHAPDataType(),
														  NULL );
				 m_bCreateCopy = true;
				 m_poDriver    = poDriver;
			}
			else
			{
				m_bCreateCopy = false;
				m_poDriver    = NULL;
				return false;
			}		
		}

		if( poDstDS == NULL )
			return false;
		
		m_poDataset = poDstDS;
		
		if(NULL != m_header.m_MapInfo)
		{
			GDALSetGeoTransform(m_poDataset, m_header.m_MapInfo->m_adfGeotransform);
			const char* lpszWKT = NULL;
			OSRExportToWkt(m_header.m_MapInfo->m_poSRS, (char**)&lpszWKT);
			GDALSetProjection(m_poDataset, lpszWKT);
		}
		
		SetFileName(szImageName);
		return true;
}

void CGDALFileManager::WriteColorTable(byte *rgbArray)
{
	if(rgbArray == NULL)
		return;
	if(m_poDataset == NULL)
		return;

	GDALColorTableH hcolortab = GDALCreateColorTable(GPI_RGB); 

	GDALColorEntry tempColorentry;
	for (int i=0; i<256; i++)
	{
		tempColorentry.c1 = rgbArray[3*i];
		tempColorentry.c2 = rgbArray[3*i+1];
		tempColorentry.c3 = rgbArray[3*i+2];
		tempColorentry.c4 = 1;
		GDALSetColorEntry(hcolortab, i, &tempColorentry);
	}


	GDALRasterBandH  poBand = GDALGetRasterBand(m_poDataset, 1);
	GDALSetRasterColorTable(poBand, hcolortab);
}

bool CGDALFileManager::HapSaveImageAs(const char *szImageName)
{
	if(szImageName == NULL)
		return false;
	
	char *szExt = getFileExtension(szImageName);
	if(szExt == NULL)
		return false;
	
	const char *szDriver = getGdalDriver(szExt);
	delete []szExt;
	
	if(szDriver == NULL)
	{
		return false;
	}
	
	return HapSaveImageAs(szImageName, szDriver);

}


bool CGDALFileManager::WriteBlock(SimDIMS *pSBlockInfo)
{
	if (!pSBlockInfo->IsValid())
	{
		return false;
	}
	
	int bands = pSBlockInfo->GetBands();
	if(bands <=0)
		return false;
		
	int *pBandArray = pSBlockInfo->GetBandArray();
	if(pBandArray == NULL)
		return false;
	
	void* poDataset = m_poDataset;
	if (poDataset)
	{
		
		GDALDatasetRasterIO(poDataset, GF_Write, \
				pSBlockInfo->ROI.Xstart, pSBlockInfo->ROI.Ystart, pSBlockInfo->GetWidth(), pSBlockInfo->GetHeight(), 
				pSBlockInfo->pData,pSBlockInfo->GetWidth(), pSBlockInfo->GetHeight(),\
				GDALGetRasterDataType(GDALGetRasterBand(poDataset,1)), \
				bands, pBandArray, 0, 0, 0);
		delete []pBandArray;
		return true;
	}

	delete []pBandArray;
	return false;
}




void CGDALFileManager::Close()
{
	if(m_bCreateCopy)
	{
		if(m_poDriver !=NULL)
		{
			void *poDs = GDALCreateCopy(m_poDriver, m_szFileName, m_poDataset, 1, NULL, GDALDummyProgress, NULL);
			GDALClose(poDs); poDs =NULL;
		}
		
		if(m_poDataset != NULL)
			GDALClose(m_poDataset); m_poDataset = NULL; 
		remove(m_szTiffFileName);
		strcpy(m_szTiffFileName, "");
	}
	else
	{
		if(m_poDataset != NULL)
			GDALClose(m_poDataset); m_poDataset = NULL; 
	}

	//destory
	if (mpoTransform!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransform);
	mpoTransform = NULL;

	if (mpoTransformT!=NULL)
		OCTDestroyCoordinateTransformation(mpoTransformT);
	mpoTransformT = NULL;

	if (mlatLongSR != NULL)
		OSRDestroySpatialReference(mlatLongSR);
	mlatLongSR = NULL;

	if (msrcSR!=NULL)
		OSRDestroySpatialReference(msrcSR);
	msrcSR = NULL;

}

bool CGDALFileManager::Open()
{
	if (strcmp(m_szFileName,"")==0)
		return false;
	if (m_poDataset!=NULL)
		return false;
	
	m_HAPFlag = 0;
	if (GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
	}
	
	m_poDataset = GDALOpenShared(m_szFileName, GA_ReadOnly);

	if (m_poDataset != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const char *CGDALFileManager::getGdalDriver(char *szExt)
{
#ifdef WINDOWS
	if(_stricmp(szExt, "IMG")==0)
	  return "HFA";
	
	if(_stricmp(szExt, "TIF")==0 || _stricmp(szExt, "TIFF")==0)
	  return "GTiff";
	
	if(_stricmp(szExt, "HDR")==0)
	  return "ENVI";
	
	if(_stricmp(szExt,"JPG")==0 || _stricmp(szExt,"JPEG")==0)
		 return "JPEG";
		
	if(_stricmp(szExt,"BMP")==0 )
		 return "BMP";

	if(_stricmp(szExt,"PNG")==0 )
		return "PNG";
	
	if(_stricmp(szExt,"DEM")==0)
	    return "USGSDEM";
	
	return NULL;
#endif
	
#ifdef LINUX
	if(strcasecmp(szExt, "IMG")==0)
	  return "HFA";
	
	if(strcasecmp(szExt, "TIF")==0 || strcasecmp(szExt, "TIFF")==0)
	  return "GTiff";
	
	if(strcasecmp(szExt, "HDR")==0)
	  return "ENVI";
	
	if(strcasecmp(szExt,"JPG")==0 || strcasecmp(szExt,"JPEG")==0)
	 return "JPEG";
	
	if(strcasecmp(szExt,"BMP")==0 )
		 return "BMP";

	if(strcasecmp(szExt,"PNG")==0 )
		return "PNG";
	
	if(strcasecmp(szExt,"DEM")==0)
	    return "USGSDEM";
	
	return NULL;
#endif
	
};


char *CGDALFileManager::getFileExtension(const char *filename)
{
	int length = strlen(filename);

	int dotIndex = -1;
	for(int i=length; i>=0; i--)
	{
		if(filename[i] == '.')
		{
			dotIndex = i;
			break;
		}
	}
	
	if(dotIndex== -1)
		return NULL;
	
	char *szExt = new char[length -dotIndex];
	strcpy(szExt, filename+dotIndex+1);
	return szExt;
}

bool CGDALFileManager::ReadImg(int srcLeft, int srcTop, int srcRight, int srcBottom, void * pBuffer, int bufferWidth, 
							   int bufferHeight, int nBandCount, int *panBandCount, int nPixelSpace, int nLineSpace, int nBandSpace)
{
	void* poDataset = m_poDataset;
	if (poDataset){
		GDALDatasetRasterIO(poDataset, GF_Read, srcLeft, srcTop, srcRight-srcLeft, srcBottom-srcTop, 
			pBuffer, bufferWidth, bufferHeight, GDALGetRasterDataType(GDALGetRasterBand(poDataset,1)), nBandCount, panBandCount, nPixelSpace, nLineSpace, nBandSpace);

		poDataset = NULL;
		return true;
	}

	return false;
}

void CGDALFileManager::GetBandMaxMin(int bandID, double &minVal, double &maxVal)
{
	double adfMinMax[2];
	GDALRasterBandH hInputBand  = GDALGetRasterBand(m_poDataset, bandID);
	GDALComputeRasterMinMax(hInputBand, 0, adfMinMax);

	minVal = adfMinMax[0];
	maxVal = adfMinMax[1];

	return;
}

bool CGDALFileManager::World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char *lpszSpatialReferrence)
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

	return S_OK;
}

bool CGDALFileManager::Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char *lpszSpatialReferrence)
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

	return S_OK;
}

bool CGDALFileManager::Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform)
{
	GDALApplyGeoTransform(geoTransform, x, y, pX, pY);

	return S_OK;
}
bool CGDALFileManager::Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform)
{
	double  adfInverseGeoTransform[6];

	GDALInvGeoTransform(geoTransform, adfInverseGeoTransform);
	GDALApplyGeoTransform(adfInverseGeoTransform, X, Y, px, py);

	return S_OK;
}

GDALDataset* CGDALFileManager::GetpoDataset()
{
	return (GDALDataset*)m_poDataset;
}

bool CGDALFileManager::world2Pixel( double lat, double lon, double *x, double *y )
{
	if (m_header.m_MapInfo == NULL)
	{
		return false;
	}


	if (mpoTransformT == NULL)
	 {
	 	mpoTransformT = OCTNewCoordinateTransformation(mlatLongSR, msrcSR);
	 }


	//
	//need redo
	if(mpoTransformT != NULL)
	{
		double height;
		OCTTransform(mpoTransformT,1, &lon, &lat, &height);

		double  adfInverseGeoTransform[6];
		GDALInvGeoTransform(m_header.m_MapInfo->m_adfGeotransform, adfInverseGeoTransform);
		GDALApplyGeoTransform(adfInverseGeoTransform, lon,lat, x, y);

		//OCTDestroyCoordinateTransformation(mpoTransformT);
		return true;
	}
	else
	{
		//OCTDestroyCoordinateTransformation(mpoTransformT);
		return false;
	}
}

bool CGDALFileManager::pixel2World( double *lat, double *lon, double x, double y )
{
	if (m_header.m_MapInfo == NULL)
	{
		return false;
	}

	//need redo
// 	if (mpoTransform!=NULL)
// 	{
// 		OCTDestroyCoordinateTransformation(mpoTransform);
// 		mpoTransform =OCTNewCoordinateTransformation(mlatLongSR, msrcSR);
// 	}

	GDALApplyGeoTransform(m_header.m_MapInfo->m_adfGeotransform, x, y, lon, lat);

	if(mpoTransform != NULL)
	{
		double height;
		OCTTransform(mpoTransform,1, lon, lat, &height);
		return true;
	}
	else
	{
		return false;
	}
}

bool CGDALFileManager::pixel2Ground( double x,double y,double* pX,double* pY )
{
	if (m_header.m_MapInfo == NULL)
	{
		return false;
	}

	GDALApplyGeoTransform(m_header.m_MapInfo->m_adfGeotransform, x, y, pX, pY);

	return true;

}

bool CGDALFileManager::ground2Pixel( double X,double Y,double* px,double* py )
{
	if (m_header.m_MapInfo == NULL)
	{
		return false;
	}

	double  adfInverseGeoTransform[6];

	GDALInvGeoTransform(m_header.m_MapInfo->m_adfGeotransform, adfInverseGeoTransform);
	GDALApplyGeoTransform(adfInverseGeoTransform, X, Y, px, py);

	return true;
}
