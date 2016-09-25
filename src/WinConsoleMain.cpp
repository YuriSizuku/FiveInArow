#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include <direct.h>

#include "Ai_kuon.h"
#include "Ai_hitagi.h"
#include "console_io.h"

#pragma comment(lib,"FiveInArow_hitagi.lib")
#pragma comment(lib,"FiveInArow_kuon.lib")

/*变量宏*/
#define MAX_LEN 200
/*END*/

/*预定义宏(这些仅为了调试方便)*/
#define CHESS_DISPLAY
//#define PRE_DEBUG
#ifndef _WinConsoleMain_cpp
#define _WinConsoleMain_cpp

#ifdef CHESS_DISPLAY
#define DISPLAY_CHESSBOARD(a) \
	printf("current: 【%d】(%d)\n",count_game,count_trun);\
	PrintChessBoard(cb,row,column);

#define DISPLAY_TITLE(name,color)\
	printf("\n%s (%c) draw at (%d,%d)\n",name,color,tstep.x,tstep.y);
#else 
#define DISPLAY_CHESSBOARD()
#define DISPLAY_TITLE(name,color)
#endif 

#ifndef CHESS_DISPLAY
#define DISPLAY_END(color)\
printf("current: 【%d】(%d)\n",count_game,count_trun,count_game);\
PrintChessBoard(cb,row,column);\
printf("=================%c win!======================\n\n",color);
#else
#define DISPLAY_END(color)\
printf("=================%c win!======================\n\n",color);
#endif




#define KUON_GO(color) \
    extern int kuon_evalTable[KUON_NUM_EVALUATE];\
	StepNode tstep;\
	tstep=Ai_kuon_play(cb,row,column,edge,color);\
	PlaceChess(cb,tstep.x,tstep.y,color,steplog);\
	DISPLAY_TITLE("クオン",color)\
	DISPLAY_CHESSBOARD()

#define USER_GO(color)\
		int res=-1;\
		Position pos;\
		printf("input the position to place your chess(x,y):\n");\
		while(res<=0)\
		{\
			scanf("%d%d",&pos.x,&pos.y);\
			StepNode tstep;\
			if(pos.x==-1) \
			{\
				BackChess(cb,steplog);\
				PrintChessBoard(cb,15,15);\
				continue;\
			}\
			res=PlaceChess(cb,pos.x,pos.y,color,steplog);\
		}\
		DISPLAY_CHESSBOARD();

#endif
/*END*/

/*全局变量区*/
extern int _declspec(dllimport) kuon_evalTable[KUON_NUM_EVALUATE];

int count_blackwin=0;
int playmode=1;											//0默认，1test，2train
int trainmode=1;										//1与ai_kuon,2与玩家,3外部dll train.dll
enum ChessType color_player=black;						//玩家颜色
int ai_mode=3;											//1 ai_kuon,2 ai_hitagi,3 mix
int  freq_save=200;										//训练保存频率

char str_StartTime[MAX_LEN/5];							//开始时间字符串
char path_traindll[MAX_LEN]="ai_train.dll";				//外部调用训练dll路径
char path_log[MAX_LEN]="log";							//日志路径
char path_weight[MAX_LEN]="train";						//训练记录路径
char file_steplog[MAX_LEN]="last_Steplog.txt";			//上一次步骤记录
char file_lastweight[MAX_LEN]="last_HitagiTrain.net";	//上一次训练记录
char file_loadweight[MAX_LEN]="HitagiTrain.net";		//神经网络权值
PF_AI_PLAY Ai_train_play;								//外部dll调用
/*END*/

