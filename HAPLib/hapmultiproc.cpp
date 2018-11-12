#include "haplib.h"
#include "hapmultiproc.h"

#ifdef WINDOWS
#include <windows.h>
void HapSleep( double dfwMilliseconds )
{
	Sleep( (DWORD) dfwMilliseconds);
}
#else
#include <time.h>
void HapSleep( double dfwMilliseconds )
{
	struct timespec sRequest, sRemain;

	sRequest.tv_sec = (int) floor(dfWaitInSeconds);
	sRequest.tv_nsec = (int) ((dfWaitInSeconds - sRequest.tv_sec)*1000000);
	nanosleep( &sRequest, &sRemain );
}
#endif