CC = gcc
CFLAGS = -g -Wall -O3 
NASM = nasm
NASMFLAGS = -f elf64 -g -F DWARF
LD = gcc
LDFLAGS = -lm

all:
	$(CC) tracer_c.c -c -o tracer_c.o $(CFLAGS)
	$(CC) ray_tracer.c -c -o ray_tracer.o $(CFLAGS)
	$(NASM) tracer_asm.asm -o tracer_asm.o $(NASMFLAGS)
	$(LD) -o ray_tracer ray_tracer.o tracer_c.o tracer_asm.o $(LDFLAGS)
