#ifndef HAPBEGFACTORY_H
#define HAPBEGFACTORY_H
#include "haplib.h"
enum ReadMode
{
	RowPriority,
	ColumnPriority,
	BlockPriority
};

class HAPBEGBase;
class HAPCLASS HAPBEGFactory
{
public:
	static HAPBEGBase* CreateHapBegInstance(ReadMode mode ,bool bUseMutithread);
};
#endif