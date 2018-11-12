#ifndef _INCLUDE_GLOBAL_H
#define _INCLUDE_GLOBAL_H

//common include file
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>


#ifdef WIN32
#define WINDOWS
#endif // WIN32

#ifdef WIN64
#define WINDOWS
#endif // WIN64

#ifdef _WINDOWS
#define WINDOWS
#endif // _WINDOWS

#ifdef _WINDOWS_
#define WINDOWS
#endif // _WINDOWS_

#ifdef WINDOWS
//TODO: add the windows include file hear
#include <Windows.h>
#endif //WINDOWS


#ifdef _LINUX
#define LINUX
#endif // _LINUX

#ifdef LINUX
//TODO: add the linux include file hear
#endif //Linux


#include "basedefine_global.h"
#endif //_INCLUDE_GLOBAL_H
