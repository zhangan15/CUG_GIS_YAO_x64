#include "hapbegbase.h"
#include <cmath>
#include <iostream>

long HAPBEGBase::mTotalMemory = 0;
long HAPBEGBase::mTotalWriteCache=0;
long HAPBEGBase::mTotalReadCache=0;

long   HAPBEGBase::GetTotalMemory()
{
	return mTotalMemory;
}

HAPBEGBase::HAPBEGBase()
{
	m_MgrIn= NULL;
	m_ppSBlockInInf=NULL;
	m_bBlockIn=NULL;
	m_MgrOut=NULL;
	m_bBlockOut=NULL;
}

HAPBEGBase::~HAPBEGBase()
{
	Close_MgrIn();
	Close_MgrOut();
}


//Read Limit The Range, Judge the Range.
void* HAPBEGBase::ReadL(long column,long line, long band)
{
	if (column<0) //采用镜像方式避免访问越界
		column = -column;
	if (column>=m_DimsIn.GetWidth())
		column = 2*m_DimsIn.GetWidth() - column - 1;
	if (line<0)
		line = -line;
	if (line>=m_DimsIn.GetHeight())
		line = 2*m_DimsIn.GetHeight() - line - 1;

	return Read(column, line, band); 
	// 由于在基类没有Read函数定义，调用ReadL时会自动调用继承类的Read函数。这样，继承类不用再定义ReadL [3/26/2012 liuxiang]
}

bool HAPBEGBase::Set_MgrIn(CGDALFileManager *mgr, DIMS dims , int iBlockOverlap)
{
	if (iBlockOverlap>8 || iBlockOverlap<0) //最高支持9*9模板
		return false;

	Close_MgrIn();
	m_iCurrentBlockNo = 0;
	m_MgrIn = mgr;

	if(m_MgrIn == NULL)
		return false;

	m_DimsIn = dims;
	m_nPtLength = m_MgrIn->m_header.GetBytesPerPt();
	m_iBlockOverlap = iBlockOverlap;

	return InitReadCache();
}

bool HAPBEGBase::Set_MgrOut(CGDALFileManager *mgr,const char* filename, bool bWaveMode )
{
	Close_MgrOut();
	mUseBSQ = bWaveMode;
	m_MgrOut = mgr;

	if(m_MgrOut == NULL)
		return false;

	m_MgrOut->m_header.InitDIMS(&m_DimsOut);
	m_nPtLengthOut  = m_MgrOut->m_header.GetBytesPerPt();

	if(filename==NULL)
		return false;

	if(!m_MgrOut->HapSaveImageAs(filename))
		return false;  

	return InitWriteCache();
}

void HAPBEGBase::Close_MgrIn()
{
	if(m_MgrIn !=NULL)
		m_MgrIn = NULL;

	if(m_ppSBlockInInf != NULL)
	{
		for(int i=0; i< m_QueueLength;i++)
		{
			if(m_ppSBlockInInf[i]!=NULL)
				delete m_ppSBlockInInf[i]; m_ppSBlockInInf[i]=NULL;
		}
		delete m_ppSBlockInInf;m_ppSBlockInInf=NULL;
	}

	if(m_bBlockIn !=NULL)
	{
		mTotalReadCache--;
		mTotalMemory -=m_iReadSamplesPerBlock * m_iReadLinesPerBlock * m_DimsIn.GetBands() * m_nPtLength *m_QueueLength;
		delete []m_bBlockIn; m_bBlockIn=NULL;
	}

}

void HAPBEGBase::Close_MgrOut()
{
	if(m_MgrOut !=NULL)
		 m_MgrOut = NULL;

	if(m_bBlockOut !=NULL)
	{
		mTotalWriteCache--;
		mTotalMemory -=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock * m_DimsOut.GetBands() *m_nPtLengthOut;;
		delete []m_bBlockOut; m_bBlockOut=NULL;
	}
}


CGDALFileManager* HAPBEGBase::Get_MgrIn()
{
	return m_MgrIn;
}

CGDALFileManager* HAPBEGBase::Get_MgrOut()
{
	return m_MgrOut;
}

