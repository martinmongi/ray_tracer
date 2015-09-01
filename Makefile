all:
	gcc tracer_c.c -c -o tracer_c.o -g -Wall -O3
	gcc ray_tracer.c -c -o ray_tracer.o -g -Wall -O3
	nasm -f elf64 -g -F DWARF tracer_asm.asm -o tracer_asm.o
	gcc -o ray_tracer ray_tracer.o tracer_c.o tracer_asm.o -lm

