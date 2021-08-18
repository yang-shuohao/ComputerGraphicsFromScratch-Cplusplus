#include <graphics.h>		// 引用图形库头文件
#include <conio.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include<float.h>
#include<limits.h>
#include <vector>
#include <memory>

using namespace glm;


float viewWidth = 1;
float viewHeight = 1;

float cWidth = 640;
float cHeight = 640;

float d=1;

struct sphere
{
	vec3 center;
	float radius;
	COLORREF color;
};

std::vector<sphere> spheres;

vec3 CanvasToViewport(float x, float y)
{
	return vec3(x* viewWidth / cWidth,y* viewHeight / cHeight,d);
}

vec2 IntersectRaySphere(vec3 origin, vec3 D, sphere s)
{
	float r=s.radius;
	vec3 CO=origin-s.center;

	float a=dot(D,D);
	float b=2*dot(CO,D);
	float c=dot(CO,CO)-r*r;

	float discriminant=b*b-4*a*c;
	if (discriminant < 0)
	{
		return vec2(DBL_MAX, DBL_MAX);
	}
	float t1=(-b + sqrt(discriminant)) / (2 * a);
	float t2 = (-b - sqrt(discriminant)) / (2 * a);
	return vec2(t1, t2);
}

COLORREF TraceRay(vec3 Origin, vec3 D, float min, float max,std::vector<sphere> spheres)
{
	float closest_t=DBL_MAX;
	sphere closet_sphere;
	closet_sphere.radius=-1;
	for (int i=0;i<3;i++)
	{
		vec2 t=IntersectRaySphere(Origin,D,spheres[i]);
		if (t.x <= max && t.x >= min && t.x < closest_t)
		{
			closest_t=t.x;
			closet_sphere=spheres[i];
		}

		if (t.y <= max && t.y >= min && t.y < closest_t)
		{
			closest_t = t.y;
			closet_sphere = spheres[i];
		}
	}

	if (closet_sphere.radius <=0)
	{
		return YELLOW;
	}
	return closet_sphere.color;
}

int main()
{
	
	initgraph(640, 640);	// 创建绘图窗口，大小为 640x480 像素

	vec3 Origin={0,0,0};

	spheres.resize(3);
	spheres[0].center = { 0.0,-1.0,3.0 };
	spheres[0].radius = 1;
	spheres[0].color = RED;
	spheres[1].center = { 2,0,4 };
	spheres[1].radius = 1;
	spheres[1].color = BLUE;
	spheres[2].center = { -2,0,4 };
	spheres[2].radius = 1;
	spheres[2].color = GREEN;

	for (int x = -cWidth/2; x < cWidth/2; x++)
	{
		for (int y=-cHeight/2; y < cHeight/2; y++)
		{
			vec3 D=CanvasToViewport(x,y);
			COLORREF color = TraceRay(Origin, D, 1, DBL_MAX,spheres);
			putpixel(x+cWidth/2,-y+cHeight/2,color);
		}
	}

	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
}