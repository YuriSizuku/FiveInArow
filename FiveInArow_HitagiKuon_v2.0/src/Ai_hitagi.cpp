#include "Ai_hitagi.h"
#pragma comment(lib,"fanndouble.lib")//静态声明库,附加目录

/*棋盘与ann转换函数*/
void hitagi_makeAnnInput(char cb[][MAX_COLUMN],int row,int column,
					struct hitagi_ann_data *ann_data)//得到ann输入
{
	int result[HITAGI_NUM_EVALUATE];
	int i;
	char color_win=judgeWin(cb,row,column);
	if(color_win==black)//为减小误差，5连珠则直接返回
	{
		memset(ann_data->data_input,0,sizeof(ann_data->data_input));
		ann_data->data_input[0]=1;
		return;
	}
	else if(color_win==white)
	{
		memset(ann_data->data_input,0,sizeof(ann_data->data_input));
		ann_data->data_input[HITAGI_NUM_EVALUATE]=1;
		return;
	}
	//黑棋局势
	hitagi_getEvalTypes(cb,row,column,black,result);
	for(i=0;i<HITAGI_NUM_EVALUATE;i++) 
		ann_data->data_input[i]=result[i];
	//白棋局势
	hitagi_getEvalTypes(cb,row,column,white,result);
	for(i=0;i<HITAGI_NUM_EVALUATE;i++) 
		ann_data->data_input[i+HITAGI_NUM_EVALUATE]=result[i];
	//屏蔽简化
	//for(i>8;i<HITAGI_NUM_EVALUATE;i++)
	//{
	//	ann_data->data_input[i]=0;
	//	ann_data->data_input[i+HITAGI_NUM_EVALUATE]=0;
	//}
}

/*fann与转换函数*/
void hitagi_ann2fann(struct hitagi_ann_data *ann_data)//此函数不用（未完）
{
	if(ann_data->extern_ann==NULL) return;
	int i,j,count=0;
	//权值
	int num_weight=sizeof(struct hitagi_ann_weight)/sizeof(double);
	fann_connection fann_weight[HITAGI_NUM_WEIGHTS];

	/*fann神经元线性排布, 注意每层都有1个偏置神经元(bias neuron)没有用（最后一个？）
	  fann_weight from外层循环，to内层，from中的偏置神经元不参与connect，但其他神经元会连接to中的偏置神经元
	*/

	//输入层X隐含层
	for(i=0;i<HITAGI_NUM_INPUT;i++)
	{
		for(j=0;j<HITAGI_NUM_HIDEN;j++)
		{
			fann_weight[count].from_neuron=i;
			fann_weight[count].to_neuron=j+HITAGI_NUM_INPUT;
			fann_weight[count].weight=ann_data->weight->weight_input[i][j];
			count++;
		}
	}
	//隐含层X隐含层
	//隐含层X输出层
	fann_set_weight_array((struct fann*)ann_data->extern_ann,
							fann_weight,
							HITAGI_NUM_WEIGHTS);
}
void hitagi_iniFannTrain(struct hitagi_ann_data *ann_data)//初始神经网络训练函数
{
	
	int num=3;
	int i,j;
	//训练最终5连权值

	struct fann_train_data *data=new struct fann_train_data;
	data->num_input=HITAGI_NUM_INPUT;
	data->num_output=HITAGI_NUM_OUTPUT;
	data->num_data=num;

	data->input=new double* [num];
	for(i=0;i<num;i++)
	{
		data->input[i]=new double[HITAGI_NUM_INPUT];
		for(j=0;j<HITAGI_NUM_INPUT;j++) 
			data->input[i][j]=0;
	}
	data->output=new double* [num];
	for(i=0;i<num;i++)
	{
		data->output[i]=new double[HITAGI_NUM_OUTPUT];
		for(j=0;j<HITAGI_NUM_OUTPUT;j++) 
			data->output[i][j]=0;
	}
	data->input[2][0]=1;data->input[1][HITAGI_NUM_EVALUATE]=1;
	data->output[2][0]=1;data->output[1][1]=1;


	fann_train_on_data((struct fann *)ann_data->extern_ann,
					data,
					HITAGI_MAX_EPOCHS,
					10,//每隔多少个值print
					HITAGI_DESIRED_ERROR);
	//hitagi_annTest(ann_data);
	//ann_data->data_input[0]=1;
	//ann_data->data_output[0]=1;
	//hitagi_annTest(ann_data);
	
	//释放空间
	for(i=0;i<num;i++)
	{
		delete[] data->input[i];
		delete[] data->output[i];
	}
	delete[] data->input;
	delete[] data->output;
	delete data;
}
void hitagi_iniFann(struct hitagi_ann_data *ann_data)//调用fnn库的初始化
{
	int i;
	//创建fann
	unsigned int num_layers[HITAGI_NUM_LAYER];
	num_layers[0]=HITAGI_NUM_INPUT;
	for(i=1;i<HITAGI_NUM_LAYER-1;i++)
		num_layers[i]=HITAGI_NUM_HIDEN;
	num_layers[i]=HITAGI_NUM_OUTPUT;
	ann_data->extern_ann=(struct fann *)fann_create_standard_array(
						HITAGI_NUM_LAYER,
						num_layers);
	//调整fann学习率
	fann_set_learning_rate((struct fann *)ann_data->extern_ann,HITAGI_RATE_ANN_LEARNING);
	//初始化权值
	fann_randomize_weights((struct fann *)ann_data->extern_ann,HITAGI_WEIGHT_MIN,HITAGI_WEIGHT_MAX);
	//激活函数(用默认0~1)
	fann_set_activation_function_output((struct fann *)ann_data->extern_ann,FANN_SIGMOID);
	fann_set_activation_function_hidden((struct fann *)ann_data->extern_ann,FANN_SIGMOID);
	//初始训练
	//hitagi_iniFannTrain(ann_data);
}

