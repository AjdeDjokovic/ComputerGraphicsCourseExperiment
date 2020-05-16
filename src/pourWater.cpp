#include <iostream>
#include <cstdio>
#include <cstring>
#include <queue>
using namespace std;
const int maxN = 1005;
bool vis[maxN][maxN];	//防止重复访问数组
int A, B, C;			//两个杯子容量 目标水量
struct node
{
	int a, b, level;	//当前两个杯子中的水量 路径长度
	int path[maxN];		//记录每一步的选择，以便输出
};
string str[] = { "FILL(1)","FILL(2)","DROP(1)","DROP(2)","POUR(1,2)","POUR(2,1)" };		//6种操作：装满1、装满2、倒空1、倒空2、1向2倒水、2向1倒水（直到目标已满或自己倒空为止）
void out(node u)									//输出次数和路径
{
	printf("glass %d\n",u.a == C ? 1 : 2);
	printf("length %d\n", u.level);						//输出操作次数
	for (int i = 0; i < u.level; i++)				//输出每一步操作
		printf("%s\n", str[u.path[i]].c_str());
}
void bfs()
{
	memset(vis, 0, sizeof(vis));
	queue<node> q;
	node f;
	f.a = f.b = f.level = 0;				//从两杯都为空出发
	q.push(f);
	while (!q.empty())
	{
		node u = q.front(); q.pop();		//当前出队节点
		vis[u.a][u.b] = true;
		if (u.a == C || u.b == C)
		{
			out(u);							//达到目标水量并输出
			return;
		}
		node v = u;							//准备入队节点
		v.level++;							//子节点路径长度为父节点路径长度+1
		if (v.a < A)						//1杯没满
		{
			v.a = A;						//fill(1)
			if (!vis[v.a][v.b])
			{
				v.path[u.level] = 0;		//路径记录为0
				q.push(v);
			}
		}
		v = u;
		v.level++;
		if (v.b < B)						//2杯没满
		{
			v.b = B;						//fill(2)
			if (!vis[v.a][v.b])
			{
				v.path[u.level] = 1;		//路径记录为1
				q.push(v);
			}
		}
		v = u;
		v.level++;
		if (v.a)							//1杯非空
		{
			v.a = 0;						//DROP(1)
			if (!vis[v.a][v.b])
			{
				v.path[u.level] = 2;		//路径记录为2
				q.push(v);
			}
		}
		v = u;
		v.level++;
		if (v.b)							//2杯非空	
		{
			v.b = 0;
			if (!vis[v.a][v.b])				//DROP(2)
			{
				v.path[u.level] = 3;		//路径记录为3
				q.push(v);
			}
		}
		v = u;
		v.level++;
		if (v.a && v.b < B)					//1杯非空且2杯不满
		{
			int pour = min(v.a, B - v.b);	//防止溢出
			v.a -= pour, v.b += pour;		//POUR(1,2)
			if (!vis[v.a][v.b])
			{
				v.path[u.level] = 4;		//路径记录为4
				q.push(v);
			}
		}
		v = u;
		v.level++;
		if (v.b && v.a < A)					//2杯非空且1杯不满
		{
			int pour = min(v.b, A - v.a);	//防止溢出
			v.a += pour, v.b -= pour;		//POUR(2,1)
			if (!vis[v.a][v.b])
			{
				v.path[u.level] = 5;		//路径记录为5
				q.push(v);
			}
		}
	}
	printf("impossible\n");					//BFS结束，队列为节点，仍未到达目标节点，则输出不可能
}
int main()
{
	printf("input 2 capacity and 1 target\n");
	while (scanf_s("%d%d%d", &A, &B, &C) == 3)
	{
		bfs();
		printf("\n\ninput 2 capacity and 1 target\n");
	}
}

