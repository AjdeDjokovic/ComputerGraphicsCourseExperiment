#include <glut.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#define first 1
#define second 2
#define third 4
using namespace std;
int windowHeight, windowWidth;
bool flag = false;
double a1, b1, c1, a2, b2, c2, a3, b3, c3;								//三角形三边一般式的系数
double A, B, C;															//要裁切直线的一般式系数
const double eps = 1e-6 * (- 1);
struct Vertex
{
	double x, y;
};
Vertex Triangle[3];														//存放三角形
Vertex Edge[2];															//存放要裁切的边
int Tlength = 0,Elength = 0;
int encode(double x,double y)
{
	int c = 0;

	//根据三角形顶点获得三角形三边的一般式 f = ax + by + c = 0	
	a1 = Triangle[1].y - Triangle[0].y, b1 = Triangle[0].x - Triangle[1].x, c1 = Triangle[1].x * Triangle[0].y - Triangle[0].x * Triangle[1].y;
	a2 = Triangle[2].y - Triangle[1].y, b2 = Triangle[1].x - Triangle[2].x, c2 = Triangle[2].x * Triangle[1].y - Triangle[1].x * Triangle[2].y;
	a3 = Triangle[0].y - Triangle[2].y, b3 = Triangle[2].x - Triangle[0].x, c3 = Triangle[0].x * Triangle[2].y - Triangle[2].x * Triangle[0].y;

	double flag1 = Triangle[2].x * a1 + Triangle[2].y * b1 + c1;		//判断f > 0时为内侧还是f < 0 时为内侧
	double flag2 = Triangle[0].x * a2 + Triangle[0].y * b2 + c2;
	double flag3 = Triangle[1].x * a3 + Triangle[1].y * b3 + c3;

	double tag1 = x * a1 + y * b1 + c1;									//要裁切直线的两顶点代入三条直线一般式,获得f的符号
	double tag2 = x * a2 + y * b2 + c2;
	double tag3 = x * a3 + y * b3 + c3;

	if (flag1 * tag1 < eps)												//判断要裁切直线的两顶点与三角形三边的位置关系，同号（>0）为在内侧，异号（<0）在外侧
		c |= first;
	if (flag2 * tag2 < eps)
		c |= second;
	if (flag3 * tag3 < eps)
		c |= third;
	return c;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && Tlength < 3)
	{
		Triangle[Tlength].x = x;												//添加三角形顶点
		Triangle[Tlength++].y = windowHeight - y;
		if (Tlength > 1)														//如已输入一个以上顶点，则对显示三角形的边
		{
			glClearColor(1, 1, 1, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glBegin(GL_LINE_LOOP);
			glColor3f(1, 0, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			for (int i = 0; i < Tlength; i++)
				glVertex2i(Triangle[i].x, Triangle[i].y);
			glEnd();
			glFlush();
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && Elength < 2)	//输入被裁切直线
	{
		Edge[Elength].x = x;
		Edge[Elength++].y = windowHeight - y;
		if (Elength > 1)
		{
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 1, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			for (int i = 0; i < Elength; i++)
				glVertex2i(Edge[i].x, Edge[i].y);
			glEnd();
			glFlush();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		A = Edge[1].y - Edge[0].y;
		B = Edge[0].x - Edge[1].x;
		C = Edge[1].x * Edge[0].y - Edge[0].x * Edge[1].y;									//求被裁切直线的一般式
		int code,code1 = encode(Edge[0].x, Edge[0].y), code2 = encode(Edge[1].x, Edge[1].y);
		bool display = true;																//是否需要显示
		while (code1 != 0 || code2 != 0)													//判断被裁切直线的两个顶点是否完全在裁切框内，直到都在裁切框内时跳出循环
		{
			double xx = 0, yy = 0;															//x y 中间变量
			if ((code1 & code2) != 0)														//完全在裁切框之外，不需要显示
			{
				display = false;
				break;
			}
			if (code1 != 0)																	//选取被裁切直线两个顶点中不在裁切框内的顶点
				code = code1;
			else
				code = code2;
			if ((first & code) != 0)														//在第一条直线的外侧
			{
				xx = (b1 * C - B * c1) / (a1 * B - A * b1);									//裁切直线与第一条直线求交
				yy = (A * c1 - a1 * C) / (a1 * B - A * b1);
			}
			else if ((second & code) != 0)													//在第二条直线的外侧
			{
				xx = (b2 * C - B * c2) / (a2 * B - A * b2);									//裁切直线与第一条直线求交
				yy = (A * c2 - a2 * C) / (a2 * B - A * b2);
			}
			else if ((third & code) != 0)													//在第三条直线的外侧
			{
				xx = (b3 * C - B * c3) / (a3 * B - A * b3);									//裁切直线与第三条直线求交
				yy = (A * c3 - a3 * C) / (a3 * B - A * b3);
			}
			if (code == code1)																//如果刚才选取的是第一个顶点，则对第一个顶点坐标进行更新，同时更新code值
			{
				Edge[0].x = xx;
				Edge[0].y = yy;
				code1 = encode(xx, yy);
			}
			else																			//如果刚才选取的是第二个顶点，则对第二个顶点坐标进行更新，同时更新code值
			{
				Edge[1].x = xx;
				Edge[1].y = yy;
				code2 = encode(xx, yy);
			}
		}
		if (display)																		//如果被裁切直线有部分在裁切框内，则进行显示
		{
			glBegin(GL_LINES);
			glColor3f(0, 0, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			glVertex2i(Edge[0].x, Edge[0].y);
			glVertex2i(Edge[1].x, Edge[1].y);
			glEnd();
			glFlush();
		}
		Tlength = Elength = 0;																//清空全局变量
		memset(Triangle, 0, sizeof(Triangle));
		memset(Edge, 0, sizeof(Edge));
	}
}

void display() {}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	windowHeight = h;
	windowWidth = w;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glClearColor(1, 1, 1, 0);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
