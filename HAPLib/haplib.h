#ifndef HAPLIB_H_
#define HAPLIB_H_


#include "include_global.h"

#if defined(_MSC_VER)
#  pragma warning(disable:4251 4275 4786 4996 4127 4100)
#endif

#ifdef  WINDOWS 
#ifdef __HAPLIB__
#define HAPCLASS	__declspec(dllexport)
#define HIPAPI		__declspec(dllexport)
#define HIPVAR		__declspec(dllexport)
#else
#define HAPCLASS	__declspec(dllimport)
#define HIPAPI		__declspec(dllimport)
#define HIPVAR		__declspec(dllimport)
#endif

#endif


#ifdef  LINUX
#define HAPCLASS	
#define HIPAPI		
#define HIPVAR		
#endif

#ifdef  LINUX
typedef  int64_t  LONGLONG;
#endif


#define BEGIN_NAMESPACE(name)	namespace name {
#define END_NAMESPACE(name)		};
#define USING_NAMESPACE(name)	using namespace name;

BEGIN_NAMESPACE(HAPLIB)

#define HAP_MAX_PATH                   512
#define MAXBAND                        3*1024
#define NAMELEN                        512


//Simple Region of Interest
struct HAPCLASS SimROI
{
	long  Xstart; 
	long  Xend; 
	long  Ystart; 
	long  Yend; 
	
	int  GetHeight()            {return Yend - Ystart; }; 
	int  GetWidth()             {return Xend - Xstart; };
	
	SimROI(int ixstart=-1, int ixend=-1, int iystart=-1, int iyend=-1)
	:Xstart(ixstart), Xend(ixend), Ystart(iystart), Yend(iyend)
	{}
	
	bool IsValid()            {return (Xstart >= 0) && (Xend > Xstart) && (Ystart >=0) && (Yend > Ystart);};
	void Reset()              {Xstart=-1; Xend=-1;Ystart=-1;Yend=-1;};
	
};


struct  HAPCLASS SimDIMS
{

	SimROI   ROI;
	int      bandNum; 
	int      blockNo;
	bool*    pBand;
	BYTE*    pData;
	
	SimDIMS(int ixstart=-1, int ixend=-1, int iystart=-1, int iyend=-1, int num=-1,int block=-1, bool *pArray= NULL, BYTE* pBlock =NULL)
	:ROI(ixstart, ixend, iystart, iyend),bandNum(num),blockNo(block), pBand(pArray), pData(pBlock)
	{}
	
	SimDIMS(SimROI roi, int num=-1,bool*pArray= NULL, BYTE* pBlock =NULL)
	:ROI(roi), bandNum(num),pBand(pArray), pData(pBlock) 
	{}
	
	SimDIMS(SimDIMS& DimsCopy); 
	SimDIMS& operator = (const SimDIMS& DimsCopy);
	~SimDIMS();
	
	long   GetHeight()              {return ROI.GetHeight(); }; 
	long   GetWidth()               {return ROI.GetWidth(); };
	long   GetCacheSize()           {return ROI.GetHeight() * ROI.GetWidth() * bandNum; };
	long   GetCacheLoc(int column, int line, int band)  {return band*GetWidth()*GetHeight() + (line-ROI.Ystart)*GetWidth() + (column - ROI.Xstart);};
	bool IsValid()                {return ROI.IsValid() && (bandNum >0) && (pBand != NULL);};
	
	void Reset()                                        
	{
		ROI.Reset(); 
		bandNum = -1;
	};
	
	int  GetBands()
	{
		if(bandNum <=0 && pBand ==NULL)
		  return 0;
		
		int bands=0;
		for(int i=0; i<bandNum; i++)
		{
			if(pBand[i])
				bands++;
		}
		return bands;
	};
	
	int*  GetBandArray()
	{
		int bands = GetBands();
		if(bands <= 0)
			return NULL;
		
		int *pArray = new int[bands];
		int index=0;
		for(int i=0; i<bandNum; i++)
		{
			if(pBand[i])
				pArray[index++] = i+1;
			
		}
		
		return pArray;
	};
	
