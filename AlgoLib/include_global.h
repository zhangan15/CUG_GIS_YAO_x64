#ifndef _INCLUDE_GLOBAL_H
#define _INCLUDE_GLOBAL_H

#ifndef Windows
#define Windows
#endif //Windows


/*
#ifndef Linux
#define Linux

#endif //Linux
*/

//common include file
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>


#ifdef Windows
//TODO: add the windows include file hear
#include <windows.h>
//#include <WinDef.h>
//#include <WTypes.h>
#endif //Windows

#ifdef Linux
//TODO: add the linux include file hear
#include <linux.h>
#endif //Linux

#include "basedefine_global.h"

#endif //_INCLUDE_GLOBAL_H
