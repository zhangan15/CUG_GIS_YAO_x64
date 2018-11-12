#include "hapheader.h"
#include "ogr_srs_api.h" 

USING_NAMESPACE(HAPLIB)


#define SRS_GEOGRAPHIC          "Geographic Lat"
#define SRS_LOCAL               "Local CS"
#define SRS_UNDEFINED           "Arbitrary"



MapInfo::MapInfo()
{
	strcpy(m_cProName, SRS_UNDEFINED);
	strcpy(m_cDatum,"");
	strcpy(m_cUint,"");
	memset(m_adfGeotransform,0, 6 *sizeof(double));

	m_poSRS = NULL;
}


MapInfo::MapInfo(MapInfo &mpinfo)
{
	
	memcpy(m_adfGeotransform, mpinfo.m_adfGeotransform , 6 *sizeof(double));
	SetSpatialReference(mpinfo.m_poSRS);
}

MapInfo::~MapInfo()
{
	if(m_poSRS != NULL)
	{
		OSRDestroySpatialReference(m_poSRS); m_poSRS = NULL;
	}
}


void MapInfo::SetSpatialReference(const char * lpszWKT)
{
	OGRSpatialReferenceH poSRS =OSRNewSpatialReference(lpszWKT);
		
	SetSpatialReference(poSRS);
	
	OSRDestroySpatialReference(poSRS); poSRS = NULL;	
}

MapInfo &MapInfo:: operator = (const MapInfo &mpinfo)
{
	memcpy(m_adfGeotransform, mpinfo.m_adfGeotransform , 6 *sizeof(double));

    SetSpatialReference(mpinfo.m_poSRS);

	return *this;
}
const char* MapInfo::GetSpatialReferenceWKT()
{
	if(m_poSRS == NULL)
		return NULL;
	
	char *szWKT =NULL;
	OSRExportToWkt(m_poSRS, &szWKT);
	return szWKT;
}
void MapInfo::SetSpatialReference(OGRSpatialReferenceH poSRS)
{
	if (NULL == poSRS )
	{
		if (NULL != m_poSRS)
		{
			OSRDestroySpatialReference(m_poSRS);
		}

		m_poSRS = NULL;
	}
	else if (NULL == m_poSRS)
	{
		m_poSRS = OSRClone( poSRS);
	}
	else if (!OSRIsSame(m_poSRS,poSRS))
	{
		OSRDestroySpatialReference(m_poSRS);
		m_poSRS = OSRClone(poSRS);
	}

	strcpy(m_cProName,SRS_UNDEFINED);
	strcpy(m_cDatum,"");
	strcpy(m_cUint,"");

	const char* pszTemp=NULL;

	if (NULL != m_poSRS)
	{
		if (OSRIsProjected(m_poSRS))
		{
			pszTemp = OSRGetAttrValue(m_poSRS, "PROJECTION", 0);
			if (NULL != pszTemp)
			{
				strcpy(m_cProName, pszTemp);

				pszTemp = OSRGetAttrValue(m_poSRS, "DATUM", 0);
				if (NULL != pszTemp)
				{
					strcpy(m_cDatum, pszTemp);
				}
				else
				{
					strcpy(m_cDatum, SRS_DN_WGS84); 
				}
				pszTemp = OSRGetAttrValue(m_poSRS, "UNIT", 0);
		
				if (NULL != pszTemp)
				{
					strcpy(m_cUint, pszTemp);
				}
				else
				{
					strcpy(m_cUint, SRS_UL_METER); 
				}
			}
			else
			{
				strcpy(m_cProName, SRS_UNDEFINED);
			}
		}
		else if (OSRIsGeographic(m_poSRS))
		{
			strcpy(m_cProName, SRS_GEOGRAPHIC);

			pszTemp = OSRGetAttrValue(m_poSRS, "DATUM", 0);
			if (NULL != pszTemp)
			{
				strcpy(m_cDatum, pszTemp);
			}
			else
			{
				strcpy(m_cDatum, SRS_DN_WGS84); 
			}

			pszTemp = OSRGetAttrValue(m_poSRS, "UNIT", 0);
			if (NULL != pszTemp)
			{
				strcpy(m_cUint, pszTemp);
			}
			else
			{
				strcpy(m_cUint, SRS_UA_DEGREE); 
			}

		}
		else if (OSRIsLocal(m_poSRS))
		{
			strcpy(m_cProName, SRS_LOCAL);
		}
		else
		{
			strcpy(m_cProName, SRS_UNDEFINED);
		}
	}
}


