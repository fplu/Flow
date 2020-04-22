


global START_CALL_INSIDE_THREAD
global MOV_RAX_CALL
global MOV_RCX_ARG1
global MOV_RDX_ARG2
global END_CALL_INSIDE_THREAD

global START_REMOTE_GET_PROC_ADDRESS
global MOV_RAX_GetModuleHandleA
global MOV_RAX_GetProcAddress
global MOV_RCX_START_REMOTE_GET_PROC_ADDRESS
global END_REMOTE_GET_PROC_ADDRESS


section .text ; makes this executable
	USE64


	%macro PUSH_REGISTER_ENTIER_CALLEE_SAFE 0
		push rdx
		push rbp
		push rsi
		push rdi
		push r12
		push r13
		push r14
		push r15
	%endmacro

	%macro PUSH_REGISTER_ENTIER_VOLATILE 0
		pushfq
		push rax
		push rcx
		push rdx
		push r8
		push r9
		push r10
		push r11
	%endmacro

	%macro PUSH_REGISTER_XMM_CALLEE_SAFE 0
		sub     rsp, 10h
		movdqu  [rsp], xmm6
		sub     rsp, 10h
		movdqu  [rsp], xmm7
		sub     rsp, 10h
		movdqu  [rsp], xmm8
		sub     rsp, 10h
		movdqu  [rsp], xmm9
		sub     rsp, 10h
		movdqu  [rsp], xmm10
		sub     rsp, 10h
		movdqu  [rsp], xmm11
		sub     rsp, 10h
		movdqu  [rsp], xmm12
		sub     rsp, 10h
		movdqu  [rsp], xmm13
		sub     rsp, 10h
		movdqu  [rsp], xmm14
		sub     rsp, 10h
		movdqu  [rsp], xmm15
	%endmacro

	%macro PUSH_REGISTER_XMM_VOLATILE 0
		sub     rsp, 10h
		movdqu  [rsp], xmm0
		sub     rsp, 10h
		movdqu  [rsp], xmm1
		sub     rsp, 10h
		movdqu  [rsp], xmm2
		sub     rsp, 10h
		movdqu  [rsp], xmm3
		sub     rsp, 10h
		movdqu  [rsp], xmm4
		sub     rsp, 10h
		movdqu  [rsp], xmm5
	%endmacro

	%macro POP_REGISTER_ENTIER_CALLEE_SAFE 0
		pop r15
		pop r14
		pop r13
		pop r12
		pop rdi
		pop rsi
		pop rbp
		pop rdx
	%endmacro

	%macro POP_REGISTER_ENTIER_VOLATILE 0
		pop r11
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		pop rax
		popfq
	%endmacro

	
	%macro POP_REGISTER_XMM_CALLEE_SAFE 0
		movdqu  xmm15, [rsp]
		add rsp, 10h
		movdqu  xmm14, [rsp]
		add rsp, 10h
		movdqu  xmm13, [rsp]
		add rsp, 10h
		movdqu  xmm12, [rsp]
		add rsp, 10h
		movdqu  xmm11, [rsp]
		add rsp, 10h
		movdqu  xmm10, [rsp]
		add rsp, 10h
		movdqu  xmm9, [rsp]
		add rsp, 10h
		movdqu  xmm8, [rsp]
		add rsp, 10h
		movdqu  xmm7, [rsp]
		add rsp, 10h
		movdqu  xmm6, [rsp]
		add rsp, 10h
	%endmacro

	%macro POP_REGISTER_XMM_VOLATILE 0
		movdqu  xmm5, [rsp]
		add rsp, 10h
		movdqu  xmm4, [rsp]
		add rsp, 10h
		movdqu  xmm3, [rsp]
		add rsp, 10h
		movdqu  xmm2, [rsp]
		add rsp, 10h
		movdqu  xmm1, [rsp]
		add rsp, 10h
		movdqu  xmm0, [rsp]
		add rsp, 10h
	%endmacro

	START_CALL_INSIDE_THREAD:

		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP

		PUSH_REGISTER_ENTIER_VOLATILE
		PUSH_REGISTER_ENTIER_CALLEE_SAFE
		PUSH_REGISTER_XMM_VOLATILE
		PUSH_REGISTER_XMM_CALLEE_SAFE

			;START mov rax, call
				db 048h
				db 0B8h
				MOV_RAX_CALL:
				dq 0
			;END mov rax, call
			;START mov rcx, arg1
				db 048h
				db 0B9h
				MOV_RCX_ARG1:
				dq 0
			;END mov rcx, arg1
			;START mov rdx, arg2
				db 048h
				db 0BAh
				MOV_RDX_ARG2:
				dq 0
			;END mov rdx, arg2

				mov r15, rsp
				sub rsp, 28h
				and rsp, 0FFFFFFFFFFFFFFF0h
				call rax
				mov rsp, r15

		POP_REGISTER_XMM_CALLEE_SAFE
		POP_REGISTER_XMM_VOLATILE
		POP_REGISTER_ENTIER_CALLEE_SAFE
		POP_REGISTER_ENTIER_VOLATILE
	ret


	END_CALL_INSIDE_THREAD:

	START_REMOTE_GET_PROC_ADDRESS:
		dq 9090909090909090h
		
		sub rsp, 8
		push rdx
		
		;START mov rax, GetModuleHandleA
			db 048h
			db 0B8h
			MOV_RAX_GetModuleHandleA
			dq 0
		;START mov rax, GetModuleHandleA

		sub rsp, 100h
			call rax
		add rsp, 100h

		pop rdx
		add rsp, 8

		mov rcx, rax
		;START mov rax, GetProcAddress
			db 048h
			db 0B8h
			MOV_RAX_GetProcAddress
			dq 0
		;START mov rax, GetProcAddress
		sub rsp, 100h
			call rax
		add rsp, 100h
		
		;START mov rcx, START_REMOTE_GET_PROC_ADDRESS
			db 048h
			db 0B9h
			MOV_RCX_START_REMOTE_GET_PROC_ADDRESS
			dq 0
		;START mov rcx, START_REMOTE_GET_PROC_ADDRESS
		mov qword [rcx], rax
	ret
	END_REMOTE_GET_PROC_ADDRESS:

