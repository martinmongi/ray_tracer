#include "ray_tracer.h"

#ifndef imagepos
#define pos(row, col, width) (row * width + col)
#endif

int tracer_c(pixel* image, int image_width, int image_height){

	unsigned int row, col;

	for(row = 0; row < image_height; ++row){
		for(col = 0; col < image_width; ++col){
			image[pos(row, col, image_width)].r = 0;
			image[pos(row, col, image_width)].g = 1;
			image[pos(row, col, image_width)].b = 0;
		}
	}
	return 0;
}