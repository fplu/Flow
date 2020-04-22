
extern initIncRip


global pContext
;global pContext2
global pStack2
global pDocker
global addressIncRip

global traceLoopData
global traceLoopPreStart
global traceLoopStart
global traceLoopTraceINSTRUCTION
global traceLoopEnd

global START_SAVE_CONTEXT
global START_LOAD_CONTEXT

;global saveRsp
;global loadR10
;global saveR15
;global loadR15

global ADDRESS_PTR
global NEW_CONTEXT_PTR
global START_LOAD_COMPLETE_CONTEXT
global END_LOAD_COMPLETE_CONTEXT

global START_callback
global JMP_RET_callback
global JMP_PTR_callback
global END_callback



global FAST_MEM_CLEAR

%include "_macros.hasm"



	align 8	
	traceLoopData:
		pContext : dq 0
		pStack2 : dq 0
		pDocker : dq 0
		addressIncRip : dq 0;or address callback

		saveRsp : dq 0
		saveR15 : dq 0
		ADDRESS_PTR : dq 0
		NEW_CONTEXT_PTR : dq 0

	traceLoopPreStart:
	traceLoopStart:

		START_SAVE_CONTEXT:
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			;db 0, 0, 0, 0, 0, 0
			;db 0, 0, 0, 0, 0, 0
		
		mov rsp, [rel pContext]
		lea rsp, [rsp + THREAD_CONTEXT.EFlags + 8]
		pushfq

		lea rax, [rel END_LOAD_COMPLETE_CONTEXT]
		mov QWORD[rel ADDRESS_PTR], rax

		mov rsp, [rel pStack2]
		mov rcx, [rel pDocker]
		mov rax, [rel addressIncRip]

		sub rsp, 10h
		add rsp, 9000h

		;PUSH_REGISTER_XMM_VOLATILE
			sub rsp, 20h
				call rax
			add rsp, 20h
		;POP_REGISTER_XMM_VOLATILE

		mov rsp, [rel pContext]
		lea rsp, [rsp + THREAD_CONTEXT.EFlags]
		popfq

		START_LOAD_CONTEXT:
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			db 0, 0, 0, 0, 0, 0, 0
			;db 0, 0, 0, 0, 0, 0


			jmp [rel ADDRESS_PTR]
	
			START_LOAD_COMPLETE_CONTEXT:
			mov rcx, [rel NEW_CONTEXT_PTR]

			movdqa xmm6, [rcx + CONTEXT.Xmm6]
			movdqa xmm7, [rcx + CONTEXT.Xmm7]
			movdqa xmm8, [rcx + CONTEXT.Xmm8]
			movdqa xmm9, [rcx + CONTEXT.Xmm9]
			movdqa xmm10, [rcx + CONTEXT.Xmm10]
			movdqa xmm11, [rcx + CONTEXT.Xmm11]
			movdqa xmm12, [rcx + CONTEXT.Xmm12]
			movdqa xmm13, [rcx + CONTEXT.Xmm13]
			movdqa xmm14, [rcx + CONTEXT.Xmm14]
			movdqa xmm15, [rcx + CONTEXT.Xmm15]
			mov ds, [rcx + CONTEXT.SegDs]
			mov es, [rcx + CONTEXT.SegEs]
			mov fs, [rcx + CONTEXT.SegFs]
			mov gs, [rcx + CONTEXT.SegGs]
			mov ss, [rcx + CONTEXT.SegSs]
			mov Rbx, [rcx + CONTEXT.Rbx]
			mov Rbp, [rcx + CONTEXT.Rbp]
			mov Rsi, [rcx + CONTEXT.Rsi]
			mov Rdi, [rcx + CONTEXT.Rdi]
			mov R12, [rcx + CONTEXT.R12]
			mov R13, [rcx + CONTEXT.R13]
			mov R14, [rcx + CONTEXT.R14]
			mov R15, [rcx + CONTEXT.R15]			
			mov Rcx, [rcx + CONTEXT.Rcx]
		END_LOAD_COMPLETE_CONTEXT :

	

		nop
	traceLoopTraceINSTRUCTION:
	traceLoopEnd:
	
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0

	jmp traceLoopStart




	START_callback :
		;db 0CCh
		PUSH_REGISTER_ENTIER_VOLATILE
		PUSH_REGISTER_XMM_VOLATILE

		call [rel JMP_PTR_callback]

		POP_REGISTER_XMM_VOLATILE
		POP_REGISTER_ENTIER_VOLATILE
	
		JMP_RET_callback :
			db 0E9h
			db 090h
			db 090h
			db 090h
			db 090h

		JMP_PTR_callback :
			dq 0

	END_callback :

