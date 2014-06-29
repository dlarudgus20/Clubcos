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
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

[cpu 686]
[bits 32]

section .text
	; gdt.h
	[global ckGdtLoad]			; void ckGdtLoad(uint16_t size, Gdt *address);
	ckGdtLoad:
		mov ax, [esp + 4]
		mov [esp + 6], ax
		lgdt [esp + 6]
		ret

	; idt.h
	[global ckIdtLoad]			; void ckIdtLoad(uint16_t size, Idt *address);
	ckIdtLoad:
		mov ax, [esp + 4]
		mov [esp + 6], ax
		lidt [esp + 6]
		ret

	; port.h
	[global ckAsmFarJmp]		; void ckAsmFarJmp(uint32_t eip, uint16_t cs);
	ckAsmFarJmp:
		jmp far [esp + 4]
		ret
