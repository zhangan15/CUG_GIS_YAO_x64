#ifndef HAPIOQUEUE_H
#define HAPIOQUEUE_H

#include <queue>
#include "haplib.h"

//Version 1, don't consider block mode, forget about it!!!
USING_NAMESPACE(HAPLIB)

class HAPCLASS HapIOQueue
{
	struct  IOWork 
	{
		SimDIMS* pSBlockInfo;
		bool     InorOut;

		IOWork(SimDIMS*  pSblockInfo,     bool inorout)
			:pSBlockInfo(pSblockInfo), InorOut(inorout)
		{

		};
	};

public:
	HapIOQueue(void);
	~HapIOQueue(void);

	void    enQueue(SimDIMS*  pSBlockInfo, bool  InOrOut);
	void    deQueue(SimDIMS*& pSBlockInfo, bool &InOrOut);
	bool    isEmpty();

private:
	 std::queue<IOWork> mIOWorkQueue;

};
#endif
