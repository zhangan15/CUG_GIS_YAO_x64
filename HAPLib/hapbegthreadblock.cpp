#include "hapbegthreadblock.h"
#include <iostream>

HAPBEGThreadBlock::HAPBEGThreadBlock(void)
{
}

HAPBEGThreadBlock::~HAPBEGThreadBlock(void)
{
}

bool HAPBEGThreadBlock::InitReadCache()
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

	m_iReadSamplesPerBlock = iCol > g_SquareBorderSize ? g_SquareBorderSize : iCol;
	m_iReadLinesPerBlock   = iRow > g_SquareBorderSize ? g_SquareBorderSize : iRow;
	m_iReadBlocksPerWidth  = iCol / m_iReadSamplesPerBlock + ((iCol % m_iReadSamplesPerBlock ==0) ? 0 : 1);
	m_iReadBlocksPerHeight = iRow / m_iReadLinesPerBlock + ((iRow %m_iReadLinesPerBlock ==0)? 0 :1);
	m_iTotalReadBlock      = m_iReadBlocksPerWidth * m_iReadBlocksPerHeight;

	m_QueueLength = g_BlockSize/ (m_iReadSamplesPerBlock *m_iReadLinesPerBlock *iBand);
	m_QueueLength = m_iTotalReadBlock > m_QueueLength ? m_QueueLength : m_iTotalReadBlock;


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
