global vector_sum

vector_sum:
	
	PUSH RBP
	MOV RBP, RSP
	PUSH RBX
	; RDI ES destination
	; RSI ES A
	; RDX es b

	MOVDQU XMM0, [RDX]
	ADDPD XMM0, [RCX]
	MOVDQU [RSI], XMM0

	MOVDQU XMM1, [RDX + 16]
	ADDPD XMM1, [RCX + 16]
	MOVDQU [RSI + 16], XMM1

	POP RBX
	POP RBP
	RET
