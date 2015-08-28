#include "ray_tracer.h"

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
	if(fscanf(ifile, "%s", ofilename) && (ofile = fopen(ofilename, "w")))
		printf("Output filename: %s\n", ofilename);
	else{
		printf("Error opening output file");
		return 1;
	}

	int image_width, image_height, focal_distance;
	unsigned int i;

	//Image matrix allocation
	if(fscanf(ifile, "%d %d", &image_width, &image_height) < 2){
		printf("Error reading image size: %dx%d", image_width, image_height);
		return 1;
	}

	pixel** image = malloc(sizeof(pixel*)*image_height);
	image[0] = malloc(sizeof(pixel)*image_height*image_width);

	for(i = 0; i < image_height; i++){
		image[i] = image[0] + i*sizeof(pixel)*image_width;
	}






	//Freeing memory
	free(image[0]);
	free(image);

	return 0;
}