#include <stdlib.h>
#include <stdbool.h>
#include "SDL/SDL.h"


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_MAX_ITER 50
#define X_LENGTH 3.0		// length from start x point to end x point on the complex plane
#define Y_LENGTH 2.0		// length from start y point to end y point on the complex plane
#define DX X_LENGTH/width 	// x step if scale = 1
#define DY Y_LENGTH/height 	// y step if scale = 1 maybe will change it
#define STEP 10 	// points we go through while move up down etc
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4


void mandelbrot(void*, int, int, int, double, double, double, double);
void move_pixels(int);
void up();
void down();
void left();
void right();
void zoom_out();
void zoom_in();
void megazoom_out();
void megazoom_in();

SDL_Surface *screen;
void * vertical;
void * horizontal;
int width, height, max_iter = DEFAULT_MAX_ITER;
double x_start = -2.0, y_start = 1.0, dx, dy, scale = 1.0, x_start0 = -2.0, y_start0 = 1.0;

// Entry point
int main(int argc, char *argv[]) {
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
		printf("\tor just with default parameters (640, 480, 50): %s \n", argv[0]);
		return 0;
	}

	horizontal = malloc(STEP * width * sizeof(unsigned int));
	vertical = malloc(STEP * height * sizeof(unsigned int));


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
				//Select surfaces based on key press 
				switch(event.key.keysym.sym) { 
					case SDLK_UP:
					case SDLK_w:
						move_pixels(DOWN);
						up();
					break;

					case SDLK_DOWN:
					case SDLK_s:
						move_pixels(UP);
						down();
					break; 

					case SDLK_LEFT:
					case SDLK_a:
						move_pixels(RIGHT);
						left(); 
					break;

					case SDLK_RIGHT:
					case SDLK_d:
						move_pixels(LEFT);
						right();
					break;

					case SDLK_COMMA:
					case SDLK_LESS:
						zoom_out();
					break;

					case SDLK_PERIOD:
					case SDLK_GREATER:
						zoom_in();
					break;

					case SDLK_m:
						megazoom_out();
					break;

					case SDLK_SLASH:
						megazoom_in();
					break;					
				}
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
		//event.key.keysym.sym = 0;
		//mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
		SDL_Flip(screen);
		printf("%lf %lf %lf %lf %lf\n", x_start, y_start, dx, dy, scale);
	}
	SDL_Quit();
	free (vertical);
	free (horizontal);
	return 0;
}

void move_pixels(int movement){
	SDL_LockSurface(screen);
	switch(movement){
		case UP:
			for (int j = 0; j < height - STEP; j++)
				for (int i = 0; i < width; i++){
					Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
					Uint8 *source_pixel = (Uint8 *)screen->pixels + (j+STEP) * screen->pitch + i * 4;
	    			*(Uint32 *)target_pixel = *(Uint32 *)source_pixel;
				}
		break;

		case DOWN:
			for (int j = height-1; j > STEP-1 ; j--)
				for (int i = 0; i < width; i++){
					Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
					Uint8 *source_pixel = (Uint8 *)screen->pixels + (j - STEP) * screen->pitch + i * 4;
	    			*(Uint32 *)target_pixel = *(Uint32 *)source_pixel;
				}
					
					//*((unsigned*)(screen->pixels+i+(j-1)*height)) = *((unsigned*)(screen->pixels+i+j*height));
		break;

		case LEFT:
			for (int j = 0; j < height; ++j)
				for (int i = 0; i < width-1; ++i)
					*((unsigned*)(screen->pixels+i+j*height)) = *((unsigned*)(screen->pixels+i+1+j*height));
		break;

		case RIGHT:
			for (int j = 0; j < height; ++j)
				for (int i = 1; i < width; ++i)
					*((unsigned*)(screen->pixels+i+j*height)) = *((unsigned*)(screen->pixels+i-1+j*height));
		break;
	};
	SDL_UnlockSurface(screen);
}

void up(){
	y_start += STEP * dy;
	y_start0 += STEP * dy;
	mandelbrot(horizontal, width, STEP, max_iter, x_start, y_start, dx, dy); //???
	for (int j = 0; j < STEP; j++)
		for (int i = 0; i < width; i++){
			Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
			*(Uint32 *)target_pixel = *((Uint32*)horizontal + (j * width + i));
		}
}

void down(){

	y_start0 -= STEP * dy;
	y_start -= STEP * dy;
	mandelbrot(horizontal, width, STEP, max_iter, x_start, y_start-dy*(height - STEP), dx, dy); //???
	for (int j = height - STEP; j < height; j++)
		for (int i = 0; i < width; i++){
			Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
			*(Uint32 *)target_pixel = *((Uint32*)horizontal + ((j - height + STEP) * width + i));
		}
}

void left(){
	x_start -= STEP * dx;
	x_start0 -= STEP * dx;
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void right(){
	x_start += STEP * dx;
	x_start0 += STEP * dx;
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void zoom_out(){
	scale -= 0.1;
	if (scale < 0.2)		//???
		scale = 0.1;
	dx = DX/scale;
	dy = DY/scale;														//???
	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);		//???
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);		//???
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void zoom_in(){
	scale += 0.1;
	dx = DX/scale;
	dy = DY/scale;

	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);

}

void megazoom_out(){
	scale /= 2;
	if (scale < 0.2)
		scale = 0.1;
	dx = DX/scale;
	dy = DY/scale;

	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void megazoom_in(){
	scale *= 2;
	dx = DX/scale;
	dy = DY/scale;

	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}