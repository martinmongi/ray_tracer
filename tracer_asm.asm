;Macros will only use xmm registers above 

%macro vector_add 3
	vaddps %1, %2, %3
%endmacro

%macro vector_sub 3
	vsubps %1, %2, %3
%endmacro

%macro vector_scale 3
	vpshufd %2, %2, 0h00
	vmulps %1, %2, %3
%endmacro

%macro vector_dot_product 3
	vmulps %1, %2, %3
	vhaddps %1, %1, %1
	vhaddps %1, %1, %1
%endmacro

%macro vector_2norm 2
	vector_dot_product %1, %2, %2
	vsqrtps %1, %1
%endmacro

%macro same_side 5 ;dest p1 p2 a b
	vector_sub xmm14, %5, %4	; b-a
	vector_sub xmm13, %2, %4	; p1-a
	vector_cross_product xmm13, xmm13, xmm14 ; cross_product(b-a,p1-a)
	vector_sub %1, %3, %4		; p2-a
	vector_cross_product %1, %1, xmm14 ; cross_product(b-a,p2-a)
	vector_dot_product %1, %1, xmm13
%endmacro

%macro vector_cross_product 3 ; x = 11 y = 10 z = 01 padd = 00
	vpshufd xmm4, %2, 0b10011100 ; shuff a
	vpshufd xmm7, %3, 0b01111000 ; shuff b
	vmulps %1, xmm4, xmm7
	vpshufd xmm4, %2, 0b01111000 ; shuff a
	vpshufd xmm7, %3, 0b10011100 ; shuff b
	vmulps xmm4, xmm4, xmm7
	vsubps %1, %1, xmm4
%endmacro

%macro invert 1
	vpshufd %1, %1, 0b00011011
%endmacro

global tracer_asm

section		.text

; typedef struct {
; 	pixel* image;			0
; 	light* lights;			8
; 	sphere* spheres;		16
; 	triangle* triangles;	24
; 	int image_width;		32
; 	int image_height;		36
; 	int light_count;		40
; 	int sphere_count;		44
; 	int triangle_count;		48
; 	float focal_distance;	52
; } scene;					56

tracer_asm:
	 	;rdi = s
	 	push rbp
	 	mov rbp, rsp
	 	push rbx
	 	push r12
	 	push r13
	 	push r14
	 	push r15


		mov rax, [rdi]				; rax = image
		mov esi, [rdi + 32]			; rsi = image_width
		mov edx, [rdi + 36]			; rdx = image_height
		vcvtsi2ss xmm0, [rdi+32]	; X xmm0 = image_width
		vcvtsi2ss xmm1, [rdi+36]	; X xmm1 = image_height
		vdivss xmm2, xmm0, xmm1		; X xmm2 = relation
		vmulss xmm3, xmm2, xmm2		; X xmm3 = r_sq
		vaddss xmm3, xmm3, [one]	; X xmm3 = r_sq+1
		vsqrtss xmm3, xmm3, xmm3	; X xmm3 = pow((r_sq + 1), 0.5)
		vmovd xmm4, [std_fd]		; X xmm4 = 35
		vdivss xmm3, xmm4, xmm3		; xmm3 = window_height
		vmulss xmm5, xmm2, xmm3		; xmm5 = window_width
		vdivss xmm6, xmm5, xmm0		; xmm6 = step
		vdivss xmm3, xmm3, [two]	; xmm3 = window_height/2
		vdivss xmm5, xmm5, [two]	; xmm5 = window_width/2
		


	; Now on, I won't touch xmm3, xmm5, xmm6. All others are not needed

		vmovq xmm0, [origin]		; xmm0 = tracer.origin

	; Now we have the first tracer, upper left corner, we will start cycling
	; xmm0 = tracer.origin
	; xmm1 = tracer.direction

		xor r8d, r8d ; row
	.row_loop:	
		xor r9d, r9d ; col
	.col_loop:

		vmovss xmm2, xmm9
		movdqu xmm15, [black]

		vcvtsi2ss xmm1, r9d			;xmm1 = col
		vaddss xmm1, xmm1, [half]	;xmm1 = col + .5
		vmulss xmm1, xmm1, xmm6		;xmm1 = (col + .5) * step
		vsubss xmm1, xmm1, xmm5		;xmm1 = (col + .5) * step - window_width/2

		vpshufd xmm1, xmm1, 0b0000000 ; xmm1 = x|x|x|x

		vcvtsi2ss xmm1, r8d			;xmm1 = row
		vaddss xmm1, xmm1, [half]	;xmm1 = row + .5
		vmulss xmm1, xmm1, xmm6		;xmm1 = (row + .5) * step
		vmulss xmm1, xmm1, [m_one]	;xmm1 = -(row + .5) * step
		vaddss xmm1, xmm1, xmm3		;xmm1 = -(row + .5) * step + window_height/2

		vpshufd xmm1, xmm1, 0b11000100 ; xmm1 = x|y|x|yd	

		vmovss xmm2, [rdi+52]

		vblendps xmm1, xmm1, xmm2, 0b0011 ; xmm1 = x|y|0|fd

		vpshufd xmm1, xmm1, 0b11100001 ; xmm1 = x|y|fd|0		

		movdqu xmm2, [black]  
		movdqu [rax], xmm2
		
		vmovss xmm2, [flt_max]	; xmm2 = nearest = flt_max


