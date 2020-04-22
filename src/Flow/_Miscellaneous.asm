global GetGs
global clearTrapFlag

section .text ; makes this executable
	USE64


clearTrapFlag:
	PUSHF
		AND word [rsp], 0FEFFh
	POPF
	ret

		
GetGs :
	mov rax, gs
	ret
