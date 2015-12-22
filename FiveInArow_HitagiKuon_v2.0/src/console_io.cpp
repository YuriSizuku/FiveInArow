#include "console_io.h"
#include <time.h>
#include "stdFunc.h"
#include <stdio.h>
#include<string>
int SaveStep_ASCI(ChessStep *pstep,char *path)
{
	FILE *fp=fopen(path,"w+");
	if(fp==NULL) return -1;
	
	int i=0;
	for(i=0;i<pstep->count;i++)
	{
		fprintf(fp,"[%d](%d,%d),%c\n",i,pstep->step[i].x,pstep->step[i].y,pstep->step[i].color);
	}
	fclose(fp);
	return 0;
}
int SaveChessBoard_ASCI(char ChessBoard[][MAX_COLUMN],int row,int column,int x,int y,enum ChessType color,char *path)
{
	static int count=0;
	FILE *fp=fopen(path,"a+");
	if(fp==NULL) return -1;
	int i,j;
	fprintf(fp,"\n【%i】,(%c,%d,%d)\n",count,color,x,y);
	fprintf(fp,"  |");
	for(i=0;i<column;i++) fprintf(fp,"%2d ",i);
	fprintf(fp,"\n-- ");
	for(i=0;i<column;i++) fprintf(fp,"-- ");
	fprintf(fp,"\n");
	for(i=0;i<row;i++)
	{
		fprintf(fp,"%2d|",i);
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]!='0')
				fprintf(fp," %c ",ChessBoard[i][j]);
			else fprintf(fp,"   ");
		}
		fprintf(fp,"\n");
	}
	count++;
	fclose(fp);
	return 0;
}
int SaveChessBoard2_ASCI(char ChessBoard[][MAX_COLUMN],int row,int column,
						int count_blackwin,int count_game,
						char *path)
{
	FILE *fp=fopen(path,"a+");
	if(fp==NULL) return -1;
	int i,j;
	fprintf(fp,"=======(%d,%d)(%d,%lf)==========\n",count_blackwin,count_game,count_game-count_blackwin,(double)(count_game-count_blackwin)/(double)count_game);
	fprintf(fp,"  |");
	for(i=0;i<column;i++) fprintf(fp,"%2d ",i);
	fprintf(fp,"\n-- ");
	for(i=0;i<column;i++) fprintf(fp,"-- ");
	fprintf(fp,"\n");
	for(i=0;i<row;i++)
	{
		fprintf(fp,"%2d|",i);
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]!='0')
				fprintf(fp," %c ",ChessBoard[i][j]);
			else fprintf(fp,"   ");
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
	return 0;
}
int LoadStep_ASCI(char ChessBoard[][MAX_COLUMN],ChessStep *pstep,char *path)
{
	FILE *fp=fopen(path,"r");
	if(fp==NULL) return -1;
	int x,y,i;
	enum ChessType color;
	while(!feof(fp))
	{
		fscanf(fp,"[%d](%d,%d),%c\n",&i,&x,&y,&color);
		PlaceChess(ChessBoard,x,y,color,pstep);
	}
	fclose(fp);
	return i;
}
int LoadChessBoard_ASCI(char ChessBoard[][MAX_COLUMN],int row,int column,char *path)
//根据打印的图表输入棋局
//读取成功返回0，否则返回-1
{
	FILE *fp=fopen(path,"r");
	if(fp==NULL) return -1;
	memset(ChessBoard,'0',MAX_COLUMN*MAX_ROW*sizeof(char));
	
	int i,j;
	char c;

	do{c=fgetc(fp);}while(c!='\n');
	do{c=fgetc(fp);}while(c!='\n');

	for(i=0;i<row;i++)
	{
		fgetc(fp);fgetc(fp);fgetc(fp);
		for(j=0;j<column;j++)
		{
			c=fgetc(fp);
			if(c=='\n') break;
			c=fgetc(fp);
			if(c==white || c==black) ChessBoard[i][j]=c;
			c=fgetc(fp);
			if(c=='\n') break;
		}
	}
	fclose(fp);
	return 0;
}
void PrintChessBoard(char ChessBoard[][MAX_COLUMN],int row,int column)
{
	int i,j;
	printf("  |");
	for(i=0;i<column;i++) printf("%2d ",i);
	printf("\n-- ");
	for(i=0;i<column;i++) printf("-- ");
	printf("\n");
	for(i=0;i<row;i++)
	{
		printf("%2d|",i);
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]!='0')
				printf(" %c ",ChessBoard[i][j]);
			else printf("   ");
		}
		printf("\n");
	}
}
void GetTimeStr(char *str)
{
	time_t rowtime;
	struct tm *timeinfo;
	time(&rowtime);
	timeinfo=localtime(&rowtime);
	sprintf(str,"%02d%02d%02d_%02d%02d%02d",timeinfo->tm_year+1900,
								timeinfo->tm_mon+1,
								timeinfo->tm_mday,
								timeinfo->tm_hour,
								timeinfo->tm_min,
								timeinfo->tm_sec);
	delete timeinfo;
}