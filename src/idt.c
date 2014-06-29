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
 * @file idt.c
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "string.h"
#include "idt.h"
#include "gdt.h"
#include "interrupt.h"

void ckIdtTableInitialize(void)
{
	for (unsigned i = 0; i < 256; i++)
	{
		ckIdtInit(g_pIdtTable + i, ckUnknownIntHandler, KERNEL_CODE_SEGMENT, 0);
	}

	// typedef void (*pf)();
	// static const pf handlers[] = {
	static void ( * const handlers[])() = {
		ckExceptIntHandler00,
		ckExceptIntHandler01,
		ckExceptIntHandler02,
		ckExceptIntHandler03,
		ckExceptIntHandler04,
		ckExceptIntHandler05,
		ckExceptIntHandler06,
		ckExceptIntHandler07,
		ckExceptIntHandler08,
		ckExceptIntHandler09,
		ckExceptIntHandler0A,
		ckExceptIntHandler0B,
		ckExceptIntHandler0C,
		ckExceptIntHandler0D,
		ckExceptIntHandler0E,
		ckExceptIntHandler0F,
		ckExceptIntHandler10,
		ckExceptIntHandler11,
		ckExceptIntHandler12,
		ckExceptIntHandler13,
		ckExceptIntHandler14,
	};
	for (unsigned i = 0; i < sizeof(handlers) / sizeof(handlers[0]); i++)
	{
		ckIdtInit(g_pIdtTable + i, handlers[i], KERNEL_CODE_SEGMENT, 0);
	}

	ckIdtLoad(256 * sizeof(Idt), g_pIdtTable);
}

void ckIdtInit(Idt *pIdt, void (*HandlerAddress)(), uint16_t HandlerSegment, uint8_t DPL)
{
	pIdt->HandlerAddress_0_15 = (uint16_t)((uint32_t)HandlerAddress);
	pIdt->HandlerAddress_16_31 = (uint16_t)((((uint32_t)HandlerAddress) >> 16) & 0xffff);

	pIdt->HandlerSegment = HandlerSegment;

	pIdt->ReservedZero1 = 0;
	pIdt->ReservedZero2 = 0;
	pIdt->ReservedZero3 = 0;

	pIdt->ReservedOne1 = 1;
	pIdt->ReservedOne2 = 1;

	pIdt->D = 1;
	pIdt->P = 1;

	pIdt->DPL = DPL;
}
