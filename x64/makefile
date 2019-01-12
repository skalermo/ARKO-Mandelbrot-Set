all: out
out: main.o mandelbrot.o
	gcc -no-pie -m64 -g -o out main.o mandelbrot.o -lSDL

main.o: main.c
	gcc -m64 -g -c -o main.o main.c -lSDL
mandelbrot.o: mandelbrot.asm
	nasm -f elf64 -F dwarf -g mandelbrot.asm
clean:
	rm out main.o mandelbrot.o

