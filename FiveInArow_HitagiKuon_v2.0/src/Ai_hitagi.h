#include "stdFunc.h"
#include "Ai_kuon.h"

#include "doublefann.h"

#define FANN_USE_DLL
#ifndef AI_hitagi_h
#define AI_hitagi_h
#define HITAGI_FANN //��fann��
/*  ai_hitagi 
	TD�������綯̬ѧϰ�㷨

	1.����һ��ʼ���Ž�����������Ϊ���룬��ʵ�ʳ��Ժ��ֲ��ǲ�����������������Ľ�
	  ò�Ʋ��ܺܺõ���Ϻ���
	2.���ڳ�������������ѧϰ��ָ����ֵ���͵�ֵ����ֵ���ͼ��±�
	  ����㣺����������ɨ��õ�˫���������͵���Ŀ
	  ����㣺��˫��ĳ�־��ƵĹ�ֵ
*/

/*  
	��ֵ����
	ͼ��˵����
		1.0��ʾ��λ��*��ʾmax��H��ʾmin
		2.�Ǳ߽������Ǳ߽��*�����ȼ�
		3.#һ���������м䣬�����Ҷ˵�ȼ�
		4.0���ԺͷǱ��ϵ�*����

	0��*****			//5��				1
	1��0****0			//��4				2
	2��#****0			//����4				3
	3��#***0*			//����4(1)			7
	4��0***0*			//����4(2)			8
	5��**0**			//����4(3)			9
	6��***0**			//����4(4)			(�ݲ�����)
	7��0***00           //����3				4
	8��0*0**0			//����3				10
	9��#***00			//����3				6
	10��#**0*0			//����3(1)			11
	11��#*0**0			//����3(2)			12
	12��0*0*0*0			//����3(1)			20
	13��#*0*0*			//����3(2)			21
	14��0**00*			//����3(3)			13
	15��#**00*			//����3(4)			14
	16��#0***0#			//�ٻ�3				5
	17��*0**0*			//˫��3(1)			15
	18��**0*0**			//˫��3(2)			16
	19��00**00			//����2				17
	20��0*0*00			//����2				21
	21��0*00*0			//������2			22
	22��#**000			//����2				18
	23��#*0*00			//����2				23
	24��#*00*0			//������2			24
	25��*000*			//����2				25
	26��#0**00#			//�ٻ�2(1)			19
	27��#0*0*0#			//�ٻ�2(2)			26
	28��00*00           //��1				27
	ʵ�ַ�����
		1.������ÿ���ᡢ������б����б��Ϊnά������n<5�����ǣ�
		2.��ÿ���������в��������ƥ��ԭ�������5���ӣ�
		   2.1 ��2��min���ӣ���߽磩���ŵ�����(x1~x2)
		   2.2 ����������(x1~x2)��max���Ӹ���count���������Ӹ���(5~1),��λ����
		   2.3 ���������ֵ����ƥ�䣬�������ƥ��(��������������)����������
		   2.4 ��ʣ�µ����仹�����������䣬����2.1
		3.���ܸ�������
*/

/*������ṹ����*/
//�궨��
#define HITAGI_NUM_EVALUATE 30
#define HITAGI_NUM_LAYER 3                      //�����ܲ���
#define	HITAGI_NUM_HIDEN 60                     //��������Ԫ����ÿ����������Ԫ��ͬ��
#define HITAGI_NUM_INPUT 2*HITAGI_NUM_EVALUATE  //������Ԫ����
#define HITAGI_NUM_OUTPUT 2                     //�����Ԫ����
#define HITAGI_NUM_WEIGHTS HITAGI_NUM_INPUT*HITAGI_NUM_HIDEN\
		+(HITAGI_NUM_LAYER-3)*HITAGI_NUM_HIDEN\
		+HITAGI_NUM_HIDEN*HITAGI_NUM_OUTPUT
	                                            //Ȩֵ����
#define HITAGI_WEIGHT_MAX 0.1                 //Ȩֵ���ֵ
#define HITAGI_WEIGHT_MIN -0.1                //Ȩֵ��Сֵ
#define HITAGI_RATE_ANN_LEARNING 0.5           //������ѧϰЧ��
#define HITAGI_RATE_TD_LEARNING 0.5			//TDѧϰЧ��
#define HITAGI_ACCURACY  0.0001                 //����Ƚ����
#define HITAGI_MAX_EPOCHS 1000					//����������
#define HITAGI_DESIRED_ERROR 0.000001				//�������

//����Ȩ��
struct hitagi_ann_weight{
	double weight_input[HITAGI_NUM_INPUT][HITAGI_NUM_HIDEN];
	double weight_output[HITAGI_NUM_HIDEN][HITAGI_NUM_OUTPUT];
#if HITAGI_NUM_LAYER >3
	double weight_hiden[HITAGI_NUM_LAYER-3][HITAGI_NUM_HIDEN][HITAGI_NUM_HIDEN];
#endif
};

//����������
struct hitagi_ann_data{
	double data_input[HITAGI_NUM_INPUT];//�����ϴ���ĳ�ֹ�ֵ���͵ĸ���
	double data_output[HITAGI_NUM_OUTPUT];//�����������ӵĹ�ֵ(0~1) =1Ϊʤ��
	struct hitagi_ann_weight *weight;//����fann�Ͳ��������
	void *extern_ann;//����ָ���ⲿann��Ľṹ
};
/*END*/



void hitagi_iniAnn(struct hitagi_ann_data *ann_data,
				   struct hitagi_ann_weight *ann_weight);
void hitagi_annTest(struct hitagi_ann_data *ann_data);
/*�ⲿ��������*/
void hitagi_getEvalTypes(char ChessBoard[][MAX_COLUMN],int row,int column,ChessType color,int *result);//��������
void hitagi_getEvalTypes_line(char *vector_chess,int start,int end,ChessType color_max,int *result);

struct hitagi_ann_data* hitagi_create(struct hitagi_ann_weight *ann_weight);//����hitagi������
struct hitagi_ann_data* hitagi_load(char *path);//��ȡhitagi������
void   hitagi_save(struct hitagi_ann_data *ann_data,char *path);//�洢hitagi������
void   hitagi_destory(struct hitagi_ann_data *ann_data);//�������ͷ���Դ
StepNode Ai_hitagi_play(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data);//aiִ�к���
StepNode Ai_hitagi_play2(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data);
void Ai_hitagi_train(char ChessBoard[][MAX_COLUMN],
					 int row,int column,
					 ChessStep *steplog,
					 ChessType color_win,
					 struct hitagi_ann_data *ann_data);//aiѵ������,��Ϸ����ʱ��
#endif