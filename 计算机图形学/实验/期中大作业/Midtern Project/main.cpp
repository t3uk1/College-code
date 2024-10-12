/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 * - 3) 绘制'J','Z','S','T',方格等形状方块
 * - 4) 随机生成方块并赋上不同的颜色
 * - 5) 方块的自动向下移动
 * - 6) 方块之间、方块与边界之间的碰撞检测
 * - 7) 棋盘格中每一行填充满之后自动消除
 * - 8) 增加了计分系统，每有一行消除增加50分
 * - 9) 增加难度系统，分为5级，5级之前每得到600分就增加一级难度，5级之后不再增加，
 *        难度越高，方块下落速度越快
 * - 10)新增按键映射： 空格：使方块马上下落到顶部， 'r': 使游戏重新开始，'p'： 暂停/恢复
 * - 11)增加了控制台游戏帮助打印以及游戏状态即时输出
 * - 12)增加了bonus系统，消除2行及以上会有bonus加成，具体体现在得分增加和技能槽蓄力速度增加，
 *        得分每多一行消除就额外增加25分，蓄力槽每有多一行消除就额外增加5%
 * - 13)增加了技能，每成功消除一次增加10%，每累计50%可以按'b'释放技能，技能可以连续释放，技能效果为消除最底层两行所有方块
 *
 *
 */

#include "include/Angel.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include<random>
#include <chrono>
#include <thread>
#include <cstdlib>
#include<iomanip>
using namespace std;

int starttime;			// 游戏开始时间
int nowtime;			//游戏当前时间
int timegap;				//从游戏开始到当前过了几秒

int pauseStartTime;
int pauseEndTime;  //计算暂停时间
int totalPausedTime;

int rotation = 0;		// 控制当前窗口中的方块旋转
glm::vec2 tile[4];			// 表示当前窗口中的方块
bool gameover = false;	// 游戏结束控制变量
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;

int Points;	//得分
int RankState;	//难度
int skillVal; //技能槽
int bonus;

bool isPaused;	//判断暂停

// 单个网格大小
int tile_width = 33;

// 网格布大小
const int board_width = 10;
const int board_height = 20;

// 网格三角面片的顶点数量
const int points_num = board_height * board_width * 6;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
const int board_line_num =  (board_width + 1) + (board_height + 1);

int ShapeTypeNum = 7;
int ShapeType = 0;//定义方块类型
// 一个二维数组表示所有可能出现的方块和方向。
glm::vec2 allRotationsLshape[28][4] =
{ {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(-1,-1)},	//   "L"
 {glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, -1)},   //
 {glm::vec2(1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0)},   //
 {glm::vec2(-1,1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)},
	{glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(0, -1)},// “T”
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(0, 1), glm::vec2(1, 0)},
	{glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1)},
	{glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(-1, 0)},
	{glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(2, 0)},//“|”
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(0, 1), glm::vec2(0, 2)},
	{glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(-2, 0)},
	{glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(0, -2)},
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(1, -1)},//方块
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(1, -1)},
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(1, -1)},
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(1, -1)},
	 {glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(0, 1), glm::vec2(-1, -1)},//J
	{glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(1, -1)},
	{glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 1)},
	{glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(-1, 1)},
	{glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1)},//Z
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(1, 1)},
	{glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1)},
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(1, 1)},
	{glm::vec2(0,0), glm::vec2(0,-1), glm::vec2(-1, - 1), glm::vec2(1,0)},//S
	{glm::vec2(0,1), glm::vec2(0,0),glm:: vec2(1,0),glm::vec2(1, - 1)},
	{glm::vec2(0,0), glm::vec2(0,-1), glm::vec2(-1, - 1), glm::vec2(1,0)} ,
	{glm::vec2(0,1), glm::vec2(0,0), glm::vec2(1,0),glm::vec2(1,-1)}
};

// 绘制窗口以及方块的颜色变量
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0);
glm::vec4 white  = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 black  = glm::vec4(0.0, 0.0, 0.0, 1.0);
glm::vec4 red = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 green = glm::vec4(0.0, 1.0, 0.0, 1.0);
glm::vec4 blue = glm::vec4(0.0, 0.0, 1.0, 1.0);
glm::vec4 pink = glm::vec4(1.0, 0.75, 0.8, 1.0);
glm::vec4 yellow = glm::vec4(1.0, 1.0, 0.0, 1.0);
glm::vec4 purple = glm::vec4(0.8, 0.0, 0.8, 1.0);
glm::vec4 bluegreen = glm::vec4(0.0, 1.0, 1.0, 1.0);

//初始化不同颜色
glm::vec4 colors[] = { orange, blue, green, red, yellow, pink, purple, bluegreen };

