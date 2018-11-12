// Comlib.h: interface for the CComlib class.
//
//////////////////////////////////////////////////////////////////////
#ifndef COMLIB_H_
#define COMLIB_H_

#ifdef WIN32
#ifdef ALGOLIB_EXPORTS
#define ALGOLIB_API __declspec(dllexport)
#else
#define ALGOLIB_API __declspec(dllimport)
#endif
#endif

#ifdef _LINUX
#define ALGOLIB_API
#endif

#include "stdio.h"

struct ALGOLIB_API sttAlgoProg
{
	sttAlgoProg();
	~sttAlgoProg();
	int ProgPo; //进度条指示位置,0-100之间。 -1：表示进度条结束，为10的倍数的时候可以更新进度条文字信息;
	char pcAlgoStatus[256]; //进度条上算法执行状态提示信息。
	char pcErrorMessage[256]; //记录出错信息。
	char pcMessageSort[5];
	FILE* ptf; //point to trace file 
};

extern ALGOLIB_API sttAlgoProg stHAPProg;

class ALGOLIB_API CComlib
{
public:
	CComlib();
	virtual ~CComlib();

public:
	/************************************************************************/
	/* 存储空间*/
	/************************************************************************/
	static void *fspace_1d(int col,int length);//分配一维动态内存
	static void **fspace_2d(int row,int col,int lenth);//分配二维动态内存
	static void ***fspace_3d(int row1,int row2,int row3,int lenth);//分配三维动态内存
	static void ffree_1d(void *a);
	static void ffree_2d(void **a,int row);
	static void ffree_3d(void ***a,int row1,int row2);
};

#endif /*COMLIB_H_*/