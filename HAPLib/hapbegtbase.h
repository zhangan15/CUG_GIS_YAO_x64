#ifndef HAPBEGTBASE_H
#define HAPBEGTBASE_H

#include "haplib.h"
#include "hapbegbase.h"
#include "hapioqueue.h"

USING_NAMESPACE(HAPLIB)


class HAPCLASS HAPBEGTBase :
	public HAPBEGBase
{
public:
	HAPBEGTBase(void);
	virtual ~HAPBEGTBase(void);
	virtual void Close_MgrIn();
	virtual void Close_MgrOut();

	bool Write(void* pvalue);

protected:

	/************************************************************************/
	/* Independent Output Thread：独立输出线程，执行过程中与写入线程互斥。*/
	/************************************************************************/
	static void* IndependentOutThread(void* dummy); //在写线程中负责写缓存区输出, 受保护成员函数，外部无法调用
	/************************************************************************/
	/*  IndependentInThread内嵌函数，负责将数据载入缓存区，并反馈相关信息。
		return: false, 已经到达最后一块，完成获取磁盘数据任务。
				true，可能还需继续取数据。*/
	/************************************************************************/
	int BlockOutToDiskData();

    virtual bool InitWriteCache();

	int    m_iWriteBlockSize2;

	SimDIMS          **m_ppSBlockOutInf;   //Read Queue Cache 
	bool              m_bTheadAbnormal;
	bool              m_bAlarmInfo;			//警告信息，仅提示用户一次。
	bool              m_bNeedsDataNow;		//唤醒图像IO读线程目前可以读数据。
	static bool       m_bIOTheadRun;        //[12/5/2011 liuxiang]标记图像IO线程正在工作，告知其他线程不要进行磁盘IO操作。
	char              pcErrMessage[64];		// 保存错误提示信息。供上层调用


	//为以后修改留的接口，读写应该整体是一个线程，而不应该每一个类都开
	static void* IndependentIOThread(void* dummy); //统一的IO线程，实际的使用中，只需要一个IO线程
												  //来为IO作准备

	static int mIOReferrence;             //用来判断是否该关闭/开启IO线程

	static HapIOQueue mIOQueue;
};
#endif

