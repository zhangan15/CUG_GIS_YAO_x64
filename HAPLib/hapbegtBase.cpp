#include "hapbegtbase.h"
#include "hapmultiproc.h"
#include "pthread.h"
#include <iostream>


int HAPBEGTBase::mIOReferrence=0;
HapIOQueue HAPBEGTBase::mIOQueue = HapIOQueue();
bool HAPBEGTBase::m_bIOTheadRun = false;


void* HAPBEGTBase::IndependentIOThread(void* dummy) //统一的IO线程，实际的使用中，只需要一个IO线程
{
	return NULL;
}

HAPBEGTBase::HAPBEGTBase(void)
{
	m_iWriteBlockSize2 =-1;
	m_ppSBlockOutInf=NULL;   //Read Queue Cache 
	m_bTheadAbnormal=false;
	m_bAlarmInfo    =false;			//警告信息，仅提示用户一次。
	m_bNeedsDataNow =false;
}

HAPBEGTBase::~HAPBEGTBase(void)
{
}


void HAPBEGTBase::Close_MgrIn()
{

	if(m_MgrIn !=NULL)
	{
		m_MgrIn = NULL;

		while(-2 != m_iCurrentReadBlock) /*wait till all read thread closed*/
			HapSleep(5);
	}

	if(m_bBlockIn !=NULL)
	{
		mTotalReadCache--;
		mTotalMemory -=m_iReadSamplesPerBlock * m_iReadLinesPerBlock * m_DimsIn.GetBands() * m_nPtLength *m_QueueLength;
		delete []m_bBlockIn; m_bBlockIn=NULL;
	}


	if(m_ppSBlockInInf != NULL)
	{
		for(int i=0; i< m_QueueLength;i++)
		{
			if(m_ppSBlockInInf[i]!=NULL)
				delete m_ppSBlockInInf[i]; m_ppSBlockInInf[i]=NULL;
		}
		delete m_ppSBlockInInf;m_ppSBlockInInf=NULL;
	}
}

void HAPBEGTBase::Close_MgrOut()
{
	if(m_iWriteDoneCount > 0 && m_iCurrentWriteBlock==m_iTotalWriteBlock-1)
	{
		while(-1 != m_iCurrentWriteBlock)
			HapSleep(5);
	}

	HAPBEGBase::Close_MgrOut();
	if(m_ppSBlockOutInf != NULL)
	{
		for(int i=0; i< 2;i++)
		{
			if(m_ppSBlockOutInf[i]!=NULL)
				delete m_ppSBlockOutInf[i]; m_ppSBlockOutInf[i]=NULL;
		
		}
		delete m_ppSBlockOutInf;m_ppSBlockOutInf=NULL;
	}
}

int HAPBEGTBase::BlockOutToDiskData()
{
	if (m_bTheadAbnormal||m_MgrOut==NULL) //线程异常 或 线程结束的条件满足
	{
		m_iCurrentWriteBlock = -1;
		return false;
	}

	if (!m_bIOTheadRun)
	{
		if(m_iWriteBlockSize2)
		{
			m_bIOTheadRun = true;                        //开启IO线程锁
			m_MgrOut->WriteBlock(m_ppSBlockOutInf[1]);
			m_bIOTheadRun = false;                       //释放IO线程锁
			m_iWriteBlockSize2 = 0;                      // 一定要最后置0 [3/12/2012 liuxiang]
			return -1;                                    //不休眠
		}
		else if (m_iWriteDoneCount == m_iWriteBlockSize && m_iCurrentWriteBlock==m_iTotalWriteBlock-1)
		{
			m_bIOTheadRun = true; //开启IO线程锁
			m_MgrOut->WriteBlock(m_ppSBlockOutInf[0]);
			m_bIOTheadRun = false; //释放IO线程锁
			m_iCurrentWriteBlock = -1;
			return 0;
		}
	}
	return 1;
}




