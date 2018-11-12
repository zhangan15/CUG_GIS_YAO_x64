#include "hapbegthreadrow.h"
#include "hapmultiproc.h"
#include "pthread.h"
#include <iostream>

HAPBEGThreadRow::HAPBEGThreadRow(void)
{
}

HAPBEGThreadRow::~HAPBEGThreadRow(void)
{
}



bool HAPBEGThreadRow::InitReadCache()
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

	//m_iCurrentReadBlock = 0;//���Զ�Ԥ����0Block
	//m_bNeedsDataNow = true;
	pthread_t pid;
	pthread_create(&pid, NULL, IndependentInThread, this);
	return true;
}

void* HAPBEGThreadRow::IndependentInThread(void* dummy)
{
	HAPBEGThreadRow* pH = (HAPBEGThreadRow*)dummy;

	int states = -1;
	while(true)
	{
		states = pH->DiskDataToBlcokIn();
		if (states == 1)
		{
			HapSleep(5);
		} 
		else if(states == 0)
		{
			pthread_detach(pthread_self());
			return NULL;
		}
	}
}


int HAPBEGThreadRow::DiskDataToBlcokIn()
{
	SimDIMS SDBlckInfoTemp; 

	if (m_bTheadAbnormal||m_MgrIn==NULL) 
	{
		m_iCurrentReadBlock = -2;
		return false;
	} 

	if(!m_bIOTheadRun && m_bNeedsDataNow)
	{
		if(m_iCurrentReadBlock<m_iTotalReadBlock)
		{
			GetReadBlockInfo(m_iCurrentReadBlock, m_ppSBlockInInf[1]);

			m_bIOTheadRun = true;//����IO�߳���
			m_MgrIn->GetDIMSDataByBlockInfo(&m_DimsIn, m_ppSBlockInInf[1]);
			m_bIOTheadRun = false;  //�ͷ�IO�߳���

			m_iCurrentReadBlock++;
			m_bNeedsDataNow = false; //��������鶼�Ѿ�װ������
		}
		else   //m_iCurrentBlockNo����Խ�磬��������ͼ����ϡ�
		{
			m_iCurrentReadBlock = -1;
			m_bNeedsDataNow = false;
		}
		return -1; //������
	}

	return true;
}

void* HAPBEGThreadRow::Read(long column,long line, long band)
{
	column -= m_DimsIn.ROI.Xstart;
	line   -= m_DimsIn.ROI.Ystart;

	while(true)
	{
		if(m_ppSBlockInInf[0]->IsContain(column, line, band))
			return &m_ppSBlockInInf[0]->pData[m_ppSBlockInInf[0]->GetCacheLoc(column,line,band)*m_nPtLength]; 
		else if(!m_bNeedsDataNow && m_ppSBlockInInf[1]->IsContain(column, line, band))
		{
			SimDIMS* pTemp     = m_ppSBlockInInf[1];
			m_ppSBlockInInf[1] = m_ppSBlockInInf[0];
			m_ppSBlockInInf[0] = pTemp;

			m_bNeedsDataNow = true;

			return &m_ppSBlockInInf[0]->pData[m_ppSBlockInInf[0]->GetCacheLoc(column,line,band)*m_nPtLength]; 
		}
		else
		{
			if (m_bNeedsDataNow == true)
			{	
					HapSleep(5); //�ȴ�IO�߳��������׼����//ʱ���趨�Ķ�һЩ�����ܻ��к��뼶��Ӱ�졣1MSҲ���ԡ�
			}
			else //�û�д���㷨û�а��ն�ȡ˳����룬IO���߳�����װ���������������
			{
				m_iCurrentReadBlock = GetBlockIndex(column, line, band);
				if(m_iCurrentReadBlock == -1)
				{
					strcpy(pcErrMessage, "��ȡͼ��ΧԽ�磡");
					m_bTheadAbnormal = true;
					return NULL;
				}
				m_bNeedsDataNow = true;
				if (m_bAlarmInfo)
				{
					strcpy(pcErrMessage, "���밴ָ��˳���ȡͼ�񡣶��̼߳������ִ�У���Ч���޷���֤��");
					m_bAlarmInfo = false;
				}
			}
		}
	}
}

bool  HAPBEGThreadRow::GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo)
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
//	pSBlockInfo->blockNo = 
	//if(pSBlockInfo->pBand != NULL)
	//	delete []pSBlockInfo->pBand; pSBlockInfo->pBand=NULL;

	//pSBlockInfo->pBand=	new bool[m_DimsIn.bandNum];
	//memcpy(pSBlockInfo->pBand, m_DimsIn.pBand, m_DimsIn.bandNum *sizeof(bool));
	return true;
}



void  HAPBEGThreadRow::GetReadBlockInfo(int blockNum, SimDIMS* pSBlockInfo)
{
	if(!m_DimsIn.IsValid())
		return ;
	if(pSBlockInfo == NULL)
		return ;

	int iCol = m_DimsIn.GetWidth();
	int iRow = m_DimsIn.GetHeight();

	if (m_iReadSamplesPerBlock >= iCol)
	{
		pSBlockInfo->ROI.Xstart = 0;
		pSBlockInfo->ROI.Xend = iCol;
	}
	else 
	{
		int NOBlock =  blockNum %  m_iReadSamplesPerBlock; 
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
		int NOBlock =  blockNum; 
		pSBlockInfo->ROI.Ystart = NOBlock * m_iValidReadLinesPerBlock; 
		if(NOBlock==m_iReadBlocksPerHeight-1)
			pSBlockInfo->ROI.Yend = iRow;
		else
			pSBlockInfo->ROI.Yend = pSBlockInfo->ROI.Ystart + m_iReadLinesPerBlock;
	}

	//pSBlockInfo->bandNum = m_DimsIn.bandNum;
	pSBlockInfo->blockNo = blockNum;

	//if(pSBlockInfo->pBand != NULL)
	//	delete []pSBlockInfo->pBand; pSBlockInfo->pBand=NULL;

	//pSBlockInfo->pBand=	new bool[m_DimsIn.bandNum];
	//memcpy(pSBlockInfo->pBand, m_DimsIn.pBand, m_DimsIn.bandNum *sizeof(bool));
	//return true;
}