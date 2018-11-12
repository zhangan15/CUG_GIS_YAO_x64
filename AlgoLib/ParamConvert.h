/******************************************************************
	param_convert.h
	This head file defines the parameter convert functions

	Last modification on : Oct. 16, 2001
	Writen by : Zhao wei 
*******************************************************************/

#ifndef __PARAM_CONVERT_H
#define __PARAM_CONVERT_H

#ifdef _cplusplus
extern "C" {
#endif

    float	c_atof ( const char* s ) ;
    double	c_atod ( const char* s ) ;
    void	c_itoa ( char *s, int int_value ) ;
    void	c_ltoa ( char *s, long long_value ) ;

#ifdef _cplusplus
}
#endif

#endif
