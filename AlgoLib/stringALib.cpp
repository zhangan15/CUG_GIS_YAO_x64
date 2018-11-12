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

	//去除字符串前面空格
	for (start = 0; start < instr_len; start++)
	{
		if(instring[start] != ' ')
			break;
	}

	//去除字符串后面空格
	for (end = instr_len; end >=0; end--)
	{
		if(instring[end] != ' ')
			break;
	}
	//处理instring全为空格的情况
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
