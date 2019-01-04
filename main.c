#include <stdlib.h>
#include <stdbool.h>
#include "SDL/SDL.h"


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_MAX_ITER 200


void mandelbrot(unsigned*, int, int, int, double, double, double);

SDL_Surface *screen;

void render(int width, int height, SDL_Surface * surface) {
	for (int i = 0; i< height; i++)
		for (int j = 0; j<width; j++)
			((unsigned int*)surface->pixels)[i*width+j] = 0x00ff0000;
}


// Entry point
int main(int argc, char *argv[]) {
	int width, height, max_iter = DEFAULT_MAX_ITER;
	double re_start = -2.0, im_start = 1.0, scale = 1.0;
	if (argc == 3){
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}else{
		width = DEFAULT_WIDTH;
		height = DEFAULT_HEIGHT;
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

	//test of asm func
	mandelbrot(screen->pixels, width, height, max_iter, re_start, im_start, scale);

	bool run = true;
	SDL_Event event;
	// Main loop: loop forever.
	while (run) {
		// Render stuff
		render(width, height, screen); 

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
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
