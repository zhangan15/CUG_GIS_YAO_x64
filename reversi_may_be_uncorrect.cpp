//C++黑白棋
//未测试，不一定正确
//记事本手写代码，未经编译
//仅供地大信工2018级思路参考使用
//抄袭0分！！！

//更新了一个小AI下白棋 2018-12-13 00:00

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
		{
			if(pData[i][j] == BLACK)
				cout << "●" << "\t";
			if(pData[i][j] == WHITE)
				cout << "○" << "\t";
			if(pData[i][j] == VALID)
				cout << "+" << "\t";
		}
		
		cout<<endl;
	}
}

bool play(int**& pInputData, int curX, int curY, int curColor = BLACK)
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

//计算当前白黑比
double calCurrentWhitePerBlack()
{
	//计算当前白黑比
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
	
	if(nBlackCount == 0)
		return -1;
	
	if(nWhiteCount == 0)
		return 1;
	
	double WhitePerBlack = (double)nWhiteCount/(double)nBlackCount;
}

void whoWinTheGame()
{
	double dWBRatio = calCurrentWhitePerBlack();
	
	if(dWBRatio == 1)
		cout << "WHITE & BLACK ended in a draw." <<endl;
	else if(dWBRatio > 1)
		cout << "WHITE WIN!" << endl;
	else
		cout << "BLACK WIN!" << endl;
	
}

void AI_findTheBestState(int curColor, int& selectX, int& selectY)
{
	int i, j;
	
	//存储当前位置的最佳状态
	int pState[GRID_SIZE][GRID_SIZE];
	
	//备份棋盘，用于计算机后台计算下载最佳位置
	int pBkupData[GRID_SIZE][GRID_SIZE];
	
	
	for (i=0; i<GRID_SIZE; i++)
	{
		for (j=0; j<GRID_SIZE; j++)
		{
			if(pData[i][j] != VALID)
			{
				pState[i][j] = -2;
				continue;
			}
			
			//复制当前数据
			for (int m=0; m<GRID_SIZE; m++)
				for(int n=0; n<GRID_SIZE; n++)
					pBkupData[m][n] = pData[m][n];	//复制现有棋盘黑白分布数据
			
			//在备份棋盘上模拟下棋，若在当前下棋的白黑比
			//可得到下棋的状态分布矩阵
			pState[i][j] = play(pBkupData, i, j, curColor);
		}
	}
	
	//如果是白棋，则白黑比越高越好；如果是黑棋，则白黑比越低越好
	selectX = selectY = 0;
	double dCurState;
	if(curColor == WHITE)
	{
		dCurState = -2;
		for (i=0; i<GRID_SIZE; i++)
		{
			for (j=0; j<GRID_SIZE; j++)
			{
				if(pState[i][j]>=dCurState)
				{
					dCurState = pState[i][j];
					selectX = i;
					selectY = j;
				}
			}
		}
	}
	
	if(curColor == BLACK)
	{
		dCurState = (double)GRID_SIZE*GRID_SIZE;
		for (i=0; i<GRID_SIZE; i++)
		{
			for (j=0; j<GRID_SIZE; j++)
			{
				if(1.0f/pState[i][j]<=dCurState)
				{
					dCurState = pState[i][j];
					selectX = i;
					selectY = j;
				}
			}
		}
	}
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
		//判断游戏是否结束
		if(isEndGame())
		{
			//输出赢家
			whoWinTheGame();
			
			//重玩
			replay();
			curColor = BLACK;
		}
		
		//如果执白时，可写个逻辑让计算机自动寻找最优位置（小AI，鼓励实现）
		if(curColor == WHITE)
		{
			cout << "AI-WHITE Running..."<<endl;
			//RUN MY AI HERE!
			int AI_x, AI_y;
			AI_findTheBestState(curColor, AI_x, AI_y);
			play(pData, AI_x, AI_y, curColor);
			
			//then display and continue the loop
			display();	//输出
			curColor = BLACK;
			continue;
		}
		
		cout << "Input Location (X, Y): "<<endl;
		cin >> curX >> curY;
		
		//修改pData
		bool bResult = play(pData, curX, curY, curColor);
		
		if(bResult)
		{
			display();	//输出
			
			// 更换当前落子颜色
			if (curColor == BLACK) curColor = WHITE;
			if (curColor == WHITE) curColor = BLACK;
		}
		if(!bResult)
			cout<<"Input X and Y Error!"<<endl;
		
	}
	
	return 0;
}
