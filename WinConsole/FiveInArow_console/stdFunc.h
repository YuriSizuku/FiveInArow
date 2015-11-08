#include "stdValue.h"

#ifndef _stdFunc_h
#define _stdFunc_h

/*��ʼ�������壬�������ж���Ӯ*/
void ini(char ChessBoard[][MAX_COLUMN],ChessStep *steplog);
int PlaceChess(char ChessBoard[][MAX_COLUMN],int x,int y,enum ChessType color,ChessStep *steplog);
//�ɹ����ز��������򷵻�0
int BackChess(char ChessBoard[][MAX_COLUMN],ChessStep *steplog);
//���壬�ɹ����ز��������򷵻�0
enum ChessType judgeWin(char ChessBoard[][MAX_COLUMN],int row,int column);//����0��black��white


/*�������ӱ߽���*/
int isEdge(int x,int y,int row,int column);
//���Ǳ�Ե����0����������(1~4),����������������(5~8)
int checkEdge(Edge *edge,Position pos,int row,int column,int dis);
//����һ���鿴�߽��Ƿ�ı䣬��û���Ҷ�û���߽緵��0�����򷵻�1
//disΪԭ���߽��dis���룬edgeΪ���е����dis���ΰ�����Σ������߽���Ϊ���߽�����棩
int findEdge(char ChessBoard[][MAX_COLUMN],int row,int column,Edge *edge,int dis );


/*������dps�õ�ջ*/
int push_ScoreStack(ScoreStack *ps,ScoreStackNode pn);
ScoreStackNode pop_ScoreStack(ScoreStack *ps);
ScoreStackNode* getTop_ScoreStack(ScoreStack *ps);
int isEmpty_ScoreStack(ScoreStack *ps);
int iniScoreStack(ScoreStack *ps,int size);//�ɹ�����0 ���򷵻�-1 


/*����*/
void randxy(int *x,int *y,int maxX,int maxY);
#endif