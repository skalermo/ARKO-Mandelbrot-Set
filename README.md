# ARKO-Mandelbrot-Set

## Requiremets
- gcc
- nasm
- sdl
- make

## Installation
To install:
1. Download or clone the repo
2. `cd` to the repo directory
3. Build:
   - `make` for 64-bit version
   - `make out32` for 32-bit version
4. Run `out` or `out32` (for 64-bit and 32-bit respectively)

## Parameters
Using parameters passed to the program you can set **width** and **height** of the window, **depth** of the Mandelbrot set, **output mode** (if you want program to print some info to the console).
### Example
Say you want the window size to be 1280x720 with initial depth at 100 iterations and with output mode turned on.
```
./out 1280 720 100 1
```

## Controls
There are some hotkeys programmed:
- **wasd** or **arrows** moving
- **i** increasing depth, **o** for decreasing
- **m** '**,**' zooming out, '**/**' '**.**' for zooming in
- **b** go to the starting point
- **esc** quit the program
- **t** write actual bitmap to the file

## Images
Program can produce images like this one in *bmp* format.

![Mandelbrot Set](https://github.com/skalermo/ARKO-Mandelbrot-Set/blob/master/image_example.png)
