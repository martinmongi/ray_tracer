#include "ray_tracer.h"

#ifndef imagepos
#define pos(row, col, width) (row * width + col)
#endif

float vector_dot_product(const vector a, const vector b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector vector_sum(const vector a, const vector b){
	vector r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	return r;
}

vector vector_sub(const vector a, const vector b){
	vector r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	return r;
}

vector vector_scale(const float k, const vector a){
	vector r;
	r.x = a.x * k;
	r.y = a.y * k;
	r.z = a.z * k;
	return r;
}

float vector_2norm(const vector a){
	return pow(vector_dot_product(a,a),0.5);
}

vector vector_normalize(const vector a){
	return vector_scale(1/vector_2norm(a), a);
}

int ray_sphere_intersection(ray r, sphere s, vector* intersection){
	
	vector l = vector_sub(r.origin,s.center);
	float c = vector_dot_product(l,l) - s.r*s.r;
	float b = 2 * vector_dot_product(r.direction, l);
	float a = vector_dot_product(r.direction, r.direction);
	
	float root_base = b*b - 4*a*c;

	if (root_base < 0) return 0;
	
	float root1 = (- b - pow(root_base, 0.5))/(2*a);
	float root2 = (- b + pow(root_base, 0.5))/(2*a);
	//root2 >= root1

	if(root2 < 0 || root1 < 0)
		return 0;
	//printf("%f %f %f %f %f %f %f\n", unit_direction.x, unit_direction.y, unit_direction.z, a,b,c, root);
	
	float root = min(root1, root2);
	*intersection = vector_sum(r.origin, vector_scale(root, r.direction));

	//print_vector(*intersection);
	return 1;
	
}

int tracer_c(pixel* image, int image_width, int image_height, float focal_distance, light* lights, int light_count, sphere* spheres, int sphere_count){

	float relation = (float)image_width/(float)image_height;
	float fd_sq = focal_distance*focal_distance;
	float r_sq = relation*relation;
	float window_height = pow(fd_sq/(r_sq + 1), 0.5);
	float window_width = relation*window_height;
	float step = window_width/(float)image_width;

	ray tracer; //ba dum tss
	tracer.origin.x = 0;
	tracer.origin.y = 0;
	tracer.origin.z = 0;

	unsigned int row, col;
	int sphere_i;

	for(row = 0; row < image_height; row++){
		for(col = 0; col < image_width; col++){
			float nearest_object_distance = FLT_MAX;
			float distance;

			tracer.direction.x = step*((float)col + 0.5) - window_width/2;
			tracer.direction.y = -step*((float)row + 0.5) + window_height/2;
			tracer.direction.z = focal_distance;

			vector intersection;

			for(sphere_i = 0; sphere_i < sphere_count; sphere_i++){

				if(ray_sphere_intersection(tracer, spheres[0], &intersection)){		

					distance = vector_2norm(vector_sub(tracer.origin, intersection));
					//printf("row = %d\tcol = %d\tdistance = %f\n", row, col, distance);
					if(distance < nearest_object_distance){

						
						nearest_object_distance = distance;

						vector intersection_to_light = vector_sub(lights[0].center, intersection);
						vector normal = vector_sub(intersection, spheres[0].center);
						
						float coef = vector_dot_product(intersection_to_light, normal)
							/vector_2norm(intersection_to_light)/vector_2norm(normal);

						if(coef > 0){
							image[pos(row, col, image_width)].r = spheres[0].color.r * coef * lights[0].intensity * lights[0].color.r;
							image[pos(row, col, image_width)].g = spheres[0].color.g * coef * lights[0].intensity * lights[0].color.g;
							image[pos(row, col, image_width)].b = spheres[0].color.b * coef * lights[0].intensity * lights[0].color.b;
						}
					}
				}
			}
		}
	}
	

	return 0;
}