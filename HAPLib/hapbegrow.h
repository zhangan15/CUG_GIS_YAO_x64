#ifndef HAPBEGROW_H
#define HAPBEGROW_H
#include "haplib.h"
#include "hapbegbase.h"

class HAPCLASS HAPBEGRow :
	public HAPBEGBase
{
public:
	HAPBEGRow(void);
	virtual ~HAPBEGRow(void);

protected:
	virtual bool InitReadCache();
    virtual bool GetBlockInfo(int col, int row, int band, SimDIMS* pSBlockInfo);
	int     m_iValidReadLinesPerBlock;
};
#endif