;=================SPHERE START=============================================================
		mov ebx, [rdi + 44]			; rbx = sphere_count
		mov r11, [rdi + 16]			; r11 = spheres

		xor r10d, r10d			; r10 = sphere_i
		mov r12, r11			; r12 = &spheres[sphere_i]

	.sphere_loop:
		cmp r10d,ebx
		je .exit_sphere_loop
		;Ray_sphere_intersection

		vmovups xmm8, [r12+16]				;xmm8 = s.center
		vpshufd xmm8, xmm8, 0b00011011
		vector_sub xmm8, xmm0, xmm8			; xmm8 = l = vector_sub(r.origin,s.center)
		vector_dot_product xmm9, xmm8, xmm8	; xmm9 = vector_dot_product(l,l)
		vmovd xmm10, [r12+32]				; xmm10 = s.r
		vmulss xmm10, xmm10, xmm10			; xmm10 = s.r^2
		vsubss xmm9, xmm9, xmm10			; xmm9 = c = vector_dot_product(l,l) - s.r^2
		vector_dot_product xmm10, xmm8, xmm1; xmm10 = b = vector_dot_product(r.direction, l);
		vaddss xmm10, xmm10, xmm10			; xmm10 = b = 2 * vector_dot_product(r.direction, l);
		vmulss xmm12, xmm10, xmm10			; xmm12 = b*b
		vector_dot_product xmm11, xmm1, xmm1; xmm11 = a = vector_dot_product(r.direction, r.direction)
		vmulss xmm13, xmm11, xmm9			; xmm13 = a*c
		vmulss xmm13, xmm13, [four]			; xmm13 = 4*a*c
		vsubss xmm12, xmm12, xmm13			; xmm12 = root_base = b*b - 4*a*c
		vcomiss xmm12, [zero]
		jb .exit_sphere_ray_intersection


		vsqrtss xmm12, xmm12, xmm12			; xmm12 = root1 = pow(root_base,0.5)
		vmovd xmm9, [zero]					; xmm9 = 0
		vsubss xmm9, xmm9, xmm12			; xmm9 = -pow(root_base,0.5)
		vsubss xmm9, xmm9, xmm10			; xmm9 = -pow(root_base,0.5) - b
		vsubss xmm12, xmm12, xmm10			; xmm12 = pow(root_base,0.5) - b
		vdivss xmm12, xmm12, xmm11			; xmm12 = (pow(root_base,0.5) - b)/a
		vdivss xmm12, xmm12, [two]			; xmm12 = root1 = (pow(root_base,0.5) - b)/a/2
		vdivss xmm9, xmm9, xmm11			; xmm9 = (-pow(root_base,0.5) - b)/a
		vdivss xmm9, xmm9, [two]			; xmm9 = root2 = (-pow(root_base,0.5) - b)/a/2
		vcomiss xmm12, [zero]
		jb .exit_sphere_ray_intersection
		vcomiss xmm9, [zero]
		jb .exit_sphere_ray_intersection
		vminss xmm8, xmm9, xmm12			; xmm8 = min(root1,root2)
		vector_scale xmm8, xmm8, xmm1		; xmm8 = vector_scale(root, r.direction)
		vector_add xmm8, xmm8, xmm0			; xmm8 = intersection = vector_add(r.origin, vector_scale(root, r.direction))

		; Here the function ray-sphere intersection ends.
		; We know the ray and sphere intersect and intersection is at xmm8

		vector_sub xmm9, xmm0, xmm8			; xmm9 = tracer.origin - intersection
		vector_dot_product xmm9, xmm9, xmm9	; xmm9 = distance squared
		vcomiss xmm9, xmm2
		ja .exit_sphere_ray_intersection	; si no es el elemento mas cercano salto

		vmovss xmm2, xmm9
		movdqu xmm15, [black]

		mov r13d, [rdi + 40]		;r13d = light_count
		mov r14, [rdi + 8]			; r14 = lights
		xor r15d, r15d				; r15d = lights_i
		mov rcx, r14				; rcx = lights[lights_i]

	.light_loop1:

		cmp r15d, r13d
		je .exit_sphere_ray_intersection	

		vmovdqu xmm9, [rcx+16]		; xmm9 = light.center
		vpshufd xmm9, xmm9, 0b00011011
		vsubps xmm9, xmm9, xmm8		; xmm9 = intersection_to_light = light.center - intersection
		vmovups xmm10, [r12+16]		; xmm10 = s.center
		vpshufd xmm10, xmm10, 0b00011011
		vsubps xmm10, xmm8, xmm10	 ;xmm10 = normal = intersection - s.center

		vector_dot_product xmm11, xmm9, xmm10	; xmm11 = dot(intersection_to_light, normal)
		vector_2norm xmm12, xmm9
		vdivps xmm11, xmm11, xmm12
		vector_2norm xmm12, xmm10
		vdivps xmm11, xmm11, xmm12	; xmm11 = coef|coef|coef|coef

		vcomiss xmm11, [zero]
		jb .shadow

		vmovdqu xmm9, [r12]			;xmm9 = s.color	
		vpshufd xmm9, xmm9, 0b00011011
		vmulps xmm9, xmm9, xmm11	;xmm9 = s.color*coef
		vmovss xmm10, [rcx+32]			;xmm10 = intensity
		vpshufd xmm10, xmm10, 0h00	;xmm10 = intensity|intensity|intensity|intensity|
		vmulps xmm9, xmm9, xmm10	;xmm9 = s.color*coef*l.intensity
		vmovdqu xmm10, [rcx]			; xmm10 = l.color
		vpshufd xmm10, xmm10, 0b00011011
		vmulps xmm9, xmm9, xmm10	;xmm9 = s.color*coef*l.intensity*l.color

		vaddps xmm15, xmm15, xmm9
		
	.shadow:
		inc r15d
		add rcx, 36
		jmp .light_loop1

	.exit_sphere_ray_intersection:
		inc r10d
		add r12, 36
		jmp .sphere_loop

	.exit_sphere_loop:
		
