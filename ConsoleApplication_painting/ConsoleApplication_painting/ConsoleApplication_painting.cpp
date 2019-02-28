#include "stdafx.h"
#include "stdafx.h"
#include <windows.h>
#define GLUT_DISABLE_ATEXIT_HACK
#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include<malloc.h>
#include <math.h>
#include <iostream>
#include <tchar.h>
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)
#include <stack>
using namespace std;
#define PI 3.1415926536f
//void FloodFill(int, int);

//void searchseed(int, int, int);
void chooseColor(int);

float xm, ym, xmm, ymm;
int first = 0;
int se = 0;

struct point {
	long x;
	long y;
	int color;
	int size;//线粗细
			 //char sm;
};//定义点的结构体

typedef struct ListNode {
	int index;//记录类型的下标
	int shape;//记录类型
	point point;
	struct ListNode * next;
}Node, *PNode;
typedef struct QueNode {
	PNode front, rear;
}Que, *PQue;
Que que;//初始化队列
PQue Pque = (PQue)malloc(sizeof(PNode));


void choosePointSize(int);
void chooseLineSize(int);
void initQ();
bool isEmpty();
void insertQ(point, int, int);
void traQ();

//void searchseed(int xleft, int xright, int y);

int shape = 1;//记录初始画图的形状
int color = 3;//记录初始颜色
int paintColor = 1;//记录涂色的颜色
int n = 100;//控制圆的精度
int screenWidth = 640;
int screenHeight = 480;
int count_rectangle = 0;
boolean mouseCount = false;
point point_rectangle[1024];
int count_triangle = 0;
point point_triangle[1024];
int count_line = 0;
point point_line[1024];
int count_circle = 0;
point point_circle[1024];
int count_point = 0;
point point_point[1024];
int point_f[1024];   //记录断点
					 //point seeds[1024];//记录种子
int tianse = 0;
int eraser = 0;//控制橡皮擦
static int line_Size = 0;//记录线粗
int  click=0;

//std::stack<point> stack_p;

//绘制调色板					
void colorBoard(int x, int y, int R, int c)
{
	glBegin(GL_POLYGON);
	// 设置颜色
	chooseColor(c);
	double delta_angle = PI / 180;
	for (double i = 0; i <= 2 * PI; i += delta_angle) {
		double    vx = x + R * cos(i);
		double    vy = y + R*sin(i);
		glVertex2d(vx, vy);
	}
	glEnd();
	glFlush();
}


void initQ() {
	Pque->front = Pque->rear = (PNode)malloc(sizeof(Node));
	if (Pque->front == NULL) {
		printf("队列内存分配失败");
		exit(-1);
	}
	Pque->front->next = NULL;
}

void insertQ(point point, int s, int index) {   //尾插
	PNode node = (PNode)malloc(sizeof(Node));
	if (node == NULL) {
		printf("jiedian内存分配失败");
		exit(-1);
	}
	node->point = point;
	node->shape = s;
	node->next = NULL;
	node->index = index;
	Pque->rear->next = node;
	Pque->rear = node;

}

void traQ() {

	if (isEmpty()) {
		printf("队列为空");
		exit(-1);
	}
	PNode node = (PNode)malloc(sizeof(Node));
	//if (node == NULL) {
	//	printf("jiedian内存分配失败");
	//	exit(-1);
	//}
	node = Pque->front->next;
	while (node != NULL) {
		printf("类型为：%d\n", node->shape);
		printf("序号为：%d\n", node->index);

		node = node->next;
	}

}

int lenQ() {
	int n = 0;
	if (isEmpty()) {
		printf("队列为空");
		return 0;
	}
	PNode node = (PNode)malloc(sizeof(Node));
	node = Pque->front->next;
	while (node != NULL) {
		n++;
		printf("类型为：%d\n", node->shape);
		printf("序号为：%d\n", node->index);

		node = node->next;
	}
	return n;
}


bool isEmpty() {

	if (Pque->front == Pque->rear) return true;
	else return false;
}

