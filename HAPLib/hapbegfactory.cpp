#include "hapbegfactory.h"
#include "hapbegrow.h"
#include "hapbegcolumn.h"
#include "hapbegblock.h"
#include "hapbegthreadrow.h"
#include "hapbegthreadcolumn.h"
#include "hapbegthreadblock.h"


HAPBEGBase* HAPBEGFactory::CreateHapBegInstance(ReadMode mode ,bool bUseMutithread)
{
	switch(mode)
	{
	case RowPriority:
		if(bUseMutithread)
			//return new HAPBEGThreadRow();
			return new HAPBEGRow();
		else
			return new HAPBEGRow();
	case ColumnPriority:
		if(bUseMutithread)
			//return new HAPBEGThreadColumn();
			return new HAPBEGColumn();
		else
			return new HAPBEGColumn();
	case BlockPriority:
		if(bUseMutithread)
			//return new HAPBEGThreadBlock();
			return new HAPBEGBlock();
		else
			return new HAPBEGBlock();
	}
	return NULL;
}