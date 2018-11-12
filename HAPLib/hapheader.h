// HAPHeader.h: interface for the HAPHeader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HAPHEADER_H
#define HAPHEADER_H

#include "haplib.h"

#define  LEN    128

BEGIN_NAMESPACE(HAPLIB)


class HAPCLASS MapInfo  
{
public:
	char	m_cProName[LEN];   
	char    m_cDatum[LEN];      
	char	m_cUint[LEN];		
	double  m_adfGeotransform[6]; //same with gdal
	void   *m_poSRS;

	MapInfo();
	MapInfo(MapInfo &mpinfo);
	~MapInfo();
	
	MapInfo & operator = (const MapInfo &mpinfo);
	
	void SetSpatialReference(const char * lpszWKT);
	const char* GetSpatialReferenceWKT();
private:
	void SetSpatialReference(void *poSR);
};


struct HAPCLASS HAPHeader   
{
	HAPHeader();
	~HAPHeader();
	int GetBytesPerPt()
	{
		switch(m_nDataType)
		{
		case 1:
			return 1;
		case 2:
			return 2;
		case 3:
			return 4;
		case 4:
			return 4;
		case 5:
			return 8;
		case 6:
			return 8;
		case 12:
			return 2;
		default:
			return -1;
		}
	};
	
	void PutInfoByDIMS(DIMS dims,HAPHeader &header);
	void InitDIMS(DIMS *pDims);
	
	HAPHeader& operator = (const HAPHeader &header);

	int	    m_nSamples;	
	int	    m_nLines;	
	int	    m_nBands;	
	int	    m_nDataType;
	MapInfo	*m_MapInfo;
	
	
// rarely used in haplib till now	
	char	m_cDescription[512];	
	char	m_sProvince[LEN];	
	char	m_sCity[LEN];	
	char	m_sTime[LEN];	
	double	m_fRank;	
	int     m_nStretch;
	char	m_sStretchType[64];	
	char	*m_cBandNames[MAXBAND];
	double	m_dWaveLength[MAXBAND];	
	double	m_dFwhm[MAXBAND];	
	double	m_dxOffset;	
	double	m_dyOffset;	
	char	**m_cSpectraName;
	double	m_dXstart;	
	double	m_dYstart;
	int	    m_nDefaultBands[3];	
	double	m_dPlotRange[2];	
	double	m_fPlotAverage;	
	char	*m_cPlotTitle[2];	
	char	m_cInterLeave[64];	
	char	m_cSensorType[64];
	int	    m_nByteOrder;	
	char	m_cFileType[32];	
	int	    m_nHeaderOffset;	
	
	short	m_nVersionMajor;	
	short	m_nVersionMinor;	
	unsigned long	m_dwID;	
};
END_NAMESPACE(HAPLIB)

#endif // HAPHEADER_H
