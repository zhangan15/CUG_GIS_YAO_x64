#include "hapbegtbase.h"

int HAPBEGTBase::mIOReferrence=0;
HapIOQueue HAPBEGTBase::mIOQueue = HapIOQueue();

void* HAPBEGTBase::IndependentIOThread(void* dummy) //统一的IO线程，实际的使用中，只需要一个IO线程
{
	return NULL;
}

HAPBEGTBase::HAPBEGTBase(void)
{
}

HAPBEGTBase::~HAPBEGTBase(void)
{
}