	bool IsContain(long column,long line, long band) 
	{
		return       (ROI.Xstart <= column)
				&&   (ROI.Ystart <= line)
				&&   (ROI.Xend   > column)
				&&   (ROI.Yend   > line)
				//&&   (pBand != NULL )
				&&   (bandNum   > band);
				//&&   (pBand[band]);
	};
	
	
	bool IsContain(SimDIMS *pSBlockInfo) 
	{
		if( (ROI.Xstart <= pSBlockInfo->ROI.Xstart)
			&&   (ROI.Ystart <= pSBlockInfo->ROI.Ystart)
			&&   (ROI.Xend   >= pSBlockInfo->ROI.Xend)
			&&   (ROI.Yend   >= pSBlockInfo->ROI.Yend)
			&&   (pBand != NULL ) && (pSBlockInfo->pBand != NULL)
			&&   (bandNum >= pSBlockInfo->bandNum) )
		{
			for(int i=0; i<pSBlockInfo->bandNum; i++)
			{
				if(pBand[i] != pSBlockInfo->pBand[i])
					return false;
			}
			
			return true;
		}
		
		return false;
	};
};


struct HAPCLASS DIMS
{
	SimROI   ROI;
	int      bandNum; 
	bool*    pBand;

	DIMS(int ixstart=-1, int ixend=-1, int iystart=-1, int iyend=-1, int num=-1,bool*pArray= NULL)
		:ROI(ixstart, ixend, iystart, iyend),bandNum(num), pBand(pArray)
		{}
		
	DIMS(SimROI roi, int num=-1,bool*pArray= NULL)
		:ROI(roi), bandNum(num), pBand(pArray)
		{}
		
	DIMS(DIMS& DimsCopy); 
	DIMS& operator = (const DIMS& muldimCopy);
	~DIMS();
	
	long        GetHeight()              {return ROI.GetHeight(); }; 
	long        GetWidth()               {return ROI.GetWidth(); };
	LONGLONG    GetImageSize()           {return ROI.GetHeight() * ROI.GetWidth() * bandNum; };
	long GetCacheLoc(int column, int line, int band)  {return band*GetWidth()*GetHeight() + (line-ROI.Ystart)*GetWidth() + (column - ROI.Xstart);};
	bool IsValid()                {return ROI.IsValid() && (bandNum >0) && (pBand != NULL);};
	
	void Reset()                                        
	{
		ROI.Reset(); 
		bandNum = -1;
		if(pBand != NULL)
			delete [] pBand;pBand=NULL;
	};
	
	
	int  GetBands()
	{
		if(bandNum <=0 && pBand ==NULL)
		  return 0;
		
		int bands=0;
		for(int i=0; i<bandNum; i++)
		{
			if(pBand[i])
				bands++;
		}
		return bands;
	};
	
	int*  GetBandArray()
	{
		int bands = GetBands();
		if(bands <= 0)
			return NULL;
		
		int *pArray = new int[bands];
		int index=0;
		for(int i=0; i<bands; i++)
		{
			if(pBand[i])
				pArray[index++] = i+1;
			
		}
		
		return pArray;
	};
	
	bool IsContain(long column,long line, long band) 
	{
		return       (ROI.Xstart <= column)
				&&   (ROI.Ystart <= line)
				&&   (ROI.Xend   > column)
				&&   (ROI.Yend   > line)
			//	&&   (pBand != NULL )
				&&   (bandNum >band);
			//	&&   (pBand[band]);
	};
	

	bool IsContain(SimDIMS *pSBlockInfo) 
	{
		if( (ROI.Xstart <= pSBlockInfo->ROI.Xstart)
			&&   (ROI.Ystart <= pSBlockInfo->ROI.Ystart)
			&&   (ROI.Xend   >= pSBlockInfo->ROI.Xend)
			&&   (ROI.Yend   >= pSBlockInfo->ROI.Yend)
			&&   (pBand != NULL ) && (pSBlockInfo->pBand != NULL)
			&&   (bandNum >= pSBlockInfo->bandNum) )
		{
			for(int i=0; i<pSBlockInfo->bandNum; i++)
			{
				if(pBand[i] != pSBlockInfo->pBand[i])
					return false;
			}
			
			return true;
		}
		
		return false;
	};

};


extern HIPVAR int		g_BlockSize;        //
extern HIPVAR int		g_BlockSizeW;       //
extern HIPVAR DIMS		g_Zdims;            //
extern HIPVAR int		g_SquareBorderSize; //

END_NAMESPACE(HAPLIB)

#endif /*HAPLIB_H_*/
