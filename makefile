all: out

out: main.o mandelbrot.o
	gcc -no-pie -m64 -g -o out main.o mandelbrot.o -lSDL

main.o: main.c
	gcc -m64 -g -c -o main.o main.c -lSDL
mandelbrot.o: mandelbrot.asm
	nasm -f elf64 -F dwarf -g mandelbrot.asm

out32: main32.o mandelbrot32.o
	gcc -m32 -g -o out32 main32.o mandelbrot32.o -lSDL

main32.o: main.c
	gcc -m32 -g -c -o main32.o main.c -lSDL

mandelbrot32.o:
	nasm -f elf32 -F dwarf -g mandelbrot32.asm

clean:
	rm -f out out32 main.o main32.o mandelbrot.o mandelbrot32.o

