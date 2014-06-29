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

; @file boot.asm
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

[cpu 686]
[bits 32]

; multiboot spec
MBALIGN equ 1<<0
MEMINFO equ 1<<1

FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1badb002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
	align 4
		dd MAGIC
		dd FLAGS
		dd CHECKSUM

[extern ckMain]

%include "memory_map.inc"

PageFlag equ 2 | 1		; r/w, present

section .setup
	[global _start]
	_start:
		cli		; 인터럽트 금지
		cld		; DF = 0
		mov esp, KERNEL_STACK_END_PHYSICAL_ADDR

		;;; higher half 페이징 초기화
		;;; misc/docs/paging_map.txt

		;; page table 초기화
		; unused lower 1MB [T#0x00000 ~ T#0x00100)
		mov edi, PAGE_TABLE_ADDRESS
		mov ecx, 0x100
		xor eax, eax
		rep stosd
		; dynamic memory [T#0x00100 ~ T#0xbf800)
		mov edx, DYN_MEMORY_START_ADDRESS | PageFlag
		mov ecx, 0xbf800 - 0x00100
		call PageTableInit
		mov edi, eax
		; memory for I/O mapping [T#0xbf800 ~ T#0xfe000)
		mov ecx, 0xfe000 - 0xbf800
		xor eax, eax
		rep stosd
		; kernel area [T#0xfe000 ~ T#0xfe800)
		mov edx, KERNEL_BASE_ADDRESS | PageFlag
		mov ecx, 0xfe800 - 0xfe000
		call PageTableInit
		;mov edi, eax		; 초기화 완료

		;; page directory 초기화
		; [D#0x000 ~ D#0x3fa]
		mov edi, PAGE_DIRECTORY_ADDRESS
		mov edx, PAGE_TABLE_ADDRESS | PageFlag
		mov ecx, 0x3fa
		call PageTableInit
		mov edi, eax
		; [D#0x3fa ~ D#0x3ff]
		mov ecx, 0x400 - 0x3fa
		xor eax, eax
		rep stosd
		;mov edi, eax		; 초기화 완료

		;; paging enable
		; identity paging in [0x00100000 - 0x00101000) [T#0x00100]
		; * 링커 스크립트로 _start를 [0x00100000 - 0x00101000)에 배치해야 함
		mov edx, 0x00100000 | PageFlag
		xchg dword [PAGE_TABLE_ADDRESS + 0x00100 * 4], edx	; 원래 값을 edx에 저장
		; paging 활성화
		mov eax, PAGE_DIRECTORY_ADDRESS
		mov cr3, eax
		mov eax, cr0
		or eax, 0x80000000
		mov cr0, eax
		; absolute jmp - higher half에 진입
		mov ecx, higher_half
		jmp ecx ; why?
	higher_half:
		; [0x00100000 - 0x00101000)의 identity paging을 원래대로 되돌림
		mov dword [PAGE_TABLE_LOGICAL_ADDRESS + 0x00100 * 4], edx
		invlpg [0x00100000]

		;;; 스택 설정
		mov esp, KERNEL_STACK_END_LOGICAL_ADDR

		;;; text mode video memory [0xb8000 - 0xb8fa0]를 I/O mapping을 위한 memory에 map
		mov ebx, PAGE_TABLE_LOGICAL_ADDRESS + 0xbf800 * 4	; page table의 logical address
		mov dword [ebx], 0xb8000 | PageFlag
		invlpg [IOMAP_MEMORY_START_ADDRESS]

		;;; clear the screen
		mov edi, 0xbf800000
		mov ecx, (80 * 25 * 2) / 4
		xor eax, eax
		rep stosd

		;;; check for cpuid
		pushfd
		pop eax				; ecx = eax = eflag;
		mov ecx, eax
		xor eax, 0x200000	; eax ^= 0x200000;
		push eax
		popfd				; eflag = eax;
		pushfd
		pop eax				; eax = eflag;
		xor eax, ecx		; eax ^= ecx; ( ZF if eax == ecx )
		jz no_cpuid			; if (ZF) goto no_cpuid
		push ecx
		popfd				; eflag = ecx

		; 끝 - C 커널 실행
		call ckMain

	stop:
		cli
		hlt
		jmp $-2

;; subroutine ;;
	; edi = table address, edx = init data, ecx = count
	; clobber register: eax, edx, ecx
	; return value(eax): end of table
	PageTableInit:
		mov eax, edi
	PageTableInit_loop:
		mov dword [eax], edx
		add edx, 4 * 1024
		add eax, 4
		loop PageTableInit_loop
		ret

	no_cpuid:
		mov esi, no_cpuid_msg
		mov edi, 0xbf800000
	err_msg_loop:
		mov al, [edi]
		test al, al
		jz stop
		mov byte [esi], al
		inc esi
		mov byte [esi], 0x04
		inc esi
		inc edi
		jmp err_msg_loop

section .rodata
	no_cpuid_msg db "Clubcos doesn't support this cpu ^o^", 0

