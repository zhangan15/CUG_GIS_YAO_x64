// ParamDll.cpp : Defines the entry point for the DLL application.
//
#ifdef WIN
#include "stdafx.h"
#endif
#include "ParamDll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include <math.h>

#include "ParamPortable.h"
#include "ParamConvert.h"

#define	MAX_LEVEL			8
#define	MAX_ITEMSTRING		512
#define MAX_LINEBUF			4096
#define MAX_DATABUF			4096
#define MAX_COMMENT			1024
#define IEEE_FLOAT_DIGIT	7
#define IEEE_DOUBLE_DIGIT	16
#define COMMENT_MARK		'#'

using namespace std;

void SeparateLevel ( char* ss, char level[MAX_LEVEL][MAX_ITEMSTRING] ) ;
void FormatArray ( int add_n_blank, char *s1, char *s2 ) ;

// constructor : initial the first node
PList :: PList ()
{
    head = new PARAMETER_LIST ;
// the first node is completely empty !
    head->item.name = NULL ;
    head->item.value = NULL ;
	head->item.comment = NULL ;
    head->next= NULL ;
    tail = head ;
}
    
// destructor : remove all the nodes and free the memory
PList :: ~PList ()
{
    PARAMETER_LIST *p ;
    
    while ( head )
    {
        p = head->next ;

		if ( head->item.name != NULL ) 
		{
			delete head->item.name ;
			head->item.name = NULL;
		}

		if ( head->item.value != NULL ) 
		{
			delete head->item.value ;
			head->item.value = NULL;
		}

		if ( head->item.comment != NULL ) 
		{
			delete head->item.comment ;
			head->item.comment = NULL;
		}

		delete head ;
		head = p ;
	}
}

// _add : add a new node with the given name and value 
int PList :: _add ( char* name, char* value ) 
{
    int retval ;
    PARAMETER_LIST *p ;
    
    retval = 1 ;
    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )	
// can not add it because the item is already exit
		{
			retval = 0 ;
			break ;
	    }
		p = p->next ;    
	}
    if ( retval == 1 ) 
    {
        p = new PARAMETER_LIST ;
        p->item.name = new char[strlen(name)+1] ;
        p->item.value = new char[strlen(value)+1] ;
        strcpy ( p->item.name, name ) ;
        strcpy ( p->item.value, value ) ;
		p->item.comment = NULL ;
        p->next = NULL ;
        tail->next = p ; 
        tail = p ;
	}
    return retval ;
}
    
// Remove : remove the node with the given name 
int PList :: Remove ( char* name ) 
{
    int retval ;
    PARAMETER_LIST *p, *pp ;
    
    retval = 0 ;

    p = head ;
    pp = p->next ;
    while ( pp )
    {
        if ( STRCASECMP(pp->item.name,name) == 0 )
// string compared with case-insensisive mode !
		{
			p->next = pp->next ;
			if ( pp->item.name != NULL ) 
			{
				delete pp->item.name ;
				pp->item.name = NULL;
			}

			if ( pp->item.value != NULL ) 
			{
				delete pp->item.value ;
				pp->item.value = NULL;
			}

			if ( pp->item.comment != NULL ) 
			{
				delete pp->item.comment ;
				pp->item.comment = NULL;
			}

			delete pp ;
			retval = 1 ;	// remove node success
			break ;
	    }
		p = p->next ;
		pp = p->next ;
	}

    return retval ;
}

// Clear : clear the whole parameter list, except for the first empty node 
void PList :: Clear ()
{
    PARAMETER_LIST *p ;
    
    p = head->next ;
    while ( p )
    {
        head->next = p->next ;
		if ( p->item.name != NULL ) 
		{
			delete p->item.name ;
			p->item.name = NULL;
		}

		if ( p->item.value != NULL ) 
		{
			delete p->item.value ;
			p->item.value = NULL;
		}

		if ( p->item.comment != NULL ) 
		{
			delete p->item.comment ;
			p->item.comment = NULL;
		}

		delete p ;
		p = head->next ;
	}
    tail = head ;

	if ( head->item.comment != NULL ) 
	{
		delete head->item.comment ;
		head->item.comment = NULL;
	}

	head->next = NULL ;
}    

// _expr : parameter expressiong analysis, using recursion method
//		title : (char*) high level title
//		buf : (buf) expression buffer
void PList :: _expr ( char* title, char* buf )
{
    char	*nbuf, *vbuf ;	// nbuf: name buffer, vbuf: value buffer
    int		size, nsize, vsize ;
    
    int		ip, jp ;
    char	*bbuf ;			// buffer pointer switch between name buffer and value buffer
    char	c ;
    int		qcount ;		// quote sign count
    
    size = strlen ( buf ) ;
    nsize = strlen ( title ) + size ;
    vsize = size ;
    nbuf = new char[nsize+1] ;
    vbuf = new char[vsize+1] ;
    
    strcpy ( nbuf, title ) ;
    jp = strlen ( nbuf ) ;
    if ( jp != 0 )
    {
        strcat ( nbuf, "." ) ;
		jp++ ;
	}

    bbuf = nbuf ;
    qcount = 0 ;
    ip = 0 ;
    while ( ip < size )
    {
        c = buf[ip] ;
		switch ( c ) 
		{
			case COMMENT_MARK :		// defines the comment line,  
				if ( qcount == 0 )
                {
					while ( buf[++ip] != '\n' ) { ; }
                    ip++ ;
                }
				break ;
			case ' ' :
			case '\t' :
			case '[' :
			case ']' :
			case '\n' :
			case '\r' :
				if ( qcount == 1 ) bbuf[jp++] = c ;
				ip++ ;
				break ;
				
			case '\"' :
				qcount = ( qcount + 1 ) % 2 ;
                bbuf[jp++] = c ;
				ip++ ;
				break ; 
			case '{' :		// starting of the below level
				if ( qcount == 1 ) bbuf[jp++] = c ;
				else
				{
					int nn ;	// level of {}

					jp = 0 ; nn = 0 ;
					c = buf[++ip] ;
					while ( (c!='}') || ((c=='}')&&(qcount==1)) || ((c=='}')&&(nn!=0)) )
					{
						vbuf[jp++] = c ;
						if ( c == '\"' ) qcount = ( qcount + 1 ) % 2 ;
						if ( c == '{' ) nn++ ;
						if ( c == '}' ) nn-- ;
						if ( (c==COMMENT_MARK) && (qcount==0) )
						{
							while ( buf[++ip] != '\n' ) { ; }
							jp-- ;
						}
						c = buf[++ip] ;
						if ( ip > size ) break ;
					}
					vbuf[jp] = '\0' ;
					_expr ( nbuf, vbuf ) ;
					bbuf = nbuf ;
					jp = strlen ( title ) ;
					if ( jp != 0 ) jp++ ;
				}
				ip++ ;
				break ;
			case '=' :		// switch from name buffer to value buffer 
				if ( qcount == 1 ) bbuf[jp++] = c ;
				else
				{
					bbuf[jp] = '\0' ;
					jp = 0 ;
					bbuf = vbuf ;
				}
                ip++ ;
				break ;
			case ';' :		// end of one parameter define 
				if ( qcount == 1 ) bbuf[jp++] = c ;
				else
				{
					bbuf[jp] = '\0' ;
					_add ( nbuf, vbuf ) ;		// add the parameter into the list 
                    strcpy ( nbuf, title ) ;	// start the next one 
					jp = strlen ( nbuf ) ;
					if ( jp != 0 )
					{
						strcat ( nbuf, "." ) ;
						jp++ ;
					}
					bbuf = nbuf ;
					strcpy ( vbuf, "" ) ;
				}
				ip++ ;
				break ;
			default :
				bbuf[jp++] = c ;
				ip++ ;
				break ;
		}
	}

    delete nbuf;
    delete vbuf;

}
		    