//设置当前方块颜色的全局变量
glm::vec4 nowColor;

// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
glm::vec2 tilepos = glm::vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[board_width][board_height];

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
glm::vec4 board_colours[points_num];

GLuint locxsize;
GLuint locysize;

GLuint vao[3];
GLuint vbo[6];

//声明一些函数
bool checkfullrow(int row);
bool movetile(glm::vec2 direction);
void calcPoints();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void printstate()
{
	cout << "俄罗斯方块游戏说明：" << endl;
	cout << "↑   :  旋转方块" << endl;
	cout << "←  : 左移方块" << endl;
	cout << "→  : 右移方块" << endl;
	cout << "↓  :  加速方块下落" << endl;
	cout << "space/空格  :  方块直接下落到底部" << endl;
	cout << "b : 释放技能（技能效果：立即消除最底下两层所有方块）" << endl;
	cout << "q : 暂停/恢复游戏" << endl;
	cout << "r  :  重启游戏" << endl;
	cout << "q/esc :  退出游戏" << endl << endl;

	cout << "当前分数： " << Points << endl;
	cout << "当前难度： " << RankState << endl;
	cout << "技能条： " << skillVal << "%" << endl;
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour)
{
	// 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
	//偏移量 6*(board_width*pos.y + pos.x) 为一个正方形的第一个点的偏移
	for (int i = 0; i < 6; i++)
		board_colours[(int)( 6 * ( board_width*pos.y + pos.x) + i)] = colour;

	glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

	// 计算偏移量，在适当的位置赋上颜色
	int offset = 6 * sizeof(glm::vec4) * (int)( board_width * pos.y + pos.x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);
		glm::vec4 p3 = glm::vec4(tile_width*2 + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p4 = glm::vec4(tile_width*2 + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);

		// 每个格子包含两个三角形，所以有6个顶点坐标
		glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), newpoints);
	}
	#ifdef __APPLE__
		glBindVertexArrayAPPLE(0);
	#else
		glBindVertexArray(0);
	#endif
}

// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块，
// 在游戏结束的时候判断，没有足够的空间来生成新的方块。
void newtile()
{
	// 将新方块放于棋盘格的最上行中间位置并设置默认的旋转方向
	tilepos = glm::vec2(5 , 19);
	rotation = 0;

	ShapeType = (time(0) + rand()) % ShapeTypeNum;
	for (int i = 0; i < 4; i++)
	{
		tile[i] = allRotationsLshape[4 * ShapeType][i];
	}

	updatetile();

	srand(114514);
	nowColor = colors[(time(0) + rand()) % 8];
	// 给新方块赋上颜色
	glm::vec4 newcolours[24];
	for (int i = 0; i < 24; i++)
		newcolours[i] = nowColor;

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

// 游戏和OpenGL初始化
void init()
{
	// 初始化棋盘格，这里用画直线的方法绘制网格
	// 包含竖线 board_width+1 条
	// 包含横线 board_height+1 条
	// 一条线2个顶点坐标，并且每个顶点一个颜色值
	
	glm::vec4 gridpoints[board_line_num * 2];
	glm::vec4 gridcolours[board_line_num * 2];

	// 绘制网格线
	// 纵向线
	for (int i = 0; i < (board_width+1); i++)
	{
		gridpoints[2*i] = glm::vec4((tile_width + (tile_width * i)), tile_width, 0, 1);
		gridpoints[2*i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height+1) * tile_width, 0, 1);
	}

	// 水平线
	for (int i = 0; i < (board_height+1); i++)
	{
		gridpoints[ 2*(board_width+1) + 2*i ] = glm::vec4(tile_width, (tile_width + (tile_width * i)), 0, 1);
		gridpoints[ 2*(board_width+1) + 2*i + 1 ] = glm::vec4((board_width+1) * tile_width, (tile_width + (tile_width * i)), 0, 1);
	}

	// 将所有线赋成白色
	for (int i = 0; i < (board_line_num * 2); i++)
		gridcolours[i] = white;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	glm::vec4 boardpoints[points_num];
	for (int i = 0; i < points_num; i++)
		board_colours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
	for (int i = 0; i < board_height; i++)
		for (int j = 0; j < board_width; j++)
		{
			glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			glm::vec4 p3 = glm::vec4(tile_width*2 + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p4 = glm::vec4(tile_width*2 + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			boardpoints[ 6 * ( board_width * i + j ) + 0 ] = p1;
			boardpoints[ 6 * ( board_width * i + j ) + 1 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 2 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 3 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 4 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 5 ] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < board_width; i++)
		for (int j = 0; j < board_height; j++)
			board[i][j] = false;

	// 载入着色器
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	
	glGenVertexArrays(3, &vao[0]);
	glBindVertexArray(vao[0]);		// 棋盘格顶点
	
	glGenBuffers(2, vbo);

	// 棋盘格顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridcolours, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[1]);		// 棋盘格每个格子

	glGenBuffers(2, &vbo[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), board_colours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[2]);		// 当前方块

	glGenBuffers(2, &vbo[4]);

	// 当前方块顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 当前方块顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(0);

	glClearColor(0, 0, 0, 0);

	// 游戏初始化
	Points = 0;
	RankState = 1;
	newtile();
	printstate();
	starttime = glutGet(GLUT_ELAPSED_TIME);
	isPaused = false;
	totalPausedTime = 0;
	skillVal = 0;
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
//新增对方块碰撞体积的检验
bool checkvalid(glm::vec2 cellpos)
{
	if((cellpos.x >=0) && (cellpos.x < board_width) && (cellpos.y >= 0) && (cellpos.y < board_height) 
		&&(board[int(cellpos.x)][int(cellpos.y)] == false))
		return true;
	else
		return false;
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;

	// 检查当前旋转之后的位置的有效性
	if (checkvalid((allRotationsLshape[nextrotation + 4 * ShapeType][0]) + tilepos)
		&& checkvalid((allRotationsLshape[nextrotation + 4 * ShapeType][1]) + tilepos)
		&& checkvalid((allRotationsLshape[nextrotation + 4 * ShapeType][2]) + tilepos)
		&& checkvalid((allRotationsLshape[nextrotation + 4 * ShapeType][3]) + tilepos))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation + 4 * ShapeType;
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsLshape[rotation][i];

		updatetile();
	}
}

// 检查棋盘格在row行有没有被填充满
bool checkfullrow(int row)
{
	bool isFull = true;
	for (int i = 0; i < 10; ++i)
	{
		if (board[i][row] == false)
		{
			isFull = false;//有一格没有被填充说明这行没有被填满
		}
	}

	if (isFull)
	{
		//消除并下移方块
		for (int i = 0; i < 10; ++i)
		{
			changecellcolour(glm::vec2(i, row), black);//颜色填充为黑色
			board[i][row] = false;//取消占用
		}
		for (int i = row + 1; i < 20; ++i)
		{
			for (int j = 0; j < 10; j++)
			{
				if (board[j][i] == true)
				{
					board[j][i] = false;//原格子取消占用
					glm::vec2 pos(j,i);
					//传入的颜色参数中的偏移为上一行的方块第一个点偏移
					//将上一行的颜色传给下一行
					changecellcolour(glm::vec2(j,i-1), board_colours[(int)(6 * (board_width * i + j))]);
					//将上一行放个的颜色置为黑色
					changecellcolour(pos, black);
					board[j][i - 1] = true;
					//所有格子颜色填充下移一行
				}
			}
		}
		return true;
	}
	return false;
}

void game_over()
{
	isPaused = true;
	gameover = true;
	cout << "Game Over!" << endl;
	cout << "本局得分为： " << Points << endl;
	cout << "Press 'r' to Restart." << endl << endl;
}

// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
	bool testGameOver = false;
	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		if (y == 19)
		{
			testGameOver = true;
		}
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;
		// 并将相应位置的颜色修改
		changecellcolour(glm::vec2(x, y), nowColor);
	}

	bonus = 0;
	for (int i = 19; i >= 0; i--)
	{
		if (checkfullrow(i))
		{
			bonus++;
		}
	}
	calcPoints();

	if (testGameOver)game_over();
}

void calcPoints()
{
	//加分
	if (bonus > 1)
	{
		for (int i = bonus - 1; i >= 0; i--)
		{
			Points += 50 + i * 25;
		}
	}
	else
	{
		Points += 50 * bonus;
	}

	//增加技能条，上限为100
	if (bonus)
	{
		skillVal += 10 + 5 * (bonus - 1);
	}

	if (RankState < 5)
	{
		RankState = Points / 600 + 1;
	}
	system("cls");
	printstate();
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction)
{
	// 计算移动之后的方块的位置坐标
	glm::vec2 newtilepos[4];
	for (int i = 0; i < 4; i++)
		newtilepos[i] = tile[i] + tilepos + direction;

	// 检查移动之后的有效性
	if (checkvalid(newtilepos[0])
		&& checkvalid(newtilepos[1])
		&& checkvalid(newtilepos[2])
		&& checkvalid(newtilepos[3]))
		{
			// 有效：移动该方块
			tilepos.x = tilepos.x + direction.x;
			tilepos.y = tilepos.y + direction.y;

			updatetile();

			return true;
		}

	return false;
}

