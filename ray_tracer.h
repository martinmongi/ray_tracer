#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

typedef struct {
	unsigned char r, g, b;
} pixel;

typedef struct {
	double x, y, z;
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
	double reflection;
};

class Sphere{
public:
	double r;
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
	double focal_distance;
};

class Triangle{
public:
	vector p1, p2, p3;
	Material mat;
};

double vector_dot_product(const vector a, const vector b){
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

vector vector_scale(const double k, const vector a){
	vector r;
	r.x = a.x * k;
	r.y = a.y * k;
	r.z = a.z * k;
	return r;
}

double vector_2norm(const vector a){
	return pow(vector_dot_product(a,a),0.5);
}

bool ray_sphere_intersection(Ray r, Sphere s, vector &intersection){
	double a = vector_dot_product(r.direction,r.direction);
	vector vr = vector_sub(r.origin,s.center);
	double b = 2 * vector_dot_product(r.direction, vr);
	double c = vector_dot_product(vr,vr) - pow(s.r,2);
	//printf("%f %f %f %f %f\n", r->direction.x, r->direction.y, a,b,c);
	double disc = pow(b,2)-4*a*c;
	if (disc < 0) return false;
	double sq_disc = pow(disc, 0.5);

	double root2 = (-b - sq_disc)/2/a;
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

	for(unsigned int i = 0; i < cam.height; i++){
		image.push_back(std::vector<pixel>(cam.width));
	}

	while(!ifile.eof()){
		ifile >> line;

		if(line[0] == '#') //comment
			continue;

		if(line == "light"){
			Light l;
			ifile >> l.center.x >> l.center.y >> l.center.z;
			ifile >> l.intensity.r >> l.intensity.g >> l.intensity.b;
			vec_lights.push_back(l);
			continue;
		}

		if(line == "sphere"){
			Sphere s;
			ifile >> s.r;
			ifile >> s.center.x >> s.center.y >> s.center.z;
			ifile >> s.mat.diffuse.r >> s.mat.diffuse.g >> s.mat.diffuse.b >> s.mat.reflection;
			vec_spheres.push_back(s);
			continue;
		}

		if(line == "triangle"){
			Triangle t;
			ifile >> t.p1.x >> t.p1.y >> t.p1.z;
			ifile >> t.p2.x >> t.p2.y >> t.p2.z;
			ifile >> t.p3.x >> t.p3.y >> t.p3.z;
			ifile >> t.mat.diffuse.r >> t.mat.diffuse.g >> t.mat.diffuse.b >> t.mat.reflection;
			vec_triangles.push_back(t);
			continue;
		}
	}

}

void Scene::Render(){

	double relation = (double)cam.width/(double)cam.height;
	double fd_sq = (double)cam.focal_distance*(double)cam.focal_distance;
	double r_sq = relation*relation;
	double window_height = pow(fd_sq/(r_sq + 1), 0.5);
	double window_width = relation*window_height;
	double step = window_width/(double)cam.width;

	Ray tracer; //ba dum tss
	tracer.origin.x = 0;
	tracer.origin.y = 0;
	tracer.origin.z = 0;

	for(int i = 0; i < cam.height; i++){
		for(int j = 0; j < cam.width; j++){
			tracer.direction.x = step*((double)j + 0.5) - window_width/2;
			tracer.direction.y = -step*((double)i + 0.5) + window_height/2;
			tracer.direction.z = cam.focal_distance;

			vector intersection;
			bool ans = ray_sphere_intersection(tracer, vec_spheres[0], intersection);
			if(ans){
				vector to_light = vector_sub(vec_lights[0].center, intersection);
				//print_vector(&to_light);
				vector normal = vector_sub(intersection,vec_spheres[0].center);
				double coef = vector_dot_product(to_light, normal);
				coef = coef/vector_2norm(to_light)/vector_2norm(normal);
				if(coef > 0)
					image[i][j].r = (unsigned char)((double)255 * coef);
				else
					image[i][j].r = 0;
				image[i][j].g = 0;
				image[i][j].b = 0;
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