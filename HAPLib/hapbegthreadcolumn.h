#ifndef HAPBEGTHREADCOLUMN_H
#define HAPBEGTHREADCOLUMN_H
#include "haplib.h"
#include "hapbegtbase.h"

class HAPCLASS HAPBEGThreadColumn :
	public HAPBEGTBase
{
public:
	HAPBEGThreadColumn(void);
	virtual ~HAPBEGThreadColumn(void);
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
};
#endif