void hitagi_fann2annOutput(struct hitagi_ann_data *ann_data)//将fann中的output转换为hitagi_ann中的output
{
	int i,num;
	struct fann* fann_data=(struct fann*)ann_data->extern_ann;
	num=fann_get_num_output(fann_data);
	for(i=0;i<num;i++)
		ann_data->data_output[i]=fann_data->output[i];
}

/*Ai_hitagi 初始化与输入输出函数*/
void hitagi_iniAnn(struct hitagi_ann_data *ann_data,
				   struct hitagi_ann_weight *ann_weight)//初始化ann值
{
	memset(ann_data->data_input,0,sizeof(ann_data->data_input));
	memset(ann_data->data_output,0,sizeof(ann_data->data_output));
	ann_data->weight=ann_weight;
	//weight暂时不用
}
struct hitagi_ann_data* hitagi_create(struct hitagi_ann_weight *ann_weight)
{
	struct hitagi_ann_data *ann_data=new hitagi_ann_data;
	hitagi_iniAnn(ann_data,ann_weight);
	hitagi_iniFann(ann_data);
	return ann_data;
}
struct hitagi_ann_data* hitagi_load(char *path)
{
	fann *fann_data=fann_create_from_file(path);
	fann_set_learning_rate(fann_data,HITAGI_RATE_ANN_LEARNING);
	struct hitagi_ann_data *ann_data=new hitagi_ann_data;
	hitagi_iniAnn(ann_data,NULL);
	ann_data->extern_ann=fann_data;
	return ann_data;
}
void   hitagi_save(struct hitagi_ann_data *ann_data,char *path)
{
	fann_save((struct fann *)ann_data->extern_ann,path);
}
void   hitagi_destory(struct hitagi_ann_data *ann_data)
{
	fann_destroy((struct fann *)ann_data->extern_ann);
	if(ann_data->weight!=NULL) delete ann_data->weight;
	delete ann_data;
}

