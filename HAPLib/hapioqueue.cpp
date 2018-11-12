#include "hapioqueue.h"

HapIOQueue::HapIOQueue(void)
{
}

HapIOQueue::~HapIOQueue(void)
{
}


void    HapIOQueue::enQueue(SimDIMS*  pSBlockInfo, bool  InOrOut)
{
	mIOWorkQueue.push(IOWork(pSBlockInfo, InOrOut));

}
void    HapIOQueue::deQueue(SimDIMS*& pSBlockInfo, bool &InOrOut)
{
	IOWork worker = mIOWorkQueue.front();
	mIOWorkQueue.pop();

	pSBlockInfo = worker.pSBlockInfo;
	InOrOut     = worker.InorOut;
}

bool    HapIOQueue::isEmpty()
{
	return mIOWorkQueue.empty();
}