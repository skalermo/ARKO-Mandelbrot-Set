#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "SDL/SDL.h"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_MAX_ITER 50
#define SCALE_START 1       // default scale value

// (-2.0, 1) is the top left point of the set
// so the working space is [-2.0; 1] x [-1; 1]
// this coordinates will be translated to pixel coordinates
#define X_START -2.0
#define Y_START 1.0
#define X_LENGTH 3.0		// length from start x point to end x point on the complex plane
#define Y_LENGTH 2.0		// length from start y point to end y point on the complex plane

#define DX X_LENGTH/width 	// x step if scale = 1
#define DY Y_LENGTH/height 	// y step if scale = 1 maybe will change it
#define STEP 10 	// points we go through while move up down etc
#define ITER_DIFF 50        // iterations change step

// some defines to use in switch statement
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

/**
 * The assembler function which calculates color
 * for each pixel of set region (not necessarily the whole screen)
 * and writes them to sdl_surface
 * @param void* pointer to where should function write colors
 * @param int width of the region which function should process
 * @param int height of the region which function should process
 * @param int iteration amount
 * @param double x_component of the starting point, which is top left
 * @param double y_component of the starting point
 * @param double step in x coordinates;
 *      required due to the fact that complex plane is continuous, but pixel plane is not
 * @param double step in y coordinates, same as above
 */
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
void inc_iter();
void dec_iter();
void back_to_start();
void write_to_file();
unsigned int hash(unsigned int);

SDL_Surface *screen;

// pointers to allocate some space to;
// are required to pass to assembler function
// that writes just small piece of plot
// to them while use movement
void * vertical;
void * horizontal;

int width, height, max_iter = DEFAULT_MAX_ITER;
char verbose;
double x_start = X_START, x_start0 = X_START, y_start = Y_START, y_start0 = Y_START, dx, dy;
unsigned long long scale = SCALE_START;

// Entry point
int main(int argc, char *argv[]) {
	if (argc == 5){
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		max_iter = atoi(argv[3]);
		verbose = atoi(argv[4]);
	}else
    if (argc == 4) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        max_iter = atoi(argv[3]);
        verbose = 0;
    }else if(argc == 1){
		width = DEFAULT_WIDTH;
		height = DEFAULT_HEIGHT;
		max_iter = DEFAULT_MAX_ITER;
		verbose = 0;
	}else{
		printf("Usage: %s [window's_width] [window's_height] [max_iterations] [verbose]\n", argv[0]);
        printf("Example: %s 640 480 50 1\n", argv[0]);
        printf("Example: %s 640 480 50\n", argv[0]);
        printf("Example: %s\n", argv[0]);

        printf("Hotkeys:\n");
        printf("wasd or arrows for movement\n");
        printf("io for iteration number change\n");
        printf("m,./ for zooming\n");
        printf("b for back to defaults");
        printf("t for writing actual content of sdl_surface to a bitmap file");
        return 0;
	}

	horizontal = malloc(STEP * width * sizeof(unsigned int));
	vertical = malloc(STEP * height * sizeof(unsigned int));


	// Initialize SDL's subsystems - in this case, only video.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Attempt to create width x height window with 32bit pixels.
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

	char run = 1;
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

					case SDLK_i:
						inc_iter();
					break;

					case SDLK_o:
						dec_iter();
					break;

					case SDLK_b:
						back_to_start();
					break;

				    case SDLK_t:
                        event.key.keysym.sym = 0;
				        write_to_file();
				    break;
				}
				break;
			case SDL_KEYUP:
				// If escape is pressed, return (and thus, quit)
				if (event.key.keysym.sym == SDLK_ESCAPE)
					run = 0;
				break;
			case SDL_QUIT:
				run = 0;
			break;
		}
		//event.key.keysym.sym = 0;
		SDL_Flip(screen);
		if (verbose)
		    printf("%.25lf %.25lf %.25lf %.25lf %llu\n", x_start, y_start, dx, dy, scale);
	}
	SDL_Quit();
	free (vertical);
	free (horizontal);
	return 0;
}

/**
 * When arrows are used pixels (rgb values stored in sdl_surface) are moved
 * in opposite direction on STEP pixels
 * so assembler function doesn't need to calculate all over again,
 * but just a small region
 * @param movement one of the four possible directions of movement
 */
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
		break;

		case LEFT:
			for (int j = 0; j < height; j++)
				for (int i = 0; i < width - STEP; i++){
					Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
					Uint8 *source_pixel = (Uint8 *)screen->pixels + j * screen->pitch + (i + STEP) * 4;
	    			*(Uint32 *)target_pixel = *(Uint32 *)source_pixel;
				}
		break;

		case RIGHT:
			for (int j = 0; j < height; j++)
				for (int i = width - 1; i > STEP - 1; i--){
					Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
					Uint8 *source_pixel = (Uint8 *)screen->pixels + j * screen->pitch + (i - STEP) * 4;
	    			*(Uint32 *)target_pixel = *(Uint32 *)source_pixel;
				}
		break;
	};
	SDL_UnlockSurface(screen);
}

