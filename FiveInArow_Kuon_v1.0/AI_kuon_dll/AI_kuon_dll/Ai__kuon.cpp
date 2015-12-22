#include <stdio.h>

#include "Ai_kuon.h"

int evalTable_kuon[EVA_TYPE]=
{
	//�����������Խ���������������֣�*�������ӣ�#�Է���0�յ�
	//��*����#��ϣ�0����ԭ���Ƿ���Ը���*����»�0*λ�ÿ��Ի�����
	//��ԳƱ任��Ч
	/*0*/100000,//*****
	/*1*/30000, //0****0,
	/*2*/10000,  //0***0*0,
	/*3*/3100,	//?**00**?
	/*4*/8000,  //#****0
	/*5*/5000,  //#***0*
	/*6*/9000,  //0***0
	/*7*/4000,	//0*0**0
	/*8*/400,   //#***00
	/*9*/1350,   //#**0*0
	/*10*/1200,  //00**0
	/*11*/300,	//0*0*0
	/*12*/100,  //#**000
	/*13*/160,  //#*0*00
	/*14*/10,  //0*0000
	/*15*/5,	//#*0000
	/*16*/10,   //#**.# X
	/*17*/150, //0*0*0*0
	/*18*/450,  //#*0*0*0
	/*19*/150,  //#**.*#
	/*20*/2200, //#**0**#
	/*21*/2500, //0**0**0
	/*22*/2300, //#**0**0
	/*23*/30000,//3+3��(#4)X(3)
};
int _eva_pos_isBreak1(int i,int sn,int flag)//�ж��Ƿ���Ϊ�Ѿ��Ǳ߽�ֹͣ����
{
	if(flag==1 || flag==5 || flag==6)//��
	{
		if(i==0 && sn==-1) return 1;
		if(i==2 && sn==-1) return 1;
		if(i==3 && sn==-1) return 1;
	}
	if(flag==2 || flag==7 || flag==8)//��
	{
		if(i==0 && sn==1) return 2;
		if(i==2 && sn==1) return 2;
		if(i==3 && sn==1) return 2;
	}
	if(flag==3 || flag==5 || flag==7)//��
	{
		if(i==1 && sn==-1) return 3;
		if(i==2 && sn==-1) return 3;
		if(i==3 && sn==1) return 3;
	}
	if(flag==4 || flag==6 || flag==8)//��
	{
		if(i==1 && sn==1) return 4;
		if(i==2 && sn==1) return 4;
		if(i==3 && sn==-1) return 4;
	}
	return 0;
}
int evaluatePos(char ChessBoard[][MAX_COLUMN],int *EvaluateTable,int row,int column,StepNode tstep)
//������
{
	int score[4]={1,1,1,1};//�ĸ����÷�
	int move[4][2]={{1,0},{0,1},{1,1},{1,-1}};//�ƶ�����
	int i,j,sn;//ѭ������
	int count_dead1=0;//��¼�����ϱض����
	enum ChessType color_old;
	StepNode step[2];//������¼ȡ��ֱ���ϵ�����������յ�
	
	color_old=(enum ChessType)ChessBoard[tstep.x][tstep.y];
	ChessBoard[tstep.x][tstep.y]=tstep.color;
	
	for(i=0;i<4;i++)//���ߺ������ϵ�
	{
		int flag_edge;//�߽��־
		int count_empty_s=0;//�������������λ��
		int count_step[2]={0,0};//���㲽��
		int count_my=1;//�ҵ�������
		int count_my_maxs=0;//�ҵ��������������
		int count_my_ts=0;
		int j_my_maxs=0;
		step[0]=step[1]=tstep;

		//��ȡ����û�ж������ӵ��������������ո񲻳���4����ÿ��߳��Ȳ�����4����
		//�˹��̶������Ӻ͵��߽�ȼ�
		for(sn=-1;sn<2;sn+=2)
		{
			flag_edge=isEdge(tstep.x,tstep.y,row,column);
			if(_eva_pos_isBreak1(i,sn,flag_edge)!=0) continue;//�ж��Ƿ�˵�Ϊ�߽磬�Ƿ����ǰ��
			
			count_empty_s=0;//��������ĸ���
			for(j=1;;j++)
			{
				int tx=tstep.x+j*sn*move[i][0];
				int ty=tstep.y+j*sn*move[i][1];
				
				if(ChessBoard[tx][ty]!='0' && ChessBoard[tx][ty]!=tstep.color) break;//������������
				
				step[(sn+1)/2].x=tx;step[(sn+1)/2].y=ty;
				count_step[(sn+1)/2]++;
				flag_edge=isEdge(tx,ty,row,column);
				
				
				if(ChessBoard[tx][ty]==tstep.color) count_my++;
				if(ChessBoard[tx][ty]=='0') count_empty_s++;
				else count_empty_s=0;
				
				if(_eva_pos_isBreak1(i,sn,flag_edge)!=0) break;//�ߵ��߽�
				if(count_empty_s >= 3) break;//����3���ո�
				if(count_step[(sn+1)/2] >=  4) break;//ÿ��������4����(���ܱ�4�����ˣ��������#****0����)
			}
		}
		//����
		for(j=0;j<(count_step[0]+count_step[1]+1);j++)//ͳ������������ִ���
		{
			if( ChessBoard[step[0].x+j*move[i][0]][step[0].y+j*move[i][1]]==step[0].color)
				count_my_ts++;
			else 
			{
				if(count_my_maxs < count_my_ts)
				{
					count_my_maxs=count_my_ts;
					j_my_maxs=j;
				}
				count_my_ts=0;
			}
		}
		if(count_my_maxs < count_my_ts)
		{
			count_my_maxs=count_my_ts;
			j_my_maxs=j;
		}
		
		if(count_my==1)//ֻ��1������
		{
			if(count_step[0]+count_step[1]>=4 &&count_step[0]>0 && count_step[1]>0) 
				score[i]=EvaluateTable[14];//0*0000
			else score[i]=EvaluateTable[15];
		}
		else if(count_my==2)//��2���Լ�����
		{
			if(count_step[0]+count_step[1]>=4 
				&& ChessBoard[step[0].x][step[0].y]=='0' 
				&& ChessBoard[step[1].x][step[1].y]=='0'  )
			{
				if(ChessBoard[tstep.x+move[i][0]][tstep.y+move[i][1]] == tstep.color ||
					ChessBoard[tstep.x-move[i][0]][tstep.y-move[i][1]] == tstep.color)
					score[i]=EvaluateTable[10];//00**0
				else
					score[i]=EvaluateTable[11];//0*0*0
			}
			else
			{
				if(count_step[0]+count_step[1]>=4 &&
					(ChessBoard[step[0].x][step[0].y]!='0' || ChessBoard[step[1].x][step[1].y]!='0' ))
				{
					if(ChessBoard[tstep.x+move[i][0]][tstep.y+move[i][1]] == tstep.color ||
						ChessBoard[tstep.x-move[i][0]][tstep.y-move[i][1]] == tstep.color)
						score[i]=EvaluateTable[12];//#00**0
					else
						score[i]=EvaluateTable[13];//#*0*0
				}
				else
				{
					score[i]=EvaluateTable[16]*(count_step[0]+count_step[1]);
				}
			}
		}
		else if(count_my==3)//��3���Լ�����
		{
			if(count_step[0]+count_step[1]>=4 
				&& ChessBoard[step[0].x][step[0].y]=='0' 
				&& ChessBoard[step[1].x][step[1].y]=='0'  )
			//���߶�û����
			{
				if(count_my_maxs==3) 
				{
					score[i]=EvaluateTable[6];//0***0
					count_dead1++;
				}
				else if(count_my_maxs==2) 
				{
					score[i]=EvaluateTable[7];//0*0**0
					count_dead1++;
				}
				else score[i]=EvaluateTable[17];//0*0*0*0
			}
			else if( count_step[0]+count_step[1]>=4 ||
					(ChessBoard[step[0].x][step[0].y]=='0' && ChessBoard[step[1].x][step[1].y]=='0'))
			//һ�߱���
			{
				if(count_my_maxs==3) score[i]=EvaluateTable[8];//#***00
				if(count_my_maxs==2) score[i]=EvaluateTable[9];//#**0*0
				if(count_my_maxs==1) score[i]=EvaluateTable[18];//#*0*0*0
			}
			else
			{
				score[i]=EvaluateTable[19]+10*(count_step[0]+count_step[1])+20*count_my_maxs;
			}
		}
		else if(count_my==4)//��4���Լ�����
		{
			if(count_my_maxs==2 && count_step[0]+count_step[1]>=5) 
				score[i]=EvaluateTable[3];//?**00**?
			else if(count_step[0]+count_step[1]>=4 
				&& ChessBoard[step[0].x][step[0].y]=='0' 
				&& ChessBoard[step[1].x][step[1].y]=='0'  )
			//���߶�û����
			{
				if(count_my_maxs==4) score[i]=EvaluateTable[1];//0****0
				else if(count_my_maxs==3) score[i]=EvaluateTable[2];//0*0***0
				else if(count_my_maxs==2) score[i]=EvaluateTable[21];//0**0**0
				else score[i]=EvaluateTable[17];//0*0*0*0
			}
			else if(count_step[0]+count_step[1]>=4 
				&& (ChessBoard[step[0].x][step[0].y]=='0' || ChessBoard[step[1].x][step[1].y]=='0'))
			//һ�߱���
			{
				if(count_my_maxs==4) 
				{
					score[i]=EvaluateTable[4];//#****0
					count_dead1++;
				}
				else if(count_my_maxs==3) 
				{
					score[i]=EvaluateTable[5];//#***0*
					count_dead1++;
				}
				else if(count_my_maxs==2) 
				{
					score[i]=EvaluateTable[22];//#**0**0
					count_dead1++;
				}
				else score[i]=EvaluateTable[17]-400;
			}
			else score[i]=130;
		}
		else if(count_my>=5)//��5���Լ�����
		{
			if(count_my_maxs>=5) score[i]=EvaluateTable[0];
			else score[i]=EvaluateTable[0]/2;
		}

	}
	ChessBoard[tstep.x][tstep.y]=color_old;
	if(count_dead1>=2) return EvaluateTable[23];
	return score[0]+score[1]+score[2]+score[3];
}
int evaluateBoard(char ChessBoard[][MAX_COLUMN],int *EvaluateTable,Edge edge,int row,int column,enum ChessType color_max)
//�Ծ�������
{
	//srand(time(0));
	//return rand()%10000;
	int i,j;
	int tscore;
	int score_max=0,score_min=0;
	enum ChessType color_min;
	StepNode tstep;

	if(color_max==black) color_min=white;
	else color_min=black;
	for(i=edge.x1;i<=edge.x2;i++)
	{
		for(j=edge.y1;j<=edge.y2;j++)
		{
			tstep.x=i;tstep.y=j;
			if(ChessBoard[i][j]==color_max)
			{
				tstep.color=color_max;
				tscore=evaluatePos(ChessBoard,EvaluateTable,row,column,tstep);
				if(tscore >= EvaluateTable[0]) return EvaluateTable[0];//������INF����������ϲ㽫û������
				if(tscore > score_max) score_max=tscore;
			}
			if(ChessBoard[i][j]==color_min)
			{
				tstep.color=color_min;
				tscore=evaluatePos(ChessBoard,EvaluateTable,row,column,tstep);
				if(tscore >= EvaluateTable[0]) return -EvaluateTable[0];
				if(tscore > score_min) score_min=tscore;
			}
		}
	}
	//return score_max-score_min;
	if(score_max - score_min >=1*score_min) return score_max;
	else if(score_min - score_max <= 1*score_max ) return -score_min;
	else return score_max-score_min;
	//if(score_max > score_min) return score_max;
	//else return -score_min;
}
ScoreStackNode findMaxScore(char ChessBoard[][MAX_COLUMN],int *EvaluateTable,int row,int column,Edge edge,PF_EVA_POS eval_p,enum ChessType color)
//��ʱû��
{
	int i,j;
	int tscore,score_max;
	StepNode tstep,point_max;
	ScoreStackNode snode;
	for(i=edge.x1;i<=edge.x2;i++)//ÿ�����ղ�
	{
		for(j=edge.y1;j<=edge.y2;j++)
		{
			if(ChessBoard[i][j]!='0') continue;
			tstep.x=i;tstep.y=j;tstep.color=color;
			tscore=eval_p(ChessBoard,EvaluateTable,row,column,tstep);
			if(tscore>score_max)
			{
				score_max=tscore;
				point_max=tstep;
			}
		}
	}
	snode.edge=edge;
	snode.pos.x=tstep.x;snode.pos.y=tstep.y;
	snode.score=score_max;
	return snode;
}
StepNode AI_kuon_pos(char ChessBoard[][MAX_COLUMN],
								int *EvaluateTable,
								int row,
								int column,
								enum ChessType color_max)
