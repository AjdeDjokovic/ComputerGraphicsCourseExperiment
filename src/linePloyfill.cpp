#include <stdlib.h>
#include <cstdio>
#include <glut.h>
int windowHeight;
int windowWidth;
int countVertex = 0;
int lowLine = windowHeight, highLine = 0;

struct Point {
	int x;
	int y;
}vertexes[65536];

//AEL节点
struct ET {
	float x;															//扫描线与边的交点x坐标
	float dX;															//从当前扫描线到下一条扫面线间x的增量，dx = - b / a
	float dY;															//间接存放与该边相交的最高扫面线，dy = yMax与当前扫描线的差
	struct ET* next;
};

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		vertexes[countVertex].x = x;									//添加多边形顶点
		vertexes[countVertex++].y = windowHeight - y;
		if (countVertex > 1)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			for (int i = 0; i < countVertex; i++)
				glVertex2i(vertexes[i].x, vertexes[i].y);				//画出多边形
			glEnd();
			glFlush();
		}
		if (windowHeight - y > highLine) highLine = windowHeight - y;	//更新最高扫描线
		if (windowHeight - y < lowLine) lowLine = windowHeight - y;		//更新最低扫描线
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)				//右键开始填充
	{

		ET* etArray = new ET[windowHeight];								//申请NET表空间
		for (int i = 0; i < windowHeight; i++)							//初始化每一条扫面线的NET边表头结点
		{
			etArray[i].x = 0;
			etArray[i].dX = 0;
			etArray[i].dY = 0;
			etArray[i].next = NULL;
		}

		for (int i = 0; i < countVertex; i++)												//根据顶点集构造NET表
		{																					//查找邻居方向，先找左邻居，再找右邻居
			for (int flag = -1; flag <= 1; flag = flag + 2)
			{
				if (i == 0 && flag == -1)													//当前顶点号为0，要访问前一个邻居，为最后一个顶点
					flag = countVertex - 1;													
				if (i == countVertex - 1 && flag == 1)										//当前顶点号为最大，要访问后一个邻居，为0号顶点
					flag = 1 - countVertex;								
				if (vertexes[i].y < vertexes[i + flag].y)									//如果 y < 邻居的y，解决横线
				{
					ET* crtElNode = new ET();
					crtElNode->x = vertexes[i].x;											//相交顶点的x值即为当前节点的x值
					crtElNode->dY = vertexes[i + flag].y - vertexes[i].y;					//dY = 邻居顶点的y值-当前顶点的y值
					crtElNode->dX = (vertexes[i + flag].x - vertexes[i].x) / crtElNode->dY; //dX = - b / a
					ET* arrLine = &etArray[vertexes[i].y];									//找到y扫描线的net边表
					while (arrLine->next != NULL && arrLine->next->x < crtElNode->x)		//找到合适的插入位置，确保边表按x升序排列
						arrLine = arrLine->next;
					if (arrLine->next != NULL && arrLine->next->x == crtElNode->x && arrLine->next->dX < crtElNode->dX)
						arrLine = arrLine->next;											//找到合适的插入位置，当出现与当前节点y值相等的节点时，dx小的在前面
					crtElNode->next = arrLine->next;										//插入当前节点
					arrLine->next = crtElNode;
				}
				if (i == 0 && flag == countVertex - 1) flag = -1;							//恢复由于特殊情况（0找左邻居，最后顶点找右邻居）改变的flag值
				if (i == countVertex - 1 && flag == 1 - countVertex) flag = 1;
			}
		}

		ET* ael = new ET();																	//活性边表AET
		ael->dX = 0;																		//初始化活性边表AET的头结点
		ael->dY = 0;
		ael->x = 0;
		ael->next = NULL;
		ET* tempAel;																		//临时节点
		for (int crtLine = lowLine; crtLine <= highLine; crtLine++)				//从最低扫面线开始扫描
		{
			ET* tempElNode = &etArray[(int)crtLine];										//当前扫面线所对应NET头结点
			ET* crtElNode;																	//临时节点
			while (tempElNode->next != NULL)
			{
				crtElNode = tempElNode->next;												//从当前扫面线所对应NET中取出节点
				tempElNode->next = tempElNode->next->next;
				tempAel = ael;																//AET链表头结点
				while (tempAel->next != NULL && tempAel->next->x < crtElNode->x)			//找到合适的插入位置，确保AET按x升序排列
					tempAel = tempAel->next;
				if (tempAel->next != NULL && tempAel->next->x == crtElNode->x && tempAel->next->dX < crtElNode->dX)
					tempAel = tempAel->next;												//找到合适的插入位置，当出现与当前节点y值相等的节点时，dx小的在前面
				crtElNode->next = tempAel->next;											//插入节点
				tempAel->next = crtElNode;
			}
											
			glBegin(GL_LINES);
			glColor3f(0, 0, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			tempAel = ael;
			while (tempAel->next != NULL)													//进行顶点配对
			{
				tempAel = tempAel->next;
				glVertex2f(tempAel->x, crtLine);
			}
			glEnd();
			glFlush();

			tempAel = ael;
			while (tempAel->next != NULL)													//遍历AET，把dY = 0（yMAX = 当前扫面线）的节点从AET中删除，并把其余节点x值递增dX
			{
				tempAel->next->dY--;														//dY--
				if (tempAel->next->dY == 0)
				{
					ET* temp = tempAel->next;
					tempAel->next = tempAel->next->next;									//if dY == 0, 节点从AET中删除
					delete temp;
				}
				else {
					tempAel->next->x = tempAel->next->x + tempAel->next->dX;				//if dY != 0,节点x值递增dX
					tempAel = tempAel->next;
				}
			}
		}

		countVertex = 0;																	//初始化
		int lowLine = windowHeight, highLine = 0;
		delete [] etArray;
		delete ael;
	}
}

void display(void){}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	windowHeight = h;
	windowWidth = w;
	lowLine = windowHeight;
	highLine = 0;
	glClear(GL_COLOR_BUFFER_BIT);
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
