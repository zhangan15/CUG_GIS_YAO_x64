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
	/* Independent Input Thread������д���̣߳�ִ�й�����������̻߳��⡣*/
	/************************************************************************/
	static void* IndependentInThread(void* dummy); //�ڶ��߳��и������������ֵ, �ܱ�����Ա�������ⲿ�޷�����
	/************************************************************************/
	/*  IndependentInThread��Ƕ�����������������뻺�����������������Ϣ��
		return: false, �Ѿ��������һ�飬��ɻ�ȡ������������
				true�����ܻ������ȡ���ݡ�*/
	/************************************************************************/
	int DiskDataToBlcokIn();

    virtual  bool GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo);
	virtual  void GetReadBlockInfo(int blockNum, SimDIMS* pSBlockInfo);

	int     m_iValidReadLinesPerBlock;


};
#endif