// MakeList : make the parameter list from a parameter file
// Input auguments :
//		paramfile : (char*) name and the full path of the parameter file
// Return value :
//		(int) : make list result --- 1 for success, 0 for fail 
int PList :: MakeList ( char* paramfile ) 
{
    FILE	*fp_param ;
    char	*fbuf ;
    int		fsize ;
    int		retval ;
    int		checkval ;
    
    retval = 1 ;
    fp_param = fopen ( paramfile, "r" ) ;
// open the parameter file in text mode, 
// which means the unprintable charactors in the file are filtered out
    if ( fp_param == NULL )
    {
        retval = 0 ;
		return retval ;
	}

    fseek ( fp_param, 0L, SEEK_END ) ;
    fsize = ftell ( fp_param ) + 3 ;
    fbuf = new char[fsize] ;
// get the byte length of the file, including the unprintable char
    
    fseek ( fp_param, 0L, SEEK_SET ) ;
    checkval = fread ( fbuf, 1, fsize, fp_param ) ;
    fclose ( fp_param ) ;
    fbuf[checkval] = '\0' ;
// "checkval" : the actual read char number, skipping the unprintable char 
	strcat ( fbuf, "\n" ) ;
    
    _expr ( "", fbuf ) ;

	delete fbuf;

    return retval ;
}
    
// Output : retrieve the parameter list and print out the parameter items
void PList :: Output ()
{
    PARAMETER_LIST *p ;
    
    p = head->next ;
    while ( p ) 
    {
        cout << p->item.name << " = " << p->item.value << "\n" ;
		p = p->next ;
	}
}
    
// GetValue : get the specific parameter value from the parameter list
// this is a multipoly function for different data type
// Input arguments :
//		name : (char*) name of the parameter
//		v : (data_type*) return parameter value 
//		number : (int) number of the value
// Return value :
//		(int) : get value result: 1 for find the parameter and return value, 0 for fail
int PList :: GetValue ( char* name, int* iv, int number ) 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    char	buf[16] ;
    int		i, j ;
    int		vcount, vlen ;
    char	c ;
    
    p = head->next ;
    retval = 0 ;
// search for the parameter
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
			vlen = strlen ( p->item.value ) ;
			value = new char[vlen+1] ;
			strcpy ( value, p->item.value ) ;
			retval = 1 ;
			break ;
	    }
		p = p->next ;
	}
// get the value of the parameter     
	if ( retval == 1 )
    {
		i = j = 0 ;
		vcount = 0 ;
		while ( (vcount<number) && (i<vlen) )
		{
			c = value[i++] ;
			if ( (c=='+') || ( c=='-') || ((c>='0')&&(c<='9')) ) buf[j++] = c ;
			if ( (c==',') || (i==vlen) )
			{
				buf[j] = '\0' ;
				j = 0 ;
				*(iv+vcount) = atoi ( buf ) ;
				vcount++ ;
			}
	    }
	    delete value ;
	}
    
    return retval ;
}
    
// see above 
int PList :: GetValue ( char* name, long* lv, int number ) 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    char	buf[32] ;
    int		i, j ;
    int		vcount, vlen ;
    char	c ;
    
    p = head->next ;
    retval = 0 ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    vlen = strlen ( p->item.value ) ;
			value = new char[vlen+1] ;
		    strcpy ( value, p->item.value ) ;
		    retval = 1 ;
			break ;
	    }
		p = p->next ;
	}
    if ( retval == 1 )
    {
		i = j = 0 ;
		vcount = 0 ;
		while ( (vcount<number) && (i<vlen) )
		{
			c = value[i++] ;
			if ( (c=='+') || ( c=='-') || ((c>='0')&&(c<='9')) ) buf[j++] = c ;
		    if ( (c==',') || (i==vlen) )
		    {
			    buf[j] = '\0' ;
				j = 0 ;
				*(lv+vcount) = atol ( buf ) ;
				vcount++ ;
			}
	    }
	    delete value ;
	}
    
    return retval ;
}

// see above
int PList :: GetValue ( char* name, float* fv, int number ) 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    char	buf[32] ;
    int		i, j ;
    int		vcount, vlen ;
    char	c ;
    
    p = head->next ;
    retval = 0 ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    vlen = strlen ( p->item.value ) ;
			value = new char[vlen+1] ;
		    strcpy ( value, p->item.value ) ;
		    retval = 1 ;
			break ;
	    }
		p = p->next ;
	}
    if ( retval == 1 )
    {
		i = j = 0 ;
		vcount = 0 ;
		while ( (vcount<number) && (i<vlen) )
		{
			c = value[i++] ;
		    switch ( c ) 
		    {
			    case '+' :
				case '-' :
		        case '.' :
		        case 'e' :
			    case 'E' :
				case '0' :
		        case '1' :
		        case '2' :
			    case '3' :
				case '4' :
		        case '5' :
		        case '6' :
			    case '7' :
				case '8' :
		        case '9' :
		            buf[j++] = c ;
				    break ;
				case ',' :
			        buf[j] = '\0' ;
				    j = 0 ;
				    *(fv+vcount) = (float)atof ( buf ) ;
				    vcount++ ;
				    break ;
			}
		    if ( i == vlen )
			{
				buf[j] = '\0' ;
				*(fv+vcount) = (float)atof ( buf ) ;
				vcount++ ;
			}
	    }
	    delete value ;
	}
    
    return retval ;
}

