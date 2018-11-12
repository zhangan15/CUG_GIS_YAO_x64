#ifndef STRING_ALGOLIB_H_
#define STRING_ALGOLIB_H_

// #include "include_global.h"
// #include "basedefine_global.h"
#include "Comlib.h"

class ALGOLIB_API stringALib
{
public:
	stringALib(void);// base construction
	~stringALib(void);// destroy matirx

	static void delspace(const char* instring, char* outstring);
};
#endif /*STRING_ALGOLIB_H_*/