global tracer_asm

section		.text

; typedef struct{
; 	pixel* image; 			size 8	offset 0
; 	int image_width;		size 4	offset 8
;	int image_height;		size 4	offset 12
; 	float focal_distance;	size 4	offset 16
; 	light* lights;			size 8	offset 24
; 	int light_count;		size 4	offset 32
; 	sphere* spheres;		size 8	offset 40
; 	int sphere_count;		size 4	offset 48
; 	triangle* triangles;	size 8	offset 56
; 	int triangle_count;		size 4	offset 64
; } scene;							offset 72

tracer_asm:
	 	;rdi = s
	 	push rbp
		mov rbp, rsp
		push rbx

		vcvtsi2ss xmm0, [rdi+32]	; X xmm0 = image_width
		vcvtsi2ss xmm1, [rdi+36]	; X xmm1 = image_height
		vdivss xmm2, xmm0, xmm1		; X xmm2 = relation
		vmulss xmm3, xmm2, xmm2		; X xmm3 = r_sq
		vaddss xmm3, xmm3, [one]	; X xmm3 = r_sq+1
		vsqrtss xmm3, xmm3, xmm3	; X xmm3 = pow((r_sq + 1), 0.5)
		vmovd xmm4, [std_fd]		; X xmm4 = 35
		vdivss xmm3, xmm4, xmm3		; xmm3 = window_height
		vmulss xmm5, xmm2, xmm3		; xmm5 = window_width
		vdivss xmm6, xmm5, xmm1		; xmm6 = step


	; Now on, I won't touch xmm3, xmm5, xmm6. All others are not needed

		vmovq xmm0, [origin]		; xmm0 = tracer.origin

	; 	mov rax, [rdi]


	; 	xor r8, r8 ; row
	; loop_1:	
	; 	xor r9, r9 ; col
	; loop_2:

	; 	movdqu [rax], xmm0

	; 	add rax, 16
	; 	inc r9
	; 	cmp r9, rsi
	; 	jne loop_2

	; 	inc r8
	; 	cmp r8, rdx
	; 	jne loop_1

	; 	pop rbx
		leave
		ret

section		.data

std_fd:
		dd 3.5e1
white:
one:
		dd 1.0e0
		dd 1.0e0
		dd 1.0e0
		dd 0.0e0
black:
origin:
		dd 0.0e0
		dd 0.0e0
		dd 0.0e0
		dd 0.0e0