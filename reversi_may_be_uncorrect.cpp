//C++黑白棋
//未测试，不一定正确
//记事本手写代码，未经编译
//仅供地大信工2018级思路参考使用
//抄袭0分！！！

//更新了一个小AI下白棋 2018-12-13 00:00

#include <iostream>
#include <time.h>
using namespace std;

#define VALID 0 	//可下棋区域
#define WHITE 1	//白棋
#define BLACK -1	//黑棋

#define GRID_SIZE 8	//棋盘大小

int** pData = NULL; //下棋存储在二维数组中 8*8，0 - 未落子区域，1 - 白棋区域，2 - 黑棋区域

void clearMemory(int**& ppD)
{
	if (ppD != NULL)
	{
		for (int i = 0; i < GRID_SIZE; i++)
			delete[] ppD[i];
		delete[] ppD;
		ppD = NULL;
	}
}

void createData(int**& ppD)
{
	clearMemory(ppD);
	ppD = new int*[GRID_SIZE];
	for (int i = 0; i < GRID_SIZE; i++)
		ppD[i] = new int[GRID_SIZE];
}

void display()	//输出pData并显示
{
	//输出pData并显示
	cout << "+\t";
	for (int i = 0; i < GRID_SIZE; i++)
		cout << i << "\t";
	cout << endl;

	for (int i = 0; i < GRID_SIZE; i++)
	{
		cout << i << "\t";
		for (int j = 0; j < GRID_SIZE; j++)
		{
			if (pData[i][j] == BLACK)
				cout << "●" << "\t";
			if (pData[i][j] == WHITE)
				cout << "○" << "\t";
			if (pData[i][j] == VALID)
				cout << "+" << "\t";
		}

		cout << endl;
	}
}

//核心算法，pInputData为棋盘数据
//将二维数组指针引用，可修改二维数组内容
bool play(int**& pInputData, int curX, int curY, int curColor = BLACK)
{
	if (curX < 0 || curY < 0 || curX >= GRID_SIZE || curY >= GRID_SIZE)
		return false;

	if (pInputData[curX][curY] != VALID)
		return false;

	pInputData[curX][curY] = curColor;

	//在这里编写黑白棋的逻辑，更新pInputData
	int searchX = curX, searchY = curY;
	int i, j;

	//横着找
	for (searchX = curX, searchY = curY; searchX < GRID_SIZE /*&& pInputData[searchX][searchY] != VALID*/; searchX++)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curX; i < searchX; i++)
				pInputData[i][searchY] = curColor;

			//break;
		}
	}

	for (searchX = curX, searchY = curY; searchX >= 0 /*&& pInputData[searchX][searchY] != VALID*/; searchX--)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curX; i >= searchX; i--)
				pInputData[i][searchY] = curColor;

			//break;
		}
	}

	//竖着找
	for (searchX = curX, searchY = curY; searchY < GRID_SIZE /*&& pInputData[searchX][searchY] != VALID*/; searchY++)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curY; i < searchY; i++)
				pInputData[searchX][i] = curColor;

			//break;
		}
	}

	for (searchX = curX, searchY = curY; searchY >= 0 /*&& pInputData[searchX][searchY] != VALID*/; searchY--)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curY; i >= searchY; i--)
				pInputData[searchX][i] = curColor;

			//break;
		}
	}

	//斜着找
	for (searchX = curX, searchY = curY; searchX < GRID_SIZE && searchY < GRID_SIZE /*&& pInputData[searchX][searchY] != VALID*/; searchX++, searchY++)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curX, j = curY; i < searchX && j < searchY; i++, j++)
				pInputData[i][j] = curColor;

			//break;
		}

	}

	for (searchX = curX, searchY = curY; searchX >= 0 && searchY >= 0 /*&& pInputData[searchX][searchY] != VALID*/; searchX--, searchY--)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curX, j = curY; i >= searchX && j >= searchY; i--, j--)
				pInputData[i][j] = curColor;

			//break;
		}

	}

	for (searchX = curX, searchY = curY; searchX < GRID_SIZE && searchY >= 0 /*&& pInputData[searchX][searchY] != VALID*/; searchX++, searchY--)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curX, j = curY; i < searchX && j >= searchY; i++, j--)
				pInputData[i][j] = curColor;

			//break;
		}

	}

	for (searchX = curX, searchY = curY; searchX >= 0 && searchY < GRID_SIZE /*&& pInputData[searchX][searchY] != VALID*/; searchX--, searchY++)
	{
		if (pInputData[searchX][searchY] == VALID)
			break;

		if (pInputData[searchX][searchY] == curColor)
		{
			for (i = curX, j = curY; i >= searchX && j < searchY; i--, j++)
				pInputData[i][j] = curColor;

			//break;
		}

	}


	return true;
}

bool isEndGame()
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			if (pData[i][j] == VALID)
				return false;
		}
	}

	return true;
}

void replay()
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			pData[i][j] = VALID;
		}
	}

	//左上角右下角分别为黑和白
// 	pData[0][0] = BLACK;
// 	pData[GRID_SIZE - 1][GRID_SIZE - 1] = WHITE;

	srand((unsigned)time(NULL));
	//随机给位置
	pData[rand() % GRID_SIZE][rand() % GRID_SIZE] = BLACK;

	//避免白棋落在相同位置
	while (1)
	{
		int rnd_x = rand() % GRID_SIZE;
		int rnd_y = rand() % GRID_SIZE;
		if (pData[rnd_x][rnd_y] != VALID)
			continue;

		pData[rnd_x][rnd_y] = WHITE;
		break;
	}
	

	display();
}

