#include "hapbegrow.h"
#include <iostream>

HAPBEGRow::HAPBEGRow(void)
{
}

HAPBEGRow::~HAPBEGRow(void)
{
}


bool HAPBEGRow::InitReadCache()
{
	int iRow = m_DimsIn.GetHeight();
	int iCol = m_DimsIn.GetWidth();
	int iDT  = m_nPtLength;
	int iBand = m_DimsIn.GetBands(); 

	if(m_bBlockIn != NULL)
	{
		mTotalMemory -=m_iReadSamplesPerBlock * m_iReadLinesPerBlock * iBand * iDT *m_QueueLength;
		mTotalReadCache--;
		delete [] m_bBlockIn; m_bBlockIn=NULL;
	}


	m_QueueLength = 2;
			
	m_iReadSamplesPerBlock = iCol;
	m_iReadLinesPerBlock   = g_BlockSize/(m_QueueLength *iCol*iBand*iDT);

	//block is too small ,the overlap block is more than that!!!
	if(m_iBlockOverlap >= m_iReadLinesPerBlock)
		return false;

	m_iValidReadLinesPerBlock = m_iReadLinesPerBlock-m_iBlockOverlap;

	m_iReadBlocksPerWidth  = 1;
	if(iRow >= m_iBlockOverlap)
		m_iReadBlocksPerHeight = (iRow-m_iBlockOverlap)/m_iValidReadLinesPerBlock + (((iRow-m_iBlockOverlap) % m_iValidReadLinesPerBlock ==0) ? 0 : 1);
	else
		m_iReadBlocksPerHeight =1;
	
	m_iTotalReadBlock  = m_iReadBlocksPerWidth * m_iReadBlocksPerHeight;


	try
	{
		m_bBlockIn =  new BYTE[m_iReadSamplesPerBlock * m_iReadLinesPerBlock * iBand * iDT *m_QueueLength];
		mTotalMemory +=m_iReadSamplesPerBlock * m_iReadLinesPerBlock * iBand * iDT *m_QueueLength;
		mTotalReadCache++;
	}
	catch(std::bad_alloc& exc)
	{
		std::cerr<<"unable to allocate block for read\t"<<exc.what()<<std::endl;
		return false;
	}

	m_ppSBlockInInf = new SimDIMS *[m_QueueLength];
	for(int i=0; i< m_QueueLength; i++)
	{
		m_ppSBlockInInf[i] = new SimDIMS;
		m_ppSBlockInInf[i]->bandNum = iBand;
		m_ppSBlockInInf[i]->pBand=	new bool[m_DimsIn.bandNum];
		memcpy(m_ppSBlockInInf[i]->pBand, m_DimsIn.pBand, m_DimsIn.bandNum *sizeof(bool));
		m_ppSBlockInInf[i]->pData = m_bBlockIn + i* m_iReadSamplesPerBlock*m_iReadLinesPerBlock *iBand *iDT;
	}
	return true;
}

bool  HAPBEGRow::GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo)
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
		int NOBlock =  row / m_iValidReadLinesPerBlock; 
		pSBlockInfo->ROI.Ystart = NOBlock * m_iValidReadLinesPerBlock; 
		if(NOBlock==m_iReadBlocksPerHeight-1)
			pSBlockInfo->ROI.Yend = iRow;
		else
			pSBlockInfo->ROI.Yend = pSBlockInfo->ROI.Ystart + m_iReadLinesPerBlock;
	}

	pSBlockInfo->blockNo = row / m_iValidReadLinesPerBlock;
	//pSBlockInfo->bandNum = m_DimsIn.bandNum;
	//if(pSBlockInfo->pBand != NULL)
	//	delete []pSBlockInfo->pBand; pSBlockInfo->pBand=NULL;

	//pSBlockInfo->pBand=	new bool[m_DimsIn.bandNum];
	//memcpy(pSBlockInfo->pBand, m_DimsIn.pBand, m_DimsIn.bandNum *sizeof(bool));
	return true;
}

