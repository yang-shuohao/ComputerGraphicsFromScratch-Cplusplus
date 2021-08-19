#include <graphics.h>		// 引用图形库头文件
#include <conio.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<float.h>
#include<limits.h>
#include <vector>
#include <memory>
#include <iostream>
#include <tuple>

using namespace glm;


float viewWidth = 1;
float viewHeight = 1;

float cWidth = 640;
float cHeight = 640;

float d = 1;

struct sphere
{
	vec3 center;
	float radius;
	COLORREF color;
	float specular;
	float reflective = 0.2;
};

std::vector<sphere> spheres;

enum LightType
{
	AMBIENTLIGHT,
	POINTLIGHT,
	DIRECTIONALLIGHT
};

struct Light
{
	LightType lightType;
	float intensity;
	vec3 position;
	vec3 direction;
};

std::vector<Light> Lights;

vec3 ReflectRay(vec3 R, vec3 N)
{
	return 2 * dot(N, R) * N - R;
}

vec2 IntersectRaySphere(vec3 origin, vec3 D, sphere s)
{
	float r = s.radius;
	vec3 CO = origin - s.center;

	float a = dot(D, D);
	float b = 2 * dot(CO, D);
	float c = dot(CO, CO) - r * r;

	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
	{
		return vec2(DBL_MAX, DBL_MAX);
	}
	float t1 = (-b + sqrt(discriminant)) / (2 * a);
	float t2 = (-b - sqrt(discriminant)) / (2 * a);
	return vec2(t1, t2);
}

std::tuple<sphere, float> ClosetIntersection(vec3 origin, vec3 D, float t_min, float t_max)
{
	float closest_t = DBL_MAX;
	sphere closest_sphere;
	closest_sphere.radius = -1;
	for (int i = 0; i < spheres.size(); i++)
	{
		vec2 t = IntersectRaySphere(origin, D, spheres[i]);
		if (t.x <= t_max && t.x >= t_min && t.x < closest_t)
		{
			closest_t = t.x;
			closest_sphere = spheres[i];
		}

		if (t.y <= t_max && t.y >= t_min && t.y < closest_t)
		{
			closest_t = t.y;
			closest_sphere = spheres[i];
		}
	}
	return std::make_tuple(closest_sphere, closest_t);
}
//计算光照
float ComputeLighting(vec3 P, vec3 N, vec3 V, float s)
{
	float i = 0;
	vec3 L;
	float t_max;
	for (int index = 0;index < Lights.size();index++)
	{
		if (Lights[index].lightType == LightType::AMBIENTLIGHT)
		{
			i += Lights[index].intensity;
		}
		else
		{
			if (Lights[index].lightType == LightType::POINTLIGHT)
			{
				L = Lights[index].position - P;
				t_max = 1;
			}
			else
			{
				L = Lights[index].direction;
				t_max = DBL_MAX;
			}
			//Shadow check
			sphere shadow_sphere;
			float shadow_t;
			std::tie(shadow_sphere, shadow_t) = ClosetIntersection(P, L, 0.001, t_max);
			if (shadow_sphere.radius > 0)
			{
				continue;
			}
			//Diffuse
			float n_dot_l = dot(N, L);
			if (n_dot_l > 0)
			{
				i += Lights[index].intensity * n_dot_l / (length(N) * length(L));
			}
			//Specular
			if (s != -1)
			{
				vec3 R = 2 * dot(N, L) * N - L;
				float r_dot_v = dot(R, V);
				if (r_dot_v > 0)
				{
					i += Lights[index].intensity * pow(r_dot_v / (length(R) * length(V)), s);
				}
			}
		}

	}
	return i;
}

vec3 CanvasToViewport(float x, float y)
{
	return vec3(x * viewWidth / cWidth, y * viewHeight / cHeight, d);
}

COLORREF TraceRay(vec3 Origin, vec3 D, float t_min, float t_max,float recursion_depth)
{
	float closest_t;
	sphere closest_sphere;
	std::tie(closest_sphere, closest_t) = ClosetIntersection(Origin, D, t_min, t_max);
	if (closest_sphere.radius <= 0)
	{
		return WHITE;
	}

	vec3 P = Origin + closest_t * D;
	vec3 N = P - closest_sphere.center;
	N = normalize(N);
	COLORREF local_color= closest_sphere.color * ComputeLighting(P, N, -D, closest_sphere.specular);
	float r = closest_sphere.reflective;
	if (recursion_depth <= 0 || r <= 0)
	{
		return local_color;
	}
	vec3 R = ReflectRay(-D, N);
	COLORREF reflected_color = TraceRay(P, R, 0.001, DBL_MAX, recursion_depth - 1);
	return local_color * (1 - r) + reflected_color * r;
}

int main()
{
	initgraph(640, 640);	// 创建绘图窗口，大小为 640x480 像素

	vec3 Origin = { 0,0,0 };

	spheres.resize(4);
	spheres[0].center = { 0,-1,3 };
	spheres[0].radius = 1;
	spheres[0].color = RED;
	spheres[0].specular = 500;
	spheres[0].reflective = 0.2;

	spheres[1].center = { -2,1,3 };
	spheres[1].radius = 1;
	spheres[1].color = BLUE;
	spheres[1].specular = 500;
	spheres[1].reflective = 0.3;


	spheres[2].center = { 2,1,3 };
	spheres[2].radius = 1;
	spheres[2].color = GREEN;
	spheres[2].specular = 10;
	spheres[2].reflective = 0.4;

	spheres[3].center = { 0,-5001,0 };
	spheres[3].radius = 5000;
	spheres[3].color = YELLOW;
	spheres[3].specular = 1000;
	spheres[3].reflective = 0.5;

	Lights.resize(3);
	Lights[0].lightType = LightType::AMBIENTLIGHT;
	Lights[0].intensity = 0.2;
	Lights[1].lightType = LightType::POINTLIGHT;
	Lights[1].intensity = 0.6;
	Lights[1].position = { 2,1,0 };
	Lights[2].lightType = LightType::DIRECTIONALLIGHT;
	Lights[2].intensity = 0.2;
	Lights[2].direction = { 1,4,4 };

	float recursion_depth = 3;

	for (int x = -cWidth / 2; x < cWidth / 2; x++)
	{
		for (int y = -cHeight / 2; y < cHeight / 2; y++)
		{
			vec3 D = CanvasToViewport(x, y);
			COLORREF color = TraceRay(Origin, D, 1, DBL_MAX,recursion_depth);
			putpixel(x + cWidth / 2, -y + cHeight / 2, color);
		}
	}

	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
}