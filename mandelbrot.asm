
; color palette
	; 0x421e0f ; brown 3
	; 0x19071a ; dark violett
	; 0x09012f ; darkest blue
	; 0x040449 ; blue 5
	; 0x000764 ; blue 4
	; 0x0c2c8a ; blue 3
	; 0x1852b1 ; blue 2
	; 0x397dd1 ; blue 1
	; 0x86b5e5 ; blue 0
	; 0xd3ecf8 ; lightest blue
	; 0xf1e9bf ; lightest yellow
	; 0xf8c95f ; light yellow
	; 0xffaa00 ; dirty yellow
	; 0xcc8000 ; brown 0
	; 0x995700 ; brown 1
	; 0x6a3403 ; brown 2

section .data
	pallette:	dd 	0x421e0f, 0x19071a, 0x09012f, 0x040449, 0x000764, 0x0c2c8a, 0x1852b1, 0x397dd1, 0x86b5e5, 0xd3ecf8, 0xf1e9bf, 0xf8c95f, 0xffaa00, 0xcc8000, 0x995700, 0x6a3403
	four: 	dq -4.0
section	.text
global  mandelbrot
	
	%define 	pallette_size 	15

	%define 	y_step 	[ebp+48] ; double
	%define 	x_step 	[ebp+40] ; double
	%define 	y_start 	[ebp+32] ; double
	%define 	x_start 	[ebp+24] ; double
	%define 	max_iter 	[ebp+20] ; int
	%define 	height 	[ebp+16] ; int
	%define 	width 	[ebp+12] ; int 
	%define 	buf 	[ebp+8] ; unsigned int*
	
	%define 	iterations [ebp-4] ; current iterations

	; ebx - pointer of surface->pixels
	; ecx - counter in main loop
	; xmm0 - x coordinate of current pixel (on the complex plane)
	; xmm1 - y coordinate of current pixel (on the complex plane)
	; xmm2 - x variable for calculations
	; xmm3 - y variable for calculations
	; xmm4 - x*x
	; xmm5 - y*y
	; xmm6 - xtemp
	; see algorithm on wiki: https://en.wikipedia.org/wiki/Mandelbrot_set
	
mandelbrot:
	push 	ebp
	mov 	ebp, esp
	sub 	esp, 4 ; allocate memory for local variables
	push 	ebx
 
 	mov 	ebx, buf ; copy pointer of surface->pixels to ebx
	mov 	ecx, height ; ecx = height
	mov 	edx, width
	movsd 	xmm1, y_start
	xor 	edx, edx

loop_y:
	push 	ecx ; current height onto stack
	mov 	ecx, width ;
	movsd 	xmm0, x_start

	loop_x:
		xorpd 	xmm2, xmm2 ; x = 0
		xorpd 	xmm3, xmm3 ; y = 0
 		mov 	dword iterations, 0x0000 ; iter = 0

 		main_loop:
 			movsd 	xmm7, x_step

 			movsd 	xmm4, xmm2 ;
 			mulsd 	xmm4, xmm4 ; x*x
 			movsd 	xmm5, xmm3
 			mulsd 	xmm5, xmm5 ; y*y
 			movsd 	xmm6, xmm4 ; xtemp = x*x
 			subsd 	xmm6, xmm5 ; xtemp -= y*y
 			addsd 	xmm6, xmm0 ; xtemp += x0
 			mulsd 	xmm3, xmm2 ; y *= x
 			addsd 	xmm3, xmm3 ; y *= 2
 			addsd 	xmm3, xmm1 ; y += y0
 			movsd 	xmm2, xmm6 ; x = xtemp
 			inc 	dword iterations ; iter++
			mov 	eax, 0xffff
 			
 			movsd 	xmm6, xmm4 ; x*x
 			addsd 	xmm6, xmm5 ; x*x + y*y
 			ucomisd	xmm6, [four] ; compare x*x + y*y with 4.0	
 			cmovnb	eax, iterations ; if x*x+y*y <= 4 then eax = current iter
 			cmp 	eax, max_iter
 			jl 		main_loop

 		mov 	eax, iterations
 		mov 	edx, eax
 		and 	eax, pallette_size
 		mov 	eax, [pallette+eax*4] ;; temporary solution
 		mov 	esi, 0x000000
 		cmp 	edx, max_iter
 		cmove 	eax, esi 
		mov 	dword [ebx], eax ; load color info of current pixel to the buffer
		mov 	eax, iterations
		
		addsd 	xmm0, x_step ; x coordinate of next point
		add 	ebx, 4 ; address of next pixel
		dec 	ecx
		jnz 	loop_x

	subsd 	xmm1, y_step ; y coordinate of next point
	pop 	ecx
	dec 	ecx
	jnz 	loop_y
	
epilog:
	pop 	ebx
	mov 	esp, ebp
	pop 	ebp
	ret