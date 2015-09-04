#ifndef HEADER_SEEN
#define HEADER_SEEN

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>

typedef struct {
	float r, g, b, padd;
} pixel;

typedef struct {
	float x, y, z, padd;
} vector;

typedef struct{
	pixel color;
	vector center;
	float intensity;
} light;

typedef struct{
	pixel color;
	vector center;
	float r;
} sphere;

typedef struct{
	pixel color;
	vector v1,v2,v3;
} triangle;

typedef struct{
	vector origin;
	vector direction;
} ray;

typedef struct /*__attribute__((__packed__))*/{
	pixel* image;
	light* lights;
	sphere* spheres;
	triangle* triangles;
	int image_width;
	int image_height;
	int light_count;
	int sphere_count;
	int triangle_count;
	float focal_distance;
} scene;

#ifndef macros
#define print_pixel(p) printf("r = %f\tg = %f\tb = %f\n", p.r, p.g, p.b)
#define print_vector(v)	printf("x = %f\ty = %f\tz = %f\n", v.x, v.y, v.z)
#define min(a,b) (a < b)? a : b
#define max(a,b) (a > b)? a : b
#define abs(a) (a >= 0)? a : -a
#endif

int tracer_c(pixel* image, int image_width, int image_height,
	float focal_distance, 	light* lights, int light_count, sphere* spheres,
	int sphere_count, triangle* triangles, int triangle_count);
extern int tracer_asm(scene* s);

#endif /*HEADER_SEEN*/