// see above
int PList :: GetValue ( char* name, double* dv, int number ) 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    char	buf[64] ;
    int		i, j ;
    int		vcount, vlen ;
    char	c ;
    
    p = head->next ;
    retval = 0 ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    vlen = strlen ( p->item.value ) ;
			value = new char[vlen+1] ;
		    strcpy ( value, p->item.value ) ;
		    retval = 1 ;
			break ;
	    }
		p = p->next ;
	}
    if ( retval == 1 )
    {
		i = j = 0 ;
		vcount = 0 ;
		while ( (vcount<number) && (i<vlen) )
		{
		    c = value[i++] ;
			switch ( c ) 
		    {
		        case '+' :
			    case '-' :
				case '.' :
		        case 'e' :
		        case 'E' :
			    case '0' :
				case '1' :
		        case '2' :
		        case '3' :
			    case '4' :
				case '5' :
		        case '6' :
		        case '7' :
			    case '8' :
				case '9' :
					buf[j++] = c ;
				    break ;
				case ',' :
					buf[j] = '\0' ;
				    j = 0 ;
				    *(dv+vcount) = atof ( buf ) ;
				    vcount++ ;
				    break ;
			}
		    if ( i == vlen )
			{
				buf[j] = '\0' ;
				*(dv+vcount) = atof ( buf ) ;
				vcount++ ;
			}
	    }
	    delete value ;
	}
    
    return retval ;
}

// see above
int PList :: GetValue ( char* name, char* sv ) 
{
    int		retval ;
    PARAMETER_LIST *p ;
    int		i, j, slen ;
	char	c ;
    
    p = head->next ;
    retval = 0 ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    strcpy ( sv, p->item.value ) ;
			i = j = 0 ; 
            slen = strlen ( sv ) ;
			while ( i < slen ) 
			{
				c = sv[i++] ;
				if ( c == '\"' ) 
				{
					sv[i-1] = 0;
					continue ;	// skip the quotation mark (")
				}

				sv[j++] = c ;
            }
			sv[j] = '\0' ;
			retval = 1 ;
			break ;
	    }
		p = p->next ;
	}

    return retval ;
}

int PList :: GetValue ( char* name, char *sv[], int number ) 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    int		ip, jp ;
    int		vcount, vlen ;
    int		qcount ;
    char	cc ;
    
    p = head->next ;
    retval = 0 ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    vlen = strlen ( p->item.value ) ;
		    value = new char[vlen+1] ;
			strcpy ( value, p->item.value ) ;
		    retval = 1 ;
		    break ;
	    }
		p = p->next ;
	}
    if ( retval == 1 )
    {
		ip = jp = 0 ;
		vcount = 0 ;
        qcount = 0 ;
		while ( (vcount<number) && (ip<vlen) )
		{
			cc = value[ip++] ;
		    if ( cc == '\"' )
		    {
			    qcount = ( qcount + 1 ) % 2 ; 
				continue ;
			}
		    if ( (cc==',') && (qcount==0) )
		    {
			    sv[vcount][jp] = '\0' ;
				vcount++ ;
				jp = 0 ;
			}
		    else
			    sv[vcount][jp++] = cc ;
	    }
        sv[number-1][jp] = '\0' ;
	    delete value ;
	}
    
    return retval ;
}

// SetValue : set the parameter item to the parameter list
// this is a multipoly function for different data type
// Input arguments :
//		name : (char*) name of the parameter
//		v : (data_type*) return parameter value 
//		number : (int) number of the value
// Return value :
//		none
void PList :: SetValue ( char* name, int* iv, int number ) 
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_itoa ( buf, *(iv+i) ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}

// see above
void PList :: SetValue ( char* name, long* lv, int number ) 
{
    char*	value ;
    char	buf[32] ;
    int		i ;
    
    value = new char[number * 32] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_ltoa ( buf, *(lv+i) ) ;
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
			strcat ( value, buf ) ;
		    if ( i != number-1 ) strcat ( value, "," ) ;
		}
	}
    _add ( name, value ) ;
    delete value ;
}

// see above
void PList :: SetValue ( char* name, float* fv, int number ) 
{
    char*	value ;
    char	buf[32] ;
    int		i ;
    
    value = new char[number * 32] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        gcvt ( *(fv+i), IEEE_FLOAT_DIGIT, buf ) ;
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;
    delete value ;
}

// see above
void PList :: SetValue ( char* name, double* dv, int number ) 
{
    char*	value ;
    char	buf[64] ;
    int		i ;
    
    value = new char[number * 64] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        gcvt ( *(dv+i), IEEE_DOUBLE_DIGIT, buf ) ;
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
			strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;
    delete value ;
}

// see above
void PList :: SetValue ( char* name, char* sv ) 
{
    char*	value ;

    value = new char[strlen(sv)+3] ;
    strcpy ( value, "\"" ) ;
    strcat ( value, sv ) ;
    strcat ( value, "\"" ) ;
    _add ( name, value ) ;
    delete value ;
}
    
// see above
void PList :: SetValue ( char* name, char* sv[], int number ) 
{
    char*	value ;
    int		i ;
    int		slen ;
    
    slen = 0 ;
    for ( i=0; i<number; i++ ) slen += strlen(sv[i]) + 3 ;
    
    value = new char[slen] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        strcat ( value, "\"" ) ;
		strcat ( value, sv[i] ) ;
		strcat ( value, "\"" ) ;
		if ( i != number-1 ) strcat ( value, "," ) ;
	}
    _add ( name, value ) ;
    delete value ;
}
    