/*Ai_hitagi 棋盘类型,ann估值与学习函数*/
void _evelTypes_add(int *result,int *tresult)
{
	int i;
	for(i=0;i<HITAGI_NUM_EVALUATE;i++)
		result[i]+=tresult[i];
}
void _evelTypes_ini(int *result)
{
	memset(result,0,HITAGI_NUM_EVALUATE*sizeof(int));
}
void hitagi_getEvalTypes_line(char *vector_chess,int start,int end,ChessType color_max,int *result)
//从一个向量[start,end)中获得某种局势类型
//边界很麻烦，自动在第一个和最后一个加入敌人棋子，（考虑到分段区间，还是再开辟一块空间吧）
//边界与敌人棋子等同
{
	int i,j,k;
	int tstart=5;
	int tend=tstart+end-start;
	int x1=tstart,x2=tstart;
	char tvector[MAX(MAX_ROW,MAX_COLUMN)+10];//为了解决边界问题，操
	int count_chess=0;//某区间我方棋子总个数
	int count_schess=0;//当前的我方连续棋子个数
	int count_schess_max=0;//某区间我方连续棋子最大个数
	int start_schess_max=0;//某区间我方连续棋子起始位置

	ChessType color_min=color_max==black ? white :black;
	for(i=0;i<tstart;i++) tvector[i]=color_min;
	for(i=start;i<end;i++) tvector[i-start+5]=vector_chess[i];
	for(i=end+tstart;i<MAX(MAX_ROW,MAX_COLUMN)+10;i++) tvector[i]=color_min;
	_evelTypes_ini(result);
	
	for(i=tstart;i<=tend;i++)
	{
		if(tvector[i]==color_min ) //出现对方棋子或到结束，最后不是对方棋子
		{
			if(count_schess>count_schess_max)//统计我方连续棋子个数
			{
				count_schess_max=count_schess;
				start_schess_max=i-count_schess;
			}
			x2=i;
			/*
				奥卡姆剃刀原则，尽量简化问题，不要钻牛角尖
				这里假设不存在6个子以上还没被堵
				连续最长优先去匹配，贪婪
			*/
			for(j=x1;j<x2;j++)
			{
				char left[5],right[5];// 1 0 * 0 1
				for(k=0;k<5;k++)
				{
					left[k]=tvector[start_schess_max-k-1];
					right[k]=tvector[start_schess_max+count_schess_max+k];
				}
				
				if(x2-x1<5) break;//位置不足5不考虑
				if(count_chess<1) break;//此区间无我方棋子时
				if(count_schess_max>=5)//*****
				{
					result[0]++;
					break;
				}
				if(count_schess_max==4)
				{
					if(tvector[start_schess_max-1] == empty 
					&& tvector[start_schess_max+count_schess_max] == empty)//0****0
					{
						result[1]++;
					}
					else if(tvector[start_schess_max-1] == empty 
						|| tvector[start_schess_max+count_schess_max] == empty)//#****0
					{
						result[2]++;
					}
					break;
				}
				if(count_schess_max==3)
				{	
					if((left[1]==empty && left[0]==empty && right[0]==empty)
						||(left[0]==empty && right[0]==empty && right[1]==empty) )//0***00
					{
						result[7]++;
						break;
					}
					if(left[1]==color_min && left[0]==empty && right[0]==empty && right[1]==color_min)//#0***0#
					{
						result[16]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==empty)
						||(right[0]==color_min && left[0]==empty && left[1]==empty))//#***00
					{
						result[9]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==color_max)
						|| (right[0]==color_min && left[0]==empty && left[1]==color_max))//#***0*
					{
						result[3]++;
						break;
					}
					if((left[0]==empty && right[0]==empty && right[1]==color_max)
						|| (right[0]==empty && left[0]==empty && left[1]==color_max))//0***0*
					{
						result[4]++;
						break;
					}
				}
				if(count_schess_max==2)
				{
					if((left[0]==empty && left[1]==color_max && left[2]==color_max) 
						|| right[0]==empty && right[1]==color_max && right[2]==color_max)//**0**
					{
						result[5]++;
						break;
					}
					if((left[0]==empty && left[1]==color_max && left[2]==empty && right[0]==empty)
						||(right[0]==empty && right[1]==color_max && right[2]==empty && left[0]==empty))//0*0**0
					{
						result[8]++;
						break;
					}
					if((left[0]==empty && left[1]==color_max && right[0]==empty && right[1]==color_max))//*0**0*
					{
						result[17]++;
						break;
					}
					if((left[0]==empty && left[1]==color_max && left[2]==empty && left[3]==color_max && left[4]==color_max)
						||(right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max && right[4]==color_max))//**0*0**
					{
						result[18]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==color_max && right[2]==empty)
						||(right[0]==color_min && left[0]==empty && left[1]==color_max && left[2]==empty))//#**0*0
					{
						result[10]++;
						break;
					}
					if((left[0]==empty && left[1]==color_max && left[2]==color_min && right[0]==empty)
						||(right[0]==empty && right[1]==color_max && right[2]==color_min && left[0]==empty))//#*0**0
					{
						result[11]++;
						break;
					}
					if((left[0]==empty && right[0]==empty && right[1]==empty && right[2]==color_max)
						||(right[0]==empty && left[0]==empty && left[1]==empty && left[2]==color_max))//0**00*
					{
						result[14]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==empty && right[2]==color_max)
						||(right[0]==color_min && left[0]==empty && left[1]==empty && left[2]==color_max))//#**00*
					{
						result[15]++;
						break;
					}
					if((left[0]==empty && left[1]==empty && right[0]==empty && right[1]==empty))//00**00
					{
						result[19]++;
						break;
					}
					if((left[0]==empty && left[1]==color_min && right[0]==empty && right[1]==empty && right[2]==color_min)
						||(right[0]==empty && right[1]==color_min && left[0]==empty && left[1]==empty && left[2]==color_min))//#0**00#
					{
						result[26]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==empty && right[2]==empty)
						||(right[0]==color_min && left[0]==empty && left[1]==empty && left[2]==empty))//#**000
					{
						result[22]++;
						break;
					}
				}
				if(count_schess_max==1)//注意有多个相同的1时，定位的是最左边的
				{
					if((left[0]==empty && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max && right[4]==empty)
						||(right[0]==empty && left[0]==empty && left[1]==color_max && left[2]==empty && left[3]==color_max && left[4]==empty))
						//0*0*0*0,只考虑边上的2个1，不考虑中间
					{
						result[12]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max)
						||(right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max && right[4]==color_min))
						//#*0*0*,只考虑边上的2个1，不考虑中间
					{
						result[13]++;
						break;
					}
					if((left[0]==empty && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==empty)
						||(left[0]==empty && left[1]==empty && right[0]==empty && right[1]==color_max && right[2]==empty))//0*0*00
					{
						result[20]++;
						break;
					}
					if(left[0]==empty && right[0]==empty && right[1]==empty && right[2]==color_max && right[3]==empty)//0*00*0
					{
						result[21]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==empty) 
						||(left[0]==empty && left[1]==empty && right[0]==empty && right[1]==color_max && right[2]==color_min))//#*0*00
					{
						result[23]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==empty && right[2]==color_max && right[3]==empty)
						||(left[0]==empty && right[0]==empty && right[1]==empty && right[2]==color_max && right[3]==color_min))//#*00*0
					{
						result[24]++;
						break;
					}
					if(right[0]==empty && right[1]==empty && right[2]==empty && right[3]==color_max)//*000*
					{
						result[25]++;
						break;
					}
					if(left[0]==empty && left[1]==color_min && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_min)
						//#0*0*0#
					{
						result[27]++;
						break;
					}
					if(left[0]==empty && left[1]==empty && right[0]==empty && right[1]==empty)//00*00
					{
						//result[28]++;
						break;
					}
				}
					
			}
			//区间♂事后
			x1=i+1;
			count_chess=0;
			count_schess=0;
			count_schess_max=0;
		}
		else if(tvector[i]==color_max)
		{
			count_chess++;
			count_schess++;
		}
		else//统计我方连续棋子个数
		{
			if(count_schess>count_schess_max)
			{
				count_schess_max=count_schess;
				start_schess_max=i-count_schess;
			}
			count_schess=0;
		}
	}
}
void hitagi_getEvalTypes(char ChessBoard[][MAX_COLUMN],int row,int column,ChessType color,int *result)
//从棋盘中获取局势类型
{
	int tresult[HITAGI_NUM_EVALUATE];
	char vector_chess[MAX(MAX_ROW,MAX_COLUMN)+2];
	int start,end;
	int i,j;
	
	_evelTypes_ini(result);

	for(i=0;i<row;i++)//检验行
	{
		start=0;end=column;
		for(j=0;j<column;j++)
		{
			vector_chess[j]=ChessBoard[i][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(j=0;j<column;j++)//检验列
	{
		start=0;end=row;
		for(i=0;i<row;i++)
		{
			vector_chess[i]=ChessBoard[i][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(i=0;i<row-4;i++)//检验下半主对角线方向
	{
		start=0;end=row-i;
		for(j=0;j<row-i;j++)
		{
			vector_chess[j]=ChessBoard[i+j][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(j=1;j<column-4;j++)//检查上半主对角线
	{
		start=0;end=column-j;
		for(i=0;i<column-j;i++)
		{
			vector_chess[i]=ChessBoard[i][j+i];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(i=4;i<row;i++)//检验上半副对角线
	{
		start=0;end=i+1;
		for(j=0;j<=i;j++)
		{
			vector_chess[j]=ChessBoard[i-j][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(j=1;j<column-4;j++)//检查下半副对角线
	{
		start=0;end=column-j;
		for(i=j;i<row;i++)
		{
			vector_chess[i-j]=ChessBoard[i][(row-1)-(i-j)];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
}
void hitagi_annTest(struct hitagi_ann_data *ann_data)//用ann进行测试输出
{
	double *output;
	int i;
	//fann_reset_MSE((struct fann *)ann_data->extern_ann);
	output=fann_test((struct fann *)ann_data->extern_ann,
		      ann_data->data_input,
			  ann_data->data_output);
	for(i=0;i<HITAGI_NUM_OUTPUT;i++)
		ann_data->data_output[i]=output[i];
	//if(output!=NULL) delete output;//不能delete，返回的是fann中的指针
}
double hitagi_evaluateBoard(char cb[][MAX_COLUMN],int row,int column,
							struct hitagi_ann_data *ann_data,
							ChessType color)//神经网络估值，放到output中,返回估计值
{
	/*debug*/
	int show=0;
	/*END*/
	double score;
	hitagi_makeAnnInput(cb,row,column,ann_data);
	hitagi_annTest(ann_data);
	fann_reset_MSE((struct fann *)ann_data->extern_ann);
	if(color==black)
	{     
		if(ann_data->data_input[0]>0)//防止权重过学习
			return 1.0;
		if(ann_data->data_input[HITAGI_NUM_EVALUATE]>0)
			return -1.0;
		return 1.04*ann_data->data_output[0]-ann_data->data_output[1];
		//return ann_data->data_output[0]>ann_data->data_output[1] ?
		//					ann_data->data_output[0] : -ann_data->data_output[1];
	}	
	else
	{
		if(ann_data->data_input[HITAGI_NUM_EVALUATE]>0)
			return 1.0;
		if(ann_data->data_input[0]>0)//防止权重过学习
			return -1.0;
		return 1.04*ann_data->data_output[1]-ann_data->data_output[0];
		//return ann_data->data_output[0]>ann_data->data_output[1] ?
		//					-ann_data->data_output[0] : ann_data->data_output[1];
	}
}

/*Ai_hitagi 参数调整*/



/* AI_hitagi内部函数*/
int hitagiXkuon_eval(char cb[][MAX_COLUMN],
						int *EvaluateTable,Edge edge,
						int row,int column,
						enum ChessType color,void *extra)//hitagi调用kuon的估值函数
{
	double eval_score=3;
	struct hitagi_ann_data *ann_data=(struct hitagi_ann_data *)extra;
	eval_score=hitagi_evaluateBoard(cb,row,column,ann_data,color);
	return eval_score*100000;
}
StepNode Ai_hitagi_pos(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//点估值
{
	/*debug*/
	int show=0;
	/*END*/
	StepNode tstep;
	double tscore,score_max=-INF;
	int i,j;
	for(i=0;i<row;i++)//赋初值，防止闪退
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]==empty)
			{
				tstep.x=i;tstep.y=j;
				goto EVEL;
			}
		}
	EVEL:

	//引入概率算子
	for(i=edge.x1;i<=edge.x2;i++)
	{
		for(j=edge.y1;j<=edge.y2;j++)
		{
			if(ChessBoard[i][j]!=empty) continue;//不是break，严重bug！！！
			ChessBoard[i][j]=color_max;
			tscore=hitagi_evaluateBoard(ChessBoard,row,column,ann_data,color_max);
			if(tscore>score_max)
			{
				score_max=tscore;
				tstep.x=i;tstep.y=j;
			}
			ChessBoard[i][j]=empty;//恢复现场
		}
	}
	return tstep;
}
StepNode Ai_hitagi_pos2(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//此估值不好用
{
	/*debug*/
	int show=0;
	/*END*/
	StepNode tstep;
	double tscore,tscore_white,tscore_black,score_max=-INF;
	int i,j;
	for(i=0;i<row;i++)//赋初值，防止闪退
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]==empty)
			{
				tstep.x=i;tstep.y=j;
				goto EVEL;
			}
		}
	EVEL:

	//引入概率算子
	for(i=edge.x1;i<=edge.x2;i++)
	{
		for(j=edge.y1;j<=edge.y2;j++)
		{
			if(ChessBoard[i][j]!=empty) continue;//不是break，严重bug！！！
			
			//双向估值
			ChessBoard[i][j]=black;
			hitagi_makeAnnInput(ChessBoard,row,column,ann_data);
			hitagi_annTest(ann_data);
			fann_reset_MSE((struct fann *)ann_data->extern_ann);
			tscore_black=ann_data->data_output[0];
			
			ChessBoard[i][j]=white;
			hitagi_makeAnnInput(ChessBoard,row,column,ann_data);
			hitagi_annTest(ann_data);
			fann_reset_MSE((struct fann *)ann_data->extern_ann);
			tscore_white=ann_data->data_output[1];
			
			tscore=color_max==black ? tscore_black-tscore_white :tscore_white-tscore_black;
			if(tscore>score_max)
			{
				score_max=tscore;
				tstep.x=i;tstep.y=j;
			}
			ChessBoard[i][j]=empty;//恢复现场
		}
	}
	return tstep;
}
StepNode Ai_hitagi_gametree(char cb[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//博弈树
{
	StepNode tstep;
	ChessAiExtend ai_info;
	ScoreStack sstack;
	extern int kuon_evalTable[KUON_NUM_EVALUATE];

	ai_info.in.row=row;
	ai_info.in.column=column;
	ai_info.in.max_depth=2;
	ai_info.in.edge=edge;
	ai_info.in.edge_dis=1;
	ai_info.in.ps_stack=&sstack;
	ai_info.in.evel_extra=ann_data;
	ai_info.in.evel_b=&hitagiXkuon_eval;

	tstep=Ai_kuon_gametree(cb,kuon_evalTable,&ai_info,color_max);
	return tstep;
}

						
/* AI_hitagi外部下棋函数*/
StepNode Ai_hitagi_play(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//ai执行函数
{
	StepNode tstep;
	//tstep=Ai_hitagi_pos(ChessBoard,row,column,edge,color_max,ann_data);
	//tstep=Ai_hitagi_pos2(ChessBoard,row,column,edge,color_max,ann_data);
	tstep=Ai_hitagi_gametree(ChessBoard,row,column,edge,color_max,ann_data);
	return tstep;
}
StepNode Ai_hitagi_play2(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)
{
	StepNode tstep;
	tstep=Ai_hitagi_pos(ChessBoard,row,column,edge,color_max,ann_data);
	//tstep=Ai_hitagi_pos2(ChessBoard,row,column,edge,color_max,ann_data);
	//tstep=Ai_hitagi_gametree(ChessBoard,row,column,edge,color_max,ann_data);
	return tstep;
}
void Ai_hitagi_train(char ChessBoard[][MAX_COLUMN],
					 int row,int column,
					 ChessStep *steplog,
					 ChessType color_win,
					 struct hitagi_ann_data *ann_data)//ai训练函数,游戏结束时用
{
	int i,j,k,x,y,temp;
	char color;
	double output_last[HITAGI_NUM_OUTPUT];
	double output_test[MAX_STEP][HITAGI_NUM_OUTPUT];
	double input_test[MAX_STEP][HITAGI_NUM_INPUT];
	double tmp;
	struct fann *fann_data=(struct fann*)ann_data->extern_ann;

	/*倒序时间差分学习:
	  从结果开始（赢了一方为1，输了为0）
	  TD学习的是修改网络前权值,
	*/
	
	ini(ChessBoard,NULL);
	//修改前每个的输出值、输入值
	for(i=0;i<steplog->count;i++)
	{
		x=steplog->step[i].x;
		y=steplog->step[i].y;
		color=steplog->step[i].color;
		ChessBoard[x][y]=color;
		//PrintChessBoard(ChessBoard,row,column);
		hitagi_makeAnnInput(ChessBoard,row,column,ann_data);
		hitagi_annTest(ann_data);
		for(j=0;j<HITAGI_NUM_INPUT;j++)//输入
			input_test[i][j]=ann_data->data_input[j];
		for(j=0;j<HITAGI_NUM_OUTPUT;j++)//输出
			output_test[i][j]=ann_data->data_output[j];
	}
	
	output_last[0]=0;output_last[1]=0;//输出层为2的情况
	if(color_win==black) output_last[0]=1;
	else output_last[1]=1;

	//训练还是改成批量训练，使这一组误差最小，防止后面学习参数破坏前面,但是貌似不行，因为td学习

	fann_reset_MSE((struct fann *)ann_data->extern_ann);
	fann_set_training_algorithm((struct fann *)ann_data->extern_ann,FANN_TRAIN_INCREMENTAL);
	for(i=steplog->count-1;i>=0;i--)//学习太多无法拟合权值？
	{
		//if(i<=steplog->count-10) break;
		//修改下次学习输出
		//PrintChessBoard(ChessBoard,row,column);
		for(j=0;j<HITAGI_NUM_OUTPUT;j++)
		{
			//此处有问题，应该train和test一起用，次估值并不是上次的
			ann_data->data_output[j]=
					output_test[i][j]+
					HITAGI_RATE_TD_LEARNING *(output_last[j]-output_test[i][j]);//TD学习
		}
		/*训练？多次训练？对称训练
		*/
		
		//反向训练
		for(j=0;j<HITAGI_NUM_EVALUATE;j++)//输入交换
		{
			tmp=ann_data->data_input[j];
			ann_data->data_input[j]=ann_data->data_input[j+HITAGI_NUM_EVALUATE];
			ann_data->data_input[j+HITAGI_NUM_EVALUATE]=tmp;
		}
		tmp=ann_data->data_output[0];//输出交换
		ann_data->data_output[0]=ann_data->data_output[1];
		ann_data->data_output[1]=tmp;
		for(k=0;k<1;k++)
			fann_train((struct fann*)ann_data->extern_ann,ann_data->data_input,ann_data->data_output);
		//正向训练
		for(j=0;j<HITAGI_NUM_EVALUATE;j++)//输入交换
		{
			tmp=ann_data->data_input[j];
			ann_data->data_input[j]=ann_data->data_input[j+HITAGI_NUM_EVALUATE];
			ann_data->data_input[j+HITAGI_NUM_EVALUATE]=tmp;
		}
		tmp=ann_data->data_output[0];//输出交换
		ann_data->data_output[0]=ann_data->data_output[1];
		ann_data->data_output[1]=tmp;
		for(k=0;k<1;k++)
			fann_train((struct fann*)ann_data->extern_ann,ann_data->data_input,ann_data->data_output);

		
		//记录这次正向结果
		//hitagi_annTest(ann_data);
		for(j=0;j<HITAGI_NUM_OUTPUT;j++)
		{
			//output_last[j]=ann_data->data_output[j];
			output_last[j]=fann_data->output[j];
		}
		
		//获取下次输入、输出
		x=steplog->step[i].x;
		y=steplog->step[i].y;
		ChessBoard[x][y]=empty;
		hitagi_makeAnnInput(ChessBoard,row,column,ann_data);
	}
}
//由于需要神经网络，不再提供对外训练函数了