#ifndef  HAPBEGBASE_H
#define  HAPBEGBASE_H
#include "haplib.h"
#include "gdalfilemanager.h"


USING_NAMESPACE(HAPLIB)


class HAPCLASS HAPBEGBase  // HAP Block Engine Base
{
public:
	HAPBEGBase();
	virtual ~HAPBEGBase();

	CGDALFileManager* Get_MgrIn();
	CGDALFileManager* Get_MgrOut();

	virtual void Close_MgrOut();
	virtual void Close_MgrIn();

	virtual bool Set_MgrIn(CGDALFileManager *mgr, DIMS dims = g_Zdims, int iBlockOverlap=0);
	virtual bool Set_MgrOut(CGDALFileManager *mgr,const char* filename, bool bWaveMode = false);

	virtual void* Read(long column,long line,long band); //Read 默认按行读取

	virtual bool  Write(void* pvalue); //顺序写出数据

	void*         ReadL(long column,long line,long band); 

	static long   GetTotalMemory();

protected:
	virtual bool InitReadCache() = 0;
	virtual bool InitWriteCache();

	virtual bool         GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo);

	CGDALFileManager *m_MgrIn;

public:
	DIMS              m_DimsIn;        //information about input image

protected:
	int               m_iBlockOverlap; //用于设置缓存区的重叠，部分图像处理算法需要用到，如滤波、插值算法等
	int               m_QueueLength;   //队列长度
	BYTE*             m_bBlockIn;
	int               m_iReadLinesPerBlock;     //lines per block used for reading
	int               m_iReadSamplesPerBlock;   //samples per block used for reading
	int               m_iReadBlocksPerWidth;    //Blocks in width
	int               m_iReadBlocksPerHeight;   //Blocks in height
	int               m_iTotalReadBlock;
	int				  m_iCurrentReadBlock;
	int               m_iCurrentBlockNo;
	int               m_nPtLength;
	SimDIMS          **m_ppSBlockInInf;   //Read Queue Cache 


	CGDALFileManager *m_MgrOut;
	DIMS              m_DimsOut;  //information about output image
	BYTE*             m_bBlockOut;
	int               m_iWriteLinesPerBlock;
	int               m_iWriteSamplesPerBlock;
	int               m_iWriteBlocksPerWidth;
	int               m_iWriteBlocksPerHeight;
	int               m_iTotalWriteBlock;
	int               m_nPtLengthOut;
	int				  m_iCurrentWriteBlock;

	int               m_iWriteBlockSize;

	int               m_iWriteDoneCount;

	SimDIMS           mSBlockOutInf;  

	virtual void  GetWriteBlockInfo(int blockNum, SimDIMS* pSBlockInfo);

	virtual void  GetReadBlockInfo(int blockNum, SimDIMS* pSBlockInfo);

	int   GetBlockIndex(int col, int row, int band);

	bool mUseBSQ;

	//用于统计使用的内存，开辟的Block数
	static long      mTotalMemory;

	static long      mTotalWriteCache;

	static long      mTotalReadCache;
};

#endif  //HAPBEGBASE_H
