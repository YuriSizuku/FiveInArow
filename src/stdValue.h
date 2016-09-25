#define MAX_ROW 15
#define MAX_COLUMN 15
#define MAX_STEP MAX_ROW*MAX_COLUMN
#define MAX_STACK 50
#define MAX(a,b) (a>b ? a:b)
#define INF 9999999

#ifndef _stdValue_h
#define _stdValue_h
enum ChessType{white='H',black='*',empty='0',even='E'};
typedef struct
{
	int x1;
	int x2;
	int y1;
	int y2;
}Edge;
typedef struct
{
	int x;
	int y;
}Position;
typedef struct
{
	int x;
	int y;
	enum ChessType color;
}StepNode;
typedef struct
{
	StepNode step[MAX_STEP+10];
	int count;
}ChessStep; 
typedef int (*PF_EVA_POS)//ָ�������
	(char ChessBoard[][MAX_COLUMN],int *EvaluateTable,int row,int column,StepNode tstep);
typedef int (*PF_EVA_BOARD)//ָ��������ֺ���
	(char ChessBoard[][MAX_COLUMN],int *EvaluateTable,Edge edge,int row,int column,enum ChessType color,void *extra);
	//����extraΪ�˷�����չ������hitagi����evaluateTable��Ϊ��
typedef StepNode (*PF_AI_PLAY) (char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge *edge,
						ChessType color_max);
	//ͨ�����庯��ָ��
typedef struct
{
	Position pos;
	Position pos_child;//MAX,MIN�������ӽڵ�
	Edge edge;
	int score;
}ScoreStackNode;
typedef struct
{
	ScoreStackNode stack[MAX_STACK];
	int count;
	int capacity;
}ScoreStack;//������������¼�����Ķ�ջ
typedef struct
{
	//����
	struct 
	{
		Edge edge;//������������
		int edge_dis;//����߽����
		int row,column;
		int max_depth;//�������ȣ�һ����ż����
		int max_count;//�����������
		PF_EVA_BOARD evel_b;//ÿ����MAX,MIN���ϵ����ֻص�����
		ScoreStack *ps_stack;
		void *evel_extra;
	}in;
	//���
	struct
	{
		int count_step;//��������
	}out;
	//Ϊ�˷�����չ 
	void *other;
}ChessAiExtend;
#endif