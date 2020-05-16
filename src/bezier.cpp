#include <glut.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
using namespace std;
const int maxN = 100;
int windowHeight, windowWidth;
int condition = 1;
bool flag = false;

struct Vertex
{
	double x, y;
};
Vertex VertexArray[maxN];											//点集
Vertex line[maxN];													//存放bezier曲线的中间节点			
Vertex moveVertex[2];												//删除或移动时范围
int VertexCount = 0;

void display(){}

void draw()															//曲线绘制函数
{
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	glColor3f(0, 0, 1);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPointSize(10);
	for (int i = 0; i < VertexCount; i++)							//画出控制点
		glVertex2i(VertexArray[i].x, VertexArray[i].y);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPointSize(1);
	for (int i = 0; i < VertexCount; i++)							//画出控制多边形
		glVertex2i(VertexArray[i].x, VertexArray[i].y);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(0, 0, 0);
	for (double t = 0; t <= 1; t += 0.001)                      
	{
		for (int j = 0; j < VertexCount - 1; j++)                   //初始化line，初始化后数组内为1次中间节点
		{
			line[j].x = (1 - t) * VertexArray[j].x + t * VertexArray[j + 1].x;
			line[j].y = (1 - t) * VertexArray[j].y + t * VertexArray[j + 1].y;
		}
		for (int i = 2; i < VertexCount; i++)						//i为中间节点次数
		{
			for (int j = 0; j < VertexCount - i; j++)				//第j个i次中间节点
			{
				line[j].x = (1 - t) * line[j].x + t * line[j + 1].x;
				line[j].y = (1 - t) * line[j].y + t * line[j + 1].y;
			}
		}
		glVertex2i(line[0].x,line[0].y);
	}
	glEnd();
	glFlush();
}

void mouse(int button, int state, int x, int y)												//捕捉鼠标点击事件
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 1)					//左键点击输入点
	{
		VertexArray[VertexCount].x = x;														//获得顶点并存入
		VertexArray[VertexCount++].y = windowHeight - y;		//鼠标点击事件坐标原点为左上角，而画图坐标系坐标原点为左下角，进行坐标变换

		if (VertexCount > 1)
			draw();																			//画bezier曲线
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 2 && !flag)	//鼠标down,记录点，确定范围
	{
		moveVertex[0].x = x;
		moveVertex[0].y = windowHeight - y;
		flag = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && condition == 2 && flag)		//鼠标up,记录点，确定范围
	{
		moveVertex[1].x = x;
		moveVertex[1].y = windowHeight - y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 2 && flag)	//确定move后新的坐标
	{
		for (int i = 0; i < VertexCount; i++)
			if (moveVertex[0].x <= VertexArray[i].x && VertexArray[i].x <= moveVertex[1].x && moveVertex[1].y <= VertexArray[i].y && VertexArray[i].y <= moveVertex[0].y)
			{
				VertexArray[i].x = x;														//旧顶点换成新顶点
				VertexArray[i].y = windowHeight - y;
				break;
			}
		flag = false;
		if (VertexCount > 1)
			draw();
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 3)			//鼠标down,记录点，确定范围
	{
		moveVertex[0].x = x;
		moveVertex[0].y = windowHeight - y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && condition == 3)				//鼠标up,记录点，确定范围
	{
		moveVertex[1].x = x;
		moveVertex[1].y = windowHeight - y;
		for (int i = 0; i < VertexCount; i++)
			if (moveVertex[0].x <= VertexArray[i].x && VertexArray[i].x <= moveVertex[1].x && moveVertex[1].y <= VertexArray[i].y && VertexArray[i].y <= moveVertex[0].y)
			{
				for (int j = i; j < VertexCount - 1; j++)									//根据范围确定顶点，删除顶点
					VertexArray[j] = VertexArray[j + 1];
				break;
			}
		VertexCount--;
		if (VertexCount > 1)
			draw();
	}
}

void onMenu(int value)
{
	condition = value;
	switch (value)
	{
	case 4:																					//清空
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		memset(VertexArray, 0, sizeof(VertexArray));
		VertexCount = 0;
		flag = false;
		break;
	default:
		break;
	}
}

void createMenus()
{
	int menu = glutCreateMenu(onMenu);
	glutAddMenuEntry("insert", 1);
	glutAddMenuEntry("move", 2);
	glutAddMenuEntry("delete", 3);
	glutAddMenuEntry("clear", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int w, int h)											//窗口大小变化时调用
{
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);									//投影矩阵
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);										//模型视图矩阵
	windowHeight = h;
	windowWidth = w;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(1200, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glClearColor(1, 1, 1, 0);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	createMenus();
	glutMainLoop();
	return 0;
}