//取像素点颜色
float *getColor(int x, int y) {

	static float pix[3] = { 0.0 ,0.0,0.0 };
	glutPostRedisplay();
	glReadBuffer(GL_FRONT);
	glReadPixels(x, screenHeight - y, 1, 1, GL_RGB, GL_FLOAT, pix);// GL_RGB, GL_FLOAT,          GL_UNSIGNED_BYTE

	printf("getColor  pix[0]=%f  pix[1]=%f   pix[2]=%f     ", pix[0], pix[1], pix[2]);
	return pix;

}

float getR(int x, int y) {
	float pix[3] = { 0.0 };
	glutPostRedisplay();
	glReadBuffer(GL_FRONT);
	glReadPixels(x, screenHeight - y, 1, 1, GL_RGB, GL_FLOAT, pix);
	//glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pix);
	return pix[0];
}
float getG(int x, int y) {
	float pix[3] = { 0.0 };
	glutPostRedisplay();
	glReadBuffer(GL_FRONT);
	glReadPixels(x, screenHeight - y, 1, 1, GL_RGB, GL_FLOAT, pix);
	//glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pix);
	return pix[1];
}
float getB(int x, int y) {
	float pix[3] = { 0.0 };
	glutPostRedisplay();
	glReadBuffer(GL_FRONT);
	glReadPixels(x, screenHeight - y, 1, 1, GL_RGB, GL_FLOAT, pix);
	//glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pix);
	return pix[2];
}



void init(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//屏幕背景白色
	glMatrixMode(GL_PROJECTION);//投影变换
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	//glReadPixels();
}

//画点
void drawPoint(int x1, int y1) {
	//	choosePointSize(line_Size);
	glBegin(GL_POINTS);
	glVertex2i(x1, y1);
	//	glVertex2i(x2, y2);
	glEnd();
}

//画线
void drawLine(int x1, int y1, int x2, int y2) {

	//	chooseLineSize(line_Size);
	glBegin(GL_LINE_LOOP);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

//画三角形
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	//	chooseLineSize(line_Size);
	glBegin(GL_POINTS);//用来封口
	glVertex2i(x3, y3);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glVertex2i(x3, y3);
	glEnd();
}

//画矩形
void drawRectangle(int x1, int y1, int x2, int y2)
{
	//	chooseLineSize(line_Size);
	glBegin(GL_POINTS);//用来封口
	glVertex2i(x1, y1);
	//glVertex2i(x2, y2);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2i(x1, y1);
	glVertex2i(x1, y2);
	glVertex2i(x2, y2);
	glVertex2i(x2, y1);

	glEnd();
}

//画圆
void drawCircle(int x1, int y1, int x2, int y2)
{
	//	chooseLineSize(line_Size);

	float R = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) / 2;//半径
	float x = (x1 + x2) / 2;//圆心横坐标
	float y = (y1 + y2) / 2;//圆心纵坐标
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i<n; ++i)
		glVertex2f(x + R*cos(2 * PI / n*i), y + R*sin(2 * PI / n*i));
	glEnd();
}

//选择颜色
void chooseColor(int c) {
	//	glClear(GL_COLOR_BUFFER_BIT);
	switch (c) {
	case 1:
		glColor3f(1.0, 0.0, 0.0);//红
		break;
	case 2:
		glColor3f(0.0, 1.0, 0.0);//绿
		break;
	case 3:
		glColor3f(0.0, 0.0, 1.0);//蓝
		break;
	case 4:glColor3f(1.0, 1.0, 1.0);
		break;//白
	case 5:glColor3f(1.0, 0.0, 1.0);
		break;//紫色
	case 6:glColor3f(0.0, 1.0, 1.0);
		break;//青色
	case 7:glColor3f(1.0, 1.0, 0.0);
		break;//黄色
	case 8:glColor3f(0.0, 0.0, 0.0);
		break;//黑

	}
}
void colorSize(float r, float g, float b) {
	printf("接收%f,  %f,  %f  ", r, g, b);
	if (r == 1.0&&g == 0.0&&b == 0.0) { color = 1; }
	else if (r == 0.0&&g == 1.0&&b == 0.0) { color = 2; }
	else if (r == 0.0&&g == 0.0&&b == 1.0) { color = 3; }
	else if (r == 1.0&&g == 1.0&&b == 1.0) { color = 4; }
	else if (r == 1.0&&g == 0.0&&b == 1.0) { color = 5; }
	else if (r == 0.0&&g == 1.0&&b == 1.0) { color = 6; }
	else if (r == 1.0&&g == 1.0&&b == 0.0) { color = 7; }
	else if (r == 0.0&&g == 0.0&&b == 0.0) { color = 8; }
	else { printf("颜色不符"); }
	printf("点%.1f,  %.1f,  %.1f  ", r, g, b);
}
//选择线粗
void chooseLineSize(int s) {
	glClear(GL_DEPTH_BUFFER_BIT);
	switch (s) {
	case 0:
	case 1:glLineWidth(1.0f); break;
	case 2:glLineWidth(2.0f); break;
	case 3:glLineWidth(3.0f); break;
	case 4:glLineWidth(4.0f); break;
	case 5:glLineWidth(5.0f); break;
	}
}
//选择点粗
void choosePointSize(int p) {
	glClear(GL_DEPTH_BUFFER_BIT);
	switch (p) {
	case 0:
	case 1:glPointSize(1.0f); break;
	case 2:glPointSize(2.0f); break;
	case 3:glPointSize(3.0f); break;
	case 4:glPointSize(4.0f); break;
	case 5:glPointSize(5.0f); break;
	}
}

