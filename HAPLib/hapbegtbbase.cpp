#include "hapbegtbase.h"

int HAPBEGTBase::mIOReferrence=0;
HapIOQueue HAPBEGTBase::mIOQueue = HapIOQueue();

void* HAPBEGTBase::IndependentIOThread(void* dummy) //ͳһ��IO�̣߳�ʵ�ʵ�ʹ���У�ֻ��Ҫһ��IO�߳�
{
	return NULL;
}

HAPBEGTBase::HAPBEGTBase(void)
{
}

HAPBEGTBase::~HAPBEGTBase(void)
{
}