bool HAPBEGBase::InitWriteCache()
{
	if(!mUseBSQ)
	{
		int iRow = m_DimsOut.GetHeight();
		int iCol = m_DimsOut.GetWidth();
		int iDT  = m_nPtLengthOut;
		int iBand= m_DimsOut.GetBands();

		if(m_bBlockOut!=NULL)
		{
			mTotalWriteCache--;
			mTotalMemory -=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock * m_DimsOut.GetBands() *m_nPtLengthOut;
			delete []m_bBlockOut; m_bBlockOut=NULL; 
		}

		m_iWriteSamplesPerBlock = iCol;
		m_iWriteLinesPerBlock   = g_BlockSizeW/(iDT*iCol*iBand);
		m_iWriteBlocksPerWidth  = 1;

		m_iWriteBlocksPerHeight = iRow /m_iWriteLinesPerBlock \
			+ ((iRow%m_iWriteLinesPerBlock==0)?0:1);

		if(1 == m_iWriteBlocksPerHeight)
			m_iWriteLinesPerBlock = iRow;

		m_iTotalWriteBlock     =  m_iWriteBlocksPerWidth * m_iWriteBlocksPerHeight;

		try
		{
			m_bBlockOut = new BYTE[m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock * iBand *iDT];
			mTotalMemory +=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock * iBand *iDT;
			mTotalWriteCache++;
		}
		catch(std::bad_alloc& exc)
		{
			std::cerr<<"unable to allocate block for write\t"<<exc.what()<<std::endl;
			return false;
		}


		m_iWriteDoneCount = 0;
		m_iCurrentWriteBlock=0;    

		mSBlockOutInf.bandNum = iBand;
		mSBlockOutInf.pBand=	new bool[m_DimsOut.bandNum];
		memcpy(mSBlockOutInf.pBand, m_DimsOut.pBand, m_DimsOut.bandNum *sizeof(bool));
		mSBlockOutInf.pData = m_bBlockOut;

		GetWriteBlockInfo(m_iCurrentWriteBlock, &mSBlockOutInf);

		m_iWriteBlockSize = mSBlockOutInf.GetCacheSize();

		return true;	
	}
	else
	{
		int iRow = m_DimsOut.GetHeight();
		int iCol = m_DimsOut.GetWidth();
		int iDT  = m_nPtLengthOut;
		int iBand= m_DimsOut.GetBands();

		if(m_bBlockOut!=NULL)
		{
			mTotalWriteCache--;
			mTotalMemory -=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock  *m_nPtLengthOut;
			delete []m_bBlockOut; m_bBlockOut=NULL; 
		}

		m_iWriteSamplesPerBlock = iCol;
		m_iWriteLinesPerBlock   = g_BlockSizeW/(iDT*iCol);
		m_iWriteBlocksPerWidth  = 1;

		m_iWriteBlocksPerHeight = iRow /m_iWriteLinesPerBlock \
			+ ((iRow%m_iWriteLinesPerBlock==0) ? 0:1);

		if(1 == m_iWriteBlocksPerHeight)
			m_iWriteLinesPerBlock = iRow;

		m_iTotalWriteBlock     =  m_iWriteBlocksPerWidth * m_iWriteBlocksPerHeight * iBand;

		try
		{
			m_bBlockOut = new BYTE[m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock *iDT];
			mTotalMemory +=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock  *iDT;
			mTotalWriteCache++;
		}
		catch(std::bad_alloc& exc)
		{
			std::cerr<<"unable to allocate block for write\t"<<exc.what()<<std::endl;
			return false;
		}


		m_iWriteDoneCount = 0;
		m_iCurrentWriteBlock=0;    

		mSBlockOutInf.bandNum = m_DimsOut.bandNum;
		mSBlockOutInf.pBand=	new bool[m_DimsOut.bandNum];
		memset(mSBlockOutInf.pBand, 0, m_DimsOut.bandNum *sizeof(bool));

		mSBlockOutInf.pData = m_bBlockOut;

		GetWriteBlockInfo(m_iCurrentWriteBlock, &mSBlockOutInf);

		m_iWriteBlockSize = mSBlockOutInf.GetWidth() *  mSBlockOutInf.GetHeight();

		return true;	
	}
	
}

