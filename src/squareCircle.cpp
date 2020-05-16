#include <glut.h>  
#include <cmath>
#include <iostream>
using namespace std;
int input;
void circlepoints(GLint x, GLint y)			//八对称性画圆
{
	glPointSize(1);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(x, y);
	glVertex2f(y, x);
	glVertex2f(-y, x);
	glVertex2f(-x, y);
	glVertex2f(-x, -y);
	glVertex2f(-y, -x);
	glVertex2f(y, -x);
	glVertex2f(x, -y);
	glEnd();
}

void MidPointCircle(GLfloat r)				//中点画圆法画1/8圆弧，并根据八对称性求出整个圆
{
	GLint x, y;
	GLfloat d;
	x = 0; y = r; d = 1.25 - r;				//初始化点（0，r）,d=F（x + 1,y - 0.5）= 1.25 - r
	circlepoints(x, y);
	while (x < y)							//循环结束条件，只画第一象限的上1/8圆弧
	{
		if (d < 0)							//取正右方像素
			d += 2 * x + 3;					//d = F（x + 2, y - 0.5） = d + 2 * x + 3
		else
		{									//取右下方像素
			d += 2 * (x - y) + 5;			//d = F（x + 2, y - 1.5） = d +  2 * (x - y) + 5
			y--;
		}
		x++;
		circlepoints(x, y);					//八对称性画其余对称点
	}
}

void draw(GLfloat r)						//方形、圆形、菱形、圆形
{
	glBegin(GL_LINE_LOOP);					//画正方形
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(r, r);
	glVertex2f(-r, r);
	glVertex2f(-r, -r);
	glVertex2f(r, -r);
	glEnd();

	MidPointCircle(r);						//画圆

	glBegin(GL_LINE_LOOP);					//画菱形
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0, r);
	glVertex2f(-r, 0);
	glVertex2f(0, -r);
	glVertex2f(r, 0);
	glEnd();

	MidPointCircle(r / sqrt(2));			//画圆，半径为r / 根号2			
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	GLfloat r = input;
	for (int i = 0; i < 10; i++)			//设置循环次数
	{
		draw(r);
		r /= 2;								//每画一次方形、圆形、菱形、圆形,r = r / 2
	}
	glFlush();
}

int main(int argc, char* argv[])
{
	printf("input r\n");
	scanf_s("%d", &input);

	glutInit(&argc, argv);   
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(700, 700);
	glutCreateWindow("My first OpenGL program");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);			//设置窗口背景颜色为白色
	glMatrixMode(GL_PROJECTION);					//指定设置投影参数
	gluOrtho2D(-1000.0, 1000.0, -1000.0, 1000.0);	
	glViewport(0, 0, 700, 700);
	glutDisplayFunc(display);						
	glutMainLoop();							
	return 0;
}