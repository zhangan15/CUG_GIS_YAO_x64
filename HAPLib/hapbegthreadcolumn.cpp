#include "hapbegthreadcolumn.h"
#include "hapmultiproc.h"
#include "pthread.h"
#include <iostream>


HAPBEGThreadColumn::HAPBEGThreadColumn(void)
{
}

HAPBEGThreadColumn::~HAPBEGThreadColumn(void)
{
}

bool HAPBEGThreadColumn::InitReadCache()
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
	m_iReadSamplesPerBlock = g_BlockSize/(m_QueueLength *iRow*iBand*iDT);
	m_iReadLinesPerBlock   =  iRow;
	m_iReadBlocksPerWidth  = iCol / m_iReadSamplesPerBlock + ((iCol % m_iReadSamplesPerBlock ==0) ? 0 : 1);
	m_iReadBlocksPerHeight = 1;
	m_iTotalReadBlock      = m_iReadBlocksPerWidth * m_iReadBlocksPerHeight;

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

	m_iCurrentReadBlock = 0;//会自动预加载0Block
	m_bNeedsDataNow = true;
	pthread_t pid;
	pthread_create(&pid, NULL, IndependentInThread, this);
	return true;
}


void* HAPBEGThreadColumn::IndependentInThread(void* dummy)
{
	HAPBEGThreadColumn* pH = (HAPBEGThreadColumn*)dummy;

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
			//pthread_exit(NULL);
			pthread_detach(pthread_self());
			return NULL;
		}
	}
}


int HAPBEGThreadColumn::DiskDataToBlcokIn()
{
	SimDIMS SDBlckInfoTemp; 

	if (m_bTheadAbnormal||m_MgrIn==NULL) 
	{
		m_iCurrentReadBlock = -1;
		return false;
	} 

	if(!m_bIOTheadRun && m_bNeedsDataNow)
	{
		if(m_iCurrentReadBlock==0)
		{
			GetReadBlockInfo(m_iCurrentBlockNo, m_ppSBlockInInf[0]);

			m_bIOTheadRun = true;//开启IO线程锁
			m_MgrIn->GetDIMSDataByBlockInfo(&m_DimsIn, m_ppSBlockInInf[0]);
			m_bIOTheadRun = false;  //释放IO线程锁

			m_iCurrentReadBlock++;

		}
		else if(m_iCurrentReadBlock<m_iTotalReadBlock)
		{
			GetReadBlockInfo(m_iCurrentReadBlock, m_ppSBlockInInf[1]);

			m_bIOTheadRun = true;//开启IO线程锁
			m_MgrIn->GetDIMSDataByBlockInfo(&m_DimsIn, m_ppSBlockInInf[1]);
			m_bIOTheadRun = false;  //释放IO线程锁

			m_iCurrentReadBlock++;
			m_bNeedsDataNow = false; //两个缓存块都已经装载数据
		}
		else   //m_iCurrentBlockNo计数越界，遍历输入图像完毕。
		{
			m_iCurrentReadBlock = -1;
			return false;
		}
		return -1; //不休眠
	}

	return true;
}

void* HAPBEGThreadColumn::Read(long column,long line, long band)
{
	while(true)
	{
		if(m_ppSBlockInInf[0]->IsContain(column, line, band))
			return &m_ppSBlockInInf[0]->pData[m_ppSBlockInInf[0]->GetCacheLoc(column,line,band)*m_nPtLength]; 
		else if(!m_bNeedsDataNow && m_ppSBlockInInf[1]->IsContain(column, line, band))
		{
			SimDIMS* pTemp      = m_ppSBlockInInf[0];
			m_ppSBlockInInf[1] = m_ppSBlockInInf[0];
			m_ppSBlockInInf[0] = pTemp;

			m_bNeedsDataNow = true;
			return &m_ppSBlockInInf[0]->pData[m_ppSBlockInInf[0]->GetCacheLoc(column,line,band)*m_nPtLength]; 
		}
		else
		{
			if (m_bNeedsDataNow == true)
			{	
				HapSleep(5); //等待IO线程完成数据准备。//时间设定的短一些对性能会有毫秒级的影响。1MS也可以。
			}
			else //用户写的算法没有按照读取顺序读入，IO读线程重新装载两个缓存的数据
			{
				m_bTheadAbnormal = false;
				m_iCurrentReadBlock = GetBlockIndex(column, line, band);
				if(m_iCurrentReadBlock == -1)
				{
					strcpy(pcErrMessage, "读取图像范围越界！");
					m_bTheadAbnormal = true;
					return NULL;
				}
				m_bNeedsDataNow = true;
				if (m_bAlarmInfo)
				{
					strcpy(pcErrMessage, "必须按指定顺序读取图像。多线程计算继续执行，但效率无法保证！");
					m_bAlarmInfo = false;
				}
			}
		}
	}
}
