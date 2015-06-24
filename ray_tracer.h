#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cfloat>

#define SPACE ' '

typedef struct {
	unsigned char r, g, b;
} pixel;

typedef struct {
	float x, y, z;
} vector;

typedef struct {
	unsigned char r,g,b;
} color;

class Light{
public:
	color intensity;
	vector center;
};

class Material{
public:
	color diffuse;
	float reflection;
};

class Sphere{
public:
	float r;
	vector center;
	Material mat;	
};

class Ray{
public:
	vector origin;
	vector direction;
};

class Camera{
public:
	int width, height;
	float focal_distance;
};

class Triangle{
public:
	vector p1, p2, p3;
	Material mat;
};

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

bool ray_sphere_intersection(Ray r, Sphere s, vector &intersection){
	float a = vector_dot_product(r.direction,r.direction);
	vector vr = vector_sub(r.origin,s.center);
	float b = 2 * vector_dot_product(r.direction, vr);
	float c = vector_dot_product(vr,vr) - pow(s.r,2);
	//printf("%f %f %f %f %f\n", r->direction.x, r->direction.y, a,b,c);
	float disc = pow(b,2)-4*a*c;
	if (disc < 0) return false;
	float sq_disc = pow(disc, 0.5);

	float root2 = (-b - sq_disc)/2/a;
	if(root2 >= 0){
		intersection.x = r.origin.x + root2*r.direction.x;
		intersection.y = r.origin.y + root2*r.direction.y;
		intersection.z = r.origin.z + root2*r.direction.z;
		return true;
	}else return false;
}

void print_vector(vector v){
	printf("x = %f y = %f z = %f\n", v.x, v.y, v.z);
}

class Scene{
public:
	Scene(char* ifilename);
	void Render();
	void WriteOutput();
private:
	std::string ofilename;
	Camera cam;
	std::vector<Light> vec_lights;
	std::vector<Sphere> vec_spheres;
	std::vector<Triangle> vec_triangles;
	std::vector<std::vector<pixel> > image;
};

Scene::Scene(char* ifilename){
	std::string line;

	std::ifstream ifile(ifilename);

	if(!ifile.is_open())
		return;

	ifile >> ofilename;
	ifile >> cam.width >> cam.height;
	ifile >> cam.focal_distance;
	pixel p;
	p.r = 0;
	p.g = 0;
	p.b = 0;

	for(unsigned int i = 0; i < cam.height; i++){
		image.push_back(std::vector<pixel>(cam.width, p));
	}

	while(!ifile.eof()){
		ifile >> line;
		//std::cout << line << std::endl;

		if(line == "light"){
			Light l;
			ifile >> l.center.x >> l.center.y >> l.center.z;
			ifile >> l.intensity.r >> l.intensity.g >> l.intensity.b;
			vec_lights.push_back(l);
		}

		if(line == "sphere"){
			Sphere s;
			int r,g,b;
			ifile >> s.r;
			ifile >> s.center.x >> s.center.y >> s.center.z;
			ifile >> r >> g >> b >> s.mat.reflection;
			// std::cout << SPACE << s.r << SPACE << std::endl;
			// std::cout << SPACE << s.center.x << SPACE << s.center.y << SPACE << s.center.z << SPACE << std::endl;
			// std::cout << SPACE << r << SPACE << g << SPACE << b << SPACE << s.mat.reflection << SPACE << std::endl;
			s.mat.diffuse.r = (unsigned char)r;
			s.mat.diffuse.g = (unsigned char)g;
			s.mat.diffuse.b = (unsigned char)b;
			vec_spheres.push_back(s);
		}

		if(line == "triangle"){
			Triangle t;
			int r,g,b;
			ifile >> t.p1.x >> t.p1.y >> t.p1.z;
			ifile >> t.p2.x >> t.p2.y >> t.p2.z;
			ifile >> t.p3.x >> t.p3.y >> t.p3.z;
			ifile >> r >> g >> b >> t.mat.reflection;
			t.mat.diffuse.r = (unsigned char)r;
			t.mat.diffuse.g = (unsigned char)g;
			t.mat.diffuse.b = (unsigned char)b;
			vec_triangles.push_back(t);
		}
	}

	std::cout << "Encontradas " << vec_spheres.size() << " esferas.\n";
	std::cout << "Encontrados " << vec_triangles.size() << " triangulos.\n";
	std::cout << "Encontradas " << vec_lights.size() << " luces.\n";

}

void Scene::Render(){

	float relation = (float)cam.width/(float)cam.height;
	float fd_sq = (float)cam.focal_distance*(float)cam.focal_distance;
	float r_sq = relation*relation;
	float window_height = pow(fd_sq/(r_sq + 1), 0.5);
	float window_width = relation*window_height;
	float step = window_width/(float)cam.width;

	Ray tracer; //ba dum tss
	tracer.origin.x = 0;
	tracer.origin.y = 0;
	tracer.origin.z = 0;

	for(int i = 0; i < cam.height; i++){
		for(int j = 0; j < cam.width; j++){
			float nearest_object_distance = FLT_MAX;
			float distance;

			tracer.direction.x = step*((float)j + 0.5) - window_width/2;
			tracer.direction.y = -step*((float)i + 0.5) + window_height/2;
			tracer.direction.z = cam.focal_distance;

			vector intersection;

			for(int sphere_i = 0; sphere_i < vec_spheres.size(); sphere_i++){
				//std::cout << "Entra " << sphere_i << std::endl;
				bool ans = ray_sphere_intersection(tracer, vec_spheres[sphere_i], intersection);
				if(ans){

					distance = vector_2norm(vector_sub(tracer.origin, intersection));
					if(distance < nearest_object_distance){
						
						nearest_object_distance = distance;

						vector to_light = vector_sub(vec_lights[sphere_i].center, intersection);
						//print_vector(&to_light);
						vector normal = vector_sub(intersection,vec_spheres[sphere_i].center);
						float coef = vector_dot_product(to_light, normal);
						coef = coef/vector_2norm(to_light)/vector_2norm(normal);
						if(coef > 0){
							image[i][j].r = (unsigned char)((float)vec_spheres[sphere_i].mat.diffuse.r * coef);
							image[i][j].g = (unsigned char)((float)vec_spheres[sphere_i].mat.diffuse.g * coef);
							image[i][j].b = (unsigned char)((float)vec_spheres[sphere_i].mat.diffuse.b * coef);
						}
					}
				}
			}
		}
	}
}

void Scene::WriteOutput(){
	std::ofstream ofile;
	ofile.open(ofilename.c_str());

	ofile << "P3" << std::endl;
	ofile << cam.width << " " << cam.height << std::endl;
	ofile << "255" << std::endl;

	for(int i = 0; i < image.size(); i++){
		for(int j = 0; j < image[0].size(); j++){
			ofile << (int)image[i][j].r << " " << (int)image[i][j].g << " " << (int)image[i][j].b << " ";
		}
		ofile << std::endl;
	}
}