HAPHeader::HAPHeader()
{
	m_nSamples = -1;
	m_nLines = -1;
	m_nBands = -1;
	m_MapInfo = NULL;
	m_nDataType = -1;

	memset(m_cFileType,0,32);
	memset(m_cInterLeave,0,8);
	memset(m_cSensorType,0,32);
	memset(m_sStretchType,0,16);
	memset(m_cDescription,0,512);
	m_nByteOrder = -1;
	m_dXstart = -1;
	m_dYstart = -1;
	m_fPlotAverage = -1;
	m_nStretch = -1;
	m_dxOffset = -1;
	m_dyOffset = -1;
	//		m_LUT = NULL;
	m_cSpectraName = NULL;
	m_nVersionMajor = -1;
	m_nVersionMinor = -1;
	m_dwID = 16435;
	m_nHeaderOffset = -1;
	int i ;
	for( i=0;i<3;i++)
	{
		m_nDefaultBands[i] = -1;
	}
	for(i=0;i<2;i++)
	{
		m_dPlotRange[i] = -1;
	}

	for(i=0; i<MAXBAND; i++)
	{
		m_cBandNames[i] = new char[NAMELEN];
		memset(m_cBandNames[i],0,NAMELEN);
		m_dWaveLength[i] = m_dFwhm[i] = -1;
	}
	for(i=0; i<2;i++)
	{
		m_cPlotTitle[i] = new char[32];
		memset(m_cPlotTitle[i],0,32);
	}

}

HAPHeader& HAPHeader::operator = (const HAPHeader &header)
{	
	m_nSamples = header.m_nSamples;
	m_nLines  = header.m_nLines;			     
	m_nBands = header.m_nBands;		
	m_nDataType = header.m_nDataType;	
	
	if(header.m_MapInfo)
	{
		m_MapInfo = new MapInfo();
		*m_MapInfo = *(header.m_MapInfo);
	}
	
	//rarely used
	strcpy(m_cDescription,header.m_cDescription);
	if(m_cSpectraName)
		{
			for(int i=0;i<m_nLines;i++)
			{
				delete []m_cSpectraName[i];
			}
			delete []m_cSpectraName;
			m_cSpectraName = NULL;
		}
	m_nHeaderOffset = header.m_nHeaderOffset;		  
	strcpy(m_cFileType,header.m_cFileType);		
	strcpy(m_cInterLeave, header.m_cInterLeave);	
	strcpy(m_cSensorType, header.m_cSensorType);	
	strcpy(m_sStretchType,header.m_sStretchType);

	m_nByteOrder = header.m_nByteOrder;	
	m_dXstart = header.m_dXstart;			
	m_dYstart = header.m_dYstart;
	m_dxOffset = header.m_dxOffset;
	m_dyOffset = header.m_dyOffset;


	if(header.m_cSpectraName)
	{
		m_cSpectraName = new char*[m_nLines];
		for(int i=0;i<m_nLines;i++)
		{
			m_cSpectraName[i] = new char[NAMELEN];
			strcpy(m_cSpectraName[i], header.m_cSpectraName[i]);
		}
	}
	int i;
	for( i=0;i<3;i++)
	{
		m_nDefaultBands[i] = header.m_nDefaultBands[i];
	}
	for(i=0;i<2;i++)
	{
		m_dPlotRange[i] = header.m_dPlotRange[i];
	}
	m_fPlotAverage = header.m_fPlotAverage;

	for(i=0;i<2;i++)
	{
		m_cPlotTitle[i] = new char[NAMELEN];
		strcpy(m_cPlotTitle[i] , header.m_cPlotTitle[i]);
	}
	
	m_nStretch = header.m_nStretch;
	for(i=0;i<MAXBAND;i++)
	{
		m_cBandNames[i] = new char[NAMELEN];
		strcpy(m_cBandNames[i], header.m_cBandNames[i]);
	}
	for(i=0;i<MAXBAND;i++)
	{
		m_dWaveLength[i] = header.m_dWaveLength[i];
	}
	for(i=0;i<64;i++)
	{
		m_dFwhm[i] = header.m_dFwhm[i];
	}
	
	
	m_nVersionMajor = header.m_nVersionMajor;            
	m_nVersionMinor = header.m_nVersionMinor;           
	m_dwID = header.m_dwID;    
	return *this;
}