;=================SPHERE END=============================================================
	

;=================TRIANGLE START=============================================================

		mov ebx, [rdi + 48]			; rbx = triangle_count
		mov r11, [rdi + 24]			; r11 = triangles

		xor r10d, r10d			; r10 = triangle_i
		mov r12, r11			; r12 = &triangles[triangle_i]

	.triangle_loop:

		cmp r10d,ebx
		je .exit_triangle_loop

	; Ray triangle intersection

		vmovdqu xmm8, [r12+16]			; xmm8 = v1
		vector_sub xmm8, xmm8, [r12+32]		; xmm8 = v1 - v2
		vmovdqu xmm9, [r12+48]			; xmm9 = v3
		vector_sub xmm9, xmm9, [r12+32]		; xmm8 = v3 - v2
		invert xmm8
		invert xmm9
		vector_cross_product xmm10, xmm8, xmm9	; xmm10 = normal = v1-v2xv3-v2

		vector_dot_product xmm8, xmm10, xmm1	; xmm8 = dp = normal . r.direction
		vcomiss xmm8, [zero]
		je .exit_triangle_ray_intersection		;jumps if triangle is parallel to ray

		vmovdqu xmm9, [r12+16]			; xmm9 = v1
		invert xmm9
		vector_sub xmm9, xmm9, xmm0		; xmm9 = v1 - r.origin
		vector_dot_product xmm9, xmm9, xmm10	; xmm9 = dot(normal, v1 - r.origin)
		vdivss xmm9, xmm9, xmm8					; xmm9 = d =dot(normal, v1 - r.origin)/dp

		vcomiss xmm9, [zero]
		jb .exit_triangle_ray_intersection

		vmovdqu xmm8, xmm1				; xmm8 = r.direction
		vector_scale xmm10, xmm9, xmm8	; xmm8 = scale(d,r.direction)
		vector_add xmm8, xmm10, xmm0		; xmm8 = intersection = scale(d,r.direction) + r.origin

		;==== same side intersection intersection v1 v2 v3

		vmovdqu xmm9, [r12+16]			;xmm9 = v1
		vmovdqu xmm10, [r12+32]			;xmm10 = v2
		vmovdqu xmm11, [r12+48]			;xmm11 = v3
		invert xmm9
		invert xmm10
		invert xmm11

		vector_sub xmm14, xmm11, xmm10				; xmm14 = ba = v3 - v2
		vector_sub xmm13, xmm8, xmm10				; xmm13 = p1a = intersection - v2
		vector_cross_product xmm12, xmm14, xmm13	; xmm12 = cp1 = cross_product(ba,p1a)
		vector_sub xmm13, xmm9, xmm10				; xmm13 = p2a = v1 - v2
		vector_cross_product xmm9, xmm14, xmm13		; xmm9 = cp2 = cross_product(ba,p2a)
		vector_dot_product xmm13, xmm12, xmm9		; xmm12 = dot(cp1, cp2)

		vcomiss xmm13, [zero]
		jb .exit_triangle_ray_intersection

		;==== same side intersection intersection v2 v3 v1

		vmovdqu xmm9, [r12+32]			;xmm9 = v1
		vmovdqu xmm10, [r12+48]			;xmm10 = v2
		vmovdqu xmm11, [r12+16]			;xmm11 = v3
		invert xmm9
		invert xmm10
		invert xmm11

		vector_sub xmm14, xmm11, xmm10				; xmm14 = ba = v3 - v2
		vector_sub xmm13, xmm8, xmm10				; xmm13 = p1a = intersection - v2
		vector_cross_product xmm12, xmm14, xmm13	; xmm12 = cp1 = cross_product(ba,p1a)
		vector_sub xmm13, xmm9, xmm10				; xmm13 = p2a = v1 - v2
		vector_cross_product xmm9, xmm14, xmm13		; xmm9 = cp2 = cross_product(ba,p2a)
		vector_dot_product xmm13, xmm12, xmm9		; xmm12 = dot(cp1, cp2)

		vcomiss xmm13, [zero]
		jb .exit_triangle_ray_intersection

		;==== same side intersection intersection v3 v1 v2

		vmovdqu xmm9, [r12+48]			;xmm9 = v1
		vmovdqu xmm10, [r12+16]			;xmm10 = v2
		vmovdqu xmm11, [r12+32]			;xmm11 = v3
		invert xmm9
		invert xmm10
		invert xmm11

		vector_sub xmm14, xmm11, xmm10				; xmm14 = ba = v3 - v2
		vector_sub xmm13, xmm8, xmm10				; xmm13 = p1a = intersection - v2
		vector_cross_product xmm12, xmm14, xmm13	; xmm12 = cp1 = cross_product(ba,p1a)
		vector_sub xmm13, xmm9, xmm10				; xmm13 = p2a = v1 - v2
		vector_cross_product xmm9, xmm14, xmm13		; xmm9 = cp2 = cross_product(ba,p2a)
		vector_dot_product xmm13, xmm12, xmm9		; xmm12 = dot(cp1, cp2)

		vcomiss xmm13, [zero]
		jb .exit_triangle_ray_intersection

		vmovdqu xmm15, [white]

	.exit_triangle_ray_intersection:
		inc r10d
		add r12, 64
		jmp .triangle_loop


	.exit_triangle_loop:

		vpshufd xmm15, xmm15, 0b00011011
		vmovdqu [rax], xmm15  

		
		add rax, 16
		inc r9d
		cmp r9d, esi
		jne .col_loop

		inc r8d
		cmp r8d, edx
		jne .row_loop

		pop r15
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret

section		.data

std_fd:
		dd 3.5e1
white:
one:
		dd 1.0e0
		dd 1.0e0
		dd 1.0e0
		dd 1.0e0
black:
origin:
zero:
		dd 0.0e0
		dd 0.0e0
		dd 0.0e0
		dd 0.0e0
start_multipliers:
		dd 1.0e0
		dd 1.0e0
		dd 2.0e0
		dd -2.0e0
flt_max:
		dd 340282346638528859811704183484516925440.000000

four:
		dd 4.0e0
two:
		dd 2.0e0
half:
		dd 5.0e-1
m_one:
		dd -1.0e0