void itd() {

	int index;//记录数组下标
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	if (isEmpty()) {
		printf("队列为空");
		//	system("pause");
		exit(-1);
	}
	PNode node = (PNode)malloc(sizeof(Node));
	node = Pque->front->next;
	while (node != NULL) {
		switch (node->shape) {
		case -1:break;
		case 4:
			index = node->index;
			//printf("队列内容：x1,x2: \n", point_circle[index].x,  point_circle[index + 1].x );
			//printf("队列内容：y1,y2: \n", point_circle[index].y, point_circle[index + 1].y);
			chooseColor(point_circle[index].color);
			chooseLineSize(point_circle[index].size);
		    printf("队列内容：index :%d\n", index);
			drawCircle(point_circle[index].x, point_circle[index].y, point_circle[index + 1].x, point_circle[index + 1].y);
		
			break;
		case 3:
			index = node->index;
			chooseColor(point_rectangle[index].color);
			chooseLineSize(point_rectangle[index].size);
			drawRectangle(point_rectangle[index].x, point_rectangle[index].y, point_rectangle[index + 1].x, point_rectangle[index + 1].y);

			break;
		case 1:
			index = node->index;
			chooseColor(point_line[index].color);
			chooseLineSize(point_line[index].size);

			drawLine(point_line[index].x, point_line[index].y, point_line[index + 1].x, point_line[index + 1].y);

			break;

		case 2:
			index = node->index;

			if (((count_triangle + 1) % 3 == 2))
			{
				chooseColor(point_triangle[count_triangle - 1].color);
				chooseLineSize(point_triangle[count_triangle - 1].size);
				//	printf("itdisplay 33333 size:%d     ", point_triangle[count_triangle - 1].size);
				drawLine(point_triangle[count_triangle - 1].x, point_triangle[count_triangle - 1].y, point_triangle[count_triangle].x, point_triangle[count_triangle].y);

			}
			else {
				//	for (int i = 0; i <= index; i = i + 3){
				chooseColor(point_triangle[index].color);
				chooseLineSize(point_triangle[index].size);
				//	printf("itdisplay 33333 size:%d     ", point_triangle[i].size);
				drawTriangle(point_triangle[index].x, point_triangle[index].y, point_triangle[index + 1].x, point_triangle[index + 1].y, point_triangle[index + 2].x, point_triangle[index + 2].y);

			}

			break;

		case 5:

			index = node->index;

			chooseColor(point_point[index].color);
			choosePointSize(point_point[index].size);
			drawPoint(point_point[index].x, point_point[index].y);
			if ((point_f[index - 1] != 1 && point_f[index] != 1) && (index - 1>0)) {// || (point_f[index - 1] != 1 && point_f[index] == 1)
				chooseColor(point_point[index].color);
				choosePointSize(point_point[index].size);
				chooseLineSize(point_point[index].size);
				drawLine(point_point[index - 1].x, point_point[index - 1].y, point_point[index].x, point_point[index].y);
				//else if ((point_f[i - 1] == 1 && point_f[i] != 1)|| (point_f[i - 1] == 1 && point_f[i] == 1))
				//	continue;
			}


			break;

		}

		node = node->next;

	}
	printf("    itttttttttttttttttttdddd           ");
	//绘制调色板
	colorBoard(200, 460, 10, 8);
	colorBoard(170, 460, 10, 7);
	colorBoard(140, 460, 10, 6);
	colorBoard(110, 460, 10, 5);
	colorBoard(80, 460, 10, 3);
	colorBoard(50, 460, 10, 2);
	colorBoard(20, 460, 10, 1);

	glFlush();  //输送到显示设备
	
}