//技能，直接消除最下两行
void bomb()
{
	//直接清空最下两行
	for (int row = 0; row < 2; row++)
	{
		//与checkfullrow中执行的下移操作一样
		for (int i = 0; i < 10; ++i)
		{
			changecellcolour(glm::vec2(i, row), black);//颜色填充为黑色
			board[i][row] = false;//取消占用
		}
		for (int i = row + 1; i < 20; ++i)
		{
			for (int j = 0; j < 10; j++)
			{
				if (board[j][i] == true)
				{
					board[j][i] = false;//原格子取消占用
					glm::vec2 pos(j, i);
					changecellcolour(glm::vec2(j, i - 1), board_colours[(int)(6 * (board_width * i + j))]);
					changecellcolour(pos, black);
					board[j][i - 1] = true;
					//所有格子颜色填充下移一行
				}
			}
		}
	}
}

//直接将方块落到底部
void movetiletothebottom()
{
	while (movetile(glm::vec2(0, -1)))
	{
	}
}

// 重新启动游戏
void restart()
{
	isPaused = false;	//重置暂停状态
	gameover = false;

	//重置得分与难度
	RankState = 1;
	Points = 0;
	skillVal = 0;

	//重新输出状态
	system("cls");
	cout << "游戏重新开始" << endl;
	printstate();

	//清空所有格子
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			board[i][j] = false;
			glm::vec2 pos(i, j);
			changecellcolour(pos, black);
		}
	}
	newtile();
	return;
}

// 游戏渲染部分
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格 (width * height * 2 个三角形)
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 24);	 // 绘制当前方块 (8 个三角形)
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, board_line_num * 2 );		 // 绘制棋盘格的线

}

// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//游戏进行中键盘响应
	if(!gameover)
	{
		switch(key)
		{	
			// 控制方块的移动方向，更改形态
			case GLFW_KEY_UP:	// 向上按键旋转方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT)
				{
					rotate();
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_DOWN: // 向下按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					if (!movetile(glm::vec2(0, -1)))
					{
						settile();
						newtile();
						break;
					}
					else
					{
						break;
				}
				}
			case GLFW_KEY_LEFT:  // 向左按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(-1, 0));
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_RIGHT: // 向右按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(1, 0));
					break;
				}
				else
				{
					break;
				}
			// 游戏设置。
			case GLFW_KEY_ESCAPE:
				if(action == GLFW_PRESS){
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_Q:
				if(action == GLFW_PRESS){
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}

			case GLFW_KEY_B:
				if (action == GLFW_PRESS)
				{
					if (skillVal > 0)
					{
						if (skillVal - 50 > 0)
						{
							bomb();
							skillVal -= 50;
						}
					}
					break;
				}
				else
				{
					cout << "技能槽未满！" << endl;
					break;
				}

			case GLFW_KEY_SPACE:
				if (action == GLFW_PRESS)
				{
					movetiletothebottom();
					break;
				}
				else
				{
					break;
				}

			case GLFW_KEY_P:
				if (action == GLFW_PRESS)
				{
					if (isPaused)
					{
						isPaused = false;
						cout << "解除暂停" << endl;
						pauseEndTime = glutGet(GLUT_ELAPSED_TIME);
						totalPausedTime += pauseEndTime - pauseStartTime;
						//cout << "暂停开始时间： " << pauseStartTime << endl;
						//cout << "暂停结束时间： " << pauseEndTime << endl;
						//cout << "暂停时长： " << totalPausedTime << endl;
					}
					else
					{
						isPaused = true;
						cout << "游戏暂停" << endl;
						pauseStartTime = glutGet(GLUT_ELAPSED_TIME);
					}
				}
		}
	}
	//全局键盘响应
	if (key == GLFW_KEY_R)
	{
		if (action == GLFW_PRESS) {
			restart();
		}
	}
}



int main(int argc, char **argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 创建窗口。
	GLFWwindow* window = glfwCreateWindow(500, 900, u8"2021150153-叶翔宇-期中大作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	

	init();
	//根据难度改变下落速度
	timegap = 0;
	while (!glfwWindowShouldClose(window))
    { 
		if (isPaused)
		{
			glfwWaitEvents(); // 等待事件
			continue; // 跳过后续代码，返回到事件循环的开头
		}
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();	
		nowtime = glutGet(GLUT_ELAPSED_TIME);
		//正常游戏的时间应减去暂停的时间
		if ((nowtime - starttime -  timegap - totalPausedTime) >= (1000 / RankState)) {
			timegap+= (1000/RankState);
			if (!movetile(glm::vec2(0, -1)))
			{
				settile();
				newtile();
			}
		}
    }
    glfwTerminate();
    return 0;
}