HAPHeader::~HAPHeader()
{
	if(m_MapInfo)
			delete m_MapInfo;

	//rarely used
	int i;
	for( i=0; i<MAXBAND; i++)
	{
		delete []m_cBandNames[i];
		m_cBandNames[i] = NULL;
	}
	for(i=0; i<2;i++)
	{
		delete []m_cPlotTitle[i];
		m_cPlotTitle[i] = NULL;
	}

	if(m_cSpectraName)
	{
		for(int i=0;i<m_nLines;i++)
		{
			delete []m_cSpectraName[i];
		}
		delete []m_cSpectraName;
		m_cSpectraName = NULL;
	}
}

void HAPHeader::InitDIMS(DIMS *pDims)
{
	pDims->ROI.Xstart=0;  pDims->ROI.Xend= m_nSamples ;
	pDims->ROI.Ystart=0;  pDims->ROI.Yend= m_nLines ;
	pDims->bandNum = m_nBands;
	
	if(m_nBands > 0)
	{
		pDims->pBand = new bool[m_nBands];
		for(int i=0; i< m_nBands; i++)
		{
			pDims->pBand[i] = true; 
		}
	}
	
}

void HAPHeader::PutInfoByDIMS(DIMS dims,HAPHeader &header)
{
	header.m_nSamples = dims.ROI.Xend - dims.ROI.Xstart;
	header.m_nLines   = dims.ROI.Yend - dims.ROI.Ystart;			     
	header.m_nBands   = dims.GetBands();
	
	header.m_nDataType = m_nDataType;
	
	if(m_MapInfo)
	{
		if(!header.m_MapInfo)
			header.m_MapInfo = new MapInfo;
		*header.m_MapInfo = *m_MapInfo;
	}
	
	
//rarely used later info
	strcpy(header.m_cDescription,m_cDescription);
	header.m_nHeaderOffset = m_nHeaderOffset;		  
	strcpy(header.m_cFileType,m_cFileType);	
	
	strcpy(header.m_cInterLeave,m_cInterLeave);	
	strcpy(header.m_cSensorType,m_cSensorType);	
	strcpy(header.m_sStretchType,m_sStretchType);

	header.m_nByteOrder = m_nByteOrder;	
	header.m_dXstart = m_dXstart;			
	header.m_dYstart = m_dYstart;
	header.m_dxOffset = m_dxOffset;
	header.m_dyOffset = m_dyOffset;
	
	int i;
	for(i=0;i<2;i++)
	{
		header.m_dPlotRange[i] = m_dPlotRange[i];
	}
	header.m_fPlotAverage = m_fPlotAverage;
	for(i=0;i<2;i++)
	{
		header.m_cPlotTitle[i] = new char[NAMELEN];
		strcpy(header.m_cPlotTitle[i] , m_cPlotTitle[i]);
	}

	header.m_nStretch = m_nStretch;
	int index = 0;
	for(i=0;i<MAXBAND;i++)
	{
		if(dims.pBand[i] != 1)
			continue;

		header.m_cBandNames[index] = new char [NAMELEN];
		strcpy(header.m_cBandNames[index], m_cBandNames[i]);
		index++;
	}
	index = 0;
	for(i=0;i<MAXBAND;i++)
	{
		if(dims.pBand[i] != 1)
			continue;
		header.m_dWaveLength[index] = m_dWaveLength[i];
		index++;
	}
	index = 0;
	for(i=0;i<MAXBAND;i++)
	{
		if(dims.pBand[i] != 1)
			continue;
		header.m_dFwhm[index] = m_dFwhm[i];
		index++;
	}

	header.m_nVersionMajor = m_nVersionMajor;            
	header.m_nVersionMinor = m_nVersionMinor;           
	header.m_dwID = m_dwID;    
}

