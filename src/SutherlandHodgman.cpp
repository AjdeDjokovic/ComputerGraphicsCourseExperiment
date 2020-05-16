#include <glut.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
const int MAX = 100;
int windowHeight, windowWidth;

struct Vertex
{
	float x, y;
};

typedef Vertex Edge[2];											//边
typedef Vertex VertexArray[MAX];								//顶点集合
VertexArray input, InVertexArray, OutVertexArray;
int inputlength = 0, Inlength = 0, Outlength = 0;
Edge ClipArray[4];												//裁切框四边

void pre()														//画出裁切框函数
{
	glClearColor(1, 1, 1, 0);									
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINE_LOOP);
	glColor3f(1, 0, 0);
	glVertex2f(windowWidth / 4, windowHeight / 4);
	glVertex2f(windowWidth * 3 / 4, windowHeight / 4);
	glVertex2f(windowWidth * 3 / 4, windowHeight * 3 / 4);
	glVertex2f(windowWidth / 4, windowHeight * 3 / 4);
	glEnd();
	glFlush();
}
void display() {}

void reshape(int w, int h)																	//对窗口大小变化的响应函数
{
	inputlength = 0, Inlength = 0, Outlength = 0;											//在初始化全局变量
	memset(input, 0, sizeof(input));
	memset(InVertexArray, 0, sizeof(InVertexArray));
	memset(OutVertexArray, 0, sizeof(OutVertexArray));

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);												
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	windowHeight = h;
	windowWidth = w;
	
																							//设置裁切框的边，存入ClipArray数组
	ClipArray[0][0].x = windowWidth / 4, ClipArray[0][0].y = windowHeight / 4;				//下边
	ClipArray[0][1].x = windowWidth * 3 / 4, ClipArray[0][1].y = windowHeight / 4;

	ClipArray[1][0].x = windowWidth / 4, ClipArray[1][0].y = windowHeight * 3 / 4;			//左边
	ClipArray[1][1].x = windowWidth / 4, ClipArray[1][1].y = windowHeight / 4;

	ClipArray[2][0].x = windowWidth * 3 / 4, ClipArray[2][0].y = windowHeight * 3 / 4;		//上边
	ClipArray[2][1].x = windowWidth / 4, ClipArray[2][1].y = windowHeight * 3 / 4;

	ClipArray[3][0].x = windowWidth * 3 / 4, ClipArray[3][0].y = windowHeight / 4;			//右边
	ClipArray[3][1].x = windowWidth * 3 / 4, ClipArray[3][1].y = windowHeight * 3 / 4;

	pre();
}

bool Inside(Vertex TestPt,Edge ClipBoundary)												//判断点是否在裁切边的可见侧
{
	if (ClipBoundary[1].x > ClipBoundary[0].x)												//下边
		if (TestPt.y >= ClipBoundary[0].y)
			return true;
		else return false;
	else if(ClipBoundary[1].x < ClipBoundary[0].x)											//上边
		if (TestPt.y <= ClipBoundary[0].y)
			return true;
		else return false;
	else if(ClipBoundary[1].y > ClipBoundary[0].y)											//右边
		if (TestPt.x <= ClipBoundary[0].x)
			return true;
		else return false;
	else if (ClipBoundary[1].y < ClipBoundary[0].y)											//左边
		if (TestPt.x >= ClipBoundary[0].x)
			return true;
		else return false;
}

void Intersect(Vertex S,Vertex P,Edge ClipBoundary,Vertex &IntersectPt)						//直线SP与窗口边界求交，返回交点IntersectPt
{
	if (ClipBoundary[0].y == ClipBoundary[1].y)												//水平裁剪边
	{
		IntersectPt.y = ClipBoundary[0].y;
		IntersectPt.x = S.x + (ClipBoundary[0].y - S.y) * (P.x - S.x) / (P.y - S.y);
	}
	else																					//垂直裁剪边
	{
		IntersectPt.x = ClipBoundary[0].x;
		IntersectPt.y = S.y + (ClipBoundary[0].x - S.x) * (P.y - S.y) / (P.x - S.x);
	}
}

void SutherlandHodgmanClip(Edge ClipBoundary)						//用裁切框各边依次进行裁切的函数
{
	Vertex S, P, ip;
	S = InVertexArray[Inlength - 1];								//(Inlength - 1 -> 0)  (0 -> 1)  (1 -> 2)  (2 -> 3) ......(Inlength - 2 -> Inlength - 1)
	for (int i = 0; i < Inlength; i++)
	{
		P = InVertexArray[i];
		if (Inside(P, ClipBoundary))
			if (Inside(S, ClipBoundary))
				OutVertexArray[Outlength++] = P;					//P S均在可见一侧，输出P
			else {											
				Intersect(S, P, ClipBoundary, ip);					//P在可见一侧，S在不可见一侧，求交点I，输出I和P
				OutVertexArray[Outlength++] = ip;
				OutVertexArray[Outlength++] = P;
			}
		else
			if (Inside(S, ClipBoundary))							//S在可见一侧，P在不可见一侧，求交点I，输出I
			{
				Intersect(S, P, ClipBoundary, ip);
				OutVertexArray[Outlength++] = ip;
			}
		S = P;											
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		input[inputlength].x = x;									//输入要裁切多边形的顶点集合
		input[inputlength++].y = windowHeight - y;
		if (inputlength > 1)										//如果顶点集合中的顶点多于1个，则对多边形进行显示
		{
			pre();
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 1, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			for (int i = 0; i < inputlength; i++)
				glVertex2i(input[i].x, input[i].y);
			glEnd();
			glFlush();
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < inputlength; i++)						//input数组复制到InVertexArray
			InVertexArray[i] = input[i];
		Inlength = inputlength;
		SutherlandHodgmanClip(ClipArray[0]);						//下边裁切

		for (int i = 0; i < Outlength; i++)
			InVertexArray[i] = OutVertexArray[i];
		Inlength = Outlength;
		Outlength = 0;
		memset(OutVertexArray, 0, sizeof(OutVertexArray));
		SutherlandHodgmanClip(ClipArray[1]);						//左边裁切

		for (int i = 0; i < Outlength; i++)
			InVertexArray[i] = OutVertexArray[i];
		Inlength = Outlength;
		Outlength = 0;
		memset(OutVertexArray, 0, sizeof(OutVertexArray));		
		SutherlandHodgmanClip(ClipArray[2]);						//上边裁切

		for (int i = 0; i < Outlength; i++)
			InVertexArray[i] = OutVertexArray[i];
		Inlength = Outlength;
		Outlength = 0;
		memset(OutVertexArray, 0, sizeof(OutVertexArray));
		SutherlandHodgmanClip(ClipArray[3]);						//右边裁切

		glBegin(GL_LINE_LOOP);										//根据边裁切输出的顶点，即为裁切后的多边形的顶点
		glColor3f(0, 0, 0);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPointSize(10);
		for(int i = 0;i < Outlength;i++)
			glVertex2i(OutVertexArray[i].x, OutVertexArray[i].y);	//画出裁切后的多边形
		glPointSize(1);
		glEnd();
		glFlush();

		inputlength = 0, Inlength = 0, Outlength = 0;				//初始化全局变量
		memset(input, 0, sizeof(input));
		memset(InVertexArray, 0, sizeof(InVertexArray));
		memset(OutVertexArray, 0, sizeof(OutVertexArray));
	}
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
