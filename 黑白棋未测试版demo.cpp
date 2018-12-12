//大一C++黑白棋答案
//未测试，不一定正确
//抄袭0分

#include <iostream>
using namespace std;

#define VALID 0
#define WHITE 1
#define BLACK 2

int pData[8][8]; //棋盘

void display()	//输出pData并显示
{
	//输出pData并显示
	for(int i=0; i<8; i++)
	{
		for(int j=0; j<8; j++)
			cout << pData[i][j] << "\t";
		
		cout<<endl;
	}
}

bool play(int curX, int curY, int curColor = BLACK)
{
	if (curX < 0 || curY < 0 || curX >= 8 || curY >= 8)
		return false;
	
	if (pData[curX][curY] == VALID)
		return false;
	
	//在这里编写黑白棋的逻辑，更新pData
	int searchX = curX, searchY = curY;
	int i, j;
	
	//横着找
	for(searchX = curX; searchX < 8, pData[searchX][searchY] == VALID; searchX++)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX; i<searchX; i++)
				pData[i][searchY] = curColor;
			
			break;
		}
	}
	
	searchX = curX;
	searchY = curY;
	for(searchX = curX; searchX >= 0, pData[searchX][searchY] == VALID; searchX--)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX; i>=searchX; i--)
				pData[i][searchY] = curColor;
			
			break;
		}
	}
	
	//竖着找
	searchX = curX;
	searchY = curY;
	for(searchY = curY; searchY < 8, pData[searchX][searchY] == VALID; searchY++)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curY; i<searchY; i++)
				pData[searchX][i] = curColor;
			
			break;
		}
	}
	
	searchX = curX;
	searchY = curY;
	for(searchY = curY; searchY >= 0, pData[searchX][searchY] == VALID; searchY--)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curY; i>=searchY; i--)
				pData[searchX][i] = curColor;
			
			break;
		}
	}
	
	//斜着找
	searchX = curX;
	searchY = curY;
	for(searchX = curX,searchY = curY; searchY < 8, searchY < 8, pData[searchX][searchY] == VALID; searchX++,searchY++)
	{			
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX, j=curY; i<searchX, j<searchY; i++, j++)
				pData[i][j] = curColor;
			
			break;
		}	
		
	}
	
	searchX = curX;
	searchY = curY;
	for(searchX = curX,searchY = curY; searchY >= 0, searchY >= 0, pData[searchX][searchY] == VALID; searchX--,searchY--)
	{			
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX, j=curY; i>=searchX, j>=searchY; i--, j--)
				pData[i][j] = curColor;
			
			break;
		}	
		
	}
	
	
	return true;
}

int main()
{
	int i, j;
	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			pData[i][j] = VALID;
		
	//左上角右下角分别为黑和白
	pData[0][7] = BLACK;
	pData[7][0] = WHITE;
	
	int curColor = BLACK;	
	int curX, curY;
	while (1)
	{
		cout << "Input Location (X, Y): "<<endl;
		cin >> curX >> curY;
		
		//修改pData
		bool bResult = play(curX, curY, curColor);
		
		if(bResult)
		{
			display();	//输出
			if (curColor == BLACK) curColor = WHITE;
			if (curColor == WHITE) curColor = BLACK;
		}
		if(!bResult)
			cout<<"Input X and Y Error!"<<endl;
		
	}
	
	return 0;
}