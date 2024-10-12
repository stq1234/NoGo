#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable:4996)
#pragma warning (disable:4244)
#include<iostream>
#include<stdio.h>
#include<Windows.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>
#include<cstring>
#include<string>
#include<algorithm>
#include<iomanip>
#include<math.h>
#include<fstream>
#include<graphics.h>
using namespace std;
int chessboard[9][9] = { 0 }, AI_chessboard[9][9] = { 0 };//0 空 1 黑 2 白
int everyround_recorder[81][2] = { 0 };
int rounds = 0;
bool first = false;//true为AI先手，false为人先手
bool mode = false;//true->AI,false->double
int b[9][9] = { 0 };//judge函数专用棋盘
int jishuqi = 3;
const int timeout = 0.95 * (CLOCKS_PER_SEC);

struct Node
{
	int board[9][9] = { 0 };
	int currentround, x, y;
	Node* father = nullptr;
	Node* children[81] = { nullptr };
	double n = 0, winnum = 0;
	int childrennum = 0;
};

void chessboardprint();
void run(int, int);
void play();
void getchesspiece();
void goback();
void coverchesspiece(int,int);
void initialize();
void record();
void readrecord();
bool judge(int,int);
void digui(int, int, int, int);
void AI();
double UCB(Node* p);
Node* Selection(Node* father);
void MCTS_process(Node* currentNode);
void Expansion(Node* father);
void Rollout(Node* targetNode);
void delete_tree(Node* target);
void decide();
void mainmenu();
void modechoosemenu();
void firstchoosemenu();
bool save_or_not();
int main()
{
	initgraph(GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
	mainmenu();
	int abcdefghijklmnopqrstuvwxyz = _getch();
	closegraph();
}
void chessboardprint()
{
	loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
	setlinecolor(BLACK);
	for (int i = 0; i < 9; i++)
		line(500, 100 + 64 * i, 500 + 8 * 64, 100 + 64 * i);
	for (int i = 0; i < 9; i++)
		line(500 + 64 * i, 100, 500 + 64 * i, 100 + 8 * 64);
	line(490, 90, 490, 100 + 8 * 64 + 10);
	line(490, 90, 500 + 8 * 64 + 10, 90);
	line(500 + 8 * 64 + 10, 90, 500 + 8 * 64 + 10, 100 + 8 * 64 + 10);
	line(490, 100 + 8 * 64 + 10, 500 + 8 * 64 + 10, 100 + 8 * 64 + 10);
	settextstyle(25, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	if (mode)
		outtextxy(900, 50, _T("人机对战"));
	if (!mode)
		outtextxy(900, 50, _T("双人对战"));
}
void goback()
{
	if (rounds == 0)
		return;
	/*if (!(judge(1, 1) && judge(2, 1)))
		return;*/
	if (mode && judge(1, 1) && judge(2, 1))
	{
		int x1 = everyround_recorder[rounds - 1][0], y1 = everyround_recorder[rounds - 1][1];
		int x2 = everyround_recorder[rounds - 2][0], y2 = everyround_recorder[rounds - 2][1];
		chessboard[everyround_recorder[rounds - 1][1]][everyround_recorder[rounds - 1][0]] = 0;
		chessboard[everyround_recorder[rounds - 2][1]][everyround_recorder[rounds - 2][0]] = 0;
		everyround_recorder[rounds - 1][0] = 0, everyround_recorder[rounds - 1][1] = 0;
		everyround_recorder[rounds - 2][0] = 0, everyround_recorder[rounds - 2][1] = 0;
		rounds -= 2;
		coverchesspiece(x1, y1);
		coverchesspiece(x2, y2);
		return;
	}
	int x1 = everyround_recorder[rounds - 1][0], y1 = everyround_recorder[rounds - 1][1];
	chessboard[everyround_recorder[rounds - 1][1]][everyround_recorder[rounds - 1][0]] = 0;
	everyround_recorder[rounds - 1][0] = 0, everyround_recorder[rounds - 1][1] = 0;
	rounds -= 1;
	coverchesspiece(x1, y1);
	return;
}
void play()//负责调度
{
	while (judge(1, 1) && judge(2, 1))
	{
		if (mode)
		{
			if (first)
			{
				if (rounds % 2 == 0)
				{
					AI();
					continue;
				}
				if (rounds % 2 != 0)
				{
					getchesspiece();
					continue;
				}
			}
			if (!first)
			{
				if (rounds % 2 != 0)
				{
					AI();
					continue;
				}
				if (rounds % 2 == 0)
				{
					getchesspiece();
					continue;
				}
			}
		}
		if (!mode)
			getchesspiece();
	}
	if (rounds % 2 != 0)
	{
		if (mode)
		{
			if (first)
			{
				setbkmode(TRANSPARENT);
				settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
				settextcolor(RED);
				outtextxy(450, 650, _T("玩家获胜！"));
			}
			if (!first)
			{
				setbkmode(TRANSPARENT);
				settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
				settextcolor(RED);
				outtextxy(450, 650, _T("电脑获胜！"));
			}
		}
		if (!mode)
		{
			setbkmode(TRANSPARENT);
			settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
			settextcolor(RED);
			outtextxy(450, 650, _T("白方获胜！"));
		}
	}
	if (rounds % 2 == 0)
	{
		if (mode)
		{
			if (!first)
			{
				setbkmode(TRANSPARENT);
				settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
				settextcolor(RED);
				outtextxy(450, 650, _T("玩家获胜！"));
			}
			if (first)
			{
				setbkmode(TRANSPARENT);
				settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
				settextcolor(RED);
				outtextxy(450, 650, _T("电脑获胜！"));
			}
		}
		if (!mode)
		{
			setbkmode(TRANSPARENT);
			settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
			settextcolor(RED);
			outtextxy(450, 650, _T("黑方获胜！"));
		}
	}
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
	outtextxy(100, 90 + 0, _T("开始新游戏"));
	outtextxy(100, 90 + 120, _T("保存棋局"));
	outtextxy(100, 90 + 120 * 2, _T("悔棋"));
	outtextxy(100, 90 + 3 * 120, _T("读取存档"));
	outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
	int thechosennumber = 0;
	int oldcondition = 0, newcondition = 0;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (chessboard[i][j] == 0)
				coverchesspiece(j, i);
	while (true)
	{
		ExMessage m = getmessage(EX_MOUSE);		
		oldcondition = newcondition;
		if (m.x >= 100 && m.y >= 90 + 0 * 120 && m.x <= 350 && m.y <= 140 + 0 * 120)
		{
			newcondition = 1;
			goto getchesspiece_point;
		}
		if (m.x >= 100 && m.y >= 90 + 1 * 120 && m.x <= 350 - 50 && m.y <= 140 + 1 * 120)
		{
			newcondition = 2;
			goto getchesspiece_point;
		}
		if (m.x >= 100 && m.y >= 90 + 2 * 120 && m.x <= 350 - 150 && m.y <= 140 + 2 * 120)
		{
			newcondition = 3;
			goto getchesspiece_point;
		}
		if (m.x >= 100 && m.y >= 90 + 3 * 120 && m.x <= 350 - 50 && m.y <= 140 + 3 * 120)
		{
			newcondition = 4;
			goto getchesspiece_point;
		}
		if (m.x >= 100 && m.y >= 90 + 4 * 120 && m.x <= 350 && m.y <= 140 + 4 * 120)
		{
			newcondition = 5;
			goto getchesspiece_point;
		}
		newcondition = 0;
	getchesspiece_point:
		if (newcondition != oldcondition && newcondition == 1 || newcondition == 1 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(100, 90 + 0, _T("开始新游戏"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 1;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 2 || newcondition == 2 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(100, 90 + 120, _T("保存棋局"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 2;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 3 || newcondition == 3 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(100, 90 + 2 * 120, _T("悔棋"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 3;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 4 || newcondition == 4 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(100, 90 + 3 * 120, _T("读取存档"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 4;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 5 || newcondition == 5 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 5;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 0)
		{
			settextcolor(BLACK);
			setbkmode(TRANSPARENT);
			settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(100, 90 + 0, _T("开始新游戏"));
			outtextxy(100, 90 + 120, _T("保存棋局"));
			outtextxy(100, 90 + 120 * 2, _T("悔棋"));
			outtextxy(100, 90 + 3 * 120, _T("读取存档"));
			outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
		}
	}
	IMAGE fugaiqipanxiafangzhuanyong;
	loadimage(&fugaiqipanxiafangzhuanyong, _T("覆盖棋盘下方专用.jpg"));
	putimage(20, 630, &fugaiqipanxiafangzhuanyong);
	switch (thechosennumber)
	{
	case 1:
	{
		cleardevice();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		if(rounds>0)
		{
			setbkmode(TRANSPARENT);
			settextcolor(RED);
			settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(340, 180, _T("是否保存当前棋局？"));
			bool flag1 = save_or_not();
			if (flag1)record();
		}
		cleardevice();
		initialize();
		modechoosemenu();
		chessboardprint();
		play();
		return;
	}
	case 2:
	{
		cleardevice();
		record();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		setbkmode(TRANSPARENT);
		settextcolor(RED);
		settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
		outtextxy(200, 180, _T("保存成功，是否返回主菜单？"));
		bool flag2 = save_or_not();
		if (flag2)
			mainmenu();
		if (!flag2)
		{
			chessboardprint();
			for (int i = 0; i < rounds; i++)
			{
				if (i % 2 == 0)setfillcolor(BLACK);
				if (i % 2 != 0)setfillcolor(WHITE);
				solidcircle(500 + everyround_recorder[i][0] * 64, 100 + everyround_recorder[i][1] * 64, 16);
			}
			play();
		}
		return;
	}
	case 3:
	{
		goback();
		play();
		return;
	}
	case 4:
	{
		cleardevice();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		setbkmode(TRANSPARENT);
		settextcolor(RED);
		settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
		outtextxy(160, 180, _T("读盘将覆盖当前棋局，是否继续？"));
		bool flag4 = save_or_not();
		if (flag4)
		{
			cleardevice();
			initialize();
			chessboardprint();
			readrecord();
			play();
		}
		if (!flag4)
		{
			chessboardprint();
			for (int i = 0; i < rounds; i++)
			{
				if (i % 2 == 0)setfillcolor(BLACK);
				if (i % 2 != 0)setfillcolor(WHITE);
				solidcircle(500 + everyround_recorder[i][0] * 64, 100 + everyround_recorder[i][1] * 64, 16);
			}
			play();
		}
		return;
	}
	case 5:
	{
		cleardevice();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		if(rounds > 0)
		{
			setbkmode(TRANSPARENT);
			settextcolor(RED);
			settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(340, 180, _T("是否保存当前棋局？"));
			bool flag5 = save_or_not();
			if (flag5)record();
		}
		cleardevice();
		mainmenu();
		return;
	}
	}
	return;	
}
void run(int x, int y)//负责落子和回合数修改
{
	IMAGE fugaiqipanxiafangzhuanyong;
	loadimage(&fugaiqipanxiafangzhuanyong, _T("覆盖棋盘下方专用.jpg"));
	settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	if (chessboard[y][x] != 0)
	{
		settextcolor(RED);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(YELLOW);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(BLUE);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(GREEN);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(BLACK);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(RED);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(RED);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(YELLOW);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(BLUE);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(GREEN);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(BLACK);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		settextcolor(RED);
		outtextxy(450, 650, _T("落子位置无效，请重新选择")); Sleep(100);
		putimage(20, 630, &fugaiqipanxiafangzhuanyong);
		return;
	}
	if (rounds % 2 == 0)
		setfillcolor(BLACK);
	if (rounds % 2 != 0)
		setfillcolor(WHITE);
	chessboard[y][x] = rounds % 2 + 1;
	everyround_recorder[rounds][0] = x, everyround_recorder[rounds][1] = y;
	solidcircle(500 + x * 64, 100 + y * 64, 14);
	setlinecolor(RED);
	circle(500 + x * 64, 100 + y * 64, 15);
	circle(500 + x * 64, 100 + y * 64, 16);
	if (rounds > 0)
	{
		coverchesspiece(everyround_recorder[rounds - 1][0], everyround_recorder[rounds - 1][1]);
		if (rounds % 2 != 0)
			setfillcolor(BLACK);
		if (rounds % 2 == 0)
			setfillcolor(WHITE);
		solidcircle(500 + 64 * everyround_recorder[rounds - 1][0], 100 + 64 * everyround_recorder[rounds - 1][1], 16);
	}
	//playmusic;
	rounds++;
}
bool save_or_not()
{
	setbkmode(TRANSPARENT);
	settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	settextcolor(BLACK);
	outtextxy(450 + 70, 350, _T("是"));
	outtextxy(560 + 70, 350, _T("否"));
	int thechosennumber = 0;
	int oldcondition = 0, newcondition = 0;
	while (1)
	{
		oldcondition = newcondition;
		ExMessage m = getmessage(EX_MOUSE);
		if (m.x >= 450 + 70 && m.y >= 350 && m.x <= 515 + 70 && m.y <= 410)
		{
			newcondition = 1;
			goto modechoose_point;
		}
		if (m.x >= 560 + 70 && m.y >= 350 && m.x <= 625 + 70 && m.y <= 410)
		{
			newcondition = 2;
			goto modechoose_point;
		}
		newcondition = 0;
	modechoose_point:
		if (newcondition != oldcondition && newcondition == 1 || newcondition == 1 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(450 + 70, 350, _T("是"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 1;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 2 || newcondition == 2 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(560 + 70, 350, _T("否"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 2;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 0)
		{
			settextcolor(BLACK);
			outtextxy(450 + 70, 350, _T("是"));
			outtextxy(560 + 70, 350, _T("否"));
		}
	}
	switch (thechosennumber)
	{
	case 1:
		return true;
	case 2:
		return false;
	}
}
void coverchesspiece(int x,int y)
{
	IMAGE img1;
	loadimage(&img1, _T("覆盖棋子用.jpg"));
	putimage(500 + x * 64 - 16, 100 + y * 64 - 16, &img1);
	setlinecolor(BLACK);
	if (x == 0 && y == 0)
	{
		line(490, 90, 516, 90); 
		line(490, 90, 490, 116);
	}
	if (x == 0 && y == 8) 
	{
		line(500  - 10, 100 + 8 * 64-16, 500  - 10, 100 + 8 * 64 + 10);
		line(490, 100 + 8 * 64 + 10, 500+16, 100 + 8 * 64 + 10);
	}
	if (x == 8 && y == 0)
	{
		line(500 + 8 * 64 + 10, 90, 500 + 8 * 64 - 16, 90);
		line(500 + 8 * 64 + 10, 90, 500 + 8 * 64 + 10, 116);
	}
	if (x == 8 && y == 8)
	{
		line(500 + 8 * 64 + 10, 100 + 8 * 64 + 10, 500 + 8 * 64 - 16, 100 + 8 * 64 + 10);
		line(500 + 8 * 64 + 10, 100 + 8 * 64 + 10, 500 + 8 * 64 + 10, 100 + 8 * 64 - 16);
	}
	if (x == 0 && y > 0 && y < 8)
	{
		line(490, 100 + y * 64 - 16, 490, 100 + y * 64 + 16);
	}
	if (x == 8 && y > 0 && y < 8)
	{
		line(500 + 8 * 64 + 10, 100 + y * 64 - 16, 500 + 8 * 64 + 10, 100 + y * 64 + 16);
	}
	if (y == 0 && x > 0 && x < 8)
	{
		line(500 + x * 64 - 16, 90, 500 + x * 64 + 16, 90);
	}
	if (y == 8 && x > 0 && x < 8)
	{
		line(500 + x * 64 - 16, 100 + 8 * 64 + 10, 500 + x * 64 + 16, 100 + 8 * 64 + 10);
	}
	if (x > 0)line(500 + 64 * x - 20, 100 + 64 * y, 500 + 64 * x, 100 + 64 * y);
	if (x < 8)line(500 + 64 * x, 100 + 64 * y, 500 + 64 * x + 20, 100 + 64 * y);
	if (y > 0)line(500 + 64 * x, 100 + 64 * y - 20, 500 + 64 * x, 100 + 64 * y);
	if (y < 8)line(500 + 64 * x, 100 + 64 * y, 500 + 64 * x, 100 + 64 * y + 20);
	return;
}
void getchesspiece()//菜单while_playing也在这里
{
	/*rectangle(100, 90 + 0 * 120, 350, 140 + 0 * 120);
	rectangle(100, 90 + 1 * 120, 350 - 50, 140 + 1 * 120);
	rectangle(100, 90 + 2 * 120, 350 - 150, 140 + 2 * 120);
	rectangle(100, 90 + 3 * 120, 350 - 50, 140 + 3 * 120);
	rectangle(100, 90 + 4 * 120, 350, 140 + 4 * 120);*/
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
	outtextxy(100, 90 + 0, _T("开始新游戏"));
	outtextxy(100, 90 + 120, _T("保存棋局"));
	outtextxy(100, 90 + 120 * 2, _T("悔棋"));
	outtextxy(100, 90 + 3 * 120, _T("读取存档"));
	outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
	int thechosennumber = 0;
	int oldcondition = 0, newcondition = 0;
	int oldchesspiecetempx = -1, oldchesspiecetempy = -1, newchesspiecetempx = -1, newchesspiecetempy = -1;
	while (true)
	{
		ExMessage m = getmessage(EX_MOUSE);
		if (m.x >= 500 - 32 && m.x <= 500 + 8 * 64 + 32 && m.y >= 100 - 32 && m.y <= 100 + 8 * 64 + 32)
		{
			oldchesspiecetempx = newchesspiecetempx, oldchesspiecetempy = newchesspiecetempy;
			int x = m.x, y = m.y;
			int i = 0, j = 0;
			if (x <= 500)
			{
				i = 0;
				goto loop1;
			}
			if (x >= 500 + 64 * 8 + 32)
			{
				i = 8;
				goto loop1;
			}
			for (i = 0; i < 9; i++)
				if (x >= 500 + i * 64 && x <= 564 + i * 64)
					break;
			if (x - i * 64 - 500 > 32)i++;
loop1:
			if (y <= 100)
			{
				j = 0;
				goto loop2;
			}
			if (y >= 100 + 8 * 64 + 32)
			{
				j = 8;
				goto loop2;
			}			
			for (j = 0; j < 9; j++)
				if (y >= 100 + j * 64 && y <= 164 + j * 64)
					break;
			if (y - j * 64 - 100 > 32)j++;
loop2:
			newchesspiecetempx = i, newchesspiecetempy = j;
			if (newchesspiecetempx != oldchesspiecetempx || newchesspiecetempy != oldchesspiecetempy)
			{
				if (oldchesspiecetempx != -1 && chessboard[oldchesspiecetempy][oldchesspiecetempx] == 0)
					coverchesspiece(oldchesspiecetempx, oldchesspiecetempy);
				if (chessboard[newchesspiecetempy][newchesspiecetempx] == 0)
				{
					setfillcolor(RGB(167, 158, 157));
					solidcircle(500 + newchesspiecetempx * 64, 100 + newchesspiecetempy * 64, 16);
				}
			}
			if (m.message == WM_LBUTTONDOWN)
			{
				run(newchesspiecetempx, newchesspiecetempy);
				return;
			}
		}
		else
		{
			if (oldchesspiecetempx != -1 && chessboard[oldchesspiecetempy][oldchesspiecetempx] == 0)
				coverchesspiece(oldchesspiecetempx, oldchesspiecetempy);
			if (newchesspiecetempx != -1 && chessboard[newchesspiecetempy][newchesspiecetempx] == 0)
				coverchesspiece(newchesspiecetempx, newchesspiecetempy);
			newchesspiecetempx = -1, newchesspiecetempy = -1, oldchesspiecetempx = -1, oldchesspiecetempy = -1;
			oldcondition = newcondition;
			if (m.x >= 100 && m.y >= 90 + 0 * 120 && m.x <= 350 && m.y <= 140 + 0 * 120)
			{
				newcondition = 1;
				goto getchesspiece_point;
			}
			if (m.x >= 100 && m.y >= 90 + 1 * 120 && m.x <= 350 - 50 && m.y <= 140 + 1 * 120)
			{
				newcondition = 2;
				goto getchesspiece_point;
			}
			if (m.x >= 100 && m.y >= 90 + 2 * 120 && m.x <= 350 - 150 && m.y <= 140 + 2 * 120)
			{
				newcondition = 3;
				goto getchesspiece_point;
			}
			if (m.x >= 100 && m.y >= 90 + 3 * 120 && m.x <= 350 - 50 && m.y <= 140 + 3 * 120)
			{
				newcondition = 4;
				goto getchesspiece_point;
			}
			if (m.x >= 100 && m.y >= 90 + 4 * 120 && m.x <= 350 && m.y <= 140 + 4 * 120)
			{
				newcondition = 5;
				goto getchesspiece_point;
			}
			newcondition = 0;
getchesspiece_point:
			if (newcondition != oldcondition && newcondition == 1 || newcondition == 1 && m.message == WM_LBUTTONDOWN)
			{
				setbkmode(TRANSPARENT);
				settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
				settextcolor(RED);
				outtextxy(100, 90 + 0, _T("开始新游戏"));
				if (m.message == WM_LBUTTONDOWN)
				{
					thechosennumber = 1;
					break;
				}
			}
			if (newcondition != oldcondition && newcondition == 2 || newcondition == 2 && m.message == WM_LBUTTONDOWN)
			{
				setbkmode(TRANSPARENT);
				settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
				settextcolor(RED);
				outtextxy(100, 90 + 120, _T("保存棋局"));
				if (m.message == WM_LBUTTONDOWN)
				{
					thechosennumber = 2;
					break;
				}
			}
			if (newcondition != oldcondition && newcondition == 3 || newcondition == 3 && m.message == WM_LBUTTONDOWN)
			{
				setbkmode(TRANSPARENT);
				settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
				settextcolor(RED);
				outtextxy(100, 90 + 2 * 120, _T("悔棋"));
				if (m.message == WM_LBUTTONDOWN)
				{
					thechosennumber = 3;
					break;
				}
			}
			if (newcondition != oldcondition && newcondition == 4 || newcondition == 4 && m.message == WM_LBUTTONDOWN)
			{
				setbkmode(TRANSPARENT);
				settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
				settextcolor(RED);
				outtextxy(100, 90 + 3 * 120, _T("读取存档"));
				if (m.message == WM_LBUTTONDOWN)
				{
					thechosennumber = 4;
					break;
				}
			}
			if (newcondition != oldcondition && newcondition == 5 || newcondition == 5 && m.message == WM_LBUTTONDOWN)
			{
				setbkmode(TRANSPARENT);
				settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
				settextcolor(RED);
				outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
				if (m.message == WM_LBUTTONDOWN)
				{
					thechosennumber = 5;
					break;
				}
			}
			if (newcondition != oldcondition && newcondition == 0)
			{
				setbkmode(TRANSPARENT);
				settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
				settextcolor(BLACK);
				outtextxy(100, 90 + 0, _T("开始新游戏"));
				outtextxy(100, 90 + 120, _T("保存棋局"));
				outtextxy(100, 90 + 120 * 2, _T("悔棋"));
				outtextxy(100, 90 + 3 * 120, _T("读取存档"));
				outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
			}
		}
	}
	switch (thechosennumber)
	{
	case 1:
	{
		cleardevice();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		if(rounds>0)
		{
			setbkmode(TRANSPARENT);
			settextcolor(RED);
			settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(340, 180, _T("是否保存当前棋局？"));
			bool flag1 = save_or_not();
			if (flag1)record();
		}
		cleardevice();
		initialize();
		modechoosemenu();
		chessboardprint();
		play();
		return;
	}
	case 2:
	{
		cleardevice();
		record();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		setbkmode(TRANSPARENT);
		settextcolor(RED);
		settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
		outtextxy(200, 180, _T("保存成功，是否返回主菜单？"));
		bool flag2 = save_or_not();
		if (flag2)
			mainmenu();
		if (!flag2)
		{
			chessboardprint();
			for (int i = 0; i < rounds; i++)
			{
				if (i % 2 == 0)setfillcolor(BLACK);
				if (i % 2 != 0)setfillcolor(WHITE);
				solidcircle(500 + everyround_recorder[i][0] * 64, 100 + everyround_recorder[i][1] * 64, 16);
			}
			play();
		}
		return;
	}
	case 3:
	{
		goback();
		return;
	}
	case 4:
	{
		cleardevice();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		setbkmode(TRANSPARENT);
		settextcolor(RED);
		settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
		outtextxy(160, 180, _T("读盘将覆盖当前棋局，是否继续？"));
		bool flag4 = save_or_not();
		if (flag4)
		{
			cleardevice();
			initialize();
			readrecord();
			play();
		}
		if (!flag4)
		{
			chessboardprint();
			for (int i = 0; i < rounds; i++)
			{
				if (i % 2 == 0)setfillcolor(BLACK);
				if (i % 2 != 0)setfillcolor(WHITE);
				solidcircle(500 + everyround_recorder[i][0] * 64, 100 + everyround_recorder[i][1] * 64, 16);
			}
			play();
		}
		return;
	}
	case 5:
	{
		cleardevice();
		loadimage(NULL, _T("棋盘.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
		if(rounds>0)
		{
			setbkmode(TRANSPARENT);
			settextcolor(RED);
			settextstyle(70, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
			outtextxy(340, 180, _T("是否保存当前棋局？"));
			bool flag5 = save_or_not();
			if (flag5)record();
		}
		cleardevice();
		mainmenu();
		return;
	}
	}
}
void initialize()
{
	first = false;
	mode = false;
	rounds = 0;
	for (int i = 0; i < 81; i++)
	{
		everyround_recorder[i][0] = 0, everyround_recorder[i][1] = 0;
	}
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			chessboard[i][j] = 0;
}
void mainmenu()
{
	cleardevice();
	loadimage(NULL, _T("主界面.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
	//rectangle(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 150, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, GetSystemMetrics(SM_CXSCREEN) * 0.4 + 150, GetSystemMetrics(SM_CYSCREEN) * 0.4 + 20);
	//rectangle(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120, GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 85);
	//rectangle(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290-aaa, GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120, GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 230-aaa);
	setbkmode(TRANSPARENT);
	settextstyle(86, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	settextcolor(YELLOW);
	outtextxy(500, 180, _T("主菜单"));
	settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 157, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, _T("开始新游戏"));
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, _T("读取记录"));
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25, _T("退出游戏"));
	int thechosennumber = 0;
	int oldcondition = 0, newcondition = 0;
	while (1)
	{
		oldcondition = newcondition;
		ExMessage m = getmessage(EX_MOUSE);
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 150 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 150 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 20)
		{
			newcondition = 1;
			goto mainmenu_point;
		}
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 85)
		{
			newcondition = 2;
			goto mainmenu_point;

		}
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 230 - 25)
		{
			newcondition = 3;
			goto mainmenu_point;

		}
		newcondition = 0;
	mainmenu_point:
		if (newcondition != oldcondition && newcondition == 1 || newcondition == 1 && m.message == WM_LBUTTONDOWN)
		{
			settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 157, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, _T("开始新游戏"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 1;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 2 || newcondition == 2 && m.message == WM_LBUTTONDOWN)
		{
			settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, _T("读取记录"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 2;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 3 || newcondition == 3 && m.message == WM_LBUTTONDOWN)
		{
			settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25, _T("退出游戏"));			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 3;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 0)
		{
			settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
			settextcolor(YELLOW);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 157, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, _T("开始新游戏"));
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, _T("读取记录"));
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25, _T("退出游戏"));
		}
	}
	switch (thechosennumber)
	{
	case 1:
		cleardevice();
		initialize();
		modechoosemenu();
		chessboardprint();
		play();
		return;
	case 2:
		initialize();		
		readrecord();
		play();
		return;
	case 3: exit(0);
	}
}
void modechoosemenu()
{
	cleardevice();
	loadimage(NULL, _T("主界面.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
	setbkmode(TRANSPARENT);
	settextcolor(YELLOW);
	settextstyle(76, 0, _T("华文行楷"), 0, 0, 10, false, false, false);	
	outtextxy(370, 200, _T("请选择游戏模式"));
	settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145 + 60 - 145 - 40, _T("人机对战"));
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25 + 60 - 145 - 40, _T("双人对战"));
	int thechosennumber = 0;
	int oldcondition = 0, newcondition = 0;
	while (1)
	{
		oldcondition = newcondition;
		ExMessage m = getmessage(EX_MOUSE);
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145 + 60 - 145 - 40 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 85 + 60 - 145 - 40)
		{
			newcondition = 1;
			goto modechoose_point;
		}
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25 + 60 - 145 - 40 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 230 - 25 + 60 - 145 - 40)
		{
			newcondition = 2;
			goto modechoose_point;
		}
		newcondition = 0;
modechoose_point:
		if (newcondition != oldcondition && newcondition == 1 || newcondition == 1 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145 + 60 - 145 - 40, _T("人机对战"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 1;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 2 || newcondition == 2 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25 + 60 - 145 - 40, _T("双人对战"));
			if(m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 2;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 0)
		{
			settextcolor(YELLOW);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145 + 60 - 145 - 40, _T("人机对战"));
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25 + 60 - 145 - 40, _T("双人对战"));
		}
	}
	switch (thechosennumber)
	{
	case 1:
		mode = true;
		firstchoosemenu();
		return;
	case 2:
		mode = false;
		return;
	}
}
void firstchoosemenu()
{
	cleardevice();
	loadimage(NULL, _T("主界面.jpg"), GetSystemMetrics(SM_CXSCREEN) * 0.8, GetSystemMetrics(SM_CYSCREEN) * 0.8);
	setbkmode(TRANSPARENT);	
	settextcolor(YELLOW);
	settextstyle(76, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	outtextxy(370, 200, _T("请选择先手顺序"));
	settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, _T("玩家先手"));
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, _T("电脑先手"));
	outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25, _T("随机决定"));
	int thechosennumber = 0;
	int oldcondition = 0, newcondition = 0;
	while (1)
	{
		oldcondition = newcondition;
		ExMessage m = getmessage(EX_MOUSE);
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 20)
		{
			newcondition = 1;
			goto firstdecide_point;
		}
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 85)
		{
			newcondition = 2;
			goto firstdecide_point;
		}
		if (m.x >= GetSystemMetrics(SM_CXSCREEN) * 0.4 - 120 && m.y >= GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25 && m.x <= GetSystemMetrics(SM_CXSCREEN) * 0.4 + 120 && m.y <= GetSystemMetrics(SM_CYSCREEN) * 0.4 + 60 + 230 - 25)
		{
			newcondition = 3;
			goto firstdecide_point;
		}
		newcondition = 0;
firstdecide_point:
		if (newcondition != oldcondition && newcondition == 1 || newcondition == 1 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, _T("玩家先手"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 1;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 2 || newcondition == 2 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, _T("电脑先手"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 2;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 3 || newcondition == 3 && m.message == WM_LBUTTONDOWN)
		{
			settextcolor(RED);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25, _T("随机决定"));
			if (m.message == WM_LBUTTONDOWN)
			{
				thechosennumber = 3;
				break;
			}
		}
		if (newcondition != oldcondition && newcondition == 0)
		{
			settextcolor(YELLOW);
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 40, _T("玩家先手"));
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 145, _T("电脑先手"));
			outtextxy(GetSystemMetrics(SM_CXSCREEN) * 0.4 - 123, GetSystemMetrics(SM_CYSCREEN) * 0.4 - 60 + 290 - 25, _T("随机决定"));
		}
	}
	switch (thechosennumber)
	{
	case 1:
		first = false;
		return;
	case 2:
		first = true;
		return;
	case 3:
		decide();
		return;
	}
}
void record()
{
	ofstream fout("不围棋记录.txt");
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			if (j)
				fout << " ";
			fout << chessboard[i][j];
			if (j == 8)
				fout << endl;
		}
	fout << first << " " << mode << " " << rounds << endl;
	for (int i = 0; i < rounds; i++)
		fout << everyround_recorder[i][0] << " " << everyround_recorder[i][1] << endl;
	fout.close();
}
void readrecord()
{
	ifstream fin("不围棋记录.txt");
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			fin >> chessboard[i][j];
	fin >> first >> mode >> rounds;
	chessboardprint();
	for (int i = 0; i < rounds; i++)
	{
		fin >> everyround_recorder[i][0] >> everyround_recorder[i][1];
		if (i % 2 == 0)
			setfillcolor(BLACK);
		if (i % 2 != 0)
			setfillcolor(WHITE);
		solidcircle(500 + everyround_recorder[i][0] * 64, 100 + everyround_recorder[i][1] * 64, 16);
		Sleep(500);
	}	
	fin.close();
}
bool judge(int color, int chessboardchoose)//(1)有提子返回false 没有返回true;(2)chessboardchoose为1时复制chessboard给b，2时复制AI_chessboard给b;
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			if (chessboardchoose == 1)
				b[i][j] = chessboard[i][j];
			if (chessboardchoose == 2)
				b[i][j] = AI_chessboard[i][j];
		}
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			if (b[i][j] == color)
			{
				bool abc = false;
				digui(i, j, color, jishuqi);
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
						if (b[i][j] == jishuqi)
							if (b[max(i - 1, 0)][j] == 0 || b[min(i + 1, 8)][j] == 0 || b[i][max(j - 1, 0)] == 0 || b[i][min(j + 1, 8)] == 0)
								abc = true;
				if (!abc)
				{
					jishuqi = 3;
					return false;
				}
				jishuqi++;
			}
		}
	jishuqi = 3;
	return true;
}
void digui(int x, int y, int color, int z)
{
	b[x][y] = z;
	if (b[max(x - 1, 0)][y] == color)
		digui(max(x - 1, 0), y, color, z);
	if (b[min(x + 1, 8)][y] == color)
		digui(min(x + 1, 8), y, color, z);
	if (b[x][max(y - 1, 0)] == color)
		digui(x, max(y - 1, 0), color, z);
	if (b[x][min(y + 1, 8)] == color)
		digui(x, min(y + 1, 8), color, z);
}
void AI()
{
	if (rounds == 0 && chessboard[8][0] == 0)
	{
		run(0,8);
		return;
	}
	if (rounds == 2 && chessboard[0][0] == 0)
	{
		run(0, 0);
		return;
	}
	Node* rootNode = new Node;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			rootNode->board[i][j] = chessboard[i][j];
	rootNode->currentround = rounds;
	rootNode->n += 1;
	Expansion(rootNode);
	int start = clock();
	while (clock() - start < timeout)
		MCTS_process(rootNode);
	Node* bestchild = rootNode->children[0];
	double maxUCB = -10000000.;
	for (int i = 0; i < rootNode->childrennum; i++)
		if (UCB(rootNode->children[i]) > maxUCB)
		{
			bestchild = rootNode->children[i];
			maxUCB = UCB(rootNode->children[i]);
		}
	if (bestchild == nullptr)
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if (chessboard[i][j] == 0)
				{
					run(j, i);
					return;
				}
	int x = bestchild->x, y = bestchild->y;
	delete_tree(rootNode);
	run(x, y);
}
void delete_tree(Node* target)
{
	for (int i = 0; i < target->childrennum; i++)
		delete_tree(target->children[i]);
	delete target;
}
void Rollout(Node* targetNode)
{
	double delta_value = 0;
	int MAX = 0;
	if (rounds <= 20)MAX = 30;
	if (rounds > 20 && rounds <= 40)MAX = 40;
	if (rounds > 40)MAX = 50;
	for(int i=0;i<MAX;i++)
	{
		int roundtemp = targetNode->currentround;
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				AI_chessboard[i][j] = targetNode->board[i][j];
		while (judge(1, 2) && judge(2, 2))
		{
			int x = rand() % 9, y = rand() % 9;
			while (AI_chessboard[x][y] != 0)
				x = rand() % 9, y = rand() % 9;
			if (roundtemp % 2 == 1)
				AI_chessboard[x][y] = 2;
			if (roundtemp % 2 == 0)
				AI_chessboard[x][y] = 1;
			roundtemp++;
		}
		if (first)
			if (roundtemp % 2 == 0)
				delta_value += 1.;
		if (!first)
			if (roundtemp % 2 != 0)
				delta_value += 1.;
	}
	Node* p = targetNode;
	while (p->father != nullptr)
	{
		p->n += 1., p->winnum += delta_value;
		p = p->father;
	}
	p->n += 1.;
}
void Expansion(Node* father)
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			if (father->board[i][j] == 0)
			{
				bool flag = false;
				for (int ii = 0; ii < 9; ii++)
					for (int jj = 0; jj < 9; jj++)
						AI_chessboard[ii][jj] = father->board[ii][jj];
				int roundtemp = father->currentround + 1;
				if (roundtemp % 2 == 1)
					AI_chessboard[i][j] = 1;
				if (roundtemp % 2 == 0)
					AI_chessboard[i][j] = 2;
				if (judge(1, 2) && judge(2, 2))
					flag = true;
				if (flag)
				{
					father->children[father->childrennum] = new Node;
					father->children[father->childrennum]->father = father;
					father->children[father->childrennum]->currentround = father->currentround + 1;
					father->children[father->childrennum]->x = j, father->children[father->childrennum]->y = i;
					for (int ii = 0; ii < 9; ii++)
						for (int jj = 0; jj < 9; jj++)
							father->children[father->childrennum]->board[ii][jj] = father->board[ii][jj];
					if ((father->children[father->childrennum]->currentround) % 2 == 1)
						father->children[father->childrennum]->board[i][j] = 1;
					if ((father->children[father->childrennum]->currentround) % 2 == 0)
						father->children[father->childrennum]->board[i][j] = 2;
					(father->childrennum) += 1;
				}
			}
		}
}
void MCTS_process(Node* currentNode)
{
	if (currentNode == nullptr)
		return;
	if (currentNode->childrennum == 0)
	{
		if (currentNode->n == 0)
		{
			Rollout(currentNode);
			return;
		}
		if ((currentNode->n) > 0)
		{
			Expansion(currentNode);
			MCTS_process(currentNode->children[0]);
			return;
		}
	}
	else
	{
		Node* childrenNodeSelected = Selection(currentNode);
		MCTS_process(childrenNodeSelected);
		return;
	}
}
Node* Selection(Node* father)
{
	Node* p = father;
	double maxUCB = -10000000.;
	while (p->childrennum != 0)
	{
		int maxUCBnumber = 0;
		for (int i = 0; i < p->childrennum; i++)
			if (UCB(p->children[i]) > maxUCB)
			{
				maxUCB = UCB(p->children[i]);
				maxUCBnumber = i;
			}
		p = p->children[maxUCBnumber];
	}
	return p;
}
double UCB(Node* p)
{
	return p->winnum / (p->n + 0.001)+ 2 * sqrt(log(p->father->n) / (p->n + 0.000001));
}
void decide()
{	
	IMAGE fugaiqipanxiafangzhuanyong;
	loadimage(&fugaiqipanxiafangzhuanyong, _T("覆盖棋盘下方专用.jpg"));
	chessboardprint();
	settextstyle(66, 0, _T("华文行楷"), 0, 0, 10, false, false, false);
	int x = rand() % 2;
	if (x == 0)
	{
		first = false;
		outtextxy(400, 650, _T("经系统随机抽签，您为先手")); 
	}
	if (x == 1)
	{
		first = true;
		outtextxy(400, 650, _T("经系统随机抽签，电脑为先手")); 
	}
	settextcolor(BLACK);
	settextstyle(55, 0, _T("华文行楷"), 0, 0, 10, false, 0, 0);
	outtextxy(100, 90 + 0, _T("开始新游戏"));
	outtextxy(100, 90 + 120, _T("保存棋局"));
	outtextxy(100, 90 + 120 * 2, _T("悔棋"));
	outtextxy(100, 90 + 3 * 120, _T("读取存档"));
	outtextxy(100, 90 + 4 * 120, _T("返回主菜单"));
	Sleep(2000);
	putimage(20, 630, &fugaiqipanxiafangzhuanyong);
	play();
}