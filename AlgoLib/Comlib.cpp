// Comlib.cpp: implementation of the CComlib class.
//
//////////////////////////////////////////////////////////////////////
#include "include_global.h"
#include "Comlib.h"

//#define  _CRT_INSECURE_DEPRECATE 1
//////////////////////////////////////////////////////////////////////
// Local functions
//////////////////////////////////////////////////////////////////////

sttAlgoProg stHAPProg;  //定义进度条指示、出错信息

sttAlgoProg::sttAlgoProg()
{
	ProgPo = -1;
	strcpy(pcErrorMessage,"");
	strcpy(pcAlgoStatus,"");
	strcpy(pcErrorMessage,"");
	strcpy(pcMessageSort,"错误！");
	ptf = NULL;
//	ptf = fopen("D:\\TraceFile.txt","w");
}
sttAlgoProg::~sttAlgoProg()
{
//	fclose(ptf);
}

static BOOL ASSERTVALIDPOINTER(void * pointer, char* message)
{
	if(pointer == NULL)
	{
		sprintf(stHAPProg.pcErrorMessage,"Error Message: %s\n",message);
		return FALSE;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComlib::CComlib()
{

}

CComlib::~CComlib()
{

}

/*****************************   fspace_1d  **************************/
/*  To allocation a 1_dimension dynamic array  */
void * CComlib::fspace_1d(int col, int length)
{
	void *b;

	b = (void *)calloc(length,col);

	char err[50];
	sprintf(err, "Sorry!  not enough space for you.\n");
	ASSERTVALIDPOINTER(b, err);

	return(b);
}

/*************      fspace_2d    ***************************************/
/*  To allocation a 2_dimension dynamic array  */
void ** CComlib::fspace_2d(int row,int col,int length)
{
	int i;
	void **b;
	
	//b = (void **)calloc(sizeof(void *),row);
	b = (void **) new void* [row];

	char err[50];
	sprintf(err, "Sorry!  not enough space for you.\n");

	ASSERTVALIDPOINTER(b, err);

	for(i=0;i<row;i++)
	{
		//b[i] = (void *)calloc(col,length);
		b[i] = (void *) new char[col*length];
		ASSERTVALIDPOINTER(b[i], err);
	}

//	b =  new void*[row];
	//for(i=0;i<row;i++)
	//	b[i]= new void[length];
	return(b);
}

/*************      fspace_3d    ***************************************/
/*  To allocation a 3_dimension dynamic array  */
void *** CComlib::fspace_3d(int row1,int row2,int row3,int lenth)
{
	int i;
	void ***b;

	b = (void ***)calloc(sizeof(void **),row1);

	char err[50];
	sprintf(err, "Sorry!  not enough space for you.\n");
	ASSERTVALIDPOINTER(b, err);

	for(i=0;i<row1;i++)
		b[i] = (void **)fspace_2d(row2,row3,lenth);

	return(b);
}

/*******************************  ffree_1d   ****************************/
/*  To free a 1_dimension dynamic array  */
void CComlib::ffree_1d(void *a)
{
	if(a==NULL) return;

	free(a);

	a = NULL;
}

/*******************************  ffree_2d   ****************************/
/*  To free a 2_dimension dynamic array  */
void CComlib::ffree_2d(void **a,int row)
{
	if(a==NULL) return ;

	int i;

	for(i=0;i<row;i++) 
		//free(a[i]);
		delete[] a[i];
	delete[] a;
	//	free(a);
	a = NULL;
}

/*******************************  ffree_3d   ****************************/
/*  To free a 3_dimension dynamic array  */
void CComlib::ffree_3d(void ***a,int row1,int row2)
{
	if(a==NULL) return ;
	int i;

	for(i=0;i<row1;i++)
		ffree_2d((void **)a[i],row2);
	free(a);

	a = NULL;
}