// ChangeValue : change the parameter item to the parameter list
// this is a multipoly function for different data type
// Input arguments :
//		name : (char*) name of the parameter
//		v : (data_type*) return parameter value 
//		number : (int) number of the value
// Return value :
//		none
int PList :: ChangeValue ( char* name, int* iv, int number ) 
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_itoa ( buf, *(iv+i) ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

// see above
int PList :: ChangeValue ( char* name, long* lv, int number ) 
{
    char*	value ;
    char	buf[32] ;
    int		i ;
    PARAMETER_LIST *p ;
    
    value = new char[number * 32] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_ltoa ( buf, *(lv+i) ) ;
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
			strcat ( value, buf ) ;
		    if ( i != number-1 ) strcat ( value, "," ) ;
		}
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

// see above
int PList :: ChangeValue ( char* name, float* fv, int number ) 
{
    char*	value ;
    char	buf[32] ;
    int		i ;
    PARAMETER_LIST *p ;
    
    value = new char[number * 32] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        gcvt ( *(fv+i), IEEE_FLOAT_DIGIT, buf ) ;
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

// see above
int PList :: ChangeValue ( char* name, double* dv, int number ) 
{
    char*	value ;
    char	buf[64] ;
    int		i ;
    PARAMETER_LIST *p ;
    
    value = new char[number * 64] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        gcvt ( *(dv+i), IEEE_DOUBLE_DIGIT, buf ) ;
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
			strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

// see above
int PList :: ChangeValue ( char* name, char* sv ) 
{
    char*	value ;
    PARAMETER_LIST *p ;

    value = new char[strlen(sv)+3] ;
    strcpy ( value, "\"" ) ;
    strcat ( value, sv ) ;
    strcat ( value, "\"" ) ;

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}
    
// see above
int PList :: ChangeValue ( char* name, char* sv[], int number ) 
{
    char*	value ;
    int		i ;
    int		slen ;
    PARAMETER_LIST *p ;
    
    slen = 0 ;
    for ( i=0; i<number; i++ ) slen += strlen(sv[i]) + 3 ;
    
    value = new char[slen] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        strcat ( value, "\"" ) ;
		strcat ( value, sv[i] ) ;
		strcat ( value, "\"" ) ;
		if ( i != number-1 ) strcat ( value, "," ) ;
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}


// AddComment : add comments to the specific parameter
// Input arguments :
//		name : (char*) name of the parameter, 
//				if name is NULL, add the comments at the beginning 
//		comment : (char*) comment to the parameter 
// Return value :
//		(int) : result : 1 for success, 0 for fail 
int PList :: AddComment ( char* name, char *comment )
{
	int		i, j, slen, retval = 0 ;
	char	cbuf[MAX_COMMENT] ;
	char	c ;
    PARAMETER_LIST *p ;

	if ( strcmp(comment,"") != 0 )
	{
		i = 0, j = 0 ;
		cbuf[j++] = COMMENT_MARK ; cbuf[j++] = COMMENT_MARK ;
		cbuf[j++] = COMMENT_MARK ; cbuf[j++] = ' ' ;
		slen = strlen ( comment ) ;
		while ( i < slen ) 
		{
			c = comment[i++] ;
			if ( c != '\n' ) cbuf[j++] = c ;
			else
			{
				cbuf[j++] = ' ' ; 
				cbuf[j++] = COMMENT_MARK ;
				cbuf[j++] = COMMENT_MARK ; 
				cbuf[j++] = COMMENT_MARK ;
				cbuf[j++] = '\n' ;
				if ( i < slen ) 
				{
					cbuf[j++] = COMMENT_MARK ; cbuf[j++] = COMMENT_MARK ;
					cbuf[j++] = COMMENT_MARK ; cbuf[j++] = ' ' ;
				}
			}
		}
		if ( c != '\n' ) 
		{
			cbuf[j++] = ' ' ; 
			cbuf[j++] = COMMENT_MARK ;
			cbuf[j++] = COMMENT_MARK ; 
			cbuf[j++] = COMMENT_MARK ;
			cbuf[j++] = '\n' ;
		}
		cbuf[j] = '\0' ;
	}

// if the name field is empty, add the comment at the beginning
	if ( strcmp(name,"") == 0 )
	{
		if ( strlen(head->item.comment) != 0 ) delete head->item.comment ;
		head->item.comment = new char[strlen(cbuf)+1] ;
		strcpy ( head->item.comment, cbuf ) ;
		retval = 1 ;
	}
// otherwise, add the comment to the specific parameter
	else
	{
		p = head->next ;
		while ( p )
		{
			if ( STRCASECMP(p->item.name,name) == 0 )
			{
				if ( strlen(p->item.comment) ) delete p->item.comment ;
				p->item.comment = new char[strlen(cbuf)+1] ;
				strcpy ( p->item.comment, cbuf ) ;
				retval = 1 ; 
				break ;
			}
			p = p->next ;
		}
	}

	return retval ;
}

// MakeFile : make the current parameter list into a parameter file
// Input arguments :
//		paramfile : (char*) name of the parameter file into be made 
// Return value :
//		(int) : result : 1 for success, 0 for fail 
int PList :: MakeFile ( char* paramfile )
{
    FILE	*fp_param ;
    int		retval ;
    char	linebuf[MAX_LINEBUF] ;
    char	databuf[MAX_DATABUF] ;
    char	cur_level[MAX_LEVEL][MAX_ITEMSTRING] ;
    char	pre_level[MAX_LEVEL][MAX_ITEMSTRING] ;
    int		cur_level_no, pre_level_no, diff_level_no ;
    int		i ;
    PARAMETER_LIST *p ;
	const int	indent_space = 4 ;		// setting indent space for different level
    
    retval = 1 ;
    fp_param = fopen ( paramfile, "wt" ) ;
    if ( fp_param == NULL )
    {
        retval = 0 ;
		return retval ;
	}
	
//    _sort ( 0 ) ;	// sorting the parameter list with ascending order 

    cur_level_no = pre_level_no = 0 ;
					// cur_level_no : current parameter level number
					// pre_level_no : previous parameter level number
	
	if ( head->item.comment )
		fputs ( head->item.comment, fp_param ) ;
    p = head->next ;
    while ( p ) 
    {
		pre_level_no = cur_level_no ;
        for ( i=0; i<cur_level_no; i++ ) 
			strcpy ( pre_level[i], cur_level[i] ) ;
        for ( i=cur_level_no; i<MAX_LEVEL; i++ )
		    strcpy ( pre_level[i], "" ) ;
		for ( i=0; i<MAX_LEVEL; i++ ) 
			strcpy ( cur_level[i], "" ) ;
		// setting the previous parameter with current parameter,
		// and clear current parameter 
	
		SeparateLevel ( p->item.name, cur_level ) ;
		// separate a new parameter name to current parameter 
		cur_level_no = MAX_LEVEL - 1 ;
		while ( strlen(cur_level[cur_level_no]) == 0 ) cur_level_no-- ; 

		diff_level_no = 0 ;
		while ( STRCASECMP(pre_level[diff_level_no],cur_level[diff_level_no]) == 0 )
		    diff_level_no++ ;
	
        for ( i=pre_level_no; i>diff_level_no; i-- ) 
		{
			memset ( linebuf, ' ', (i-1)*indent_space ) ;
		    linebuf[(i-1)*indent_space] = '\0' ;
		    strcat ( linebuf, "}\n" ) ;
			fputs ( linebuf, fp_param ) ;
	    }
	
		for ( i=diff_level_no; i<cur_level_no; i++ )
		{
			memset ( linebuf, ' ', i*indent_space ) ;
		    linebuf[i*indent_space] = '\0' ;
		    strcat ( linebuf, cur_level[i] ) ;
			strcat ( linebuf, " = \n" ) ;
		    fputs ( linebuf, fp_param ) ;

		    memset ( linebuf, ' ', i*indent_space ) ;
			linebuf[i*indent_space] = '\0' ;
		    strcat ( linebuf, "{\n" ) ;
		    fputs ( linebuf, fp_param ) ;
	    }
    
        memset ( linebuf, ' ', cur_level_no*indent_space ) ;
		linebuf[cur_level_no*indent_space] = '\0' ;
		strcat ( linebuf, cur_level[cur_level_no] ) ;
		strcat ( linebuf, " = " ) ;
		FormatArray ( strlen(linebuf), p->item.value, databuf ) ;
		strcat ( linebuf, databuf ) ;
		if ( p->item.comment )
			fputs ( p->item.comment, fp_param ) ;
		fputs ( linebuf, fp_param ) ;
	    
		p = p->next ;
	}

    for ( i=cur_level_no; i>0; i-- ) 
    {
		memset ( linebuf, ' ', (i-1)*indent_space ) ;
		linebuf[(i-1)*indent_space] = '\0' ;
		strcat ( linebuf, "}\n" ) ;
		fputs ( linebuf, fp_param ) ;
	}
	
    fclose ( fp_param ) ;
    return retval ;
}    

// _sort : sort the parameter items in the parameter list
// Input arguments :
//		sortmode : (int) sorting mode : 0 for ascend, 1 for descend 
// Return value :
//		none
void PList :: _sort ( int sortmode )
{
    PARAMETER_LIST *p ;
    PARAMETER_LIST *p1, *p2 ;
    int isChange ;
    
    if ( head->next == NULL ) return ;
    do
    {
        isChange = 0 ;
        p = head ;
		p1 = p->next ;
		p2 = p->next->next ;
		while ( p2 )
		{
			switch ( sortmode )
			{
				case 0 :
				    if ( STRCASECMP(p1->item.name,p2->item.name) > 0 )
				    {
						p1->next = p2->next ;
						p2->next = p1 ;
						p->next = p2 ;
						isChange = 1 ;
					}
					break ;
		        case 1 :
				    if ( STRCASECMP(p1->item.name,p2->item.name) < 0 )
				    {
						p1->next = p2->next ;
						p2->next = p1 ;
						p->next = p2 ;
						isChange = 1 ;
					}
				    break ;
			}
		    p = p->next ;
    	    p1 = p->next ;
		    p2 = p->next->next ;
	    }
	} while ( isChange == 1 ) ;
}

// SeparateLevel : separate the parameter item into levels
// Input arguments :
//		ss : (char*) name of the parameter item 
//		level : (char**) level separation result
// Return value :
//		none 
void SeparateLevel ( char* ss, char level[MAX_LEVEL][MAX_ITEMSTRING] )
{
    int i, j ,nn ;
    int slen ;
    
    slen = strlen ( ss ) ;
    for ( i=0,j=0,nn=0; i<slen; i++ ) 
    {
        if ( (ss[i]==' ') || (ss[i]=='\t') ) continue ;
		if ( ss[i] == '.' )
		{
			level[nn++][j] = '\0' ;
		    j = 0 ;
	    }
		else
			level[nn][j++] = ss[i] ;
	}
    level[nn][j] = '\0' ;
}
    
// FormatArray : arrange the array to the output format
// Input arguments :
//		add_n_blank : (int) number of space to be added at the beginning of each line
//		s1 : (char *) source value string
//		s2 : (char *) target string 
// Return value :
//		none 
void FormatArray ( int add_n_blank, char *s1, char *s2 )
{
    int		ip, jp ;
    int		qcount ;
    int		datanumber ;
    char	ss[MAX_ITEMSTRING] ; 
    char	bb[MAX_ITEMSTRING] ;
    char	cc ;
    
    strcpy ( ss, " ,\n" ) ;
    memset ( bb, ' ', (add_n_blank+2) ) ;
    bb[add_n_blank+2] = '\0' ;
    strcat ( ss, bb ) ; 
    
    ip = jp = 0 ;
    qcount = 0 ;
    datanumber = 1 ;
    strcpy ( s2, "[ " ) ;
    strcpy ( bb, "" ) ;
    while ( ip < (int)strlen(s1) )
    {
        cc = s1[ip++] ;
		if ( cc == '\"' ) qcount = ( qcount + 1 ) % 2 ;
		if ( (cc==',') && (qcount==0) )
		{
		    bb[jp] = '\0' ;
			strcat ( s2, bb ) ;
		    strcat ( s2, ss ) ;
		    jp = 0 ;
			datanumber++ ;
	    }
		else
			bb[jp++] = cc ;
	}
    bb[jp] = '\0' ;
    strcat ( s2, bb ) ;
    strcat ( s2, " ] ;\n" ) ;
    if ( datanumber == 1 ) 
    {   
        strcpy ( s2, s1 ) ;
		strcat ( s2, " ;\n" ) ;
	}
}


/*
c_atof : convert ascii string to 4-byte single float data
	input arguments :
		s : (const char*), string to be converted
	return value :
		(float), conversion result
*/
float c_atof ( const char* s ) 
{
    char	int_part[16], digit_part[16], power_part[4] ;
    int		i, j, slen ;
    char	c ;
    float	retval ;
    
    strcpy ( int_part, "" ) ;
    strcpy ( digit_part, "" ) ;
    strcpy ( power_part, "" ) ;
    
	slen = strlen ( s ) ;
    i = j = 0 ;
    while  ( (i<slen) && (j<15) ) 
/* get the integer part */
    {
		c = s[i++] ;
		if ( (c!='.') && (c!='e') && (c!='E') ) 
			int_part[j++] = c ;
		else
			break ;
	}
	int_part[j] = '\0' ;

    if ( c == '.' )
/* get the digit part */
	{
		j = 0 ;
		while  ( (i<slen) && (j<15) ) 
		{
			c = s[i++] ;
			if ( (c!='e') && (c!='E') ) 
				digit_part[j++] = c ;
			else
				break ;
		}
		digit_part[j] = '\0' ;
	}
	
	if ( (c=='e') || (c=='E') )
/* get the exponential part */
	{
		j = 0 ;
		while  ( (i<slen) && (j<3) ) power_part[j++] = s[i++] ;
	    power_part[j] = '\0' ;
	}
	    
    retval = (float)fabs ( atof(int_part) ) ;
    slen = 0 - strlen ( digit_part ) ;
    retval += (float)atof ( digit_part ) * pow((float)10, slen ) ;
    retval *= pow( (float)10, atoi(power_part) ) ;
    if ( int_part[0] == '-' ) 
        retval *= (-1) ;
	
    return ( retval ) ;
}

/*
c_atod : convert ascii string to 8-byte double float data
	input arguments :
		s : (const char*), string to be converted
	return value :
		(double), conversion result
*/
double c_atod ( const char* s ) 
{
    char	int_part[32], digit_part[32], power_part[8] ;
    int		i, j, slen ;
    char	c ;
    double	retval ;
    
    strcpy ( int_part, "" ) ;
    strcpy ( digit_part, "" ) ;
    strcpy ( power_part, "" ) ;
    
	slen = strlen ( s ) ;
    i = j = 0 ;
    while  ( (i<slen) && (j<31) ) 
/* get the integer part */
    {
		c = s[i++] ;
		if ( (c!='.') && (c!='e') && (c!='E') ) 
			int_part[j++] = c ;
		else
			break ;
	}
	int_part[j] = '\0' ;

    if ( c == '.' )
/* get the digit part */
	{
		j = 0 ;
		while  ( (i<slen) && (j<31) ) 
		{
			c = s[i++] ;
			if ( (c!='e') && (c!='E') ) 
				digit_part[j++] = c ;
			else
				break ;
		}
		digit_part[j] = '\0' ;
	}
	
	if ( (c=='e') || (c=='E') )
/* get the exponential part */
	{
		j = 0 ;
		while  ( (i<slen) && (j<8) ) power_part[j++] = s[i++] ;
		power_part[j] = '\0' ;
	}
	    
    retval = fabs ( atof(int_part) ) ;
    slen = 0 - strlen ( digit_part ) ;
    retval += atof ( digit_part ) * pow ((float)10, slen ) ;
    retval *= pow ((float)10, atoi(power_part) ) ;
    if ( int_part[0] == '-' ) 
        retval *= (-1) ;
	
    return ( retval ) ;
}

/*
c_itoa : convert short integer value to ascii string
	input arguments :
		s : (char*), string to store the conversion result
		int_value : (int) integer value to be converted
	return value :
		none
*/
void c_itoa ( char *s, int int_value )	
{
    int		index ;
    int		base ;
    
    index = 0 ;
    base = 1 ;
    if ( int_value < 0 )
    {
        s[index] = '-' ;
		index++ ;
		int_value = abs ( int_value ) ;
	}
    while ( int_value/10/base > 0 ) base *= 10 ;
    while ( base )
    {
        s[index] = (int)( int_value / base ) + '0' ;
		int_value = int_value - ( int_value / base ) * base ;
		base = base / 10 ;
		index++ ;
	}
    s[index] = '\0' ;
    return ;
}

/*
c_ltoa : convert long integer value to ascii string
	input arguments :
		s : (char*), string to store the conversion result
		long_value : (long) long integer value to be converted
	return value :
		none
*/
void c_ltoa ( char *s, long long_value )	
{
    int index ;
    long base ;
    
    index = 0 ;
    base = 1 ;
    if ( long_value < 0 )
    {
        s[index] = '-' ;
		index++ ;
		long_value = labs ( long_value ) ;
	}
    while ( long_value/10/base > 0 ) base *= 10 ;
    while ( base )
    {
        s[index] = (long)( long_value / base ) + '0' ;
		long_value = long_value - ( long_value / base ) * base ;
		base = base / 10 ;
		index++ ;
	}
    s[index] = '\0' ;
    return ;
}
//
//void _ultoa (unsigned long ulong_value, char *s, int num )	
//{
//    int index ;
//    long base ;
//    
//    index = 0 ;
//    base = 1 ;
//    while ( ulong_value/10/base > 0 ) base *= 10 ;
//    while ( base )
//    {
//        s[index] = (long)( ulong_value / base ) + '0' ;
//		ulong_value = ulong_value - ( ulong_value / base ) * base ;
//		base = base / 10 ;
//		index++ ;
//	}
//    s[index] = '\0' ;
//    return ;
//}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//�����

#ifdef WIN
//#ifdef Linux_x64
//int PList :: GetValue ( char* name, char* iv, int number) 
//#endif
//int PList :: GetValue ( char* name, __int8* iv, int number) 
//
//{
//    int		retval ;
//    char*	value ;
//    PARAMETER_LIST *p ;
//    
//    char	buf[16] ;
//    int		i, j ;
//    int		vcount, vlen ;
//    char	c ;
//    
//    p = head->next ;
//    retval = 0 ;
//// search for the parameter
//    while ( p )
//    {
//        if ( STRCASECMP(p->item.name,name) == 0 )
//		{
//			vlen = strlen ( p->item.value ) ;
//			value = new char[vlen+1] ;
//			strcpy ( value, p->item.value ) ;
//			retval = 1 ;
//			break ;
//	    }
//		p = p->next ;
//	}
//// get the value of the parameter     
//	if ( retval == 1 )
//    {
//		i = j = 0 ;
//		vcount = 0 ;
//		while ( (vcount<number) && (i<vlen) )
//		{
//			c = value[i++] ;
//			if ( (c=='+') || ( c=='-') || ((c>='0')&&(c<='9')) ) buf[j++] = c ;
//			if ( (c==',') || (i==vlen) )
//			{
//				buf[j] = '\0' ;
//				j = 0 ;
//				#ifdef WIN
//      *(iv+vcount) = (__int8)atoi ( buf ) ;
//      #endif
//
//      #ifdef Linux_x64
//      *(iv+vcount) = (char)atoi ( buf ) ; 
//      #endif
//				
//				vcount++ ;
//			}
//	    }
//	    delete value ;
//	}
//    
//    return retval ;
//}	

#endif

#ifdef WIN
int PList :: GetValue ( char* name, __int16* iv, int number)
#endif 

#ifdef Linux_x64
int PList :: GetValue ( char* name, short* iv, int number)
#endif 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    char	buf[16] ;
    int		i, j ;
    int		vcount, vlen ;
    char	c ;
    
    p = head->next ;
    retval = 0 ;
// search for the parameter
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
			vlen = strlen ( p->item.value ) ;
			value = new char[vlen+1] ;
			strcpy ( value, p->item.value ) ;
			retval = 1 ;
			break ;
	    }
		p = p->next ;
	}
// get the value of the parameter     
	if ( retval == 1 )
    {
		i = j = 0 ;
		vcount = 0 ;
		while ( (vcount<number) && (i<vlen) )
		{
			c = value[i++] ;
			if ( (c=='+') || ( c=='-') || ((c>='0')&&(c<='9')) ) buf[j++] = c ;
			if ( (c==',') || (i==vlen) )
			{
				buf[j] = '\0' ;
				j = 0 ;
				#ifdef WIN
				*(iv+vcount) = (__int16)atoi ( buf ) ;
      #endif

      #ifdef Linux_x64
      *(iv+vcount) = (short)atoi ( buf ) ; 
      #endif

				vcount++ ;
			}
	    }
	    delete value ;
	}
    
    return retval ;
}	

