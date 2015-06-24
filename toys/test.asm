global vector_sum

vector_sum:
	
	PUSH RBP
	MOV RBP, RSP
	PUSH RBX
	; RDI ES destination
	; RSI ES A
	; RDX es b

	MOVDQU XMM0, [RSI]
	ADDPS XMM0, [RDX]
	MOVDQU [RDI], XMM0

	POP RBX
	POP RBP
	RET
