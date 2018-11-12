/******************************************************************
	param_portable.h
	This head file is used for port the source code to different 
	compiler and operating system

	Last modification on : Oct. 16, 2001
	Writen by : Zhao wei 
*******************************************************************/

#ifndef __PORTABLE_H
#define	__PORTABLE_H

//#if	defined(Linux)
//	#define	STRCASECMP(str1,str2) strcasecmp(str1,str2)
//
//	#include <unistd.h>
//	#define ACCESS(fn,mode) access(fn,mode)
//#endif
//
//#if	defined(Windows)
	#define STRCASECMP(str1,str2) _stricmp(str1,str2)
	
	#include <stdio.h>
	#define R_OK	4
	#define W_OK	2
	#define ACCESS(fn,mode) _access(fn,mode)
//#endif

#endif