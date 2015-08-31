#include <cstdio>

typedef struct {
	float x, y, z, dummy;
} vector;

extern "C" {vector vector_sum(vector *destination, vector *a, vector *b);}

void print_vector(vector v){
	printf("x = %f y = %f z = %f\n", v.x, v.y, v.z);
}

int main(int argc, char const *argv[])
{
	vector a,b;
	a.x = 2.5;
	a.y = 5.0;
	a.z = 7.5;
	b.x = 12.5;
	b.y = 15.0;
	b.z = 17.5;
	vector res;
	res.x = -40;
	res.y = -40;
	res.z = -40;
	vector_sum(&res, &a, &b);
	print_vector(res);

	return 0;
}