#include <glut.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
using namespace std;
const int maxN = 100;
int windowHeight, windowWidth;
int k = 4; 
int condition = 1;
bool flag = false;

struct Vertex
{
	double x, y;
};
Vertex VertexArray[maxN];			
Vertex line[maxN];
Vertex moveVertex[2];
int VertexCount = 0;

void drawControl()
{
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	glColor3f(0, 0, 1);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPointSize(10);
	for (int i = 0; i < VertexCount; i++)					//画出控制点
		glVertex2i(VertexArray[i].x, VertexArray[i].y);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPointSize(1);
	for (int i = 0; i < VertexCount; i++)					//画出控制多边形
		glVertex2i(VertexArray[i].x, VertexArray[i].y);
	glEnd();
	glFlush();
}

void drawBspline()											//deboor割角
{
	int n = VertexCount - 1;
	for (double t = k - 1; t < n + 1; t += 0.01)			//t(k-1)<= t < t(n+1)
	{
		glBegin(GL_LINE_STRIP);
		glColor3f(0, 0, 0);
		int j = (int)t;										//j为当前t所在区间左值
		for (int b = j - k + 1; b <= j; b++)				//初始化0次p
			line[b] = VertexArray[b];
		for (int r = 1; r <= k - 1; r++)					//r为次数
		{
			for (int i = j;i >= j - k + 1 + r;i--)
			{
				line[i].x = (t - i) / (i + k - r - i) * line[i].x + (i + k - r - t) / (i + k - r - i) * line[i - 1].x;
				line[i].y = (t - i) / (i + k - r - i) * line[i].y + (i + k - r - t) / (i + k - r - i) * line[i - 1].y;
			}
		}
		glVertex2i(line[j].x, line[j].y);					//画出第k-1次p
	}
	glEnd();
	glFlush();
}

void display() {}

void mouse(int button, int state, int x, int y)												//捕捉鼠标点击事件
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 1)					//左键点击输入点
	{
		VertexArray[VertexCount].x = x;														//获得顶点并存入
		VertexArray[VertexCount++].y = windowHeight - y;									//鼠标点击事件坐标原点为左上角，而画图坐标系坐标原点为左下角，进行坐标变换
		if (VertexCount > 1)
			drawControl();
		if (VertexCount > k - 1)
			drawBspline();
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 2 && !flag)	//移动顶点：鼠标down,记录点，确定范围		
	{
		moveVertex[0].x = x;
		moveVertex[0].y = windowHeight - y;
		flag = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && condition == 2 && flag)		//移动顶点：鼠标up,记录点，确定范围		
	{
		moveVertex[1].x = x;
		moveVertex[1].y = windowHeight - y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 2 && flag)	//确定move后新的坐标
	{
		for(int i = 0;i < VertexCount;i++)
			if (moveVertex[0].x <= VertexArray[i].x && VertexArray[i].x <= moveVertex[1].x && moveVertex[1].y <= VertexArray[i].y && VertexArray[i].y <= moveVertex[0].y)
			{
				VertexArray[i].x = x;														//旧顶点换成新顶点
				VertexArray[i].y = windowHeight - y;
				break;
			}
		flag = false;
		if (VertexCount > 1)
			drawControl();
		if (VertexCount > k - 1)
			drawBspline();
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && condition == 3)			//删除顶点：鼠标down,记录点，确定范围
	{
		moveVertex[0].x = x;
		moveVertex[0].y = windowHeight - y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && condition == 3)				//删除顶点：鼠标down,记录点，确定范围
	{
		moveVertex[1].x = x;
		moveVertex[1].y = windowHeight - y;
		for (int i = 0; i < VertexCount; i++)
			if (moveVertex[0].x <= VertexArray[i].x && VertexArray[i].x <= moveVertex[1].x && moveVertex[1].y <= VertexArray[i].y && VertexArray[i].y <= moveVertex[0].y)
			{
				for (int j = i; j < VertexCount - 1; j++)									//删除该顶点
					VertexArray[j] = VertexArray[j + 1];
				break;
			}
		VertexCount--;
		if (VertexCount > 1)
			drawControl();
		if (VertexCount > k - 1)
			drawBspline();
	}
}

void onMenu(int value)
{
	condition = value;
	switch (value)
	{
	case 4:														//通过键盘输入修改阶数
		scanf_s("%d", &k);
		if (VertexCount > 1)
			drawControl();
		if (VertexCount > k - 1)
			drawBspline();
		break;
	case 5:														//清除
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
	glutAddMenuEntry("degree", 4);
	glutAddMenuEntry("clear", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void onKeyboard(unsigned char key, int x, int y)					//通过键盘数字按键修改阶数
{
	if ('1' <= key && key <= '9')
		k = key - '0';
	if (VertexCount > 1)
		drawControl();
	if (VertexCount > k - 1)
		drawBspline();
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
	glutInitWindowPosition(200, 100);
	glutCreateWindow(argv[0]);
	glClearColor(1, 1, 1, 0);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	createMenus();
	glutKeyboardFunc(onKeyboard);
	glutMainLoop();
	return 0;
}