void myMouse(int button, int state, int x, int y) {

	float *pix;
	if (button == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {
			//点击颜色条
			if (y < 35 && x < 220)
			{
				//	printf("命中");
				pix = getColor(x, y);
				//	printf("送%f,  %f,  %f  ",pix[0],pix[1],pix[2] );
				float r = pix[0]; float g = pix[1]; float b = pix[2];
				printf("打印RGB %f,  %f,  %f  ", r, g, b);
				printf("送2%f,  %f,  %f  ", pix[0], pix[1], pix[2]);
				printf("送333%f,  %f,  %f  ", pix[0], pix[1], pix[2]);
				colorSize(r, g, b);

				printf("送4444%f,  %f,  %f  ", pix[0], pix[1], pix[2]);
				glColor3f(pix[0], pix[1], pix[2]);

			}
			else {

				//取像素点颜色
				//	float *pix = getColor(x, y);
				mouseCount = true;
				//if (tianse == 1) {
				////	//	FloodFill(x, screenHeight-y);
				//	FloodFill(x, y);
				//}

				switch (eraser) {
				case 1:
					chooseColor(4);
					color = 4;
					line_Size = 5;
					shape = 7;
					break;
				}

				switch (shape) {
				case 1:
					//eraser = 0;
					point_line[count_line].color = color;
					point_line[count_line].x = x;
					point_line[count_line].y = screenHeight - y;
					point_line[count_line].size = line_Size;
					if (count_line % 2 == 0)//两次画一个
						insertQ(point_line[count_line], 1, count_line);
					//		printf("mymouse d %d    ", point_line[count_line].size);
					count_line++;
					//	myDisplay();
					itd();

					break;

				case 2:
					//eraser = 0;
					point_triangle[count_triangle].color = color;
					point_triangle[count_triangle].x = x;
					point_triangle[count_triangle].y = screenHeight - y;
					point_triangle[count_triangle].size = line_Size;
					if (count_triangle % 3 == 0)//三次画一个
						insertQ(point_triangle[count_triangle], 2, count_triangle);
					//		printf("now size:%d    ", point_triangle[count_triangle].size);
					count_triangle++;

					itd();
					//myDisplay();
					break;

				case 3:
					//eraser = 0;
					point_rectangle[count_rectangle].color = color;
					point_rectangle[count_rectangle].x = x;
					point_rectangle[count_rectangle].y = screenHeight - y;
					point_rectangle[count_rectangle].size = line_Size;
					if (count_rectangle % 2 == 0)//两次画一个
						insertQ(point_rectangle[count_rectangle], 3, count_rectangle);
					count_rectangle++;

					itd();
					//myDisplay();
					break;

				case 4:
					//	eraser = 0;
					point_circle[count_circle].color = color;
					point_circle[count_circle].x = x;
					point_circle[count_circle].y = screenHeight - y;
					point_circle[count_circle].size = line_Size;
					if (count_circle % 2 == 0)//两次画一个圆
						insertQ(point_circle[count_circle], 4, count_circle);
					count_circle++;

					itd();
					//myDisplay();
					break;
				case 7:  //橡皮擦和画笔实际效果一样
				case 5:
					point_point[count_point].color = color;
					point_f[0] = 1;
					point_f[count_point] = 0;
					point_point[count_point].x = x;
					point_point[count_point].y = screenHeight - y;
					point_point[count_point].size = line_Size;
					printf("插入队列            ");
					insertQ(point_point[count_point], 5, count_point);
					count_point++;
					itd();
					//myDisplay();

					break;



				}
			}


		}


		if (state == GLUT_UP && mouseCount) {
			mouseCount = false;

			switch (shape) {
			case 1:
				point_line[count_line].color = color;
				point_line[count_line].x = x;
				point_line[count_line].y = screenHeight - y;
				point_line[count_line].size = line_Size;
				//		printf("mymouse u %d    ", point_line[count_line].size);
				if (count_line % 2 == 0)//两次画一个
					insertQ(point_line[count_line], 1, count_line);

				count_line++;
				itd();
				//myDisplay();

				break;
			case 7:
			case 5:
				point_point[count_point].color = color;
				point_f[count_point] = 1;
				point_point[count_point].x = x;
				point_point[count_point].y = screenHeight - y;
				point_point[count_point].size = line_Size;
				insertQ(point_point[count_point], 5, count_point);
				count_point++;
				itd();
				//myDisplay();

				break;
			

		

			}

		}

	}

}


