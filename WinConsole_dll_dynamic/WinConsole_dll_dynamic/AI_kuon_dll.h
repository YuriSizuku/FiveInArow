#include "stdFunc.h"
#ifndef _AI_kuon_h
#define _AI_kuon_h

typedef StepNode (*P_AI_kuon_pos)
								(char ChessBoard[][MAX_COLUMN],
								int *EvaluateTable,
								int row,
								int column,
								enum ChessType color_max);
typedef StepNode (*P_AI_kuon_gametree)
							(char ChessBoard[][MAX_COLUMN],
							int *EvaluateTable,
							ChessAiExtend *ai_info,
							enum ChessType color_max);
P_AI_kuon_pos AI_kuon_pos;
P_AI_kuon_gametree AI_kuon_gametree;

#endif