GLOBAL _getAllRegister

%include "_macros.hasm"


section .text ; makes this executable
	USE64


_getAllRegister:

	movdqa [rcx + CONTEXT.Xmm6], xmm6
	movdqa [rcx + CONTEXT.Xmm7], xmm7
	movdqa [rcx + CONTEXT.Xmm8], xmm8
	movdqa [rcx + CONTEXT.Xmm9], xmm9
	movdqa [rcx + CONTEXT.Xmm10], xmm10
	movdqa [rcx + CONTEXT.Xmm11], xmm11
	movdqa [rcx + CONTEXT.Xmm12], xmm12
	movdqa [rcx + CONTEXT.Xmm13], xmm13
	movdqa [rcx + CONTEXT.Xmm14], xmm14
	movdqa [rcx + CONTEXT.Xmm15], xmm15


	mov [rcx + CONTEXT.SegCs], cs
	mov [rcx + CONTEXT.SegDs], ds
	mov [rcx + CONTEXT.SegEs], es
	mov [rcx + CONTEXT.SegFs], fs
	mov [rcx + CONTEXT.SegGs], gs
	mov [rcx + CONTEXT.SegSs], ss

ret

	
