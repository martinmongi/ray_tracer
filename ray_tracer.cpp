#include "ray_tracer.h"

int main(int argc, char const *argv[])
{
	if(argc < 2){
		std::cout << "Especifique el archivo de entrada de la escena: " << argv[0] << " input_file.in" <<  std::endl;
		return 1;
	}

	Scene sc((char*) argv[1]);

	sc.Render();

	sc.WriteOutput();
	return 0;
}