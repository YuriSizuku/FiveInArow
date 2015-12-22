#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "stdFunc.h"

//��ʼ�������壬�������ж���Ӯ
void ini(char ChessBoard[][MAX_COLUMN],ChessStep *steplog)
{
	memset(ChessBoard,'0',MAX_COLUMN*MAX_ROW*sizeof(char));
	if(steplog!=NULL) steplog->count=0;
}
int PlaceChess(char ChessBoard[][MAX_COLUMN],int x,int y,enum ChessType color,ChessStep *steplog)
//�ɹ����ز��������򷵻�0
{
	if(ChessBoard[x][y]!='0') return 0;
	ChessBoard[x][y]=color;
	steplog->step[steplog->count].x=x;
	steplog->step[steplog->count].y=y;
	steplog->step[steplog->count].color=color;
	steplog->count++;
	return steplog->count;
}
int BackChess(char ChessBoard[][MAX_COLUMN],ChessStep *steplog)
//���壬�ɹ����ز��������򷵻�0
{
	if(steplog->count<=0) return -1;
	ChessBoard[steplog->step[steplog->count-1].x][steplog->step[steplog->count-1].y]='0';
	(steplog->count)--;
	ChessBoard[steplog->step[steplog->count-1].x][steplog->step[steplog->count-1].y]='0';
	return --(steplog->count);
}
enum ChessType judgeWin(char ChessBoard[][MAX_COLUMN],int row,int column)//����0��black��white
{
	int i,j,count=0;//count��ֶ��ٸ���������
	int count_chess=0;
	for(i=0;i<row;i++)//����Ƿ�����
		for(j=0;j<column;j++)
			if(ChessBoard[i][j]!=empty)
				count_chess++;
	for(i=0;i<row;i++)//�������Ƿ���5����
	{
		count=1;
		for(j=1;j<column;j++)
		{
			if(ChessBoard[i][j]==ChessBoard[i][j-1] && ChessBoard[i][j]!='0')
			{
				count++;
				if(count>=5) return (enum ChessType)ChessBoard[i][j];
			}
			else
				count=1;
		}
	}
	for(j=0;j<column;j++)//�������Ƿ���5����
	{
		count=1;
		for(i=1;i<row;i++)
		{
			if(ChessBoard[i][j]==ChessBoard[i-1][j] && ChessBoard[i][j]!='0')
			{
				count++;
				if(count>=5) return (enum ChessType)ChessBoard[i][j];
			}
			else
				count=1;
		}
	}
	for(i=0;i<row-4;i++)//�����°����Խ��߷���������
	{
		count=1;
		for(j=1;j<row-i;j++)
		{
			if(ChessBoard[i+j][j]==ChessBoard[i+j-1][j-1] && ChessBoard[i+j][j]!='0')
			{
				count++;
				if(count>=5) return  (enum ChessType)ChessBoard[i+j][j];
			}
			else 
				count=1;
		}
	}
	for(j=1;j<column-4;j++)//����ϰ����Խ��߷���������
	{
		count=1;
		for(i=1;i<column-j;i++)
		{
			if(ChessBoard[i][j+i]==ChessBoard[i-1][j+i-1] && ChessBoard[i][j+i]!='0')
			{
				count++;
				if(count>=5) return  (enum ChessType)ChessBoard[i][j+i];
			}
			else 
				count=1;
		}
	}
	for(i=4;i<row;i++)//�����ϰ븱�Խ��߷���������
	{
		count=1;
		for(j=1;j<=i;j++)
		{
			if(ChessBoard[i-j][j]==ChessBoard[i-j+1][j-1] && ChessBoard[i-j][j]!='0')
			{
				count++;
				if(count>=5) return  (enum ChessType)ChessBoard[i-j][j];
			}
			else 
				count=1;
		}
	}
	for(j=1;j<column-4;j++)//����°븱�Խ��߷���������
	{
		count=1;
		for(i=j+1;i<row;i++)
		{
			if(ChessBoard[i][(column-1)-(i-j)]==ChessBoard[i-1][(column-1)-(i-j)+1] && ChessBoard[i][(column-1)-(i-j)]!='0')
			{
				count++;
				if(count>=5) return  (enum ChessType)ChessBoard[i][(column-1)-(i-j)];
			}
			else 
			count=1;
		}
	}
	if(count_chess >=row * column) return even;
	return  (enum ChessType)0;
}
//�������ӱ߽���
int isEdge(int x,int y,int row,int column)//���Ǳ�Ե����0����������(1~4),����������������(5~8)
{
	if(x>0 && x<row-1 && y>0 && y<column-1) return 0;
	if(x==0)
	{
		if(y==0) return 5;//����
		if(y==column-1) return 6;//����
		return 1;//��
	}
	if(x==row-1)
	{
		if(y==0) return 7;//����
		if(y==column-1) return 8;//����
		return 2;
	}
	if(y==0) return 3;//��
	if(y==column-1) return 4;//��
}
int checkEdge(Edge *edge,Position pos,int row,int column,int dis)
//����һ���鿴�߽��Ƿ�ı䣬��û���Ҷ�û���߽緵��0�����򷵻�1
//disΪԭ���߽��dis���룬edgeΪ���е����dis���ΰ�����Σ������߽���Ϊ���߽�����棩
{
	if(pos.x - edge->x1 >=dis && edge->x2 - pos.x >=dis
		&& pos.y - edge->y1 >=dis && edge->y2-pos.y >=dis) return 0;
	if(pos.x==0) edge->x1=0;
	else if(pos.x==row-1) edge->x2=row-1;
	else
	{
		if(pos.x - edge->x1 < dis) 
			edge->x1 = (pos.x-dis < 0) ? 0 : pos.x-dis;
		if(edge->x2 - pos.x < dis) 
			edge->x2 = (pos.x+dis >row-1) ? row-1 :pos.x+dis;
	}
	if(pos.y==0) edge->y1=0;
	else if(pos.y==column-1) edge->y2=column-1;
	else
	{
		if(pos.y - edge->y1 < dis) 
			edge->y1 = (pos.y-dis <0) ? 0 : pos.y-dis;
		if(edge->y2 - pos.y < dis) 
			edge->y2 = (pos.y+dis > column-1) ? column-1:pos.y+dis;
	}
	return 1;
}
int findEdge(char ChessBoard[][MAX_COLUMN],int row,int column,Edge *edge,int dis )
{
	int i,j,count=0;
	Position pos;
	for(i=0;i<row;i++)
	{
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]!=empty)
			{
				if(count==0)
				{
					edge->x1=edge->x2=i;
					edge->y1=edge->y2=j;
				}
				pos.x=i;pos.y=j;
				checkEdge(edge,pos,row,column,dis);
				count++;
			}
		}
	}
	return count;
}
//������dps�õ�ջ
int push_ScoreStack(ScoreStack *ps,ScoreStackNode pn)
{
	ps->stack[ps->count++]=pn;
	return ps->count;
}
ScoreStackNode pop_ScoreStack(ScoreStack *ps)
{
	return ps->stack[--ps->count];
}
ScoreStackNode* getTop_ScoreStack(ScoreStack *ps)
{
	return &ps->stack[ps->count-1];
}
int isEmpty_ScoreStack(ScoreStack *ps)
{
	if(ps->count>0) return 0;
	else return 1;
}
int iniScoreStack(ScoreStack *ps,int size)//�ɹ�����0 ���򷵻�-1 
{
	ps->count=0;
	ps->capacity=size;
	return 0;
}
//����
void randxy(int *x,int *y,int maxX,int maxY)
{
	//static int c=time(0);
	srand(time(0));
	*x=rand()%maxX;
	*y=rand()%maxY;
}