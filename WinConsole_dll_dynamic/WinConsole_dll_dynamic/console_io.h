#include "stdValue.h"

#ifndef _console_io_h
#define _console_io_h
int SaveStep_ASCI(ChessStep *pstep,char *path);
int SaveChessBoard_ASCI(char ChessBoard[][MAX_COLUMN],
							int row,int column,
							int x,int y,
							enum ChessType color,char *path);
int LoadStep_ASCI(char ChessBoard[][MAX_COLUMN],ChessStep *pstep,char *path);
int LoadChessBoard_ASCI(char ChessBoard[][MAX_COLUMN],int row,int column,char *path);
void PrintChessBoard(char ChessBoard[][MAX_COLUMN],int row,int column);
#endif