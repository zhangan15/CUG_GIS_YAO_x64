#ifndef BASEDEFINE_GLOBAL_H_
#define BASEDEFINE_GLOBAL_H_

#ifndef BASETYPES  //prevent window define these macro again!!
#define BASETYPES
#endif 

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
#define BOOL  int
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

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

#ifndef PI
#define PI 3.14159265358979
#endif

#endif //BASEDEFINE_GLOBAL_H_

