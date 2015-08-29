#include "ray_tracer.h"

#ifndef SYMBOL
#define pos(row, col, width) (row * width + col)
#endif

int main(int argc, char const *argv[])
{
	if(argc < 3){
		printf("Usage: %s input_file.in I\n", argv[0]);
		printf("I =\t0 for C implementation\n");
		printf("\t1 for SIMD implementation\n");
		return 1;
	}

	//Input parsing
	FILE *ofile, *ifile = fopen(argv[1], "r");

	//Output file opening
	char ofilename[255];
	if(fscanf(ifile, "%s", ofilename))
		printf("Output filename: %s\n", ofilename);
	else{
		printf("Error opening output file");
		return 1;
	}

	int image_width, image_height, focal_distance;
	unsigned int row, col;

	//Image matrix allocation
	if(fscanf(ifile, "%d %d", &image_width, &image_height) < 2){
		printf("Error reading image size: %dx%d", image_width, image_height);
		return 1;
	}

	pixel* image = malloc(sizeof(pixel)*image_height*image_width);
	//printf("Image allocated at start=%p end=%p\n", image[0], image[0]+sizeof(pixel)*image_height*image_width);

	//Proper ray tracing
	for (row = 0; row < image_height; ++row) {
		for (col = 0; col < image_width; ++col) {
			image[pos(row, col, image_width)].r = 0;
			image[pos(row, col, image_width)].g = 1;
			image[pos(row, col, image_width)].b = 0;
		}
	}

	//Outputting image
	ofile = fopen(ofilename, "w");
	
	fprintf(ofile,"P3\n");
	fprintf(ofile,"%d %d\n", image_width, image_height);
	fprintf(ofile,"255\n");

	for(row = 0; row < image_height; row++){
		for(col = 0; col < image_width; col++){
			fprintf(ofile, "%d ", (int)(image[pos(row, col, image_width)].r*255));
			fprintf(ofile, "%d ", (int)(image[pos(row, col, image_width)].g*255));
			fprintf(ofile, "%d ", (int)(image[pos(row, col, image_width)].b*255));
		}
		fprintf(ofile,"\n");
	}

	//Freeing memory and closing pipes
	fclose(ifile);
	fclose(ofile);
	free(image);

	return 0;
}