/*棋盘事件回调函数*/
void onInfomation()
{
	printf("=====FiveInArow_win32console_v2.0 release  \n");
	printf("             ---ai_(くおん,FiveInArow_kuon.dll)(0.3.2)\n");
	printf("             ---ai_(ひたぎ,FiveInArow_hitagi.dll)(0.2.0)\n");
	printf("             ---designed by devseed(Github name YuriSizuku)\n");;
	printf("\n\n\n\n--------------description--------------------\n");
	printf("The project is compiled on entire c with extern dependence fann.\n");
	printf("The ai(hitagi) is based on ann network and reinformance learning(bp-td)\n");
	printf("The ai(hitagi) can be trained to be more intelligent\n");
	printf("The default hitagi ann weight(%s) is what I trained %d times.\n",file_loadweight,92600);
	printf("You can train the ai(hitagi) by youself or by extern dll with the interface below:\n");
	printf("typedef StepNode (*PF_AI_PLAY) (char ChessBoard[][MAX_COLUMN],int row,int column,Edge *edge,ChessType color_max)\n");
	printf("\n\n");
	printf("-input x y to place the chess\n");
	printf("-input -1 0 to back the step\n");
	printf("-'%c' stand for you,'%c' stand for ai(くおん)\n",black,white);
	printf("(level ？hyper primary)\n");
	printf("\n\n");
}  
void onSet()
{
	mkdir(path_log);
	mkdir(path_weight);
	GetTimeStr(str_StartTime);
	int select;
SetStart:
	printf("========================Start=======================\n");
	printf("--- 1.  play with ai ひたぎ (td-bp gametreeα-β)\n");
	printf("--- 2.  play with ai クオン (gametreeα-β)\n");
	printf("--- 3.  play with ai ひたぎＸクオン (mix)\n");
	printf("--- 4.  train ai ひたぎ　with ai クオン\n");
	printf("--- 5.  train ai ひたぎ　with player\n");
	printf("--- 6.  train ai ひたぎ　with extern ai(%s)\n",path_traindll);
	printf("--- 7.  set player's chess color\n");
	printf("--- 8.  set saving frequence\n");
	printf("--- 9.  set ひたぎ ann weight\n");
	printf("input a number to go:");
	scanf("%d",&select);
	if(select>=1 && select<=3)
	{
		playmode=1;
		if(select==1) ai_mode=2;
		else if(select==2) ai_mode=1;
		else if(select==3) ai_mode=3;
	}
	else if(select>=4 && select<=6)
	{
		playmode=2;
		if(select==4) trainmode=1;
		else if(select==5) trainmode=2;
		else if(select==6) trainmode=3;
	}
	else if(select==7)
	{
		printf("\ncurrent player's chess color is %c\n",color_player);
		printf("input a number to go (1 blcak %c,2 white %c ):",black,white);
		scanf("%d",&select);
		if(select==1)
			color_player=black;
		else if(select==2)
			color_player=white;
		goto SetStart;
	}
	else if(select==8)
	{
		printf("\ncurrent frequence is %d\n",freq_save);
		printf("input frequence:");
		scanf("%d",&select);
		if(select >=0)  freq_save=select;
		goto SetStart;
	}
	else if(select==9)
	{
		printf("\ncurrent ひたぎ ann weight path is:%s\n",file_loadweight);
		printf("input the new path (0 for default):");
		char buf[200];
		scanf("%s",buf);
		if(!strcmp(buf,"0"))
			strcpy(file_loadweight,"HitagiTrain.net");
		else
			strcpy(file_loadweight,buf);
		goto SetStart;
	}
	else goto SetStart;
}
void onGameStart(char cb[][MAX_COLUMN],int row,int column,
				 ChessStep *steplog,Edge *edge,
				 const int count_game,
				 void *extra)
{
	//PlaceChess(cb,7,7,white,steplog);
}
void onGameEnd(char cb[][MAX_COLUMN],int row,int column,
			   ChessStep *steplog,Edge *edge,
			   const int count_game,
			   void *extra)
{

}
void onBlackPlay(char cb[][MAX_COLUMN],int row,int column,
				 ChessStep *steplog,Edge *edge,
				 const int count_trun,const int count_game,
				 void *extra)//已经不用，用onChessPlay替代
{
}
void onWhitePlay(char cb[][MAX_COLUMN],int row,int column,
				 ChessStep *steplog,
				 Edge *edge,
				 const int count_trun,const int count_game,
				 void *extra)//已经不用，用onChessPlay替代
{
}
void onChessPlay(char cb[][MAX_COLUMN],int row,int column,
				 ChessType color,
				 ChessStep *steplog,
				 Edge *edge,
				 const int count_trun,const int count_game,
				 void *extra)
{
	if(playmode==1)//测试模式
	{
		if(color_player==color)
		{
			USER_GO(color);
		}
		else 
		{
			if(count_trun==1 && color==black)
			{
				PlaceChess(cb,7,7,black,steplog);
			}
			else
			{
				StepNode tstep;
				srand(time(0));
				int p=rand()%100;
				if((p<50 && ai_mode==3) || ai_mode==1)
				{
					tstep=Ai_kuon_play(cb,row,column,edge,color);
					DISPLAY_TITLE("クオン",color);
				}
				if((p>=50 && ai_mode==3 || ai_mode==2))
				{
					tstep=Ai_hitagi_play(cb,row,column,*edge,color,(struct hitagi_ann_data *)extra);
					DISPLAY_TITLE("ひたぎ",color);
				}
				PlaceChess(cb,tstep.x,tstep.y,color,steplog);
			}
			DISPLAY_CHESSBOARD()
		}
	}
	else if(playmode==2)//训练模式
	{
		StepNode tstep;
		if(trainmode==1)//ai_kuon
		{
			if(count_trun==1) 
			{
				tstep.x=7;
				tstep.y=7;
			}
			else
			{
				if(color==white)
				{
					tstep=Ai_hitagi_play2(cb,row,column,*edge,color,(struct hitagi_ann_data *)extra);
				}
				else
				{
					//tstep=Ai_kuon_play(cb,row,column,edge,color);
					//extern int kuon_evalTable[KUON_NUM_EVALUATE];
					tstep=Ai_kuon_pos(cb,kuon_evalTable,row,column,color);
				}
			}
			PlaceChess(cb,tstep.x,tstep.y,color,steplog);
		}
		else if(trainmode==2)
		{
			if(color_player==color)
			{
				USER_GO(color);
			}
			else
			{
				tstep=Ai_hitagi_play2(cb,row,column,*edge,color,(struct hitagi_ann_data *)extra);
				PlaceChess(cb,tstep.x,tstep.y,color,steplog);
				DISPLAY_TITLE("ひたぎ",color);
				DISPLAY_CHESSBOARD()
			}
		}
		else if(trainmode==3)//extern dll
		{
			if(count_trun==1) 
			{
				tstep.x=7;
				tstep.y=7;
			}
			else
			{
				if(color==white)
				{
					tstep=Ai_hitagi_play2(cb,row,column,*edge,color,(struct hitagi_ann_data *)extra);
				}
				else
				{
					tstep=Ai_train_play(cb,row,column,edge,color);
				}
			}
			PlaceChess(cb,tstep.x,tstep.y,color,steplog);	
		}
	}
	findEdge(cb,row,column,edge,1);
}
void onWin(char cb[][MAX_COLUMN],int row,int column,
			ChessStep *steplog,
			ChessType color_win,
			const int count_trun,const int count_game,
			void *extra)
{
	char path[100];

	PrintChessBoard(cb,row,column);
	DISPLAY_END(color_win);
	if(color_win==black) count_blackwin++;
	printf("=======(%d,%d)(%d,%lf)==========\n",count_blackwin,count_game,count_game-count_blackwin,(double)(count_game-count_blackwin)/(double)count_game);
	
	sprintf(path,"%s\\%s.txt",path_log,str_StartTime);
	SaveChessBoard2_ASCI(cb,row,column,count_blackwin,count_game,path);//记录棋盘
	SaveStep_ASCI(steplog,file_steplog);
	
	Ai_hitagi_train(cb,row,column,steplog,color_win,(struct hitagi_ann_data *)extra);//训练
	hitagi_save((struct hitagi_ann_data *)extra,file_lastweight);
	if(count_game%freq_save==0)
	{
		sprintf(path,"%s\\%s_%d.net",path_weight,str_StartTime,count_game);
		hitagi_save((struct hitagi_ann_data *)extra,path);
	}
}
/*END*/

