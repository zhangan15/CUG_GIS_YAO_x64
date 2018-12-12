//大一C++黑白棋答案
//未测试，不一定正确
//记事本手写代码，未经编译
//仅供地大信工2018级思路参考使用
//抄袭0分！！！

#include <iostream>
using namespace std;

#define VALID 0 	//可下棋区域
#define WHITE 1		//白棋
#define BLACK 2		//黑棋

#define GRID_SIZE 8	//棋盘大小

int pData[GRID_SIZE][GRID_SIZE]; //下棋存储在二维数组中 8*8，0 - 未落子区域，1 - 白棋区域，2 - 黑棋区域

void display()	//输出pData并显示
{
	//输出pData并显示
	for(int i=0; i<GRID_SIZE; i++)
	{
		for(int j=0; j<GRID_SIZE; j++)
			cout << pData[i][j] << "\t";
		
		cout<<endl;
	}
}

bool play(int curX, int curY, int curColor = BLACK)
{
	if (curX < 0 || curY < 0 || curX >= GRID_SIZE || curY >= GRID_SIZE)
		return false;
	
	if (pData[curX][curY] == VALID)
		return false;
	
	//在这里编写黑白棋的逻辑，更新pData
	int searchX = curX, searchY = curY;
	int i, j;
	
	//横着找
	for(searchX = curX; searchX < GRID_SIZE || pData[searchX][searchY] == VALID; searchX++)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX; i<searchX; i++)
				pData[i][searchY] = curColor;
			
			break;
		}
	}
	
	for(searchX = curX; searchX >= 0 || pData[searchX][searchY] == VALID; searchX--)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX; i>=searchX; i--)
				pData[i][searchY] = curColor;
			
			break;
		}
	}
	
	//竖着找
	for(searchY = curY; searchY < GRID_SIZE || pData[searchX][searchY] == VALID; searchY++)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curY; i<searchY; i++)
				pData[searchX][i] = curColor;
			
			break;
		}
	}
	
	for(searchY = curY; searchY >= 0 || pData[searchX][searchY] == VALID; searchY--)
	{
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curY; i>=searchY; i--)
				pData[searchX][i] = curColor;
			
			break;
		}
	}
	
	//斜着找
	for(searchX = curX,searchY = curY; searchX < GRID_SIZE || searchY < GRID_SIZE || pData[searchX][searchY] == VALID; searchX++,searchY++)
	{			
		if (pData[searchX][searchY] == curColor)
		{
			for (i=curX, j=curY; i<searchX, j<searchY; i++, j++)
				pData[i][j] = curColor;
			
			break;
		}	
		
	}
	
	for(searchX = curX,searchY = curY; searchX >= 0 || searchY >= 0 || pData[searchX][searchY] == VALID; searchX--,searchY--)
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

bool isEndGame()
{
	for (int i=0; i<GRID_SIZE; i++)
	{
		for (int j=0; j<GRID_SIZE; j++)
		{
			if (pData[i][j] == VALID)
				return false;
		}
	}
	
	return true;
}

void replay()
{
	for (int i=0; i<GRID_SIZE; i++)
	{
		for (int j=0; j<GRID_SIZE; j++)
		{
			pData[i][j] = VALID;				
		}
	}
	
	//左上角右下角分别为黑和白
	pData[0][0] = BLACK;
	pData[GRID_SIZE-1][GRID_SIZE-1] = WHITE;
}

void whoWinTheGame()
{
	int nWhiteCount = 0;
	int nBlackCount = 0;
	
	for (int i=0; i<GRID_SIZE; i++)
	{
		for (int j=0; j<GRID_SIZE; j++)
		{
			if (pData[i][j] == WHITE)
				nWhiteCount++;
			if (pData[i][j] == BLACK)
				nBlackCount++;
		}
	}
	
	if(nWhiteCount == nBlackCount)
		cout << "WHITE & BLACK ended in a draw." <<endl;
	else if(nWhiteCount>nBlackCount)
		cout << "WHITE WIN!" << endl;
	else
		cout << "BLACK WIN!" << endl;
	
}

int main()
{
	int i, j;
	for (i=0; i<GRID_SIZE; i++)
		for (j=0; j<GRID_SIZE; j++)
			pData[i][j] = VALID;
		
	//左上角右下角分别为黑和白
	pData[0][0] = BLACK;
	pData[GRID_SIZE-1][GRID_SIZE-1] = WHITE;
	
	// 当前落子颜色
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
			
			// 更换当前落子颜色
			if (curColor == BLACK) curColor = WHITE;
			if (curColor == WHITE) curColor = BLACK;
		}
		if(!bResult)
			cout<<"Input X and Y Error!"<<endl;
		
		//判断游戏是否结束
		if(isEndGame())
		{
			//输出赢家
			whoWinTheGame();
			
			//重玩
			replay();
			curColor = BLACK;
		}
		
	}
	
	return 0;
}
