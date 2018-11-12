#ifndef HAPBEGTHREADBLOCK_H
#define HAPBEGTHREADBLOCK_H
#include "haplib.h"
#include "hapbegtbase.h"

class HAPCLASS HAPBEGThreadBlock :
	public HAPBEGTBase
{
public:
	HAPBEGThreadBlock(void);
	virtual ~HAPBEGThreadBlock(void);
protected:
	virtual bool InitReadCache();
};
#endif