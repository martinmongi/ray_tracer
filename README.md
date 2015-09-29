# ray_tracer

DISCLAIMER: This project is extremely experimental. Use at your own risk.

TP Final Orga2 Ray Tracer

This project is meant to be presented as a final in the course Computer Architecture 201 of the FCEN-UBA.

The main aim is to appreciate the advantages of the SIMD instruction set, in relation with the performance in an application of this nature. For more information, please contact martinmongi (at) gmail.com

==========================================================================

Usage:

Program should be executed with:
```
./ray_tracer sphere.in 0
```
The input file will be a text file containing the scene information. It will be passed to the program as the first argument after the executable. Also, the program will receive a second parameter indicating which version of the algorithm it should run (0 for C implementation, 1 for optimized SIMD implementation).

Firstly, the input file will begin with 3 lines. First one will have the name of the output image. Second line will have two numbers: image width and height, separated by a space. Third line will have the simulated focal distance of the camera. Camera will be located at (0,0,0).

Secondly, the input file will describe the objects of the scene. Objects fall into three categories: lights, spheres and triangles.
Firstly, there will be a line with the light count. Then, the description of every light. Every light will be described with the following values in order, separated by a space: intensity, color (rgb), position (x,y,z).
Secondly, there will be a line with the sphere count. Then, the description of every sphere. Every sphere will be described with the following values in order, separated by a space: radius, color (rgb), position (x,y,z).
Finally, there will be a line with the triangle count. Then, the description of every triangle. Every triangle will be described with the following values in order, separated by a space: color (rgb), position of vertex 1 (x,y,z), position of vertex 2 (x,y,z), position of vertex 3 (x,y,z).

Example:
```
render.pgm
640 480
35

1				#light count		

1				#intensity
1 1 1			#color (rgb)
-20 20 0		#position (x,y,z)

1				#sphere count

30				#radius
1 1 1			#color (rgb)
0 0 120			#position (x,y,z)

1

1 1 1			#color (rgb)
0 -20 100		#vertex 1 (x,y,z)
20 -20 134.64	#vertex 2 (x,y,z)
-20 -20 134.64	#vertex 3 (x,y,z)
```

Output will be a PGM image. This keeps the code simple and focusing in the objective of the project. It can be converted later with several applications in every platform. It could be later implemented using existing libraries