#ifdef WIN
//int PList :: GetValue ( char* name, __int32* iv, int number) 
//{
//	int *temp_iv = (int *)iv;
//	return GetValue ( name, temp_iv, number) ;
//}
#endif

#ifdef WIN
int PList :: GetValue ( char* name, __int64* iv, int number) 
{
    int		retval ;
    char*	value ;
    PARAMETER_LIST *p ;
    
    char	buf[16] ;
    int		i, j ;
    int		vcount, vlen ;
    char	c ;
    
    p = head->next ;
    retval = 0 ;
// search for the parameter
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
			vlen = strlen ( p->item.value ) ;
			value = new char[vlen+1] ;
			strcpy ( value, p->item.value ) ;
			retval = 1 ;
			break ;
	    }
		p = p->next ;
	}
// get the value of the parameter     
	if ( retval == 1 )
    {
		i = j = 0 ;
		vcount = 0 ;
		while ( (vcount<number) && (i<vlen) )
		{
			c = value[i++] ;
			if ( (c=='+') || ( c=='-') || ((c>='0')&&(c<='9')) ) buf[j++] = c ;
			if ( (c==',') || (i==vlen) )
			{
				buf[j] = '\0' ;
				j = 0 ;
				*(iv+vcount) = _atoi64 ( buf ) ;
				vcount++ ;
			}
	    }
	    delete value ;
	}
    
    return retval ;
}
#endif
	
