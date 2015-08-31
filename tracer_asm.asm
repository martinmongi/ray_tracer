global tracer_asm

section		.text

;int tracer_c(pixel* image, int image_width, int image_height)

tracer_asm:
	 	;rdi = image
	 	;rsi = image_width
	 	;rdx = image_height
	 	push rbp
		mov rbp, rsp
		push rax
		push rbx
		movdqu xmm0, [white]
		mov rax, rdi

		xor r8, r8 ; row
	loop_1:	
		xor r9, r9 ; col
	loop_2:

		
		; mov rbx, r8
		; mul rbx, rsi
		; add rbx, r9
		; shl rbx, 4
		; add rax, rbx

		
		movdqu [rax], xmm0

		add rax, 16
		inc r9
		cmp r9, rsi
		jne loop_2

		inc r8
		cmp r8, rdx
		jne loop_1

		pop rbx
		pop rax
		leave
		ret

section		.data

white:
		dd 1.0e1
		dd 1.0e1
		dd 1.0e1
		dd 0.0e1