bool  HAPBEGBase::Write(void* pvalue) //顺序写出数据
{
	if(!mUseBSQ)
	{
		int band = m_iWriteDoneCount % mSBlockOutInf.GetBands();
		//int bandBlockSize = mSBlockOutInf.GetWidth()* mSBlockOutInf.GetHeight();
		//int placeholder = (band * m_iWriteBlockSize +m_iWriteDoneCount)/ mSBlockOutInf.GetBands()*m_nPtLengthOut;

		memcpy(mSBlockOutInf.pData+  m_iWriteBlockSize / mSBlockOutInf.GetBands()* band * m_nPtLengthOut +m_iWriteDoneCount/ mSBlockOutInf.GetBands()*m_nPtLengthOut, pvalue, m_nPtLengthOut);

		m_iWriteDoneCount++;

		if(m_iWriteDoneCount== m_iWriteBlockSize)
		{
			m_MgrOut->WriteBlock(&mSBlockOutInf);

			m_iCurrentWriteBlock++;
	
			GetWriteBlockInfo(m_iCurrentWriteBlock, &mSBlockOutInf);

			m_iWriteDoneCount=0;

			m_iWriteBlockSize=mSBlockOutInf.GetCacheSize();
		}
		return true;
	}
	else
	{
		memcpy(mSBlockOutInf.pData+ m_iWriteDoneCount*m_nPtLengthOut, pvalue, m_nPtLengthOut);

		m_iWriteDoneCount++;

		if(m_iWriteDoneCount== m_iWriteBlockSize)
		{
			m_MgrOut->WriteBlock(&mSBlockOutInf);

			m_iCurrentWriteBlock++;

			GetWriteBlockInfo(m_iCurrentWriteBlock, &mSBlockOutInf);

			m_iWriteDoneCount=0;
			m_iWriteBlockSize=mSBlockOutInf.GetWidth() * mSBlockOutInf.GetHeight();
		}
		return true;
	}
	
}


void  HAPBEGBase::GetWriteBlockInfo(int blockNum, SimDIMS* pSBlockInfo)
{
	if(blockNum >= m_iTotalWriteBlock)// Write is Finished
		return;

	if(!mUseBSQ)
	{
		int iRow = m_DimsOut.GetHeight();
		int iCol = m_DimsOut.GetWidth();

		int  iWidthBlockNum  = blockNum % m_iWriteBlocksPerWidth;
		int  iHeightBlockNum = blockNum / m_iWriteBlocksPerWidth;

		pSBlockInfo->ROI.Xstart = iWidthBlockNum * m_iWriteSamplesPerBlock; 
		if(iWidthBlockNum==m_iWriteBlocksPerWidth-1)
			pSBlockInfo->ROI.Xend = iCol;
		else
			pSBlockInfo->ROI.Xend = pSBlockInfo->ROI.Xstart + m_iWriteSamplesPerBlock;

		pSBlockInfo->ROI.Ystart = iHeightBlockNum * m_iWriteLinesPerBlock; 
		if(iHeightBlockNum==m_iWriteBlocksPerHeight-1)
			pSBlockInfo->ROI.Yend = iRow;
		else
			pSBlockInfo->ROI.Yend = pSBlockInfo->ROI.Ystart + m_iWriteLinesPerBlock;

		pSBlockInfo->blockNo = blockNum;
	}
	else
	{
		int iRow = m_DimsOut.GetHeight();
		int iCol = m_DimsOut.GetWidth();

		int blockBand = blockNum / (m_iWriteBlocksPerWidth * m_iWriteBlocksPerHeight);

		blockNum = blockNum % (m_iWriteBlocksPerWidth * m_iWriteBlocksPerHeight);

		memset(pSBlockInfo->pBand, 0, m_DimsOut.bandNum *sizeof(bool));
		int* pArray = m_DimsOut.GetBandArray();
		mSBlockOutInf.pBand[pArray[blockBand]-1] = true;

		int  iWidthBlockNum  = blockNum % m_iWriteBlocksPerWidth;
		int  iHeightBlockNum = blockNum / m_iWriteBlocksPerWidth;

		pSBlockInfo->ROI.Xstart = iWidthBlockNum * m_iWriteSamplesPerBlock; 

		if(iWidthBlockNum==m_iWriteBlocksPerWidth-1)
			pSBlockInfo->ROI.Xend = iCol;
		else
			pSBlockInfo->ROI.Xend = pSBlockInfo->ROI.Xstart + m_iWriteSamplesPerBlock;

		pSBlockInfo->ROI.Ystart = iHeightBlockNum * m_iWriteLinesPerBlock; 

		if(iHeightBlockNum==m_iWriteBlocksPerHeight-1)
			pSBlockInfo->ROI.Yend = iRow;
		else
			pSBlockInfo->ROI.Yend = pSBlockInfo->ROI.Ystart + m_iWriteLinesPerBlock;

		pSBlockInfo->blockNo = blockNum;
	}
}


