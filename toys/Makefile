all:
	nasm -f elf64 -g -F DWARF test.asm -o test_asm.o
	g++ test.cpp -c -m64 -o test_c.o -O3 -g
	g++ test_c.o test_asm.o -m64 -o test