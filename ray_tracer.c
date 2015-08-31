#include "ray_tracer.h"

#ifndef imagepos
#define pos(row, col, width) (row * width + col)
#endif

unsigned long long rdtscl(void)
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));                        
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );  
}

int main(int argc, char const *argv[]){

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

	int image_width, image_height/*, focal_distance*/;

	//Image matrix allocation
	if(fscanf(ifile, "%d %d", &image_width, &image_height) < 2){
		printf("Error reading image size: %dx%d", image_width, image_height);
		return 1;
	}

	pixel* image = malloc(sizeof(pixel)*image_height*image_width);
	//printf("Image allocated at start=%p end=%p\n", image[0], image[0]+sizeof(pixel)*image_height*image_width);

	unsigned long long ini = rdtscl();
	
	//Proper ray tracing
	if(atoi(argv[2]) == 0){
		printf("C implementation\n");
		tracer_c(image, image_width, image_height);
	}else{
		printf("SIMD implementation\n");
		tracer_asm(image, image_width, image_height);
	}

	unsigned long long end = rdtscl();

	printf("Took %llu cycles\n", end-ini);
	//Outputting image
	ofile = fopen(ofilename, "w");
	
	fprintf(ofile,"P3\n");
	fprintf(ofile,"%d %d\n", image_width, image_height);
	fprintf(ofile,"255\n");

	unsigned int row, col;
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