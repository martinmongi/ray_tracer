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

	int image_width, image_height;
	float focal_distance;

	//Image matrix allocation
	if(fscanf(ifile, "%d %d", &image_width, &image_height) < 2){
		printf("Error reading image size: %dx%d", image_width, image_height);
		return 1;
	}

	pixel* image = malloc(sizeof(pixel)*image_height*image_width);
	//printf("Image allocated at start=%p end=%p\n", image[0], image[0]+sizeof(pixel)*image_height*image_width);

	unsigned int row, col;
	for(row = 0; row < image_height; row++){
		for(col = 0; col < image_width; col++){
			image[pos(row, col, image_width)].r = 0;
			image[pos(row, col, image_width)].g = 0;
			image[pos(row, col, image_width)].b = 0;
		}
	}

	if(fscanf(ifile, "%f", &focal_distance)){
		printf("Focal distance = %f.\n", focal_distance);
	}else{
		printf("Error reading focal_distance: %f\n", focal_distance);
		return 1;
	}

	//Lights reading
	int light_count;
	if(fscanf(ifile, "%d", &light_count)){
		printf("Found %d lights.\n", light_count);
	}else{
		printf("Error reading lights: %d\n", light_count);
		return 1;
	}

	unsigned int i,a; //a to ignore unused return value warning

	light* lights = malloc(sizeof(light)*light_count);

	for(i = 0; i < light_count; ++i){
		a = fscanf(ifile, "%f", &lights[i].intensity);
		printf("Light %d:\nintensity = %f\n", i, lights[i].intensity);
		a = fscanf(ifile, "%f %f %f", &lights[i].color.r, &lights[i].color.g, &lights[i].color.b);
		print_pixel(lights[i].color);
		a = fscanf(ifile, "%f %f %f", &lights[i].center.x, &lights[i].center.y, &lights[i].center.z);
		print_vector(lights[i].center);
	}

	//Spheres reading
	int sphere_count;
	if(fscanf(ifile, "%d", &sphere_count)){
		printf("Found %d spheres.\n", sphere_count);
	}else{
		printf("Error reading spheres: %d\n", sphere_count);
		return 1;
	}

	sphere* spheres = malloc(sizeof(sphere)*sphere_count);

	for(i = 0; i < sphere_count; ++i){
		a = fscanf(ifile, "%f", &spheres[i].r);
		printf("Sphere %d:\nradio = %f\n", i, spheres[i].r);
		a = fscanf(ifile, "%f %f %f", &spheres[i].color.r, &spheres[i].color.g, &spheres[i].color.b);
		print_pixel(spheres[i].color);
		a = fscanf(ifile, "%f %f %f", &spheres[i].center.x, &spheres[i].center.y, &spheres[i].center.z);
		print_vector(spheres[i].center);
	}

	if(a) a = 0; //ignore set but unused warning

	unsigned long long ini = rdtscl();
	
	//Proper ray tracing
	if(atoi(argv[2]) == 0){
		printf("C implementation\n");
		tracer_c(image, image_width, image_height, focal_distance,
			lights, light_count, spheres, sphere_count);
	}else{
		printf("SIMD implementation\n");
		tracer_asm(image, image_width, image_height, focal_distance,
			lights, light_count, spheres, sphere_count);
	}

	unsigned long long end = rdtscl();

	fprintf(stderr, "%llu\n", end-ini);

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
	free(lights);
	free(spheres);

	return 0;
}