#ifdef WIN
//int PList :: GetValue ( char* name, unsigned __int8* iv, int number) 
//{
//	__int8 *temp_iv = (__int8 *)iv;
//	return GetValue(name, temp_iv, number);
//}

int PList :: GetValue ( char* name, unsigned __int16* iv, int number)
{
	__int16 *temp_iv = (__int16 *)iv;
	return GetValue(name, temp_iv, number);
}

//int PList :: GetValue ( char* name, unsigned __int32* iv, int number)
//{
//	__int32 *temp_iv = (__int32 *)iv;
//	return GetValue(name, temp_iv, number);
//}

int PList :: GetValue ( char* name, unsigned __int64* iv, int number) 
{
	__int64 *temp_iv = (__int64 *)iv;
	return GetValue(name, temp_iv, number);
}	

int PList :: GetValue ( char* name, unsigned int* iv, int number) 
{
	int *temp_iv = (int *)iv;
	return GetValue(name, temp_iv, number);
}

	
int PList :: GetValue ( char* name, unsigned long* iv, int number) 
{
	long *temp_iv = (long *)iv;
	return GetValue(name, temp_iv, number);
}
#endif

#ifdef Linux_x64
//int PList :: GetValue ( char* name, unsigned char* iv, int number) 
//{
//	char *temp_iv = (char *)iv;
//	return GetValue(name, temp_iv, number);
//}

