#include "stdFunc.h"
#include "stdValue.h"
#include <stdlib.h>
#include <time.h>
#ifndef _AI_kuon_h
#define _AI_kuon_h
/*  AI_kuon
    静态估值与博弈树剪枝
*/
#define KUON_NUM_EVALUATE 30
int kuon_evaluateBoard(char ChessBoard[][MAX_COLUMN],
						int *EvaluateTable,Edge edge,
						int row,int column,enum ChessType color_max,
						void *extra);
StepNode Ai_kuon_pos(char ChessBoard[][MAX_COLUMN],
								int *EvaluateTable,
								int row,
								int column,
								enum ChessType color_max);
StepNode Ai_kuon_gametree(char ChessBoard[][MAX_COLUMN],
							int *EvaluateTable,//一定是符合ai_kuon格式的评分表
							ChessAiExtend *ai_info,
							enum ChessType color_max);
StepNode Ai_kuon_play(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge *edge,
						ChessType color_max);
StepNode Ai_extern_play(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge *edge,
						ChessType color_max);//用于外部训练,公用接口
#endif