bool HAPBEGTBase::InitWriteCache()
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
		m_iWriteLinesPerBlock   = g_BlockSizeW/(iDT*iCol*iBand*2);
		m_iWriteBlocksPerWidth  = 1;

		m_iWriteBlocksPerHeight = iRow /m_iWriteLinesPerBlock \
			+ ((iRow%m_iWriteLinesPerBlock==0)?0:1);
		m_iTotalWriteBlock     =  m_iWriteBlocksPerWidth * m_iWriteBlocksPerHeight;

		try
		{
			m_bBlockOut = new BYTE[m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock * iBand *iDT *2];
			mTotalMemory +=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock * iBand *iDT;
			mTotalWriteCache++;
		}
		catch(std::bad_alloc& exc)
		{
			std::cerr<<"unable to allocate block for write\t"<<exc.what()<<std::endl;
			return false;
		}

		m_ppSBlockOutInf = new SimDIMS *[2];
		for(int i=0; i< 2; i++)
		{
			m_ppSBlockOutInf[i] = new SimDIMS;
			m_ppSBlockOutInf[i]->bandNum = iBand;
			m_ppSBlockOutInf[i]->pBand=	new bool[m_DimsOut.bandNum];
			memcpy(m_ppSBlockOutInf[i]->pBand, m_DimsOut.pBand, m_DimsOut.bandNum *sizeof(bool));
			m_ppSBlockOutInf[i]->pData = m_bBlockOut + i* m_iWriteSamplesPerBlock*m_iWriteLinesPerBlock *iBand *iDT;
		}

		m_iWriteDoneCount = 0;
		m_iCurrentWriteBlock = 0;

		GetWriteBlockInfo(m_iCurrentWriteBlock, m_ppSBlockOutInf[0]);
		m_iWriteBlockSize =m_ppSBlockOutInf[0]->GetCacheSize();
		m_iWriteBlockSize2=0;

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
		m_iWriteLinesPerBlock   = g_BlockSizeW/(iDT*iCol*2);
		m_iWriteBlocksPerWidth  = 1;

		m_iWriteBlocksPerHeight = iRow /m_iWriteLinesPerBlock \
		+ ((iRow%m_iWriteLinesPerBlock==0) ? 0:1);

		if(1 == m_iWriteBlocksPerHeight)
			m_iWriteLinesPerBlock = iRow;

		m_iTotalWriteBlock     =  m_iWriteBlocksPerWidth * m_iWriteBlocksPerHeight * iBand;

		try
		{
			m_bBlockOut = new BYTE[m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock  *iDT *2];
			mTotalMemory +=m_iWriteSamplesPerBlock * m_iWriteLinesPerBlock *iDT;
			mTotalWriteCache++;
		}
		catch(std::bad_alloc& exc)
		{
			std::cerr<<"unable to allocate block for write\t"<<exc.what()<<std::endl;
			return false;
		}

		m_ppSBlockOutInf = new SimDIMS *[2];
		for(int i=0; i< 2; i++)
		{
			m_ppSBlockOutInf[i] = new SimDIMS;
			m_ppSBlockOutInf[i]->pBand=	new bool[m_DimsOut.bandNum];
			memset(m_ppSBlockOutInf[i]->pBand, 0 , m_DimsOut.bandNum *sizeof(bool));

			m_ppSBlockOutInf[i]->pData = m_bBlockOut + i* m_iWriteSamplesPerBlock*m_iWriteLinesPerBlock  *iDT;
		}

		m_iWriteDoneCount = 0;
		m_iCurrentWriteBlock = 0;

		GetWriteBlockInfo(m_iCurrentWriteBlock, m_ppSBlockOutInf[0]);

		m_iWriteBlockSize =m_ppSBlockOutInf[0]->GetWidth() * m_ppSBlockOutInf[0]->GetHeight();
		m_iWriteBlockSize2=0;
	}

	pthread_t pid;
	pthread_create(&pid, NULL, IndependentOutThread, this);
	return true;
	
}

void* HAPBEGTBase::IndependentOutThread(void* dummy)
{
	HAPBEGTBase* pH = (HAPBEGTBase*)dummy;

	int states = -1;
	while(true)
	{
		states = pH->BlockOutToDiskData();
		if (states == 1)
		{
			HapSleep(5);
		} 
		else if(states == 0)
		{
			pthread_detach(pthread_self()); //清除占用的资源
			return NULL;
		}
	}
}

bool HAPBEGTBase::Write(void* pvalue)
{
	while(true)
	{
		if(m_iWriteDoneCount<m_iWriteBlockSize)
		{
			if(!mUseBSQ)
			{
				int band = m_iWriteDoneCount % m_ppSBlockOutInf[0]->GetBands();
				memcpy(m_ppSBlockOutInf[0]->pData+ (band * m_iWriteBlockSize +m_iWriteDoneCount)/ m_ppSBlockOutInf[0]->GetBands() * m_nPtLengthOut, pvalue, m_nPtLengthOut);
				m_iWriteDoneCount++;
				return true;
			}
			else
			{
				memcpy(mSBlockOutInf.pData+ m_iWriteDoneCount*m_nPtLengthOut, pvalue, m_nPtLengthOut);
				m_iWriteDoneCount++;

				
				return true;
			}
	
		}
		else if(m_iWriteBlockSize2==0) //表示2号缓存区没有执行写任务
		{
			if(!mUseBSQ)
			{
				//置换1、2缓冲区指针
				SimDIMS* pTemp      = m_ppSBlockOutInf[1];
				m_ppSBlockOutInf[1] = m_ppSBlockOutInf[0];
				m_ppSBlockOutInf[0] = pTemp;

				m_iWriteBlockSize2 = m_iWriteBlockSize; //执行写任务
				m_iCurrentWriteBlock++;
				GetWriteBlockInfo(m_iCurrentWriteBlock, m_ppSBlockOutInf[0]);
				m_iWriteBlockSize = m_ppSBlockOutInf[0]->GetCacheSize();
				memcpy(m_ppSBlockOutInf[0]->pData, pvalue, m_nPtLengthOut);
				m_iWriteDoneCount = 1;
				return true;
			}
			else
			{
				//置换1、2缓冲区指针
				SimDIMS* pTemp      = m_ppSBlockOutInf[1];
				m_ppSBlockOutInf[1] = m_ppSBlockOutInf[0];
				m_ppSBlockOutInf[0] = pTemp;

				m_iWriteBlockSize2 = m_iWriteBlockSize; //执行写任务

				m_iCurrentWriteBlock++;

				GetWriteBlockInfo(m_iCurrentWriteBlock,m_ppSBlockOutInf[0]);
				m_iWriteBlockSize = m_ppSBlockOutInf[0]->GetWidth() * m_ppSBlockOutInf[0]->GetHeight();

				memcpy(m_ppSBlockOutInf[0]->pData, pvalue, m_nPtLengthOut);

				m_iWriteDoneCount = 1;
				return true;
				
			}
			
		}
		else
		{
			HapSleep(5); //等待IO线程完成数据准备。时间设定的短一些对性能会有毫秒级的影响。1MS也可以。
		}
	}
}