//鼠标按住移动事件
void myMotion(int x, int y) {
	switch (shape) {
	case 1:
		point_line[count_line].color = color;
		point_line[count_line].x = x;
		point_line[count_line].y = screenHeight - y;
		point_line[count_line].size = line_Size;
		//	printf("mymotion %d    ", point_line[count_line].size);
		itd();
		//itDisplay();
		break;
	case 2:
		point_triangle[count_triangle].color = color;
		point_triangle[count_triangle].x = x;
		point_triangle[count_triangle].y = screenHeight - y;
		point_triangle[count_triangle].size = line_Size;
		itd();
		//itDisplay();
		break;
	case 3:
		point_rectangle[count_rectangle].color = color;
		point_rectangle[count_rectangle].x = x;
		point_rectangle[count_rectangle].y = screenHeight - y;
		point_rectangle[count_rectangle].size = line_Size;
		itd();
		//itDisplay();
		break;

	case 4:
		point_circle[count_circle].color = color;
		point_circle[count_circle].x = x;
		point_circle[count_circle].y = screenHeight - y;
		point_circle[count_circle].size = line_Size;
		itd();
		//itDisplay();
		break;
	case 7:
	case 5:

		point_point[count_point].color = color;
		point_point[count_point].x = x;
		point_point[count_point].y = screenHeight - y;
		point_point[count_point].size = line_Size;
		insertQ(point_point[count_point], 5, count_point);
		count_point++;
		itd();
		//itDisplay();
		break;
	
	}

}

//鼠标松开移动事件

void myPassiveMotion(int x, int y) {

	switch (shape) {
	case 1:
		point_line[count_line].color = color;
		point_line[count_line].x = x;
		point_line[count_line].y = screenHeight - y;
		point_line[count_line].size = line_Size;
		//	printf("myP d %d    ", point_line[count_line].size);

		itd();
		//myDisplay();
		break;
	case 2:
		point_triangle[count_triangle].color = color;
		point_triangle[count_triangle].x = x;
		point_triangle[count_triangle].y = screenHeight - y;
		point_triangle[count_triangle].size = line_Size;
		itd();
		//myDisplay();
		//itDisplay();
		break;
	case 3:
		point_rectangle[count_rectangle].color = color;
		point_rectangle[count_rectangle].x = x;
		point_rectangle[count_rectangle].y = screenHeight - y;
		point_rectangle[count_rectangle].size = line_Size;
		itd();
		//myDisplay();
		//itDisplay();
		break;
	case 4:
		point_circle[count_circle].color = color;
		point_circle[count_circle].x = x;
		point_circle[count_circle].y = screenHeight - y;
		point_circle[count_circle].size = line_Size;
		printf("myppppppppppassssssssss");
		itd();
		//myDisplay();
		//itDisplay();
		break;
	case 7:
	case 5:
		//	point_f = 0;
		point_point[count_point].color = color;
		point_point[count_point].x = x;
		point_point[count_point].y = screenHeight - y;
		point_point[count_point].size = line_Size;
		//	point_f[count_point] = 1;
		itd();
		//myDisplay();
		//itDisplay();
		break;
	}
	//myDisplay();
}


