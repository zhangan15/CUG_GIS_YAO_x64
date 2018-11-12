#ifndef HAPBEGTHREADROW_H
#define HAPBEGTHREADROW_H

#include "haplib.h"
#include "hapbegtbase.h"

class HAPCLASS HAPBEGThreadRow :
	public HAPBEGTBase
{
public:
	HAPBEGThreadRow(void);
	virtual ~HAPBEGThreadRow(void);
	virtual void* Read(long column,long line, long band);

protected:
	virtual bool InitReadCache();

		/************************************************************************/
	/* Independent Input Thread：独立写入线程，执行过程中与输出线程互斥。*/
	/************************************************************************/
	static void* IndependentInThread(void* dummy); //在读线程中负责读缓存区赋值, 受保护成员函数，外部无法调用
	/************************************************************************/
	/*  IndependentInThread内嵌函数，负责将数据载入缓存区，并反馈相关信息。
		return: false, 已经到达最后一块，完成获取磁盘数据任务。
				true，可能还需继续取数据。*/
	/************************************************************************/
	int DiskDataToBlcokIn();

    virtual  bool GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo);
	virtual  void GetReadBlockInfo(int blockNum, SimDIMS* pSBlockInfo);

	int     m_iValidReadLinesPerBlock;


};
#endif
