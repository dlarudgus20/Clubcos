; Copyright (c) 2014, 임경현 (dlarudgus20)
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; * Redistributions of source code must retain the above copyright notice, this
;   list of conditions and the following disclaimer.
;
; * Redistributions in binary form must reproduce the above copyright notice,
;   this list of conditions and the following disclaimer in the documentation
;   and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; @file asmfunc.asm
; @brief misc assembly functions
; @date 2015. 11. 8.
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

[cpu 686]
[bits 32]

[extern _boot_stackframe_top]

section .text
	[global ckStackDump]		; void ckStackDump(void (*pfCallback)(void *fn, uint32_t num), uint32_t skip, uint32_t limit);
	ckStackDump:
		push ebp
		mov ebp, esp
		push ebp

		push edi
		push ebx
		push esi

		mov edi, [ebp + 8]	; pfCallback
		mov edx, [ebp + 12]	; skip
		mov esi, [ebp + 16]	; limit

		xor ecx, ecx
		mov ebx, ebp
		sub ebx, 4

	.loop:
		mov ebx, [ebx]
		mov eax, [ebx + 4]

		or edx, edx
		jz .cont
		dec edx
		jmp .loop

	.cont:
		cmp eax, _boot_stackframe_top
		je .break
		cmp eax, 0
		je .break

		push eax
		push ecx
		push edx

		push ecx
		push eax
		call edi
		add esp, 8

		pop edx
		pop ecx
		pop eax

		inc ecx
		cmp ecx, esi
		jbe .loop

	.break:
		pop esi
		pop ebx
		pop edi

		add esp, 4
		leave
		ret
