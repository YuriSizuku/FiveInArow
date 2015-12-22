#include "stdFunc.h"
#include "Ai_kuon.h"

#include "doublefann.h"

#define FANN_USE_DLL
#ifndef AI_hitagi_h
#define AI_hitagi_h
#define HITAGI_FANN //用fann库
/*  ai_hitagi 
	TD与神经网络动态学习算法

	1.本来一开始想着将整个棋盘作为输入，当实际尝试后发现不是不收敛就收敛到错误的解
	  貌似不能很好的拟合函数
	2.现在尝试用神经网络来学习对指定估值类型的值（估值类型见下表）
	  输入层：对整个棋盘扫描得到双方各种类型的数目
	  输出层：对双方某种局势的估值
*/

/*  
	估值类型
	图例说明：
		1.0表示空位，*表示max，H表示min
		2.非边界的零与非边界的*交换等价
		3.#一定不能在中间，在左右端点等价
		4.0可以和非边上的*交换

	0：*****			//5连				1
	1：0****0			//活4				2
	2：#****0			//连冲4				3
	3：#***0*			//跳冲4(1)			7
	4：0***0*			//跳冲4(2)			8
	5：**0**			//跳冲4(3)			9
	6：***0**			//跳冲4(4)			(暂不考虑)
	7：0***00           //连活3				4
	8：0*0**0			//跳活3				10
	9：#***00			//连眠3				6
	10：#**0*0			//跳眠3(1)			11
	11：#*0**0			//跳眠3(2)			12
	12：0*0*0*0			//特眠3(1)			20
	13：#*0*0*			//特眠3(2)			21
	14：0**00*			//特眠3(3)			13
	15：#**00*			//特眠3(4)			14
	16：#0***0#			//假活3				5
	17：*0**0*			//双眠3(1)			15
	18：**0*0**			//双眠3(2)			16
	19：00**00			//连活2				17
	20：0*0*00			//跳活2				21
	21：0*00*0			//大跳活2			22
	22：#**000			//连眠2				18
	23：#*0*00			//跳眠2				23
	24：#*00*0			//大跳眠2			24
	25：*000*			//特眠2				25
	26：#0**00#			//假活2(1)			19
	27：#0*0*0#			//假活2(2)			26
	28：00*00           //活1				27
	实现方法：
		1.将棋盘每个横、竖、左斜、右斜视为n维向量（n<5不考虑）
		2.对每个向量进行操作（最大匹配原则，最多找5个子）
		   2.1 找2个min棋子（或边界）夹着的区间(x1~x2)
		   2.2 计算在区间(x1~x2)中max棋子个数count，连续棋子个数(5~1),空位个数
		   2.3 区间中与估值类型匹配，优先左侧匹配(或者再找子区间)，汇总区间
		   2.4 若剩下的区间还可以找子区间，返回2.1
		3.汇总各个向量
*/

/*变量与结构声明*/
//宏定义
#define HITAGI_NUM_EVALUATE 30
#define HITAGI_NUM_LAYER 3                      //网络总层数
#define	HITAGI_NUM_HIDEN 60                     //隐含层神经元数（每个隐含层神经元数同）
#define HITAGI_NUM_INPUT 2*HITAGI_NUM_EVALUATE  //输入神经元个数
#define HITAGI_NUM_OUTPUT 2                     //输出神经元个数
#define HITAGI_NUM_WEIGHTS HITAGI_NUM_INPUT*HITAGI_NUM_HIDEN\
		+(HITAGI_NUM_LAYER-3)*HITAGI_NUM_HIDEN\
		+HITAGI_NUM_HIDEN*HITAGI_NUM_OUTPUT
	                                            //权值数量
#define HITAGI_WEIGHT_MAX 0.1                 //权值最大值
#define HITAGI_WEIGHT_MIN -0.1                //权值最小值
#define HITAGI_RATE_ANN_LEARNING 0.5           //神经网络学习效率
#define HITAGI_RATE_TD_LEARNING 0.5			//TD学习效率
#define HITAGI_ACCURACY  0.0001                 //浮点比较误差
#define HITAGI_MAX_EPOCHS 1000					//最大迭代次数
#define HITAGI_DESIRED_ERROR 0.000001				//期望误差

//连接权重
struct hitagi_ann_weight{
	double weight_input[HITAGI_NUM_INPUT][HITAGI_NUM_HIDEN];
	double weight_output[HITAGI_NUM_HIDEN][HITAGI_NUM_OUTPUT];
#if HITAGI_NUM_LAYER >3
	double weight_hiden[HITAGI_NUM_LAYER-3][HITAGI_NUM_HIDEN][HITAGI_NUM_HIDEN];
#endif
};

//神经网络数据
struct hitagi_ann_data{
	double data_input[HITAGI_NUM_INPUT];//棋盘上存在某种估值类型的个数
	double data_output[HITAGI_NUM_OUTPUT];//输出黑子与白子的估值(0~1) =1为胜利
	struct hitagi_ann_weight *weight;//调用fann就不用这个了
	void *extern_ann;//用于指向外部ann库的结构
};
/*END*/



void hitagi_iniAnn(struct hitagi_ann_data *ann_data,
				   struct hitagi_ann_weight *ann_weight);
void hitagi_annTest(struct hitagi_ann_data *ann_data);
/*外部函数声明*/
void hitagi_getEvalTypes(char ChessBoard[][MAX_COLUMN],int row,int column,ChessType color,int *result);//棋盘类型
void hitagi_getEvalTypes_line(char *vector_chess,int start,int end,ChessType color_max,int *result);

struct hitagi_ann_data* hitagi_create(struct hitagi_ann_weight *ann_weight);//建立hitagi神经网络
struct hitagi_ann_data* hitagi_load(char *path);//读取hitagi神经网络
void   hitagi_save(struct hitagi_ann_data *ann_data,char *path);//存储hitagi神经网络
void   hitagi_destory(struct hitagi_ann_data *ann_data);//结束，释放资源
StepNode Ai_hitagi_play(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data);//ai执行函数
StepNode Ai_hitagi_play2(char ChessBoard[][MAX_COLUMN],
						int row,int column,
						Edge edge,
						ChessType color_max,
						struct hitagi_ann_data *ann_data);
void Ai_hitagi_train(char ChessBoard[][MAX_COLUMN],
					 int row,int column,
					 ChessStep *steplog,
					 ChessType color_win,
					 struct hitagi_ann_data *ann_data);//ai训练函数,游戏结束时用
#endif