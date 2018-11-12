#ifndef HAPBEGBLOCK_H
#define HAPBEGBLOCK_H

#include "haplib.h"
#include "hapbegbase.h"

class HAPCLASS HAPBEGBlock :
	public HAPBEGBase
{
public:
	HAPBEGBlock(void);
	virtual ~HAPBEGBlock(void);
protected:
	virtual bool InitReadCache();
};

#endif
