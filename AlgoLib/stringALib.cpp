#include "stringALib.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

stringALib::stringALib(void)
{
}

stringALib::~stringALib(void)
{
}

void stringALib::delspace( const char* instring,char* outstring )
{
	int instr_len, start, end;
	instr_len = strlen(instring);

	//ȥ���ַ���ǰ��ո�
	for (start = 0; start < instr_len; start++)
	{
		if(instring[start] != ' ')
			break;
	}

	//ȥ���ַ�������ո�
	for (end = instr_len; end >=0; end--)
	{
		if(instring[end] != ' ')
			break;
	}
	//����instringȫΪ�ո�����
	if (start > end)
	{
		outstring[1]='\0';
	}
	else
	{
		strncpy(outstring, instring+start, end-start+1);
		outstring[end-start+1]='\0';
	}
}
