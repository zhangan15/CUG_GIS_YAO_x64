#ifndef HAPBEGCOLUMN_H
#define HAPBEGCOLUMN_H
#include "haplib.h"
#include "hapbegbase.h"

class HAPCLASS HAPBEGColumn :
	public HAPBEGBase
{
public:
	HAPBEGColumn(void);
	virtual ~HAPBEGColumn(void);
protected:
	virtual bool InitReadCache();
};

#endif

