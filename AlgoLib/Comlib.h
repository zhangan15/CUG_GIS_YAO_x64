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
	int ProgPo; //������ָʾλ��,0-100֮�䡣 -1����ʾ������������Ϊ10�ı�����ʱ����Ը��½�����������Ϣ;
	char pcAlgoStatus[256]; //���������㷨ִ��״̬��ʾ��Ϣ��
	char pcErrorMessage[256]; //��¼������Ϣ��
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
	/* �洢�ռ�*/
	/************************************************************************/
	static void *fspace_1d(int col,int length);//����һά��̬�ڴ�
	static void **fspace_2d(int row,int col,int lenth);//�����ά��̬�ڴ�
	static void ***fspace_3d(int row1,int row2,int row3,int lenth);//������ά��̬�ڴ�
	static void ffree_1d(void *a);
	static void ffree_2d(void **a,int row);
	static void ffree_3d(void ***a,int row1,int row2);
};

#endif /*COMLIB_H_*/