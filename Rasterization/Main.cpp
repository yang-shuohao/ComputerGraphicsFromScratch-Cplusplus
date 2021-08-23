#include <graphics.h>		// 引用图形库头文件
#include <conio.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

using namespace glm;
using namespace std;

float Cw = 200;
float Ch = 200;
float Vw = 1;
float Vh = 1;
float d = 1;


void DrawLine1(vec2 P0, vec2 P1, COLORREF color)
{
	float a = (P1.y - P0.y) / (P1.x - P0.x);
	float b = P0.y - a * P0.x;
	for (int x = P0.x;x <= P1.x;x++)
	{
		float y = a * x + b;
		putpixel(x, y, color);
	}

}

void DrawLine2(vec2 P0, vec2 P1, COLORREF color)
{
	if (P1.x < P0.x)
	{
		swap(P0, P1);
	}
	float a = (P1.y - P0.y) / (P1.x - P0.x);
	float y = P0.y;
	for (int x = P0.x;x <= P1.x;x++)
	{
		putpixel(x, y, color);
		y = y + a;
	}

}

void DrawLine3(vec2 P0, vec2 P1, COLORREF color)
{
	if (P1.y < P0.y)
	{
		swap(P0, P1);
	}
	float a = (P1.x - P0.x) / (P1.y - P0.y);
	float x = P0.x;
	for (int y = P0.y;y <= P1.y;y++)
	{
		putpixel(x, y, color);
		x = x + a;
	}

}

void DrawLine4(vec2 P0, vec2 P1, COLORREF color)
{
	float dx = P1.x - P0.x;
	float dy = P1.y - P0.y;
	if (abs(dx) > abs(dy))
	{
		if (P0.x > P1.x)
		{
			swap(P0, P1);
		}
		float a = dy / dx;
		float y = P0.y;
		for (int x = P0.x;x <= P1.x;x++)
		{
			putpixel(x, y, color);
			y = y + a;
		}
	}
	else
	{
		if (P0.y > P1.y)
		{
			swap(P0, P1);
		}
		float a = dx/dy;
		float x = P0.x;
		for (int y = P0.y;y <= P1.y;y++)
		{
			putpixel(x, y, color);
			x = x + a;
		}
	}

}

vector<float> Interpolate(float i0, float d0, float i1, float d1)
{
	vector<float> values;
	if (abs(i0 - i1)< 1e-6)
	{
		values.push_back(d0);
		return values;
	}
	float a = (d1 - d0) / (i1 - i0);
	float d = d0;
	for (int i=i0;i<i1;i++)
	{
		values.push_back(d);
		d += a;
	}
	return values;
}

void DrawLine5(vec2 P0, vec2 P1, COLORREF color)
{
	if (abs(P1.x-P0.x) > abs(P1.y-P0.y))
	{
		if (P0.x > P1.x)
		{
			swap(P0, P1);
		}
		vector<float> ys = Interpolate(P0.x,P0.y,P1.x,P1.y);
		for (int x = P0.x;x < P1.x;x++)
		{
			putpixel(x, ys[x-P0.x], color);
		}
	}
	else
	{
		if (P0.y > P1.y)
		{
			swap(P0, P1);
		}
		vector<float> xs = Interpolate(P0.y, P0.x, P1.y, P1.x);
		for (int y = P0.y;y < P1.y;y++)
		{
			putpixel(xs[y-P0.y], y, color);
		}
	}
}

void DrawWireframeTriangle(vec2 P0, vec2 P1, vec2 P2, COLORREF color)
{
	DrawLine5(P0, P1, color);
	DrawLine5(P1, P2, color);
	DrawLine5(P2, P0, color);
}

void DrawFilledTriangle(vec2 P0, vec2 P1, vec2 P2, COLORREF color)
{
	//P0.y<=P1.y<=P2.y
	if (P1.y < P0.y)
	{
		swap(P1, P0);
	}
	if (P2.y < P0.y)
	{
		swap(P2, P0);
	}
	if (P2.y < P1.y)
	{
		swap(P2, P1);
	}

	vector<float> x01 = Interpolate(P0.y, P0.x, P1.y, P1.x);
	vector<float> x12 = Interpolate(P1.y, P1.x, P2.y, P2.x);
	vector<float> x02 = Interpolate(P0.y, P0.x, P2.y, P2.x);

	x01.pop_back();
	x01.insert(x01.end(),x12.begin(),x12.end());
	vector<float> x012(x01);

	float m = floor(x012.size() / 2);
	vector<float> x_left;
	vector<float> x_right;
	if (x02[m] < x012[m])
	{
		x_left = x02;
		x_right = x012;
	}
	else
	{
		x_left = x012;
		x_right = x02;
	}
	for (int y=P0.y;y<P2.y;y++)
	{
		for (int x = x_left[y - P0.y];x < x_right[y - P0.y];x++)
		{
			putpixel(x, y, color);
		}
	}
}