/**
 * These four functions (except for the up() ) are designed to use assembler function,
 * write the result to 'horizontal' or 'vertical' buffer,
 * and then copy it to the sdl_surface
 *
 * SDL_surface's actual size is bigger than the screen size,
 * because sdl uses remaining space for some reasons. (Read about it on wiki or stackoverflow)
 * So there is this translation of pointers of different types (uint8 and uint32),
 * I haven't found better solution
 * Although seems like assembler function has no problems with it and treats sdl_surface's size
 * just as intended (the screen size)
 */
void up(){
	y_start += STEP * dy;
	y_start0 += STEP * dy;
	mandelbrot(screen->pixels, width, STEP, max_iter, x_start, y_start, dx, dy);
}

void down(){
	y_start0 -= STEP * dy;
	y_start -= STEP * dy;
	mandelbrot(horizontal, width, STEP, max_iter, x_start, y_start-dy*(height - STEP), dx, dy);
	for (int j = height - STEP; j < height; j++)
		for (int i = 0; i < width; i++){
			Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
			*(Uint32 *)target_pixel = *((Uint32*)horizontal + ((j - height + STEP) * width + i));
		}
}

void left(){
	x_start -= STEP * dx;
	x_start0 -= STEP * dx;
	mandelbrot(vertical, STEP, height, max_iter, x_start, y_start, dx, dy);
	for (int j = 0; j < height; j++)
		for (int i = 0; i < STEP; i++){
			Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
			*(Uint32 *)target_pixel = *((Uint32*)vertical + (j * STEP + i));
		}
}

void right(){
	x_start += STEP * dx;
	x_start0 += STEP * dx;
	mandelbrot(vertical, STEP, height, max_iter, x_start + dx * (width - STEP), y_start, dx, dy);
	for (int j = 0; j < height; j++)
		for (int i = width - STEP; i < width; i++){
			Uint8 *target_pixel = (Uint8 *)screen->pixels + j * screen->pitch + i * 4;
			*(Uint32 *)target_pixel = *((Uint32*)vertical + (j * STEP + i - width + STEP));
		}
}

void zoom_out(){
	scale -= 1;
	if (scale < 1)		//???
		scale = 1;
	dx = DX/scale;
	dy = DY/scale;														//???
	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);		//???
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);		//???
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void zoom_in(){
	scale += 1;
	dx = DX/scale;
	dy = DY/scale;

	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);

}

void megazoom_out(){
	scale >>= 1;
	if (scale < 1)
		scale = 1;
	dx = DX/scale;
	dy = DY/scale;

	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

/**
 * @bug: strange artifacts start to appear when scale is greater than 0x4000000000
 */
void megazoom_in(){
	scale <<= 1;
	// if (scale > 0x4000000000)
	// 	scale = 0x4000000000;
	dx = DX/scale;
	dy = DY/scale;

	x_start = x_start0 + X_LENGTH/2*(1.0-1.0/scale);
	y_start = y_start0 - Y_LENGTH/2*(1.0-1.0/scale);
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void inc_iter(){
	max_iter += ITER_DIFF;
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

void dec_iter(){
	max_iter -= ITER_DIFF;
	if (max_iter <= 0)
		max_iter = 50;
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

/**
 * function sets all values to default
 * so you'll be at the start point
 */
void back_to_start(){
	x_start = x_start0 = X_START;
	y_start = y_start0 = Y_START;
	scale = SCALE_START;
	dx = DX;
	dy = DY;
	mandelbrot(screen->pixels, width, height, max_iter, x_start, y_start, dx, dy);
}

/**
 * function writes actual content of sdl_surface
 * to a file, which name is by hash function
 */
void write_to_file(){
    char name[45];
    char temp[15];

    // check if directory "img/" already exists
    // if it doesn't make one
    struct stat st = {0};
    if (stat("img", &st) == -1) {
        mkdir("img", 0700);
    }

    // add "img/" at the start of the name
    // so it will be written to the directory
    sprintf(name, "%s", "img/");

    // generate filename from program parameters
    sprintf(temp, "%ld", width);
    strncat(name, temp, sizeof(temp));
    sprintf(temp, "%ld", height);
    strncat(name, temp, sizeof(temp));
    sprintf(temp, "%ld", max_iter);
    strncat(name, temp, sizeof(temp));
    sprintf(temp, "%ld", scale);
    strncat(name, temp, sizeof(temp));
    sprintf(temp, "%ld", x_start0 + y_start0);
    strncat(name, temp, sizeof(temp));

    // check if file with generated name exists
    // it does, add "(1)" at end
    while (1)
        if( access( name, F_OK ) != -1 ) {
            strncat(name, "(1)", sizeof("(1)"));
        } else {
            break;
        }

    // write to file
    printf("Writing to file %s\n", name);
    SDL_SaveBMP(screen, name);
    printf("Successfully written to the file\n");
}