void* HAPBEGBase::Read(long column,long line,long band) //Read 默认按行读取
{
	column -= m_DimsIn.ROI.Xstart;
	line   -= m_DimsIn.ROI.Ystart;

	BYTE* pBBlockIn=NULL;
	for(int i=0;i<m_iCurrentBlockNo;i++)
	{
		if(m_ppSBlockInInf[i]->IsContain(column, line, band))
		{
			pBBlockIn=m_ppSBlockInInf[i]->pData;
			return &pBBlockIn[m_ppSBlockInInf[i]->GetCacheLoc(column,line,band)*m_nPtLength]; 
		}
	}

	if (m_iCurrentBlockNo<m_QueueLength)
		m_iCurrentBlockNo++;

	//缓存信息队列地址轮换一圈,开始的时候转小圈，最后转大圈。
	SimDIMS* pSDIMTemp = m_ppSBlockInInf[m_iCurrentBlockNo-1]; 
	for(int i=m_iCurrentBlockNo-2;i>=0;i--)
	{
		m_ppSBlockInInf[i+1] = m_ppSBlockInInf[i];
	}
	m_ppSBlockInInf[0] = pSDIMTemp;

	GetBlockInfo(column,line,band,m_ppSBlockInInf[0]);

	m_MgrIn->GetDIMSDataByBlockInfo(&m_DimsIn, m_ppSBlockInInf[0]);
	pBBlockIn=m_ppSBlockInInf[0]->pData;

	return &pBBlockIn[m_ppSBlockInInf[0]->GetCacheLoc(column,line,band)*m_nPtLength]; 
}


int  HAPBEGBase::GetBlockIndex(int col, int row, int band)
{
	return row/m_iReadLinesPerBlock  * m_iReadBlocksPerWidth + col/m_iReadSamplesPerBlock;
}


void  HAPBEGBase::GetReadBlockInfo(int blockNum, SimDIMS* pMBlockInfo)
{
	int iRow = m_DimsIn.GetHeight();
	int iCol = m_DimsIn.GetWidth();

	int  iWidthBlockNum  = blockNum % m_iReadBlocksPerWidth;
	int  iHeightBlockNum = blockNum / m_iReadBlocksPerWidth;

	pMBlockInfo->ROI.Xstart = iWidthBlockNum * m_iReadSamplesPerBlock; 
	if(iWidthBlockNum==m_iReadBlocksPerWidth-1)
		pMBlockInfo->ROI.Xend = iCol;
	else
		pMBlockInfo->ROI.Xend = pMBlockInfo->ROI.Xstart + m_iReadSamplesPerBlock;

	pMBlockInfo->ROI.Ystart = iHeightBlockNum * m_iReadLinesPerBlock; 
	if(iHeightBlockNum==m_iReadBlocksPerHeight-1)
		pMBlockInfo->ROI.Yend = iRow;
	else
		pMBlockInfo->ROI.Yend = pMBlockInfo->ROI.Ystart + m_iReadLinesPerBlock;

	pMBlockInfo->blockNo = blockNum;
}

bool  HAPBEGBase::GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo)
{
	if(!m_DimsIn.IsValid())
		return false;
	if(pSBlockInfo == NULL)
		return false;

	int iCol = m_DimsIn.GetWidth();
	int iRow = m_DimsIn.GetHeight();

	if (m_iReadSamplesPerBlock >= iCol)
	{
		pSBlockInfo->ROI.Xstart = 0;
		pSBlockInfo->ROI.Xend = iCol;
	}
	else 
	{
		int NOBlock =  col / m_iReadSamplesPerBlock; 
		pSBlockInfo->ROI.Xstart = NOBlock * m_iReadSamplesPerBlock;  //col%iSamplesPerBlock
		if(NOBlock==m_iReadBlocksPerWidth-1)
			pSBlockInfo->ROI.Xend = iCol;
		else
			pSBlockInfo->ROI.Xend = pSBlockInfo->ROI.Xstart + m_iReadSamplesPerBlock;
	}

	if (m_iReadLinesPerBlock >= iRow)
	{
		pSBlockInfo->ROI.Ystart = 0;
		pSBlockInfo->ROI.Yend = iRow;
	}
	else 
	{
		int NOBlock =  row / m_iReadLinesPerBlock; 
		pSBlockInfo->ROI.Ystart = NOBlock * m_iReadLinesPerBlock;  //col%iSamplesPerBlock
		if(NOBlock==m_iReadBlocksPerHeight-1)
			pSBlockInfo->ROI.Yend = iRow;
		else
			pSBlockInfo->ROI.Yend = pSBlockInfo->ROI.Ystart + m_iReadLinesPerBlock;
	}

	pSBlockInfo->blockNo = GetBlockIndex(row,col,band);

	return true;
}