int PList :: GetValue ( char* name, unsigned short* iv, int number)
{
	short *temp_iv = (short *)iv;
	return GetValue(name, temp_iv, number);
}

int PList :: GetValue ( char* name, unsigned int* iv, int number)
{
	int *temp_iv = (int *)iv;
	return GetValue(name, temp_iv, number);
}

int PList :: GetValue ( char* name, unsigned long* iv, int number) 
{
	long *temp_iv = (long *)iv;
	return GetValue(name, temp_iv, number);
}	
#endif

		

/////////////////////////////////////////////////////////////////////
#ifdef WIN
void PList ::SetValue ( char* name, __int8* iv, int number) 	
//#endif

//#ifdef Linux_x64
//void PList ::SetValue ( char* name, char* iv, int number) 	
//#endif
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_itoa ( buf, *(iv+i) ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}
#endif 


#ifdef WIN
void PList ::SetValue ( char* name, __int16* iv, int number) 
#endif

#ifdef Linux_x64
void PList ::SetValue ( char* name, short* iv, int number) 	
#endif

{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_itoa ( buf, *(iv+i) ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}

#ifdef WIN
//void PList ::SetValue ( char* name, __int32* iv, int number) 
//{
//	int  *temp_iv = (int *)iv;
//	SetValue(name, temp_iv, number);
//}

void PList ::SetValue ( char* name, __int64* iv, int number) 
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _i64toa ( *(iv+i), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}


void PList ::SetValue ( char* name, unsigned __int8* iv, int number) 
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( (unsigned long)(*(iv+i)), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}
	
void PList ::SetValue ( char* name, unsigned __int16* iv, int number)
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( (unsigned long)(*(iv+i)), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}

//void PList ::SetValue ( char* name, unsigned __int32* iv, int number)
//{
//	unsigned int *temp_iv = (unsigned int *)iv;
//	SetValue(name, temp_iv, number);
//}

void PList ::SetValue ( char* name, unsigned __int64* iv, int number)	
{
	char*	value ;
    char	buf[24] ;
    int		i ;
    
    value = new char[number * 24] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ui64toa ( *(iv+i), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}


void PList ::SetValue ( char* name, unsigned int* iv, int number)	
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( (unsigned long)(*(iv+i)), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}


void PList ::SetValue ( char* name, unsigned long* iv, int number)
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( *(iv+i), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}
#endif


#ifdef Linux_x64
//void PList ::SetValue ( char* name, unsigned char* iv, int number) 
//{
//	  char*	value ;
//    char	buf[16] ;
//    int		i ;
//    
//    value = new char[number * 16] ;
//    strcpy ( value, "" ) ;
//    for ( i=0; i<number; i++ )
//    {
//        _ultoa ( (unsigned long)(*(iv+i)), buf, 10 ) ;	// convert data to string
//		if ( number == 1 ) strcpy ( value, buf ) ;
//		else
//		{
//		    strcat ( value, buf ) ;
//			if ( i != number-1 ) strcat ( value, "," ) ;
//	    }
//	}
//    _add ( name, value ) ;			// add parameter item into the parameter list
//    delete value ;
//}
	
void PList ::SetValue ( char* name, unsigned short* iv, int number)
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( (unsigned long)(*(iv+i)), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}

void PList ::SetValue ( char* name, unsigned int* iv, int number)	
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( (unsigned long)(*(iv+i)), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}


void PList ::SetValue ( char* name, unsigned long* iv, int number)
{
	char*	value ;
    char	buf[16] ;
    int		i ;
    
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa ( *(iv+i), buf, 10 ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}
    _add ( name, value ) ;			// add parameter item into the parameter list
    delete value ;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//change value
#ifdef WIN
int PList ::ChangeValue ( char* name, __int8* iv, int number)
//#endif

//ifdef Linux_x64
//int PList ::ChangeValue ( char* name, char* iv, int number)
//#endif
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_itoa ( buf, *(iv+i) ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}
#endif


#ifdef WIN
int PList ::ChangeValue ( char* name, __int16* iv, int number)
#endif

#ifdef Linux_x64
int PList ::ChangeValue ( char* name, short* iv, int number)
#endif

{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        c_itoa ( buf, *(iv+i) ) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

#ifdef WIN
//int PList ::ChangeValue ( char* name, __int32* iv, int number)
//{
//	int *temp_iv = (int *)iv;
//	return ChangeValue(name, temp_iv, number);
//}

int PList ::ChangeValue ( char* name, __int64* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _i64toa(  *(iv+i), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}


int PList ::ChangeValue ( char* name, unsigned __int8* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( (unsigned long)(*(iv+i)), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

int PList ::ChangeValue ( char* name, unsigned __int16* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( (unsigned long)(*(iv+i)), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}
//
//int PList ::ChangeValue ( char* name, unsigned __int32* iv, int number)
//{
//	unsigned int *temp_iv = (unsigned int *)iv;
//	return ChangeValue(name, temp_iv, number);
//}

int PList ::ChangeValue ( char* name, unsigned __int64* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ui64toa( *(iv+i), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

int PList ::ChangeValue ( char* name, unsigned int* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( (unsigned long)(*(iv+i)), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

int PList ::ChangeValue ( char* name, unsigned long* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( *(iv+i), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}
#endif

#ifdef Linux_x64
//int PList ::ChangeValue ( char* name, unsigned char* iv, int number)
//{
//    char*	value ;
//    char	buf[16] ;
//    int		i ;
//    PARAMETER_LIST *p ;
//   
//    value = new char[number * 16] ;
//    strcpy ( value, "" ) ;
//    for ( i=0; i<number; i++ )
//    {
//        _ultoa( (unsigned long)(*(iv+i)), buf, 10) ;	// convert data to string
//		if ( number == 1 ) strcpy ( value, buf ) ;
//		else
//		{
//		    strcat ( value, buf ) ;
//			if ( i != number-1 ) strcat ( value, "," ) ;
//	    }
//	}
//
//    p = head->next ;
//    while ( p )
//    {
//        if ( STRCASECMP(p->item.name,name) == 0 )
//		{
//		    if(p->item.value != NULL)
//				delete p->item.value;
//			p->item.value = value;
//			return 1;
//	    }
//		p = p->next ;
//	}
//
//	delete value;	
//    return 0;
//}

int PList ::ChangeValue ( char* name, unsigned short* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( (unsigned long)(*(iv+i)), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

int PList ::ChangeValue ( char* name, unsigned int* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( (unsigned long)(*(iv+i)), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}

int PList ::ChangeValue ( char* name, unsigned long* iv, int number)
{
    char*	value ;
    char	buf[16] ;
    int		i ;
    PARAMETER_LIST *p ;
   
    value = new char[number * 16] ;
    strcpy ( value, "" ) ;
    for ( i=0; i<number; i++ )
    {
        _ultoa( *(iv+i), buf, 10) ;	// convert data to string
		if ( number == 1 ) strcpy ( value, buf ) ;
		else
		{
		    strcat ( value, buf ) ;
			if ( i != number-1 ) strcat ( value, "," ) ;
	    }
	}

    p = head->next ;
    while ( p )
    {
        if ( STRCASECMP(p->item.name,name) == 0 )
		{
		    if(p->item.value != NULL)
				delete p->item.value;
			p->item.value = value;
			return 1;
	    }
		p = p->next ;
	}

	delete value;	
    return 0;
}
#endif
//2005��11��10�����ǿ����������
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 