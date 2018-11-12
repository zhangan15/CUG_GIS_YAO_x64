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
	/* Independent Output Thread����������̣߳�ִ�й�������д���̻߳��⡣*/
	/************************************************************************/
	static void* IndependentOutThread(void* dummy); //��д�߳��и���д���������, �ܱ�����Ա�������ⲿ�޷�����
	/************************************************************************/
	/*  IndependentInThread��Ƕ�����������������뻺�����������������Ϣ��
		return: false, �Ѿ��������һ�飬��ɻ�ȡ������������
				true�����ܻ������ȡ���ݡ�*/
	/************************************************************************/
	int BlockOutToDiskData();

    virtual bool InitWriteCache();

	int    m_iWriteBlockSize2;

	SimDIMS          **m_ppSBlockOutInf;   //Read Queue Cache 
	bool              m_bTheadAbnormal;
	bool              m_bAlarmInfo;			//������Ϣ������ʾ�û�һ�Ρ�
	bool              m_bNeedsDataNow;		//����ͼ��IO���߳�Ŀǰ���Զ����ݡ�
	static bool       m_bIOTheadRun;        //[12/5/2011 liuxiang]���ͼ��IO�߳����ڹ�������֪�����̲߳�Ҫ���д���IO������
	char              pcErrMessage[64];		// ���������ʾ��Ϣ�����ϲ����


	//Ϊ�Ժ��޸����Ľӿڣ���дӦ��������һ���̣߳�����Ӧ��ÿһ���඼��
	static void* IndependentIOThread(void* dummy); //ͳһ��IO�̣߳�ʵ�ʵ�ʹ���У�ֻ��Ҫһ��IO�߳�
												  //��ΪIO��׼��

	static int mIOReferrence;             //�����ж��Ƿ�ùر�/����IO�߳�

	static HapIOQueue mIOQueue;
};
#endif

