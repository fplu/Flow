;extern Beep 
global START_INJECTOR_
global END_INJECTOR_
global RetAddress_
global GetModuleHandle_
global GetProcAddress_
global LoadLibrary_
;global Beep_
global DLL_PATHS_LENGTH
global DLL_PATHS

%include "_macros.hasm"

section .text ; makes this executable
	USE64


	START_INJECTOR_:

	PUSH_REGISTER_ENTIER_CALLEE_SAFE
	PUSH_REGISTER_ENTIER_VOLATILE
	PUSH_REGISTER_XMM_CALLEE_SAFE
	PUSH_REGISTER_XMM_VOLATILE
	mov rbp, rsp
	add rsp, 010h
	and rsp, 0FFFFFFFFFFFFFFF0h
	
	;	db 0CCh

	;	mov rcx, 1000
	;	mov rdx, 1000
	;	call [rel Beep_]

	


		lea r14, [rel LoadLibrary_]

		movsxd r12, [rel DLL_PATHS_LENGTH]
		mov r13, 0
		loadLibraryLoop :

			lea rcx, [rel DLL_PATHS]
			lea rcx, [rcx + r13]
				sub rsp, 20h
				call [r14]
				add rsp, 20h

		add r13, 1000
		dec r12
		jnz loadLibraryLoop
		
;		lea rax, [rel LoadLibrary_]
;		lea rcx, [rel DLL1_PATH]
;			sub rsp, 20h
;			call [rax]
;			add rsp, 20h
;
;		lea rax, [rel LoadLibrary_]
;		lea rcx, [rel DLL2_PATH]
;			sub rsp, 20h
;			call [rax]
;			add rsp, 20h

		lea rax, [rel GetModuleHandle_]
		lea rcx, [rel DLL1_NAME]
			sub rsp, 20h
			call [rax]
			add rsp, 20h

		mov r12, rax

		mov rcx, r12
		lea rax, [rel GetProcAddress_]
		lea rdx, [rel FUNC1_NAME]
			sub rsp, 20h
			call [rax]
			add rsp, 20h


		movsxd rdx, [rel DLL_PATHS_LENGTH]
;		dec rdx
;		imul rbx, rbx, 1000
		lea rcx, [rel DLL_PATHS]
;		lea rcx, [rcx + rbx]
;		lea rcx, [rel DLL2_PATH]
			sub rsp, 20h
			call rax
			add rsp, 20h	

		mov rcx, r12
		lea rax, [rel GetProcAddress_]
		lea rdx, [rel FUNC2_NAME]
			sub rsp, 20h
			call [rax]
			add rsp, 20h

			sub rsp, 20h
			call rax
			add rsp, 20h	

			sub rsp, 20h
			call rax
			add rsp, 20h	


	mov rsp, rbp
	POP_REGISTER_XMM_VOLATILE
	POP_REGISTER_XMM_CALLEE_SAFE
	POP_REGISTER_ENTIER_VOLATILE
	POP_REGISTER_ENTIER_CALLEE_SAFE
	jmp [rel RetAddress_]
	
	RetAddress_ : dq 0
	GetModuleHandle_ : dq 0
	GetProcAddress_ : dq 0
	LoadLibrary_ : dq 0
	;Beep_ : dq 0
	DLL1_NAME	:	dw __utf16__('Flow.dll'), 0
	FUNC1_NAME	:	db 'globalFlowInit', 0
	FUNC2_NAME	:	db 'TAKE_THIS', 0
	DLL_PATHS_LENGTH	:	dd 0
	DLL_PATHS	: resw 5000
END_INJECTOR_:
