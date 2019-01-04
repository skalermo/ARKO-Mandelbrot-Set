section	.text
global  mandelbrot

	%define 	buf 	[ebp+8]
	%define 	width 	[ebp+12]
	%define 	height 	[ebp+16]
	%define 	max_iter 	[ebp+20]
	%define 	re_start 	[ebp+24] ; double
	%define 	im_start 	[ebp+32] ; double
	%define 	scale 	[ebp+40] ; double

mandelbrot:
	push 	ebp
	mov 	ebp, esp

	mov 	eax, buf
	mov 	eax, width
	mov 	eax, height
	mov 	eax, max_iter
	; movaps 	xmm0, re_start
	; movaps	xmm0, im_start
	; movaps	xmm0, scale


epilog:
	mov 	esp, ebp
	pop 	ebp
	ret