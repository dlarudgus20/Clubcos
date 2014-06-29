/*
 * Copyright (c) 2014, 임경현 (dlarudgus20)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * @file link_script.ld
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

/* 엔트리 포인트 */
ENTRY(_start)

SECTIONS
{
	. += 0xfe000000;

	.text ALIGN(4K) : AT(ADDR(.text) + 0x00100000 - 0xfe000000)
	{
		/* 먼저 .multiboot 섹션과 .setup 섹션을 넣고 .text 섹션을 넣습니다. */
		*(.multiboot)
		*(.setup)
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) + 0x00100000 - 0xfe000000)
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.rodata1)
	}

	.data ALIGN(4K) : AT(ADDR(.data) + 0x00100000 - 0xfe000000)
	{
		*(.data .data.* .gnu.linkonce.d.*)
		SORT(CONSTRUCTORS)
	}

	__bss_start = .;
	.bss ALIGN(4K) : AT(ADDR(.bss) + 0x00100000 - 0xfe000000)
	{
		*(COMMON)
		*(.bss .bss.* .gnu.linkonce.b.*)
	}
}
