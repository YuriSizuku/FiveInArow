#include "stdValue.h"

#ifndef _stdFunc_h
#define _stdFunc_h

/*初始化，下棋，悔棋与判断输赢*/
void ini(char ChessBoard[][MAX_COLUMN],ChessStep *steplog);
int PlaceChess(char ChessBoard[][MAX_COLUMN],int x,int y,enum ChessType color,ChessStep *steplog);
//成功返回步数，否则返回0
int BackChess(char ChessBoard[][MAX_COLUMN],ChessStep *steplog);
//悔棋，成功返回步数，否则返回0
enum ChessType judgeWin(char ChessBoard[][MAX_COLUMN],int row,int column);//返回0，black或white


/*所有棋子边界检查*/
int isEdge(int x,int y,int row,int column);
//不是边缘返回0，上下左右(1~4),左上右上左下右下(5~8)
int checkEdge(Edge *edge,Position pos,int row,int column,int dis);
//增加一点后查看边界是否改变，若没变且都没到边界返回0，否则返回1
//dis为原来边界多dis距离，edge为所有点距离dis矩形包络矩形（若到边界则为到边界包络面）
int findEdge(char ChessBoard[][MAX_COLUMN],int row,int column,Edge *edge,int dis );


/*博弈树dps用的栈*/
int push_ScoreStack(ScoreStack *ps,ScoreStackNode pn);
ScoreStackNode pop_ScoreStack(ScoreStack *ps);
ScoreStackNode* getTop_ScoreStack(ScoreStack *ps);
int isEmpty_ScoreStack(ScoreStack *ps);
int iniScoreStack(ScoreStack *ps,int size);//成功返回0 否则返回-1 


/*其他*/
void randxy(int *x,int *y,int maxX,int maxY);
#endif