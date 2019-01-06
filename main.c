#include <stdlib.h>
#include <stdbool.h>
#include "SDL/SDL.h"


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_MAX_ITER 200
#define X_LENGTH 3.0
#define Y_LENGTH 2.0


void mandelbrot(void*, int, int, int, double, double, double, double);

SDL_Surface *screen;

// Entry point
int main(int argc, char *argv[]) {
	int width, height, max_iter = DEFAULT_MAX_ITER;
	double x_start = -2.0, y_start = 1.0, dx, dy, scale = 1.0;
	if (argc == 4){
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		max_iter = atoi(argv[3]);
	}else if(argc == 1){
		width = DEFAULT_WIDTH;
		height = DEFAULT_HEIGHT;
		max_iter = DEFAULT_MAX_ITER;
	}else{
		printf("Usage: %s window's_width window's_height max_iterations\n", argv[0]);
		printf("\tor just with default parameters (640, 480, 200): %s \n", argv[0]);
		return 0;
	}

	// Initialize SDL's subsystems - in this case, only video.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Attempt to create a 640x480 window with 32bit pixels.
	screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);

	// If we fail, return error.
	if (screen == NULL) {
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}
	
	// calc value of single step for x and y
	dx = X_LENGTH/width/scale;
	dy = Y_LENGTH/height/scale;
	printf("%lf %lf\n", dx, dy);
	//test of asm func
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);

	bool run = true;
	SDL_Event event;
	// Main loop: loop forever.
	while (run) {
		// Poll for events, and handle the ones we care about.
		SDL_PollEvent(&event); 
			switch (event.type) {
				case SDL_KEYDOWN:
					break;
				case SDL_KEYUP:
					// If escape is pressed, return (and thus, quit)
					if (event.key.keysym.sym == SDLK_ESCAPE)
						run = false;
					break;
				case SDL_QUIT:
					run = false;
					break;
			}
		//mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