void shapeSubMenu(GLint shapeOption) {


	shape = shapeOption;
	switch (shape) {
	case 0:case 1:case 2:case 3:
	case 5:case 6:case 7:case 8:
		eraser = 0; break;
	}
}
//
//void colorSubMenu(GLint colorOption) {
//
//
//	color = colorOption;
//	switch (color) {
//	case 0:case 1:case 2:case 3: 
//	case 5:case 6:case 7:case 8:
//		eraser = 0; break;
//	}
//
//}
//
//void paintColorSubMenu(GLint paintColorOption) {
//
//	//eraser = 0;
//	paintColor = paintColorOption;
//}

void lineSizeSubMenu(GLint lineSizeOption) {

	line_Size = lineSizeOption;
	//switch (line_Size) {
	//case 0:
	//case 1:
	//case 2:
	//case 3: 
	//case 4:
	//case 5:eraser = 0; break;
	//}
}

void load() {
	FILE *fp;
	fp = fopen("db1.txt", "r");//读取文件
	if (fp == NULL)
	{
		printf("文件打开失败\n");
		exit(0);
	}
	
	PNode p = Pque->front->next;
	PNode q = NULL;
	Pque->rear = Pque->front;
	Pque->front->next = NULL;
	while (p != NULL) {
		q = p;
		p = p->next;
		free(q);
	}

	printf("清除成功");
	point point_ini;
	point_ini.color = 0;
	point_ini.size = 0;
	point_ini.x = 0;
	point_ini.y = 0;
	printf("执行到插入   ");
	insertQ(point_ini, -1, -1);

	int number,shape, index, x1, y1, x2, y2, x3 ,y3, color, size;
	fscanf(fp, "%d\n", &number);
	for (int i = 0; i < number ;i=i+1) {
	
		fscanf(fp, "%d\n", &shape);
		switch (shape) {
		case 4:
			fscanf(fp, "%d %d %d %d %d %d %d %d\n", &index, &x1, &y1, &x2, &y2, &color, &size,&count_circle);//获取点的个数
			printf("load类型为：%d\n",shape);
			printf("load序号1为：%d\n", index);
			point_circle[index].x = x1; point_circle[index].y = y1; point_circle[index].color = color; point_circle[index].size = size;
			point_circle[index + 1].x = x2; point_circle[index + 1].y = y2; point_circle[index + 1].color = color; point_circle[index + 1].size = size;
		//	printf("load序号2为：%d\n", index);
			insertQ(point_circle[index], shape, index);
			
			itd();
			break;
		case 1:
			fscanf(fp, "%d %d %d %d %d %d %d %d\n", &index, &x1, &y1, &x2, &y2, &color, &size,&count_line);//获取点的个数
			printf("load类型为：%d\n", shape);
			printf("load序号1为：%d\n", index);
			point_line[index].x = x1; point_line[index].y = y1; point_line[index].color = color; point_line[index].size = size;
			point_line[index + 1].x = x2; point_line[index + 1].y = y2; point_line[index + 1].color = color; point_line[index + 1].size = size;
			//	printf("load序号2为：%d\n", index);
			insertQ(point_line[index], shape, index);

			itd();
			break;

		case 3:
			fscanf(fp, "%d %d %d %d %d %d %d %d\n", &index, &x1, &y1, &x2, &y2, &color, &size,&count_rectangle);//获取点的个数
			printf("load类型为：%d\n", shape);
			printf("load序号1为：%d\n", index);
			point_rectangle[index].x = x1; point_rectangle[index].y = y1; point_rectangle[index].color = color; point_rectangle[index].size = size;
			point_rectangle[index + 1].x = x2; point_rectangle[index + 1].y = y2; point_rectangle[index + 1].color = color; point_rectangle[index + 1].size = size;
			//	printf("load序号2为：%d\n", index);
			insertQ(point_rectangle[index], shape, index);

			itd();
			break;
		case 2:
			fscanf(fp, "%d %d %d %d %d %d %d %d %d %d\n", &index, &x1, &y1, &x2, &y2, &x3, &y3, &color, &size,&count_triangle);//获取点的个数
			printf("load类型为：%d\n", shape);
			printf("load序号1为：%d\n", index);
			point_triangle[index].x = x1; point_triangle[index].y = y1; point_triangle[index].color = color; point_triangle[index].size = size;
			point_triangle[index + 1].x = x2; point_triangle[index + 1].y = y2; point_triangle[index + 1].color = color; point_triangle[index + 1].size = size;
			point_triangle[index + 2].x = x3; point_triangle[index + 2].y = y3; point_triangle[index + 2].color = color; point_triangle[index + 2].size = size;
			//	printf("load序号2为：%d\n", index);
			insertQ(point_triangle[index], shape, index);

			itd();
			break;
		case 5:
			fscanf(fp, "%d %d %d %d %d %d %d %d %d %d\n", &index, &x1, &y1, &x2, &y2, &x3, &y3, &color, &size, &count_point);//获取点的个数
			printf("load类型为：%d\n", shape);
			printf("load序号1为：%d\n", index);
			point_point[index].x = x1; point_point[index].y = y1; point_point[index].color = color; point_point[index].size = size;
			point_point[index -1].x = x2; point_point[index -1].y = y2; point_point[index - 1].color = color; point_point[index - 1].size = size;
			point_f[index ] = x3; point_f[index -1] = y3; 
			//	printf("load序号2为：%d\n", index);
			insertQ(point_point[index], shape, index);

			itd();
			break;
		}
	}
	fclose(fp);
	
}

