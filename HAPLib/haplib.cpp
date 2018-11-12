#include "haplib.h"

USING_NAMESPACE(HAPLIB)

DIMS HAPLIB::g_Zdims(0,1,0,1); //ok

int HAPLIB::g_BlockSize  = 50*1024*1024; 
int HAPLIB::g_BlockSizeW = 50*1024*1024; 
int HAPLIB::g_SquareBorderSize = 512;

SimDIMS::SimDIMS(SimDIMS& DimsCopy)
{
	ROI = DimsCopy.ROI;
	bandNum=DimsCopy.bandNum; 
	pData = DimsCopy.pData;
	
	if(DimsCopy.pBand !=NULL && bandNum > 0)
	{
		pBand = new bool[bandNum];
		memcpy(pBand,DimsCopy.pBand, bandNum*sizeof(bool));
	}
	
}

SimDIMS& SimDIMS:: operator = (const SimDIMS& SimDIMS)
{
	if(this == &SimDIMS)
		return *this;
	
	ROI = SimDIMS.ROI;
	bandNum=SimDIMS.bandNum; 
	pData = SimDIMS.pData;

	if(pBand != NULL)
		delete [] pBand; pBand =NULL;
	
	if(SimDIMS.pBand !=NULL && bandNum > 0)
	{
		pBand = new bool[bandNum];
		memcpy(pBand,SimDIMS.pBand, bandNum*sizeof(bool));
	}
	
	return *this;
}

SimDIMS::~SimDIMS()
{
	if (pBand) 
	{
		delete[] pBand;
		pBand = NULL;
	}
}

DIMS::DIMS(DIMS& dimsCopy)
{
	ROI = dimsCopy.ROI;
	bandNum=dimsCopy.bandNum; 
	
	if(dimsCopy.pBand !=NULL && bandNum > 0)
	{
		pBand = new bool[bandNum];
		memcpy(pBand,dimsCopy.pBand, bandNum*sizeof(bool));
	}
}

DIMS& DIMS::operator = (const DIMS& dimsCopy)
{
	if(this == &dimsCopy)
			return *this;
		
		ROI = dimsCopy.ROI;
		bandNum=dimsCopy.bandNum; 

		if(pBand != NULL)
			delete [] pBand; pBand =NULL;
		
		if(dimsCopy.pBand !=NULL && bandNum > 0)
		{
			pBand = new bool[bandNum];
			memcpy(pBand,dimsCopy.pBand, bandNum*sizeof(bool));
		}
		
		return *this;
}
DIMS::~DIMS()
{
	/*if (pBand) 
	{
		delete[] pBand;
		pBand = NULL;
	}*/
}

