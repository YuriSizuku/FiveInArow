#include "Ai_hitagi.h"
#pragma comment(lib,"fanndouble.lib")//��̬������,����Ŀ¼

/*������annת������*/
void hitagi_makeAnnInput(char cb[][MAX_COLUMN],int row,int column,
					struct hitagi_ann_data *ann_data)//�õ�ann����
{
	int result[HITAGI_NUM_EVALUATE];
	int i;
	char color_win=judgeWin(cb,row,column);
	if(color_win==black)//Ϊ��С��5������ֱ�ӷ���
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
	//�������
	hitagi_getEvalTypes(cb,row,column,black,result);
	for(i=0;i<HITAGI_NUM_EVALUATE;i++) 
		ann_data->data_input[i]=result[i];
	//�������
	hitagi_getEvalTypes(cb,row,column,white,result);
	for(i=0;i<HITAGI_NUM_EVALUATE;i++) 
		ann_data->data_input[i+HITAGI_NUM_EVALUATE]=result[i];
	//���μ�
	//for(i>8;i<HITAGI_NUM_EVALUATE;i++)
	//{
	//	ann_data->data_input[i]=0;
	//	ann_data->data_input[i+HITAGI_NUM_EVALUATE]=0;
	//}
}

/*fann��ת������*/
void hitagi_ann2fann(struct hitagi_ann_data *ann_data)//�˺������ã�δ�꣩
{
	if(ann_data->extern_ann==NULL) return;
	int i,j,count=0;
	//Ȩֵ
	int num_weight=sizeof(struct hitagi_ann_weight)/sizeof(double);
	fann_connection fann_weight[HITAGI_NUM_WEIGHTS];

	/*fann��Ԫ�����Ų�, ע��ÿ�㶼��1��ƫ����Ԫ(bias neuron)û���ã����һ������
	  fann_weight from���ѭ����to�ڲ㣬from�е�ƫ����Ԫ������connect����������Ԫ������to�е�ƫ����Ԫ
	*/

	//�����X������
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
	//������X������
	//������X�����
	fann_set_weight_array((struct fann*)ann_data->extern_ann,
							fann_weight,
							HITAGI_NUM_WEIGHTS);
}
void hitagi_iniFannTrain(struct hitagi_ann_data *ann_data)//��ʼ������ѵ������
{
	
	int num=3;
	int i,j;
	//ѵ������5��Ȩֵ

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
					10,//ÿ�����ٸ�ֵprint
					HITAGI_DESIRED_ERROR);
	//hitagi_annTest(ann_data);
	//ann_data->data_input[0]=1;
	//ann_data->data_output[0]=1;
	//hitagi_annTest(ann_data);
	
	//�ͷſռ�
	for(i=0;i<num;i++)
	{
		delete[] data->input[i];
		delete[] data->output[i];
	}
	delete[] data->input;
	delete[] data->output;
	delete data;
}
void hitagi_iniFann(struct hitagi_ann_data *ann_data)//����fnn��ĳ�ʼ��
{
	int i;
	//����fann
	unsigned int num_layers[HITAGI_NUM_LAYER];
	num_layers[0]=HITAGI_NUM_INPUT;
	for(i=1;i<HITAGI_NUM_LAYER-1;i++)
		num_layers[i]=HITAGI_NUM_HIDEN;
	num_layers[i]=HITAGI_NUM_OUTPUT;
	ann_data->extern_ann=(struct fann *)fann_create_standard_array(
						HITAGI_NUM_LAYER,
						num_layers);
	//����fannѧϰ��
	fann_set_learning_rate((struct fann *)ann_data->extern_ann,HITAGI_RATE_ANN_LEARNING);
	//��ʼ��Ȩֵ
	fann_randomize_weights((struct fann *)ann_data->extern_ann,HITAGI_WEIGHT_MIN,HITAGI_WEIGHT_MAX);
	//�����(��Ĭ��0~1)
	fann_set_activation_function_output((struct fann *)ann_data->extern_ann,FANN_SIGMOID);
	fann_set_activation_function_hidden((struct fann *)ann_data->extern_ann,FANN_SIGMOID);
	//��ʼѵ��
	//hitagi_iniFannTrain(ann_data);
}

void hitagi_fann2annOutput(struct hitagi_ann_data *ann_data)//��fann�е�outputת��Ϊhitagi_ann�е�output
{
	int i,num;
	struct fann* fann_data=(struct fann*)ann_data->extern_ann;
	num=fann_get_num_output(fann_data);
	for(i=0;i<num;i++)
		ann_data->data_output[i]=fann_data->output[i];
}

