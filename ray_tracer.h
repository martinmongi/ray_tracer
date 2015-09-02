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
	float x, y, z;
} vector;

typedef struct{
	float intensity;
	pixel color;
	vector center;
} light;

typedef struct{
	float r;
	pixel color;
	vector center;
} sphere;

typedef struct{
	vector origin;
	vector direction;
} ray;

#ifndef macros
#define print_pixel(p) printf("r = %f\tg = %f\tb = %f\n", p.r, p.g, p.b)
#define print_vector(v)	printf("x = %f\ty = %f\tz = %f\n", v.x, v.y, v.z)
#define min(a,b) (a < b)? a : b
#define max(a,b) (a > b)? a : b
#endif


int tracer_c(pixel* image, int image_width, int image_height, float focal_distance,
	light* lights, int light_count, sphere* spheres, int sphere_count);
extern int tracer_asm(pixel* image, int image_width, int image_height, float focal_distance,
	light* lights, int light_count, sphere* spheres, int sphere_count);

#endif /*HEADER_SEEN*/