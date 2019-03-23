#include "pch.h"
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <iostream>
#include <stack>
#include <map>
#include <windows.h>
using namespace std;

int N = 30;

int get_poi(int x) {
	return (x + N / 2) / N ;
}

int chess[20][20];
bool wins[20][20][600];
int win_tot,win_cnt[600];
int value[20][20];
stack<pair<int,int>> chess_record;

//初始化胜利方案
void initwins() {
	memset(wins, false, sizeof(wins));
	win_tot = 0;
	for (int i = 1; i <= 15; i++)
		for (int j = 1; j <= 11; j++)
			for (int k = 0; k < 5; k++)
				wins[i][j+k][++win_tot] = true;
	for (int i = 1; i <= 11; i++)
		for (int j = 1; j <= 15; j++)
			for (int k = 0; k < 5; k++)
				wins[i+k][j][++win_tot] = true;
	for (int i = 1; i <= 11; i++)
		for (int j = 1; j <= 11; j++)
			for (int k = 0; k < 5; k++)
				wins[i+k][j+k][++win_tot] = true;
	for (int i = 1; i <= 11; i++)
		for (int j = 5; j <= 15; j++)
			for (int k = 0; k < 5; k++)
				wins[i+k][j-k][++win_tot] = true;
}

//评估胜利方案
void initcnt(int color) {
	memset(win_cnt, 0, sizeof(win_cnt));
	for (int k = 1; k <= win_tot; k++) {
		for (int i = 1; i <= 15; i++)
			for (int j = 1; j <= 15; j++) 
				if (wins[i][j][k]) {
					if (chess[i][j] == color) 
						win_cnt[k]++;
					if (chess[i][j] == color*(-1))
						win_cnt[k]=-5;
				}	
	}
}

//更新评估价值
void valuing(int color) {
	memset(value, 0, sizeof(value));
	initcnt(color);
	for (int i=1; i<=15; i++)
		for (int j=1; j<=15; j++)
			if (chess[i][j] == 0) {
				for (int k = 1; k <= win_tot; k++) {
					switch (win_cnt[k])
					{
					case 1:value[i][j] += 320;
						break;
					case 2:value[i][j] += 420;
						break;
					case 3:value[i][j] += 4200;
						break;
					case 4:value[i][j] += 20000;
						break;
					}
				}
			}
	initcnt(color*(-1));
	for (int i = 1; i <= 15; i++)
		for (int j = 1; j <= 15; j++)
			if (chess[i][j] == 0) {
				for (int k = 1; k <= win_tot; k++) {
					switch (win_cnt[k])
					{
					case 1:value[i][j] += 200;
						break;
					case 2:value[i][j] += 400;
						break;
					case 3:value[i][j] += 2000;
						break;
					case 4:value[i][j] += 10000;
						break;
					}
				}
			}
}

//最大价值落子
void makechess(int &x, int &y) {
	x = 0; y = 0;
	for (int i = 1; i <= 15; i++) 
		for (int j = 1; j <= 15; j++) 
			if (chess[i][j] == 0)
				if ((x == 0 && y == 0) || value[i][j] >= value[x][y]) {
					x = i;
					y = j;
				}
}

//更新棋盘图像
void initboard() {
	// 清空棋盘
	clearrectangle(0, 0, 600, 600);
	setfillcolor(BLUE);
	fillrectangle(0, 0, 600, 600);
	// 绘制棋盘
	for (int i = N; i <= 15 * N; i+=N)
		line(i, 30, i, 15 * N);
	for (int i = N; i <= 15 * N; i+=N)
		line(30, i, 15 * N, i);
	// 放置棋子
	for (int i = 1; i <= 15; i ++)
		for (int j = 1; j <= 15; j++) {
			if (chess[i][j] == 1) {
				setfillcolor(WHITE);
				fillcircle(i*N, j*N, 10);
			}
			if (chess[i][j] == -1) {
				setfillcolor(BLACK);
				fillcircle(i*N, j*N, 10);
			}
		}
}

//直线and斜线搜索
int findLine(int x, int y, int dx, int dy, int col) {
	if (chess[x][y] != col) return 0;
	return findLine(x+dx, y+dy, dx, dy, col)+1;
}

//胜利判断
bool findAll(int x, int y, int col) {
	if (findLine(x, y, -1, 0, col) + findLine(x, y, 1, 0, col)-1 >= 5) return true;
	if (findLine(x, y, 0, -1, col) + findLine(x, y, 0, 1, col)-1 >= 5) return true;
	if (findLine(x, y, -1, -1, col) + findLine(x, y, 1, 1, col)-1 >= 5) return true;
	if (findLine(x, y, -1, 1, col) + findLine(x, y, 1, -1, col)-1 >= 5) return true;
	return false;
}

//平局判断
bool findEND() {
	for (int i = 1; i <= 15; i++) {
		for (int j = 1; j <= 15; j++) {
			if (chess[i][j] == 0) return false;
		}
	}
	return true;
}

int main(){
	// 初始化
	initwins();
	memset(chess, 0, sizeof(chess));
	initgraph(600, 600);
	initboard();

	MOUSEMSG m;	// 定义鼠标消息
	int chess_color = 1;//当前棋子颜色
	outtextxy(200, 470, (chess_color == 1) ? L"当前执子：白" : L"当前执子：黑");
	while (true){
		// 获取一条鼠标消息
		m = GetMouseMsg();
		switch (m.uMsg){
		case WM_LBUTTONDOWN: {
			int x, y;
			//按住Ctrl电脑落子
			if (m.mkCtrl) {
				if (chess_record.empty()) {
					x = (int)rand() % 14 + 1;
					y = (int)rand() % 14 + 1;
				}
				else {
					initcnt(chess_color);
					valuing(chess_color);
					makechess(x, y);
				}
				chess_record.push(make_pair(x, y));//记录棋子坐标
				chess[x][y] = chess_color;	
				initboard();//重新绘制棋盘
				if (findAll(x, y, chess[x][y]) || findEND()) goto END;//判断终局
				chess_color *= -1;//反转棋子颜色
				outtextxy(200, 470, (chess_color == 1) ? L"当前执子：白" : L"当前执子：黑");
			}
			//点击落子
			else {
				x = get_poi(m.x);
				y = get_poi(m.y);
				if (x>=1 && x<=15 && y>=1 && y<=15 && chess[x][y] == 0) {
					chess_record.push(make_pair(x, y));//记录棋子坐标
					chess[x][y] = chess_color;
					initboard();//重新绘制棋盘
					if (findAll(x, y, chess[x][y]) || findEND()) goto END;//判断终局
					chess_color *= -1;//反转棋子颜色
					outtextxy(200, 470, (chess_color == 1) ? L"当前执子：白" : L"当前执子：黑");
				}
			}
			
			break;				
		}
		case WM_RBUTTONDOWN:
			if (!chess_record.empty()) {
				chess[chess_record.top().first][chess_record.top().second] = 0;//删除棋子
				chess_record.pop();//删除棋子记录
				chess_color *= -1;//反转棋子颜色
				outtextxy(200, 470, (chess_color == 1) ? L"当前执子：白" : L"当前执子：黑");
				initboard();
			}
			break;
		}
	}

	END:
	if (findEND())  outtextxy(230, 230,  L"平局!");
	else outtextxy(230, 230, (chess_color==1)?L"白方胜利!": L"黑方胜利!");
	_getch();
	closegraph();// 关闭图形窗口
	return 0;
}