
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PARAMDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PARAMDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef __PARAM_CLASS_H
#define __PARAM_CLASS_H

#include "Comlib.h"

//for linux x64
//#define Linux_x64
//#define DIR_CHAR		"/"
//for windows x64
//#define Win_x64

//for linux x32
//#define Linux_x32

//for windows x32
//#define Win_x32
#define WIN
#define DIR_CHAR "\\"

#define _cplusplus

// This class is exported from the ParamDll.dll
/* define item for parameter */
typedef struct
{
    char* name ;
    char* value ;
	char* comment ;
} PARAMETER_ITEM ;
    
/* define parameter item link */
typedef struct p_list PARAMETER_LIST ;
struct p_list
{
    PARAMETER_ITEM item ;
    PARAMETER_LIST *next ;
} ;


#ifdef WIN
class ALGOLIB_API PList
#endif

#ifdef Linux_x64
class PList
#endif
{
    private :
        PARAMETER_LIST *head ;	/* head of the link */
		PARAMETER_LIST *tail ;	/* tail of the link */
		int  _add  ( char* name, char* value ) ;	/* add a new item in the link */
		void _expr ( char* title, char* buf ) ;		/* expression analysis */
        
		void _sort ( int sortmode ) ;		/* item sort */
		/* 0 --- ascend, 1 --- descend */
    
	public :
        PList () ;	// constructor
		~PList () ;	// destructor
		int Remove ( char* name ) ;			// remove a parameter item from the list
		int MakeList ( char* paramfile ) ;	// make a parameter list from a parameter file
		void Output () ;					// print a parameter list
		void Clear () ;						// remove all the parameter item 

		////////////////////////////////////////////////////////////////////////
		//
#ifdef WIN
//		int GetValue ( char* name, __int8* iv, int number = 1) ;		
		int GetValue ( char* name, __int16* iv, int number = 1) ;
//		int GetValue ( char* name, __int32* iv, int number = 1) ;
		int GetValue ( char* name, __int64* iv, int number = 1) ;		
		int GetValue ( char* name, unsigned __int8* iv, int number = 1) ;		
		int GetValue ( char* name, unsigned __int16* iv, int number = 1) ;
//		int GetValue ( char* name, unsigned __int32* iv, int number = 1) ;
		int GetValue ( char* name, unsigned __int64* iv, int number = 1) ;		
		int GetValue ( char* name, unsigned int* iv, int number = 1) ;		
		int GetValue ( char* name, unsigned long* iv, int number = 1) ;		


		void SetValue ( char* name, __int8* iv, int number = 1) ;		
		void SetValue ( char* name, __int16* iv, int number = 1) ;
//		void SetValue ( char* name, __int32* iv, int number = 1) ;
		void SetValue ( char* name, __int64* iv, int number = 1) ;		
		void SetValue ( char* name, unsigned __int8* iv, int number = 1) ;		
		void SetValue ( char* name, unsigned __int16* iv, int number = 1) ;
//		void SetValue ( char* name, unsigned __int32* iv, int number = 1) ;
		void SetValue ( char* name, unsigned __int64* iv, int number = 1) ;		
		void SetValue ( char* name, unsigned int* iv, int number = 1) ;		
		void SetValue ( char* name, unsigned long* iv, int number = 1) ;	

		int ChangeValue ( char* name, __int8* iv, int number = 1) ;		
		int ChangeValue ( char* name, __int16* iv, int number = 1) ;
//		int ChangeValue ( char* name, __int32* iv, int number = 1) ;
		int ChangeValue ( char* name, __int64* iv, int number = 1) ;		
		int ChangeValue ( char* name, unsigned __int8* iv, int number = 1) ;		
		int ChangeValue ( char* name, unsigned __int16* iv, int number = 1) ;
//		int ChangeValue ( char* name, unsigned __int32* iv, int number = 1) ;
		int ChangeValue ( char* name, unsigned __int64* iv, int number = 1) ;		
		int ChangeValue ( char* name, unsigned int* iv, int number = 1) ;		
		int ChangeValue ( char* name, unsigned long* iv, int number = 1) ;	
		//
		////////////////////////////////////////////////////////////////////////

		
		int GetValue ( char* name, int* iv, int number = 1) ;		// get value from an expression
		int GetValue ( char* name, long* lv, int number = 1) ;
		int GetValue ( char* name, float* fv, int number = 1) ;
		int GetValue ( char* name, double* dv, int number = 1) ;
		int GetValue ( char* name, char* sv ) ;
		int GetValue ( char* name, char* sv[], int number ) ;

   int  MakeFile ( char* paramfile ) ;	// make a parameter file from a parameter list
		void SetValue ( char* name, int* iv, int number = 1 ) ;	// set value 
		void SetValue ( char* name, long* lv, int number = 1 ) ;
		void SetValue ( char* name, float* fv, int number = 1  ) ;
		void SetValue ( char* name, double* dv, int number = 1  ) ;
		void SetValue ( char* name, char* sv ) ;
		void SetValue ( char* name, char* sv[], int number ) ;

		int ChangeValue ( char* name, int* iv, int number = 1 ) ;	// Change value 
		int ChangeValue ( char* name, long* lv, int number = 1 ) ;
		int ChangeValue ( char* name, float* fv, int number = 1  ) ;
		int ChangeValue ( char* name, double* dv, int number = 1  ) ;
		int ChangeValue ( char* name, char* sv ) ;
		int ChangeValue ( char* name, char* sv[], int number ) ;
#endif
		
#ifdef Linux_x64
//		int GetValue ( char* name, char* iv, int number = 1) ;		
		int GetValue ( char* name, short* iv, int number = 1) ;
		int GetValue ( char* name, int* iv, int number = 1) ;
		int GetValue ( char* name, long* iv, int number = 1) ;		
//		int GetValue ( char* name, unsigned char* iv, int number = 1) ;		
		int GetValue ( char* name, unsigned short* iv, int number = 1) ;
		int GetValue ( char* name, unsigned int* iv, int number = 1) ;
		int GetValue ( char* name, unsigned long* iv, int number = 1) ;		


	//	void SetValue ( char* name, char* iv, int number = 1) ;		
		void SetValue ( char* name, short* iv, int number = 1) ;
		void SetValue ( char* name, int* iv, int number = 1) ;
		void SetValue ( char* name, long* iv, int number = 1) ;		
		void SetValue ( char* name, unsigned char* iv, int number = 1) ;		
		void SetValue ( char* name, unsigned short* iv, int number = 1) ;
		void SetValue ( char* name, unsigned int* iv, int number = 1) ;
		void SetValue ( char* name, unsigned long* iv, int number = 1) ;		


//		int ChangeValue ( char* name, char* iv, int number = 1) ;		
		int ChangeValue ( char* name, short* iv, int number = 1) ;
		int ChangeValue ( char* name, int* iv, int number = 1) ;
		int ChangeValue ( char* name, long* iv, int number = 1) ;		
//  	int ChangeValue ( char* name, unsigned char* iv, int number = 1) ;		
		int ChangeValue ( char* name, unsigned short* iv, int number = 1) ;
		int ChangeValue ( char* name, unsigned int* iv, int number = 1) ;
		int ChangeValue ( char* name, unsigned long* iv, int number = 1) ;		

		//2005��11��10�����ǿ����������
		////////////////////////////////////////////////////////////////////////

		
		int GetValue ( char* name, float* fv, int number = 1) ;
		int GetValue ( char* name, double* dv, int number = 1) ;
		int GetValue ( char* name, char* sv ) ;
		int GetValue ( char* name, char* sv[], int number ) ;

		int  MakeFile ( char* paramfile ) ;	// make a parameter file from a parameter list

		void SetValue ( char* name, float* fv, int number = 1  ) ;
		void SetValue ( char* name, double* dv, int number = 1  ) ;
		void SetValue ( char* name, char* sv ) ;
		void SetValue ( char* name, char* sv[], int number ) ;


		int ChangeValue ( char* name, float* fv, int number = 1  ) ;
		int ChangeValue ( char* name, double* dv, int number = 1  ) ;
		int ChangeValue ( char* name, char* sv ) ;
		int ChangeValue ( char* name, char* sv[], int number ) ;
#endif
		int  AddComment ( char* name, char* comment ) ;		// add comments to the parameter
} ;

#endif
