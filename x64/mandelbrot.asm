
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

	%define 	y_step 		[rbp+24] ; double
	%define 	x_step 		[rbp+16] ; double
	%define 	x_start 	[rbp-8] ; double
	%define 	max_iter 	rcx ; int
	%define 	height 		rdx ; int
	%define 	width 		rsi ; int 

mandelbrot:
	push 	rbp
	mov 	rbp, rsp
	sub 	rsp, 8 
	mov 	r9, height ; r9 = height
	movsd 	x_start, xmm0
loop_y:
	; push 	r9 ; current height onto stack
	mov 	r10, width ;
	movsd 	xmm0, x_start

loop_x:
	xorpd 	xmm4, xmm4 ; x = 0
	xorpd 	xmm5, xmm5 ; y = 0
	xor 	r8, r8

main_loop:
	movsd 	xmm6, xmm4 ; xmm6[0] = x
	shufpd 	xmm6, xmm5, 0x0	; xmm6[1] = y 
	mulpd 	xmm6, xmm6 ; xmm6[0] = x^2, xmm6[1] = y^2
	movsd 	xmm7, xmm6
	shufps 	xmm6, xmm6, 0x4e ; swap x^2 and y^2
	subsd 	xmm7, xmm6
	addsd 	xmm7, xmm0
	mulsd 	xmm5, xmm4 ; y *= x
	addsd 	xmm5, xmm5 ; y *= 2
	addsd 	xmm5, xmm1 ; y += y0
	movsd 	xmm4, xmm7 ; x = xtemp
	inc 	r8 ; iter++
	mov 	rax, 0xffffffff
	movsd 	xmm7, xmm6 ; y*y
	shufps 	xmm6, xmm6, 0x4e
	addsd 	xmm7, xmm6 ; x*x + y*y
	ucomisd	xmm7, [four] ; compare x*x + y*y with 4.0	
	cmovnb	rax, r8 ; if x*x+y*y <= 4 then eax = current iter
	cmp 	rax, rcx
	jl 		main_loop
	mov 	rax, r8
	and 	rax, pallette_size
	mov 	eax, [pallette+eax*4] ;; temporary solution
	mov 	r11, 0x000000	
	cmp 	r8, rcx
	cmove 	rax, r11
	mov 	dword [rdi], eax ; load color info of current pixel to the buffer
	mov 	rax, r8
	addsd 	xmm0, xmm2 ; x coordinate of next point
	add 	rdi, 4 ; address of next pixel
	dec 	r10
	jnz 	loop_x

	subsd 	xmm1, xmm3 ; y coordinate of next point
	dec 	r9
	jnz 	loop_y
	
epilog:
	; pop 	rbx
	mov 	rsp, rbp
	pop 	rbp
	ret