void DrawShadedTriangle(vec2 P0,vec2 P1,vec2 P2,COLORREF color)
{
	//P0.y<=P1.y<=P2.y
	if (P1.y < P0.y)
	{
		swap(P1, P0);
	}
	if (P2.y < P0.y)
	{
		swap(P2, P0);
	}
	if (P2.y < P1.y)
	{
		swap(P2, P1);
	}
	float h0 = 0;
	float h1 = 0.5;
	float h2 = 1;

	vector<float> x01 = Interpolate(P0.y, P0.x, P1.y, P1.x);
	vector<float> h01 = Interpolate(P0.y, h0, P1.y, h1);

	vector<float> x12 = Interpolate(P1.y, P1.x, P2.y, P2.x);
	vector<float> h12 = Interpolate(P1.y, h1, P2.y, h2);

	vector<float> x02 = Interpolate(P0.y, P0.x, P2.y, P2.x);
	vector<float> h02 = Interpolate(P0.y, h0, P2.y, h2);

	x01.pop_back();
	x01.insert(x01.end(), x12.begin(), x12.end());
	vector<float> x012(x01);

	h01.pop_back();
	h01.insert(h01.end(), h12.begin(), h12.end());
	vector<float> h012(h01);

	vector<float> x_left;
	vector<float> x_right;

	vector<float> h_left;
	vector<float> h_right;

	float m = floor(x012.size() / 2);
	if (x02[m] < x012[m])
	{
		x_left = x02;
		h_left = h02;

		x_right = x012;
		h_right = h012;
	}
	else
	{
		x_left = x012;
		h_left = h012;

		x_right = x02;
		h_right = h02;
	}
	float x_l;
	float x_r;
	vector<float> h_segment;
	COLORREF shaded_color;
	for (int y=P0.y;y<P2.y;y++)
	{
		x_l = x_left[y - P0.y];
		x_r = x_right[y - P0.y];

		h_segment = Interpolate(x_l, h_left[y - P0.y], x_r, h_right[y - P0.y]);
		for (int x = x_l;x < x_r;x++)
		{
			shaded_color = color * h_segment[x - x_l];
			putpixel(x, y, shaded_color);
		}
	}
}

vec2 ViewportToCanvas(float x, float y)
{
	return vec2(x * Cw / Vw+Cw, y * Ch / Vh+Ch);
}


vec2 ProjectVertex(vec3 v)
{
	mat4 perspectivemat = perspective<float>(radians(90.0f), Cw / Ch, 1.0f, 1000.0f);
	vec4 tmp = perspectivemat * vec4(v,1);
	//return ViewportToCanvas(v.x * d / v.z, v.y * d / v.z);
	return ViewportToCanvas(tmp.x/tmp.w, tmp.y / tmp.w);

}

struct Triangle
{
	vec3 index;

	COLORREF color;
};

struct Model
{
	string name;
	vector<vec3> vertices;
	vector<Triangle> triangles;
};

struct Transform
{
	mat4 srt;
};
struct Instance
{
	Model model;
	Transform transform;
};

struct Scene
{
	vector<Instance> instances;
};

void RenderTriangle(Triangle triangle,vector<vec2> projected)
{
	DrawFilledTriangle(projected[triangle.index.x],
						  projected[triangle.index.y],
						  projected[triangle.index.z],
						  triangle.color);
}

void RenderObject(vector<vec3> vertices, vector<Triangle> triangles)
{
	vector<vec2> projected;
	for (int i = 0;i < vertices.size();i++)
	{
		projected.push_back(ProjectVertex(vertices[i]));
	}
	for (int i = 0;i < triangles.size();i++)
	{
		RenderTriangle(triangles[i], projected);
	}
}