//���㷨�������ϵ�λ�ü��ÿ���㣬ȡ���ҷ�max��з�min����
//max-min>=��ֵ ������maxλ�ý���
//max-min<��ֵ  ������minλ�÷���
{
	StepNode point_max={0,0};
	StepNode point_min={0,0};
	StepNode tstep; 
	PF_EVA_POS eval_p=&evaluatePos;//�ô˹�ֵ����
	enum ChessType color_min;//������ɫ
	int score_max=-1;
	int score_min=1;//����
	int i,j;
	int tscore;


	if(color_max==white) color_min=black;
	else color_min=white;

	for(i=0;i<row;i++)//ÿ�����ղ�
	{
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]!='0') continue;
			tstep.x=i;tstep.y=j;tstep.color=color_max;
			tscore=eval_p(ChessBoard,EvaluateTable,row,column,tstep);
			if(tscore>score_max)
			{
				score_max=tscore;
				point_max=tstep;
			}
			
			tstep.color=color_min;
			tscore=-eval_p(ChessBoard,EvaluateTable,row,column,tstep);
			if(tscore<score_min)
			{
				score_min=tscore;
				point_min=tstep;
			}
		}
	}
	if(score_max+score_min>-3*score_min/10) return point_max;
	else return point_min;
}
StepNode AI_kuon_gametree(char ChessBoard[][MAX_COLUMN],
							int *EvaluateTable,
							ChessAiExtend *ai_info,
							enum ChessType color_max)
