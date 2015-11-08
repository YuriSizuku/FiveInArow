#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>

#include "AI_kuon_dll.h"
#include "console_io.h"
#include "stdFunc.h"
#include "stdValue.h"


ChessStep steplog={{0},0};
char ChessBoard[15][15];
ScoreStack s_stack;
//extern int evalTable_kuon[EVA_TYPE];
//extern "C" int _declspec(dllimport) evalTable_kuon[EVA_TYPE];
int *evalTable_kuon;


void main()
{
	//printf("\n\n\n");
	printf("=====FiveChess_win32console__1.0 release  \n");
	printf("             ---ai_(くおん,ai_kuon.dll)(0.3)\n");
	printf("             ---designed by devseed(Github name YuriSizuku)\n");;
	ini(ChessBoard,&steplog);

	int sequ;//顺序，=1玩家先手
	int res;
	StepNode step_ai;
	Edge edge;
	Position pos;
	enum ChessType win;
	long start,end;//计时变量
	char *outpath2=".\\chessboard.txt";
	char *outpath=".\\steplog.txt";


	
	printf("\n\n\n\n--------------description--------------------\n");
	printf("-input x y to place the chess\n");
	printf("-input -1 0 to back the step\n");
	printf("-'%c' stand for you,'%c' stand for ai(くおん)\n",black,white);
	printf("-the step will be saved in %s,the chessboard will be saved in %s\n\n",outpath,outpath);
	printf("(level ？primary++)\n");
	printf("select your chess color(1 black,0 white,black first):");
	scanf("%d",&sequ);
	
	//导入dll函数与全局变量
	HMODULE hModule=LoadLibrary(TEXT("AI_kuon.dll"));
	if(hModule==NULL)
	{
		printf("error:require AI_kuon.dll to start!\n");
		CloseHandle(hModule);
		system("pause");
		exit(0);
	}
	AI_kuon_pos=(P_AI_kuon_pos)GetProcAddress(hModule,LPSTR("AI_kuon_pos"));
	AI_kuon_gametree=(P_AI_kuon_gametree)GetProcAddress(hModule,LPSTR("AI_kuon_gametree"));
	evalTable_kuon=(int *)GetProcAddress(hModule,LPSTR("evalTable_kuon"));

	ChessAiExtend ai_info;
	ai_info.in.row=15;
	ai_info.in.column=15;
	ai_info.in.evel_b=(PF_EVA_BOARD)GetProcAddress(hModule,"evaluateBoard");
	ai_info.in.max_depth=2;
	ai_info.in.edge_dis=1;
	ai_info.in.ps_stack=&s_stack;
	
	
	if(sequ!=1)
	{
		PlaceChess(ChessBoard,7,7,white,&steplog);
		edge.x1=edge.x2=7;
		edge.y1=edge.y2=7;
		PrintChessBoard(ChessBoard,15,15);
	}
	while(1)
	{
		printf("input the position to place your chess(x,y):\n");
		res=-1;
		while(res<=0)
		{
			//randxy(&pos.x,&pos.y,15,15);
			scanf("%d%d",&pos.x,&pos.y);
			if(pos.x==-1) 
			{
				BackChess(ChessBoard,&steplog);
				PrintChessBoard(ChessBoard,15,15);
				continue;
			}
			res=PlaceChess(ChessBoard,pos.x,pos.y,black,&steplog);
		}
		if(steplog.count==1 && sequ==1)
		{
			edge.x1=edge.x2=pos.x;
			edge.y1=edge.y2=pos.y;
		}
		checkEdge(&edge,pos,15,15,1);
		ai_info.in.edge=edge;
		printf("x=%d,y=%d\n",pos.x,pos.y);
		printf("edge(%d,%d,%d,%d)\n",edge.x1,edge.x2,edge.y1,edge.y2);
		PrintChessBoard(ChessBoard,15,15);
		SaveChessBoard_ASCI(ChessBoard,15,15,pos.x,pos.y,black,outpath2);
		win=judgeWin(ChessBoard,15,15);
		if(win!=0) break;
		if(steplog.count >= MAX_STEP-1)//棋盘还剩最后一步下满的时候
		{
			int i,j;
			for(i=0;i<15;i++)
				for(j=0;j<15;j++)
				if(ChessBoard[i][j]==empty)
				{
					PlaceChess(ChessBoard,i,j,white,&steplog);
					goto le1;
				}
le1:
			win=judgeWin(ChessBoard,15,15);
			if(win==0) printf("deuce!\n");
			else printf("%c win!\n",win);
			exit(0);
		}
		printf("input the position to place  (くおん)'s chess(x,y):\n");
		res=-1;
		while(res<=0)
		{
			start=clock();
			//randxy(&pos.x,&pos.y,15,15);
			srand(time(0));
			if(rand()%100>=50)//引入概率算子
			{
				step_ai=AI_kuon_pos(ChessBoard,evalTable_kuon,15,15,white);
			}
			else
			{
				step_ai=AI_kuon_gametree(ChessBoard,evalTable_kuon,&ai_info,white);
			}
			pos.x=step_ai.x;pos.y=step_ai.y;
			res=PlaceChess(ChessBoard,pos.x,pos.y,white,&steplog);
		}
		end=clock();
		checkEdge(&edge,pos,15,15,1);
		ai_info.in.edge=edge;
		printf("%d steps after %ld ms.\n",ai_info.out.count_step,end-start);
		printf("x=%d,y=%d\n",pos.x,pos.y);
		printf("edge(%d,%d,%d,%d)\n",edge.x1,edge.x2,edge.y1,edge.y2);
		PrintChessBoard(ChessBoard,15,15);
		SaveChessBoard_ASCI(ChessBoard,15,15,pos.x,pos.y,white,outpath2);
		win=judgeWin(ChessBoard,15,15);
		if(win!=0)
		{
			printf("input 1 to back step,other to end:");
			scanf("%d",&res);
			if(res!=1) break;
			BackChess(ChessBoard,&steplog);
			PrintChessBoard(ChessBoard,15,15);
		}
		//SaveStep_ASCI(&steplog,"D:\\StudyCreateLog\\programMake\\いま\\五子棋AI\\五子棋AI\\steplog2.txt");
		SaveStep_ASCI(&steplog,outpath);
	}
	printf("%c win!\n",win);
	CloseHandle(hModule);
	system("pause");
}