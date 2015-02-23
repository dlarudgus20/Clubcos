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

; @file interrupt.asm
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

[cpu 686]
[bits 32]

segment .text

%macro PushRegisters 0
		pushad
		;Temp = esp
		;push eax
		;push ecx
		;push edx
		;push ebx
		;push Temp
		;push ebp
		;push esi
		;push edi
		push ds
		push es
		push fs
		push gs

		mov ax, 1 * 8	; Kernel Data Segment Selector
		mov ds, ax
		mov es, ax
		mov gs, ax
		mov fs, ax
%endmacro
%macro PopRegisters 0
		pop gs
		pop fs
		pop es
		pop ds
		;pop edi
		;pop esi
		;pop ebp
		;add esp, 4 ; for "push esp"
		;pop ebx
		;pop edx
		;pop ecx
		;pop eax
		popad
%endmacro

%macro MakeIntHandler 1
	[extern ck_%1]
	[global ck%1]
	ck%1:
		PushRegisters

		push esp
		call ck_%1
		add esp, 4

		PopRegisters
		iretd
%endmacro

%macro MakeIntHandlerWithErr 1
	[extern ck_%1]
	[global ck%1]
	ck%1:
		PushRegisters

		push esp
		call ck_%1
		add esp, 4 * 2 ; "push esp" + error code

		PopRegisters
		iretd
%endmacro

	MakeIntHandler			ExceptIntHandler00	; Divide-by-Zero
	MakeIntHandler			ExceptIntHandler01	; Debug
	MakeIntHandler			ExceptIntHandler02	; NMI
	MakeIntHandler			ExceptIntHandler03	; Breakpoint
	MakeIntHandler			ExceptIntHandler04	; Overflow
	MakeIntHandler			ExceptIntHandler05	; Bound Range Exceeded
	MakeIntHandler			ExceptIntHandler06	; Invalid Opcode
	MakeIntHandler			ExceptIntHandler07	; Device Not Available
	MakeIntHandlerWithErr	ExceptIntHandler08	; Double Fault
	MakeIntHandler			ExceptIntHandler09	; Coprocessor Segment Overrun
	MakeIntHandlerWithErr	ExceptIntHandler0A	; Invalid TSS
	MakeIntHandlerWithErr	ExceptIntHandler0B	; Segment Not Present
	MakeIntHandlerWithErr	ExceptIntHandler0C	; Stack-Segment Fault
	MakeIntHandlerWithErr	ExceptIntHandler0D	; General Protection Fault
	MakeIntHandlerWithErr	ExceptIntHandler0E	; Page Fault
	MakeIntHandler			ExceptIntHandler0F	; (reserved)
	MakeIntHandler			ExceptIntHandler10	; x87 Floating-Point Exception
	MakeIntHandlerWithErr	ExceptIntHandler11	; Alignment Check
	MakeIntHandler			ExceptIntHandler12	; Machine Check
	MakeIntHandler			ExceptIntHandler13	; SIMD Floating-Point Exception
	MakeIntHandler			ExceptIntHandler14	; Virtualization Exception
	MakeIntHandler UnknownIntHandler

	MakeIntHandler TimerIntHandler
	MakeIntHandler KeyboardIntHandler
	MakeIntHandler PATAPrimaryIntHandler
	MakeIntHandler PATASecondaryIntHandler