vec4 ApplyTransform(vec3 vertex, Transform transform)
{
	mat4 viewMat = lookAtRH(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
	return  viewMat*transform.srt * vec4(vertex.x,vertex.y,vertex.z,1);
}

void RenderInstance(Instance instance)
{
	vector<vec2> projected;
	Model model=instance.model;
	Transform transform = instance.transform;
	for (int i=0;i<model.vertices.size();i++)
	{
		projected.push_back(ProjectVertex(ApplyTransform(model.vertices[i], transform)));
	}
	for (int i=0;i<model.triangles.size();i++)
	{
		RenderTriangle(model.triangles[i], projected);
	}
}

void RenderScene(Scene s)
{
	for (int i=0;i<s.instances.size();i++)
	{
		RenderInstance(s.instances[i]);
	}
}

int main()
{
	initgraph(640, 640);	// 创建绘图窗口，大小为 640x480 像素
	
	vec2 P0(100, 100);
	vec2 P1(500, 100);
	vec2 P2(300, 200);

// 	vec3 vAf(-1, 1, 1);
// 	vec3 vBf( 1, 1, 1);
// 	vec3 vCf(1, -1, 1);
// 	vec3 vDf(-1,-1, 1);
// 
// 	vec3 vAb(-1, 1, 2);
// 	vec3 vBb(1, 1, 2);
// 	vec3 vCb(1, -1, 2);
// 	vec3 vDb(-1, -1, 2);
// 	//front
// 	DrawLine5(ProjectVertex(vAf), ProjectVertex(vBf), BLUE);
// 	DrawLine5(ProjectVertex(vBf), ProjectVertex(vCf), BLUE);
// 	DrawLine5(ProjectVertex(vCf), ProjectVertex(vDf), BLUE);
// 	DrawLine5(ProjectVertex(vDf), ProjectVertex(vAf), BLUE);
// 	//back
// 	DrawLine5(ProjectVertex(vAb), ProjectVertex(vBb), RED);
// 	DrawLine5(ProjectVertex(vBb), ProjectVertex(vCb), RED);
// 	DrawLine5(ProjectVertex(vCb), ProjectVertex(vDb), RED);
// 	DrawLine5(ProjectVertex(vDb), ProjectVertex(vAb), RED);
// 	//The front-to-back edges
// 	DrawLine5(ProjectVertex(vAf), ProjectVertex(vAb), GREEN);
// 	DrawLine5(ProjectVertex(vBf), ProjectVertex(vBb), GREEN);
// 	DrawLine5(ProjectVertex(vCf), ProjectVertex(vCb), GREEN);
// 	DrawLine5(ProjectVertex(vDf), ProjectVertex(vDb), GREEN);

	vec3 T = { -1.5,0,10 };

	vector<vec3> Vertices;
	Vertices.resize(8);
	Vertices[0] = { 1,1,1 };
	Vertices[1] = { -1,1,1 };
	Vertices[2] = { -1,-1,1 };
	Vertices[3] = { 1,-1,1 };
	Vertices[4] = { 1,1,-1 };
	Vertices[5] = { -1,1,-1 };
	Vertices[6] = { -1,-1,-1 };
	Vertices[7] = { 1,-1,-1 };

	for (int i = 0;i < 8;i++)
	{
		Vertices[i] += T;
	}

	vector<Triangle> triangles;
	triangles.resize(12);
	triangles[0].index = { 0,1,2 };
	triangles[0].color = RED;
	triangles[1].index = { 0,2,3 };
	triangles[1].color = RED;
	triangles[2].index = { 4,0,3 };
	triangles[2].color = GREEN;
	triangles[3].index = { 4,3,7 };
	triangles[3].color = GREEN;
	triangles[4].index = { 5,4,7 };
	triangles[4].color = BLUE;
	triangles[5].index = { 5,7,6 };
	triangles[5].color = BLUE;
	triangles[6].index = { 1,5,6 };
	triangles[6].color = YELLOW;
	triangles[7].index = { 1,6,2 };
	triangles[7].color = YELLOW;
	triangles[8].index = { 4,5,1 };
	triangles[8].color = WHITE;
	triangles[9].index = { 4,1,0 };
	triangles[9].color = WHITE;
	triangles[10].index = { 2,6,7 };
	triangles[10].color = CYAN;
	triangles[11].index = { 2,7,3 };
	triangles[11].color = CYAN;

	//RenderObject(Vertices, triangles);
	Scene s;
	s.instances.resize(2);
	s.instances[0].model.name = "1";
	s.instances[0].model.vertices = Vertices;
	s.instances[0].model.triangles = triangles;
	mat4 srt = mat4(1.0);
	srt = scale(srt, vec3(1, 1, 1));
	srt = rotate(srt, radians(-45.0f), vec3(0, 0, 1));
	srt = translate(srt, vec3(5, 0, 0));
	s.instances[0].transform.srt = srt;

	s.instances[1].model.name = "2";
	s.instances[1].model.vertices = Vertices;
	s.instances[1].model.triangles = triangles;
	mat4 srt2 = mat4(1.0);
	srt2 = scale(srt2, vec3(1, 1, 1));
	srt2 = rotate(srt2, radians(45.0f), vec3(0, 0, 1));
	srt2 = translate(srt2, vec3(-5, 0, 0));
	s.instances[1].transform.srt = srt2;

	//mat4 projection = perspective(radians(90), Cw / Ch, 0.1f, 100.0f);
	ExMessage m;		// 定义消息变量

	while (true)
	{
		RenderScene(s);
		// 获取一条鼠标或按键消息
		m = getmessage(EM_MOUSE | EM_KEY);

		switch (m.message)
		{
		case WM_MOUSEMOVE:
			// 鼠标移动的时候
			break;

		case WM_LBUTTONDOWN:
			// 如果点左键的同时按下了 Ctrl 键
			break;

		case WM_KEYDOWN:
			if (m.vkcode == VK_ESCAPE)
				return 0;	// 按 ESC 键退出程序
		}

	}


	//_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
}