/*Ai_hitagi ��ʼ���������������*/
void hitagi_iniAnn(struct hitagi_ann_data *ann_data,
				   struct hitagi_ann_weight *ann_weight)//��ʼ��annֵ
{
	memset(ann_data->data_input,0,sizeof(ann_data->data_input));
	memset(ann_data->data_output,0,sizeof(ann_data->data_output));
	ann_data->weight=ann_weight;
	//weight��ʱ����
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

/*Ai_hitagi ��������,ann��ֵ��ѧϰ����*/
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
//��һ������[start,end)�л��ĳ�־�������
//�߽���鷳���Զ��ڵ�һ�������һ������������ӣ������ǵ��ֶ����䣬�����ٿ���һ��ռ�ɣ�
//�߽���������ӵ�ͬ
{
	int i,j,k;
	int tstart=5;
	int tend=tstart+end-start;
	int x1=tstart,x2=tstart;
	char tvector[MAX(MAX_ROW,MAX_COLUMN)+10];//Ϊ�˽���߽����⣬��
	int count_chess=0;//ĳ�����ҷ������ܸ���
	int count_schess=0;//��ǰ���ҷ��������Ӹ���
	int count_schess_max=0;//ĳ�����ҷ���������������
	int start_schess_max=0;//ĳ�����ҷ�����������ʼλ��

	ChessType color_min=color_max==black ? white :black;
	for(i=0;i<tstart;i++) tvector[i]=color_min;
	for(i=start;i<end;i++) tvector[i-start+5]=vector_chess[i];
	for(i=end+tstart;i<MAX(MAX_ROW,MAX_COLUMN)+10;i++) tvector[i]=color_min;
	_evelTypes_ini(result);
	
	for(i=tstart;i<=tend;i++)
	{
		if(tvector[i]==color_min ) //���ֶԷ����ӻ򵽽���������ǶԷ�����
		{
			if(count_schess>count_schess_max)//ͳ���ҷ��������Ӹ���
			{
				count_schess_max=count_schess;
				start_schess_max=i-count_schess;
			}
			x2=i;
			/*
				�¿�ķ�굶ԭ�򣬾��������⣬��Ҫ��ţ�Ǽ�
				������費����6�������ϻ�û����
				���������ȥƥ�䣬̰��
			*/
			for(j=x1;j<x2;j++)
			{
				char left[5],right[5];// 1 0 * 0 1
				for(k=0;k<5;k++)
				{
					left[k]=tvector[start_schess_max-k-1];
					right[k]=tvector[start_schess_max+count_schess_max+k];
				}
				
				if(x2-x1<5) break;//λ�ò���5������
				if(count_chess<1) break;//���������ҷ�����ʱ
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
				if(count_schess_max==1)//ע���ж����ͬ��1ʱ����λ��������ߵ�
				{
					if((left[0]==empty && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max && right[4]==empty)
						||(right[0]==empty && left[0]==empty && left[1]==color_max && left[2]==empty && left[3]==color_max && left[4]==empty))
						//0*0*0*0,ֻ���Ǳ��ϵ�2��1���������м�
					{
						result[12]++;
						break;
					}
					if((left[0]==color_min && right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max)
						||(right[0]==empty && right[1]==color_max && right[2]==empty && right[3]==color_max && right[4]==color_min))
						//#*0*0*,ֻ���Ǳ��ϵ�2��1���������м�
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
			//������º�
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
		else//ͳ���ҷ��������Ӹ���
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
//�������л�ȡ��������
{
	int tresult[HITAGI_NUM_EVALUATE];
	char vector_chess[MAX(MAX_ROW,MAX_COLUMN)+2];
	int start,end;
	int i,j;
	
	_evelTypes_ini(result);

	for(i=0;i<row;i++)//������
	{
		start=0;end=column;
		for(j=0;j<column;j++)
		{
			vector_chess[j]=ChessBoard[i][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(j=0;j<column;j++)//������
	{
		start=0;end=row;
		for(i=0;i<row;i++)
		{
			vector_chess[i]=ChessBoard[i][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(i=0;i<row-4;i++)//�����°����Խ��߷���
	{
		start=0;end=row-i;
		for(j=0;j<row-i;j++)
		{
			vector_chess[j]=ChessBoard[i+j][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(j=1;j<column-4;j++)//����ϰ����Խ���
	{
		start=0;end=column-j;
		for(i=0;i<column-j;i++)
		{
			vector_chess[i]=ChessBoard[i][j+i];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(i=4;i<row;i++)//�����ϰ븱�Խ���
	{
		start=0;end=i+1;
		for(j=0;j<=i;j++)
		{
			vector_chess[j]=ChessBoard[i-j][j];
		}
		hitagi_getEvalTypes_line(vector_chess,start,end,color,tresult);
		_evelTypes_add(result,tresult);
	}
	for(j=1;j<column-4;j++)//����°븱�Խ���
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
void hitagi_annTest(struct hitagi_ann_data *ann_data)//��ann���в������
{
	double *output;
	int i;
	//fann_reset_MSE((struct fann *)ann_data->extern_ann);
	output=fann_test((struct fann *)ann_data->extern_ann,
		      ann_data->data_input,
			  ann_data->data_output);
	for(i=0;i<HITAGI_NUM_OUTPUT;i++)
		ann_data->data_output[i]=output[i];
	//if(output!=NULL) delete output;//����delete�����ص���fann�е�ָ��
}
double hitagi_evaluateBoard(char cb[][MAX_COLUMN],int row,int column,
							struct hitagi_ann_data *ann_data,
							ChessType color)//�������ֵ���ŵ�output��,���ع���ֵ
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
		if(ann_data->data_input[0]>0)//��ֹȨ�ع�ѧϰ
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
		if(ann_data->data_input[0]>0)//��ֹȨ�ع�ѧϰ
			return -1.0;
		return 1.04*ann_data->data_output[1]-ann_data->data_output[0];
		//return ann_data->data_output[0]>ann_data->data_output[1] ?
		//					-ann_data->data_output[0] : ann_data->data_output[1];
	}
}

/*Ai_hitagi ��������*/



/* AI_hitagi�ڲ�����*/
int hitagiXkuon_eval(char cb[][MAX_COLUMN],
						int *EvaluateTable,Edge edge,
						int row,int column,
						enum ChessType color,void *extra)//hitagi����kuon�Ĺ�ֵ����
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
						struct hitagi_ann_data *ann_data)//���ֵ
{
	/*debug*/
	int show=0;
	/*END*/
	StepNode tstep;
	double tscore,score_max=-INF;
	int i,j;
	for(i=0;i<row;i++)//����ֵ����ֹ����
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]==empty)
			{
				tstep.x=i;tstep.y=j;
				goto EVEL;
			}
		}
	EVEL:

	//�����������
	for(i=edge.x1;i<=edge.x2;i++)
	{
		for(j=edge.y1;j<=edge.y2;j++)
		{
			if(ChessBoard[i][j]!=empty) continue;//����break������bug������
			ChessBoard[i][j]=color_max;
			tscore=hitagi_evaluateBoard(ChessBoard,row,column,ann_data,color_max);
			if(tscore>score_max)
			{
				score_max=tscore;
				tstep.x=i;tstep.y=j;
			}
			ChessBoard[i][j]=empty;//�ָ��ֳ�
		}
	}
	return tstep;
}
StepNode Ai_hitagi_pos2(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//�˹�ֵ������
{
	/*debug*/
	int show=0;
	/*END*/
	StepNode tstep;
	double tscore,tscore_white,tscore_black,score_max=-INF;
	int i,j;
	for(i=0;i<row;i++)//����ֵ����ֹ����
		for(j=0;j<column;j++)
		{
			if(ChessBoard[i][j]==empty)
			{
				tstep.x=i;tstep.y=j;
				goto EVEL;
			}
		}
	EVEL:

	//�����������
	for(i=edge.x1;i<=edge.x2;i++)
	{
		for(j=edge.y1;j<=edge.y2;j++)
		{
			if(ChessBoard[i][j]!=empty) continue;//����break������bug������
			
			//˫���ֵ
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
			ChessBoard[i][j]=empty;//�ָ��ֳ�
		}
	}
	return tstep;
}
StepNode Ai_hitagi_gametree(char cb[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//������
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

						
/* AI_hitagi�ⲿ���庯��*/
StepNode Ai_hitagi_play(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data)//aiִ�к���
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
					 struct hitagi_ann_data *ann_data)//aiѵ������,��Ϸ����ʱ��
{
	int i,j,k,x,y,temp;
	char color;
	double output_last[HITAGI_NUM_OUTPUT];
	double output_test[MAX_STEP][HITAGI_NUM_OUTPUT];
	double input_test[MAX_STEP][HITAGI_NUM_INPUT];
	double tmp;
	struct fann *fann_data=(struct fann*)ann_data->extern_ann;

	/*����ʱ����ѧϰ:
	  �ӽ����ʼ��Ӯ��һ��Ϊ1������Ϊ0��
	  TDѧϰ�����޸�����ǰȨֵ,
	*/
	
	ini(ChessBoard,NULL);
	//�޸�ǰÿ�������ֵ������ֵ
	for(i=0;i<steplog->count;i++)
	{
		x=steplog->step[i].x;
		y=steplog->step[i].y;
		color=steplog->step[i].color;
		ChessBoard[x][y]=color;
		//PrintChessBoard(ChessBoard,row,column);
		hitagi_makeAnnInput(ChessBoard,row,column,ann_data);
		hitagi_annTest(ann_data);
		for(j=0;j<HITAGI_NUM_INPUT;j++)//����
			input_test[i][j]=ann_data->data_input[j];
		for(j=0;j<HITAGI_NUM_OUTPUT;j++)//���
			output_test[i][j]=ann_data->data_output[j];
	}
	
	output_last[0]=0;output_last[1]=0;//�����Ϊ2�����
	if(color_win==black) output_last[0]=1;
	else output_last[1]=1;

	//ѵ�����Ǹĳ�����ѵ����ʹ��һ�������С����ֹ����ѧϰ�����ƻ�ǰ��,����ò�Ʋ��У���Ϊtdѧϰ

	fann_reset_MSE((struct fann *)ann_data->extern_ann);
	fann_set_training_algorithm((struct fann *)ann_data->extern_ann,FANN_TRAIN_INCREMENTAL);
	for(i=steplog->count-1;i>=0;i--)//ѧϰ̫���޷����Ȩֵ��
	{
		//if(i<=steplog->count-10) break;
		//�޸��´�ѧϰ���
		//PrintChessBoard(ChessBoard,row,column);
		for(j=0;j<HITAGI_NUM_OUTPUT;j++)
		{
			//�˴������⣬Ӧ��train��testһ���ã��ι�ֵ�������ϴε�
			ann_data->data_output[j]=
					output_test[i][j]+
					HITAGI_RATE_TD_LEARNING *(output_last[j]-output_test[i][j]);//TDѧϰ
		}
		/*ѵ�������ѵ�����Գ�ѵ��
		*/
		
		//����ѵ��
		for(j=0;j<HITAGI_NUM_EVALUATE;j++)//���뽻��
		{
			tmp=ann_data->data_input[j];
			ann_data->data_input[j]=ann_data->data_input[j+HITAGI_NUM_EVALUATE];
			ann_data->data_input[j+HITAGI_NUM_EVALUATE]=tmp;
		}
		tmp=ann_data->data_output[0];//�������
		ann_data->data_output[0]=ann_data->data_output[1];
		ann_data->data_output[1]=tmp;
		for(k=0;k<1;k++)
			fann_train((struct fann*)ann_data->extern_ann,ann_data->data_input,ann_data->data_output);
		//����ѵ��
		for(j=0;j<HITAGI_NUM_EVALUATE;j++)//���뽻��
		{
			tmp=ann_data->data_input[j];
			ann_data->data_input[j]=ann_data->data_input[j+HITAGI_NUM_EVALUATE];
			ann_data->data_input[j+HITAGI_NUM_EVALUATE]=tmp;
		}
		tmp=ann_data->data_output[0];//�������
		ann_data->data_output[0]=ann_data->data_output[1];
		ann_data->data_output[1]=tmp;
		for(k=0;k<1;k++)
			fann_train((struct fann*)ann_data->extern_ann,ann_data->data_input,ann_data->data_output);

		
		//��¼���������
		//hitagi_annTest(ann_data);
		for(j=0;j<HITAGI_NUM_OUTPUT;j++)
		{
			//output_last[j]=ann_data->data_output[j];
			output_last[j]=fann_data->output[j];
		}
		
		//��ȡ�´����롢���
		x=steplog->step[i].x;
		y=steplog->step[i].y;
		ChessBoard[x][y]=empty;
		hitagi_makeAnnInput(ChessBoard,row,column,ann_data);
	}
}
//������Ҫ�����磬�����ṩ����ѵ��������