void save() {

	int index;//记录数组下标
	if (isEmpty()) {
		printf("队列为空");
		system("pause");
		//exit(-1);
	}

	PNode node = (PNode)malloc(sizeof(Node));
	node = Pque->front->next;
	FILE *fp;
	fp = fopen("db1.txt", "w");//写入文件
	if (fp == NULL)
	{
		printf("文件打开失败\n");
		exit(0);
	}
	//fprintf(fp, "%d\n", count_line);//写入点的个数
	//for (int i = 0; i<count_line; i++)
	//{
	//	fprintf(fp, "%d %d %d\n", point_line[i].x, point_line[i].y, point_line[i].color);
	//}
	//fclose(fp);
	int n = lenQ();
	fprintf(fp, "%d\n", n-1);
	while (node != NULL) {
		switch (node->shape) {
		case -1:break;
		case 4:
			index = node->index;

			//chooseColor(point_circle[index].color);
			//chooseLineSize(point_circle[index].size);
			//drawCircle(point_circle[index].x, point_circle[index].y, point_circle[index + 1].x, point_circle[index + 1].y);
			printf("save类型为：%d\n", node->shape);
			printf("save序号为：%d\n", node->index);
			fprintf(fp, "%d\n", node->shape);
			fprintf(fp, "%d %d %d %d %d %d %d %d\n",index, point_circle[index].x, point_circle[index].y, point_circle[index+1].x, point_circle[index+1].y, point_circle[index].color, point_circle[index].size,count_circle);
			break;
		case 1:
			index = node->index;
			fprintf(fp, "%d\n", node->shape);
			fprintf(fp, "%d %d %d %d %d %d %d %d\n", index, point_line[index].x, point_line[index].y, point_line[index + 1].x, point_line[index + 1].y, point_line[index].color, point_line[index].size,count_line);
			break;
		case 3:
			index = node->index;
			fprintf(fp, "%d\n", node->shape);
			fprintf(fp, "%d %d %d %d %d %d %d %d\n", index, point_rectangle[index].x, point_rectangle[index].y, point_rectangle[index + 1].x, point_rectangle[index + 1].y, point_rectangle[index].color, point_rectangle[index].size, count_rectangle);
			break;
		case 2:
			index = node->index;
			fprintf(fp, "%d\n", node->shape);
			fprintf(fp, "%d %d %d %d %d %d %d %d %d %d\n", index, point_triangle[index].x, point_triangle[index].y, point_triangle[index + 1].x, point_triangle[index + 1].y, point_triangle[index + 2].x, point_triangle[index + 2].y, point_triangle[index].color, point_triangle[index].size,count_triangle);
			break;

		case 5:
			index = node->index;
			fprintf(fp, "%d\n", node->shape);
			fprintf(fp, "%d %d %d %d %d %d %d %d %d %d\n", index, point_point[index].x, point_point[index].y, point_point[index - 1].x, point_point[index - 1].y, point_f[index ], point_f[index-1], point_point[index].color, point_point[index].size, count_point);
			break;



		}

		node = node->next;

	}
	fclose(fp);
}


