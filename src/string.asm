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

; @file string.asm
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

[cpu 686]
[bits 32]

section .text
	[global _memcpy_forward]	; void *_memcpy_forward(void *dest, const void *src, size_t count);
	_memcpy_forward:
		push edi
		push esi

		mov edi, [esp + 8 + 4]
		mov esi, [esp + 8 + 8]
		mov ecx, [esp + 8 + 12]
		mov edx, edi

		mov eax, ecx
		and eax, 3
		shr ecx, 2

		cld
		rep movsd
		mov ecx, eax
		rep movsb

		mov eax, edx
		pop esi
		pop edi
		ret

	[global _memcpy_reverse]	; void *_memcpy_reverse(void *dest, const void *src, size_t count);
	_memcpy_reverse:
		push edi
		push esi

		mov edi, [esp + 8 + 4]
		mov esi, [esp + 8 + 8]
		mov ecx, [esp + 8 + 12]
		mov edx, edi

		mov eax, ecx
		dec eax
		add esi, eax
		add edi, eax

		inc eax
		shr eax, 2
		and ecx, 3

		std
		rep movsb
		mov ecx, eax
		sub esi, 3
		sub edi, 3
		rep movsd
		cld			; DF가 항상 0이 되어아 하는 것 같다.

		mov eax, edx
		pop esi
		pop edi
		ret

	[global memset]				; void *memset(void *dest, int ch, size_t count);
	memset:
		push edi
		push ebx

		mov edi, [esp + 8 + 4]
		mov eax, [esp + 8 + 8]
		mov ecx, [esp + 8 + 12]
		mov ebx, edi

		mov ah, al
		mov edx, eax
		shl eax, 16
		or eax, edx

		mov edx, ecx
		shr ecx, 2
		cld
		rep stosd

		mov ecx, edx
		and ecx, 3
		rep stosb

		mov eax, ebx
		pop ebx
		pop edi
		ret
	[global _memset_2]			; void *_memset_2(void *dest, int ch, size_t count);
	_memset_2:
		push edi
		push ebx

		mov edi, [esp + 8 + 4]
		mov eax, [esp + 8 + 8]
		mov ecx, [esp + 8 + 12]
		mov ebx, edi

		mov edx, eax
		shl eax, 16
		or eax, edx

		mov edx, ecx
		shr ecx, 1
		cld
		rep stosd

		mov ecx, edx
		and ecx, 1
		rep stosw

		mov eax, ebx
		pop ebx
		pop edi
		ret

	[global memchr]				; void *memchr(const void *ptr, int ch, size_t count);
	memchr:
		push edi

		mov edi, [esp + 4 + 4]
		mov eax, [esp + 4 + 8]
		mov ecx, [esp + 4 + 12]

		cld
		repne scasb
		jne memchr_nofound

		mov eax, edi
		dec eax
		pop edi
		ret
	memchr_nofound:
		mov eax, 0
		pop edi
		ret

	[global _memchr_4]				; void *_memchr_4(const void *ptr, int ch, size_t count);
	_memchr_4:
		push edi

		mov edi, [esp + 4 + 4]
		mov eax, [esp + 4 + 8]
		mov ecx, [esp + 4 + 12]

		cld
		repne scasd
		jne memchr_nofound

		mov eax, edi
		sub eax, 4
		pop edi
		ret
	_memchr_4_nofound:
		mov eax, 0
		pop edi
		ret