//计算当前白黑比
double calCurrentWhitePerBlack(int** &pInputData)
{
	//计算当前白黑比
	int nWhiteCount = 0;
	int nBlackCount = 0;

	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			if (pInputData[i][j] == WHITE)
				nWhiteCount++;
			if (pInputData[i][j] == BLACK)
				nBlackCount++;
		}
	}

	if (nBlackCount == 0)
		return -1;

	double WhitePerBlack = (double)nWhiteCount / (double)nBlackCount;
	return WhitePerBlack;
}

void whoWinTheGame()
{
	double dWBRatio = calCurrentWhitePerBlack(pData);

	if (dWBRatio == 1)
		cout << "WHITE & BLACK ended in a draw." << endl;
	else if (dWBRatio > 1)
		cout << "WHITE WIN!" << endl;
	else
		cout << "BLACK WIN!" << endl;

}

void AI_findTheBestState(int curColor, int& selectX, int& selectY)
{
	int i, j;

	//存储当前位置的最佳状态
	double pState[GRID_SIZE][GRID_SIZE];

	//备份棋盘，用于计算机后台计算下载最佳位置
	int** pBkupData = NULL;
	createData(pBkupData);


	for (i = 0; i < GRID_SIZE; i++)
	{
		for (j = 0; j < GRID_SIZE; j++)
		{
			if (pData[i][j] != VALID)
			{
				pState[i][j] = -2;
				continue;
			}

			//复制当前数据
			for (int m = 0; m < GRID_SIZE; m++)
				for (int n = 0; n < GRID_SIZE; n++)
					pBkupData[m][n] = pData[m][n];	//复制现有棋盘黑白分布数据

			//在备份棋盘上模拟下棋，若在当前下棋的白黑比
			//可得到下棋的状态分布矩阵
			bool bResult = play(pBkupData, i, j, curColor);
			if (bResult)
				pState[i][j] = calCurrentWhitePerBlack(pBkupData);
			else
				pState[i][j] = -1;
		}
	}

	clearMemory(pBkupData);

	//如果所有状态全部<=0，则随机选个位置
	bool bflag = true;
	for (i = 0; i < GRID_SIZE; i++)
	{
		for (j = 0; j < GRID_SIZE; j++)
		{
			if (pState[i][j] > 1 && curColor == WHITE)
				bflag = false;
			if (pState[i][j] < 1 && curColor == BLACK)
				bflag = false;
		}
	}

	if (bflag)
	{
		srand((unsigned)time(NULL));
		while (1)
		{
			int rnd_x = rand() % GRID_SIZE;
			int rnd_y = rand() % GRID_SIZE;
			if (pData[rnd_x][rnd_y] != VALID)
				continue;

			selectX = rnd_x;
			selectY = rnd_y;
			return;
		}
	}


	//如果是白棋，则白黑比越高越好
	selectX = selectY = 0;
	double dCurState;
	if (curColor == WHITE)
	{
		dCurState = -2;
		for (i = 0; i < GRID_SIZE; i++)
		{
			for (j = 0; j < GRID_SIZE; j++)
			{
				if (pData[i][j] != VALID)
					continue;

				if (pState[i][j] >= dCurState)
				{
					dCurState = pState[i][j];
					selectX = i;
					selectY = j;
				}
			}
		}
	}

	//如果是黑棋，则白黑比越低越好
	if (curColor == BLACK)
	{
		dCurState = (double)GRID_SIZE*GRID_SIZE;
		for (i = 0; i < GRID_SIZE; i++)
		{
			for (j = 0; j < GRID_SIZE; j++)
			{
				if (pData[i][j] != VALID)
					continue;

				if (1.0f / pState[i][j] < dCurState && 1.0f / pState[i][j] >= 0)
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
	//创建
	createData(pData);
	replay();

	// 当前落子颜色
	int curColor = BLACK;
	int curX, curY;
	while (1)
	{
		//判断游戏是否结束
		if (isEndGame())
		{
			//输出赢家
			whoWinTheGame();

			//重玩
			//replay();
			//curColor = BLACK;
		}

		if (curColor == WHITE)
			cout << "Input Location " << "WHITE" << " (X, Y): " << endl;
		else
			cout << "Input Location " << "BLACK" << " (X, Y): " << endl;

		//如果执白时，可写个逻辑让计算机自动寻找最优位置（小AI，鼓励实现）
		if (curColor == WHITE)
		{
			cout << "AI-WHITE Running..." << endl;
			//RUN MY AI HERE!
			int AI_x, AI_y;
			AI_findTheBestState(curColor, AI_x, AI_y);
			play(pData, AI_x, AI_y, curColor);

			//then display and continue the loop
			display();	//输出
			curColor = BLACK;
			continue;
		}

		//如果执黑时，给出推荐位置
		if (curColor == BLACK)
		{
			cout << "AI-WHITE Running..." << endl;
			//RUN MY AI HERE!
			int AI_x, AI_y;
			AI_findTheBestState(curColor, AI_x, AI_y);

			cout << "Recommendation location is (" << AI_x << ", " << AI_y << ")" << endl;

// 			play(pData, AI_x, AI_y, curColor);
// 
// 			//then display and continue the loop
// 			display();	//输出
// 			curColor = BLACK;
// 			continue;
		}
		
		
		cin >> curX >> curY;

		//修改pData
		bool bResult = play(pData, curX, curY, curColor);

		if (bResult)
		{
			display();	//输出

						// 更换当前落子颜色
			if (curColor == BLACK) curColor = WHITE;
			else curColor = BLACK;
		}
		if (!bResult)
			cout << "Input X and Y Error!" << endl;

	}

	clearMemory(pData);

	return 0;
}

