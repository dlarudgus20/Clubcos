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

; @file spinlock.asm
; @brief misc assembly functions
; @date 2015. 11. 10.
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

[cpu 686]
[bits 32]

section .text
	[global ckSpinlockLock]		; void ckSpinlockLock(Spinlock *psl);
	ckSpinlockLock:
		mov eax, [esp + 4]

	.start
		lock bts dword [eax], 0
		jc .spin_with_pause
		ret

	.spin_with_pause:
		pause
		test dword [eax], 1
		jnz .spin_with_pause
		jmp .start

	[global ckSpinlockUnlock]	; void ckSpinlockUnlock(Spinlock *psl);
	ckSpinlockUnlock:
		mov eax, [esp + 4]
		mov dword [eax], 0
		ret