//����������+alpha_beta��֦
//��0��Ϊmax�£���1��Ϊmax�����min���ǵľ��棨��˭����˭�Ľڵ㣬��max�ڵ���min�����ľ��ƣ�
//		alpha��֦:min�������ϲ�max�ڵ�Ƚϣ�min����С���ϲ�max
//      beta��֦:max�����е��ϲ�min�ڵ�Ƚϣ�max���ܴ����ϴ�min
//���нڵ��¼�����������MAX��
//�������ۺ����Ƕ�������ƣ������ǵ���һ���㣬��Ҷ�ӽڵ㵹��
//��ʱ�Ȳ�������������

//ò�Ʋ�����ûʲô�������ˣ����ǹ�ֵ����̫�������������2����ʱ�ж�ֵ���ԣ��Ժ���˵��ֵ�����ɡ�������
{	
	StepNode tstep;
	ScoreStackNode *pnode=NULL,bnode,tnode;//pnode���ڵ㣨ջ������bnode�ӽڵ㣨�յ�ջ�Ľڵ㣩,tnode��ʱ�ڵ�
	int depth=0;//��ǰ�����
	int score_max=INF,score_min=-INF,tscore;
	int i,j,k;
	int tx,ty;
	enum ChessType color_min,tcolor;
	Edge tedge;

	int flag_next=1;//�Ƿ���1������һ��
	int flag_back=0;//�Ƿ񷵻���һ��ı�־
	int flag_break=0;//����ѭ���ı�־
	int count_step=0;//��������
	
	//��Щֻ��Ϊ����ǿ�ɶ���
	int max_depth=ai_info->in.max_depth;//��������
	int row=ai_info->in.row;
	int column=ai_info->in.column;
	int edge_dis=ai_info->in.edge_dis;
	PF_EVA_BOARD eval_b=ai_info->in.evel_b;
	ScoreStack *ps_stack=ai_info->in.ps_stack;//���ֶ�ջ,ջ��ѹ��·����max��min��2����1�����
	tedge=ai_info->in.edge;
	tstep.color=color_max;
	
	iniScoreStack(ps_stack,MAX_STACK);
	if(color_max==white) color_min=black;
	else color_min=white;
	
	//��0��
	tnode.edge=tedge;
	tnode.score=-INF;
	push_ScoreStack(ps_stack,tnode);
	
	while(1)
	//DFSÿ����ֻ��1�δ��ϵ���·��
	{
		count_step++;
		pnode=getTop_ScoreStack(ps_stack);
		flag_next=0;
		
		//if(count_step>=1)
		//{
		//	PrintChessBoard(ChessBoard,15,15);
		//	printf("count= %d\n",count_step);
		//	system("sleep 10");
		//	//system("pause");
		//}

		//��û�е��ײ㣬Ҫ�ҵ���һ��������(tx,ty)
		if(flag_back==1)//Ϊ�˲��ظ�����
		{
			for(j=bnode.pos.y+1;j<=pnode->edge.y2;j++)
			{
				if(ChessBoard[bnode.pos.x][j]=='0')
				{
					flag_next=1;
					tx=bnode.pos.x;ty=j;
					break;
				}
			}
			if(flag_next==0)
			{
				for(i=bnode.pos.x+1;i<=pnode->edge.x2;i++)
				{
					for(j=pnode->edge.y1;j<=pnode->edge.y2;j++)
					{
						if(ChessBoard[i][j]=='0')
						{
							flag_next=1;
							tx=i;ty=j;
							goto _break_s1;
						}
					}
				}
			}
		}
		else
		{
			for(i=pnode->edge.x1;i<=pnode->edge.x2;i++)
			{
				for(j=pnode->edge.y1;j<=pnode->edge.y2;j++)
				{
					if(ChessBoard[i][j]=='0')
					{
						flag_next=1;
						tx=i;ty=j;
						goto _break_s1;
					}
				}
			}
		}
_break_s1:
		if(depth<1 && flag_next==0) //ֻʣ0��һ�������ʱ��
			break;
		
		//����һ����Ҷ�ӽڵ�ʱ������Ҷ�ڵ����֣��ҽ���Ҷ�ڵ���Ҫ�������ֺ���
		if(depth == max_depth)
		{
			bnode=pop_ScoreStack(ps_stack);
			pnode=getTop_ScoreStack(ps_stack);		
			tscore=eval_b(ChessBoard,EvaluateTable,bnode.edge,row,column,color_max);
			if(tscore < pnode->score)
			{
				pnode->score=tscore;
				pnode->pos_child=bnode.pos;
			}		
			ChessBoard[bnode.pos.x][bnode.pos.y]='0';
			depth--;
			flag_back=1;
			
			for(k=0;k<depth;k+=2)//beta��֦
			{
				if(pnode->score <= ps_stack->stack[k].score)
				{
					flag_break=1;
					break;
				}
			}
			if(flag_break>0) //��֦���ʱ����ڵ㲻������
			{
				bnode=pop_ScoreStack(ps_stack);
				depth--;
				flag_back=1;
				ChessBoard[bnode.pos.x][bnode.pos.y]='0';
			}
			continue;
		}

		//�鿴�Ƿ���Ҫ��֦
		flag_break=0;
		if(depth%2==1)//MIN
		{
			for(k=0;k<depth;k+=2)//beta��֦
			{
				if(pnode->score <= ps_stack->stack[k].score)
				{
					flag_break=1;
					break;
				}
			}
		}
		else//MAX
		{
			for(k=1;k<depth;k+=2)//alpha��֦
			{
				if(pnode->score >= ps_stack->stack[k].score)
				{
					flag_break=1;
					break;
				}
			}
		}
		if(flag_break>0 && depth>1) //��֦���ʱ����ڵ㲻������
		{
			bnode=pop_ScoreStack(ps_stack);
			depth--;
			flag_back=1;
			ChessBoard[bnode.pos.x][bnode.pos.y]='0';
			continue;
		}
		
		//if(depth>=max_depth-1 && flag_back==1) 
		//	continue; 
		
		if(flag_next > 0)//�е�������ӣ���ʼ����
		{
			flag_back=0;
			depth++;
			
			tnode.pos.x=tx;tnode.pos.y=ty;
			//tnode.pos_child.x=-1;tnode.pos_child.y=-1;
			tnode.edge=pnode->edge;
			checkEdge(&tnode.edge,tnode.pos,row,column,edge_dis);
		
			if(depth%2==1) 
			{
				tnode.score=INF;//MIN���ߵľ���(MAX�����)
				ChessBoard[tnode.pos.x][tnode.pos.y]=color_max;
			}
			else 
			{
				tnode.score=-INF;//MAX���ߵľ���(MIN�����)
				ChessBoard[tnode.pos.x][tnode.pos.y]=color_min;
			}
			push_ScoreStack(ps_stack,tnode);
			
			//����Ƿ�����Ӯ��
			pnode=getTop_ScoreStack(ps_stack);
			tstep.x=pnode->pos.x;tstep.y=pnode->pos.y;
			tscore=evaluatePos(ChessBoard,EvaluateTable,row,column,tstep);
			if(tscore >= EvaluateTable[0])
			{
				if(ChessBoard[pnode->pos.x][pnode->pos.y]==color_max)
				{
					for(i=1;i<=depth;i++)
						ChessBoard[ps_stack->stack[i].pos.x][ps_stack->stack[i].pos.y]=empty;
					tstep.x=ps_stack->stack[1].pos.x;tstep.y=ps_stack->stack[1].pos.y;
					return tstep;

				}
				else
				{
					depth--;
					flag_back=1;
					bnode=pop_ScoreStack(ps_stack);
					ChessBoard[bnode.pos.x][bnode.pos.y]=empty;
					continue;
				}
			}
		}
		else//��֧�Ѿ��������ˣ����ϻ���(���з�֧���������ˣ������������֦)
		{
			flag_back=1;
			depth--;
			bnode=pop_ScoreStack(ps_stack);
			pnode=getTop_ScoreStack(ps_stack);
			
			if(depth%2==1)//MIN
			{
				if(bnode.score < pnode->score)
				{
					pnode->score=bnode.score;
					pnode->pos_child=bnode.pos;
				}	
			}
			else//MAX
			{
				if(bnode.score > pnode->score)
				{
					pnode->score=bnode.score;
					pnode->pos_child=bnode.pos;
				}	
			}
			ChessBoard[bnode.pos.x][bnode.pos.y]='0';//�ָ�����
		}
	}

	tstep.x=ps_stack->stack[0].pos_child.x;tstep.y=ps_stack->stack[0].pos_child.y;
	ai_info->out.count_step=count_step;
	return tstep;
}