void main()
{
	/*变量定义*/
	const int row=15;
	const int column=15;
	char ChessBoard[MAX_ROW][MAX_COLUMN];
	ChessStep steplog={0,{0}};
	Edge edge;
	long count_game=0;//记录游戏局数
	int count_turn=0;//每局回合数
	const long max_game=-1;//最大游戏局数
	char color_win=0;
	void *extra;
	struct hitagi_ann_data *hitagi_data=NULL;
	/*END*/
	
/*PRE_DEBUG REGION*/
#ifdef PRE_DEBUG
#define _ empty
#define x black
#define o white
	int result[HITAGI_NUM_EVALUATE];
	char vector[15]={o,x,_,x,x,x,o,o,_,o,x,x,_,_};
	hitagi_getEvalTypes_line(vector,0,14,x,result);
	ini(ChessBoard,&steplog);
	
	/*PlaceChess(ChessBoard,4,4,white,&steplog);
	PlaceChess(ChessBoard,5,5,white,&steplog);
	PlaceChess(ChessBoard,5,4,white,&steplog);
	PlaceChess(ChessBoard,5,7,white,&steplog);
	PlaceChess(ChessBoard,4,5,white,&steplog);
	PlaceChess(ChessBoard,6,5,white,&steplog);
	PlaceChess(ChessBoard,4,6,white,&steplog);
	PlaceChess(ChessBoard,6,4,white,&steplog);
	PrintChessBoard(ChessBoard,row,column);
	hitagi_getEvalTypes(ChessBoard,row,column,white,result);*/
#endif

	onInfomation();
	onSet();
	/*各变量初始值*/
	count_game=0;
	edge.x1=edge.y1=7;edge.x2=edge.y2=7;

	FILE *fp=fopen(file_loadweight,"r");
	if(fp==NULL)
	{
		printf("\nThe ひたぎ　ann weight (%s) is not exsit\n",file_loadweight);
		printf("creating new...\n");
		hitagi_data=hitagi_create(NULL);
	}
	else
	{
		fclose(fp);
		printf("\nLoading ひたぎ　ann weight (%s)...\n",file_loadweight);
		hitagi_data=hitagi_load(file_loadweight);
	}
	if(hitagi_data==NULL)
	{
		printf("\nひたぎ　ann weight fatal error!\n");
		system("pause");
		exit(0);
	}
	printf("ひたぎ　ann create successfully!\n\n");
	extra=hitagi_data;
	if(trainmode==3)//外部dll训练
	{
		printf("\nloading extern dll(%s) to train...\n",path_traindll);
		HMODULE hModule=LoadLibrary(path_traindll);
		if(hModule==NULL)
		{
			printf("error:require %s to start!\n",path_traindll);
			CloseHandle(hModule);
			system("pause");
			exit(0);
		}
		printf("\nloading extern dll(%s) successfully\n",path_traindll);
		Ai_train_play=(PF_AI_PLAY)GetProcAddress(hModule,LPSTR("Ai_extern_play"));
	}
	//system("pause");
	/*END*/
	
	while(count_game!=max_game)
	{
		count_turn=0;
		color_win=0;
		count_game++;
		ini(ChessBoard,&steplog);
		onGameStart(ChessBoard,row,column,&steplog,&edge,count_game,extra);
		while(color_win==0)
		{
			count_turn++;
			//onBlackPlay(ChessBoard,row,column,&steplog,&edge,count_turn,count_game,extra);
			onChessPlay(ChessBoard,row,column,black,&steplog,&edge,count_turn,count_game,extra);
			color_win=judgeWin(ChessBoard,row,column);
			if(color_win!=0)
			{
				onWin(ChessBoard,row,column,&steplog,(enum ChessType)color_win,count_turn,count_game,extra);
				break;
			}
			//onWhitePlay(ChessBoard,row,column,&steplog,&edge,count_turn,count_game,extra);
			onChessPlay(ChessBoard,row,column,white,&steplog,&edge,count_turn,count_game,extra);
			color_win=judgeWin(ChessBoard,row,column);
			if(color_win!=0)
			{
				onWin(ChessBoard,row,column,&steplog,(enum ChessType)color_win,count_turn,count_game,extra);
				break;
			}
		}
		onGameEnd(ChessBoard,row,column,&steplog,&edge,count_game,extra);
	}
}