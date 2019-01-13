all: out
out: main.o mandelbrot.o
	gcc -m32 -g -o out main.o mandelbrot.o -lSDL

main.o: main.c
	gcc -m32 -g -c -o main.o main.c -lSDL
mandelbrot.o: mandelbrot.asm
	nasm -f elf32 -F dwarf -g mandelbrot.asm
clean:
	rm out main.o mandelbrot.o

