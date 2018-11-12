#ifndef BASEDEFINE_GLOBAL_H_
#define BASEDEFINE_GLOBAL_H_

#ifndef HRESULT
#define HRESULT unsigned int
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef LONG
#define LONG long
#endif

#ifndef DOUBLE
#define DOUBLE double
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef BOOL
//#define BOOL unsigned int
#define BOOL int  // 必须与为微软的定义一致，要么不定义，否则在使用MFC的时候会提示很多错误。 [3/6/2012 liuxiang]
//参看文件“c:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\WinDef.h”
#endif

#ifndef USHORT
#define USHORT unsigned short
#endif

#ifndef UINT
#define UINT unsigned int
#endif


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef S_OK
#define S_OK 0
#endif

#ifndef S_FALSE
#define S_FALSE 1
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef HINSTANCE_
typedef void *HINSTANCE_;
#endif

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef PI
#define PI 3.14159265358979
#endif


//AT = pi / 180.0
#define AT (1.745329251994329500E-2)
//AT1 = 180/pi
#define AT1 57.29577951308232087

#ifndef MINUINT
#define MINUINT 0.00001
#endif

//版本号控制
#ifndef _VERSION_HAPLIB_NO_
#define VERSION_HAPLIB_NO 1.3
#define _VERSION_HAPLIB_NO_
#endif

#endif //BASEDEFINE_GLOBAL_H_

