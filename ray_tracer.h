#include <stdio.h>
#include <stdlib.h>

typedef struct {
	float r, g, b, padd;
} pixel;

int tracer_c(pixel* image, int image_width, int image_height);
extern int tracer_asm(pixel* image, int image_width, int image_height);