void mainMenu(GLint mainOption) {

	switch (mainOption) {
	case 1:
		
		count_point = -10000;
		//LoadPoints();
		
		load();
		itd();
		//myDisplay();
		break;
	case 2:
		//SavePoints();
		save();
		break;
	case 3: {
		count_line = 0;
		count_triangle = 0;
		count_rectangle = 0;
		count_circle = 0;
		count_point = 0;
		PNode p = Pque->front->next;
		PNode q = NULL;
		Pque->rear = Pque->front;
		Pque->front->next = NULL;
		while (p!=NULL) {
			q = p;
			p = p->next;
			free(q);
		}

		printf("清除成功");
		point point_ini;
		point_ini.color = 0;
		point_ini.size = 0;
		point_ini.x = 0;
		point_ini.y = 0;
		insertQ(point_ini, -1, -1);
		if (isEmpty()) {
			printf("为空");
			system("pause");
		}
		itd();
		//myDisplay();
		break;
	}
	//case 4:{
	//	//isEmpty();
	//	//insertQ( point_line[1], 1,1);
	//	//insertQ( point_triangle[1], 2,2);
	//	//insertQ( point_rectangle[1], 3,3);
	//	//insertQ( point_circle[1], 4,4);
	//	//isEmpty();
	//	//traQ();

	//	//int n;
	//	//scanf_s("%d", &n);

	//	//system("pause");
	//	click = 0;
	//	int n = lenQ();
	//	printf("队列长为 : %d",n);
	//	//tianse = 1;
	//	break;
	//}
	case 5:
		eraser = 1;
		break;

	}
}



int main(int argc, char *argv[])
{



	initQ();
	point point_ini;
	point_ini.color = 0;
	point_ini.size = 0;
	point_ini.x = 0;
	point_ini.y = 0;
	printf("执行到插入   ");
	insertQ(point_ini, -1, -1);
	int subMenu_shape;//定义子菜单
	int subMenu_line_Size;//定义线粗
	glutInit(&argc, argv);//固定写法
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(600, 300);//设置窗口在屏幕上的位置       13.7     11.6    16.9   14.8  16.9  30.3    =  104.2


	glutCreateWindow("Painter");

	init();
	subMenu_shape = glutCreateMenu(shapeSubMenu);
	glutAddMenuEntry("line", 1);
	glutAddMenuEntry("triangle", 2);
	glutAddMenuEntry("rectangle", 3);
	glutAddMenuEntry("circle", 4);
	glutAddMenuEntry("pencil", 5);

	//subMenu_color = glutCreateMenu(colorSubMenu);
	//glutAddMenuEntry("red", 1);
	//glutAddMenuEntry("green", 2);
	//glutAddMenuEntry("blue", 3);
	//subMenu_paint_color = glutCreateMenu(paintColorSubMenu);
	//glutAddMenuEntry("red", 1);
	//glutAddMenuEntry("green", 2);
	//glutAddMenuEntry("blue", 3);
	subMenu_line_Size = glutCreateMenu(lineSizeSubMenu);
	glutAddMenuEntry("size 1", 1);
	glutAddMenuEntry("size 2", 2);
	glutAddMenuEntry("size 3", 3);
	glutAddMenuEntry("size 4", 4);
	glutAddMenuEntry("size 5", 5);




	glutCreateMenu(mainMenu);
	glutAddSubMenu("choose the shape", subMenu_shape);
	//glutAddSubMenu("choose the color", subMenu_color);
	//glutAddSubMenu("painting the shape", subMenu_paint_color);
	glutAddSubMenu("choose the lineSize", subMenu_line_Size);
	glutAddMenuEntry("loading", 1);
	glutAddMenuEntry("save", 2);
	glutAddMenuEntry("clean", 3);
	//glutAddMenuEntry("tianse", 4);
	glutAddMenuEntry("eraser", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMouseFunc(myMouse);
	glutMotionFunc(myMotion);
	glutPassiveMotionFunc(myPassiveMotion);
	printf("执行到这");
	glutDisplayFunc(itd);




	glutMainLoop();

}