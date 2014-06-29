// Copyright (c) 2014, 임경현 (dlarudgus20)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file gdt.c
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "string.h"
#include "gdt.h"
#include "port.h"

void ckGdtTableInitialize(void)
{
	memset(g_pGdtTable, 0, 8192 * sizeof(Gdt));

	// 커널 세그먼트 디스크럽터
	ckGdtInit(g_pGdtTable + 1, 0x00000000, 0xffffffff, MAKE_GDT_TYPE1(DATA_SEGMENT_TYPE, 1, 0, 1));
	ckGdtInit(g_pGdtTable + 2, 0x00000000, 0xffffffff, MAKE_GDT_TYPE1(CODE_SEGMENT_TYPE, 1, 0, 1));

	ckGdtLoad(0xffff, g_pGdtTable);

	// 세그먼트 셀렉터 재로드
	__asm__ __volatile__
	(
		"mov %0, %%eax				\n\t"
		"movw %%ax, %%ds			\n\t"
		"movw %%ax, %%es			\n\t"
		"movw %%ax, %%fs			\n\t"
		"movw %%ax, %%gs			\n\t"
		"movw %%ax, %%ss			\n\t"
		"ljmp %1, $cs_update		\n\t"
		"cs_update:					\n\t"
		:
		: "Z"(KERNEL_DATA_SEGMENT), "Z"(KERNEL_CODE_SEGMENT)
		: "eax"
	);
}

void ckGdtInit(Gdt *pGdt, uint32_t address, uint32_t size, GdtType1 type1)
{
	uint32_t Size20bit;

	pGdt->Address_0_15 = (uint16_t) address;
	pGdt->Address_16_23 = (uint8_t)((address >> 16) & 0xff);
	pGdt->Address_24_32 = (uint8_t)((address >> 24) & 0xff);

	if(size >= 0xfffff)
	{
		pGdt->G = 1;
		Size20bit = size >> 12;
	}
	else
	{
		pGdt->G = 0;
		Size20bit = size;
	}
	pGdt->Size_0_15 = (uint16_t) Size20bit;
	pGdt->Size_16_19 = (uint16_t)((Size20bit >> 16) & 0x0f);

	pGdt->type1 = type1;

	pGdt->AVL = 0;
	pGdt->reserved = 0;
	pGdt->D = 1;
}

