#include "ray_tracer.h"

#ifndef imagepos
#define pos(row, col, width) (row * width + col)
#endif

float vector_dot_product(const vector a, const vector b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector vector_cross_product(const vector a, const vector b){
	vector r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;
	return r;
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
	// printf("a = %f\tb = %f\tc = %f\trb = %f\n", a,b,c,root_base);

	if (root_base < 0) return 0;
	
	float root1 = (- b - pow(root_base, 0.5))/(2*a);
	float root2 = (- b + pow(root_base, 0.5))/(2*a);
	//printf("roo1 = %f\troot2 = %f\n", root1, root2);
	//root2 >= root1

	if(root2 < 0 || root1 < 0)
		return 0;
	//printf("%f %f %f %f %f %f %f\n", unit_direction.x, unit_direction.y, unit_direction.z, a,b,c, root);
	
	float root = min(root1, root2);
	*intersection = vector_sum(r.origin, vector_scale(root, r.direction));

	//print_vector(*intersection);
	return 1;
	
}

int same_side_semiplane(vector p1, vector p2, vector a, vector b){
	
	
	//Returns true if p1 and p2 are on the same side of the plane divided by the line that goes through a and b
	// printf("input\n");
	// print_vector(p1);
	// print_vector(p2);
	// print_vector(a);
	// print_vector(b);
	// printf("subs\n");
	vector ba = vector_sub(b,a);
	vector p1a = vector_sub(p1,a);
	vector p2a = vector_sub(p2,a);

	// print_vector(ba);
	// print_vector(p1a);
	// print_vector(p2a);
	vector cp1 = vector_cross_product(ba, p1a);
	vector cp2 = vector_cross_product(ba, p2a);
	
	// printf("cps\n");
	// print_vector(cp1);
	// print_vector(cp2);

	float res = vector_dot_product(cp1, cp2);
	// printf("res = %f\n", res);
	// printf("======================\n");
	
	// print_vector(cp1);
	// print_vector(cp2);
	
	return (res >= 0);
}

int ray_triangle_intersection(ray r, triangle t, vector* intersection){

	vector normal = vector_cross_product(vector_sub(t.v1, t.v2), vector_sub(t.v3, t.v2));
	//print_vector(normal);

	//All points p in the plane (p-v1)*normal = 0

	float dp = vector_dot_product(r.direction, normal);
	if(dp == 0) return 0;

	float d = vector_dot_product(vector_sub(t.v1, r.origin), normal)/dp;

	if(d < 0) return 0;

	//printf("d = %f\n", d);
	*intersection = vector_sum(vector_scale(d,r.direction),r.origin);
	//print_vector((*intersection));

	/*I have intersected the plane the triangle is contained with the ray
	Now I have to see if that point is inside the triangle*/

	return (same_side_semiplane(*intersection, t.v1, t.v2, t.v3) &&
		same_side_semiplane(*intersection, t.v2, t.v3, t.v1) &&
		same_side_semiplane(*intersection, t.v3, t.v1, t.v2));
}

int tracer_c(pixel* image, int image_width, int image_height,
	float focal_distance, 	light* lights, int light_count, sphere* spheres,
	int sphere_count, triangle* triangles, int triangle_count){

	float relation = (float)image_width/(float)image_height;
	float r_sq = relation*relation;
	float window_height = 35.0/pow((r_sq + 1), 0.5);
	float window_width = relation*window_height;
	float step = window_width/(float)image_width;

	ray tracer; //ba dum tss
	tracer.origin.x = 0;
	tracer.origin.y = 0;
	tracer.origin.z = 0;
	tracer.direction.z = focal_distance;

	unsigned int row, col;
	int sphere_i, light_i, triangle_i;
	vector intersection;

	for(row = 0; row < image_height; row++){
		for(col = 0; col < image_width; col++){
			float nearest_object_distance = FLT_MAX;
			float distance;

			tracer.direction.x = step*((float)col+.5) - window_width/2;
			tracer.direction.y = -step*((float)row+.5) + window_height/2;

			// printf("row = %d\tcol = %d\t", row, col);
			// print_vector(tracer.direction);
			
			for(sphere_i = 0; sphere_i < sphere_count; sphere_i++){

				if(ray_sphere_intersection(tracer, spheres[sphere_i], &intersection)){
					//print_vector(intersection);	
					distance = vector_2norm(vector_sub(tracer.origin, intersection));
					//printf("row = %d\tcol = %d\tdistance = %f\n", row, col, distance);
					if(distance < nearest_object_distance){
		
						nearest_object_distance = distance;

						image[pos(row, col, image_width)].r = 0;
						image[pos(row, col, image_width)].g = 0;
						image[pos(row, col, image_width)].b = 0;
						
						for(light_i = 0; light_i < light_count; light_i++){

							vector intersection_to_light = vector_sub(lights[light_i].center, intersection);
							// print_vector(intersection_to_light);
							vector normal = vector_sub(intersection, spheres[sphere_i].center);
							// print_vector(normal);
							float coef = vector_dot_product(intersection_to_light, normal);
							// printf("%f\n", coef);
							coef = coef/vector_2norm(intersection_to_light)/vector_2norm(normal);
							// printf("%f\n", coef);
							if(coef > 0){
								image[pos(row, col, image_width)].r += spheres[sphere_i].color.r * coef * lights[light_i].intensity * lights[light_i].color.r;
								image[pos(row, col, image_width)].g += spheres[sphere_i].color.g * coef * lights[light_i].intensity * lights[light_i].color.g;
								image[pos(row, col, image_width)].b += spheres[sphere_i].color.b * coef * lights[light_i].intensity * lights[light_i].color.b;
							}
						}
					}
				}
			}

			for(triangle_i = 0; triangle_i < triangle_count; triangle_i++){

				if(ray_triangle_intersection(tracer, triangles[triangle_i], &intersection)){

					distance = vector_2norm(vector_sub(tracer.origin, intersection));
					// printf("row = %d\tcol = %d\tdistance = %f\n", row, col, distance);
					if(distance < nearest_object_distance){
			
						nearest_object_distance = distance;

						image[pos(row, col, image_width)].r = 0;
						image[pos(row, col, image_width)].g = 0;
						image[pos(row, col, image_width)].b = 0;
						
						for(light_i = 0; light_i < light_count; light_i++){

							vector intersection_to_light = vector_sub(lights[light_i].center, intersection);
							vector normal = vector_cross_product(vector_sub(triangles[triangle_i].v1, triangles[triangle_i].v2), vector_sub(triangles[triangle_i].v3, triangles[triangle_i].v2));
							
							float coef = abs(vector_dot_product(intersection_to_light, normal)
								/vector_2norm(intersection_to_light)/vector_2norm(normal));

							if(coef > 0){
								image[pos(row, col, image_width)].r += triangles[triangle_i].color.r * coef * lights[light_i].intensity * lights[light_i].color.r;
								image[pos(row, col, image_width)].g += triangles[triangle_i].color.g * coef * lights[light_i].intensity * lights[light_i].color.g;
								image[pos(row, col, image_width)].b += triangles[triangle_i].color.b * coef * lights[light_i].intensity * lights[light_i].color.b;
							}
						}
					}
				}
			}
		}
	}
	
	return 0;
}