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
 * @file pic.c
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "pic.h"
#include "port.h"

void ckPicInitialize(void)
{
	ckPortOutByte(PIC_MASTER_PORT1, 0x11);
	ckPortOutByte(PIC_MASTER_PORT2, PIC_INTERRUPT_NUMBER);
	ckPortOutByte(PIC_MASTER_PORT2, 0x04);
	ckPortOutByte(PIC_MASTER_PORT2, 0x01);	// 수동 EOI

	ckPortOutByte(PIC_SLAVE_PORT1, 0x11);
	ckPortOutByte(PIC_SLAVE_PORT2, PIC_INTERRUPT_NUMBER + 8);
	ckPortOutByte(PIC_SLAVE_PORT2, 0x02);
	ckPortOutByte(PIC_SLAVE_PORT2, 0x01);	// 수동 EOI

	ckPicMaskInterrupt(~PIC_MASKBIT_SLAVE);
}

void ckPicMaskInterrupt(uint16_t mask)
{
	ckPortOutByte(PIC_MASTER_PORT2, (uint8_t)mask);
	ckPortOutByte(PIC_SLAVE_PORT2, (uint8_t)(mask >> 8));
}

void ckPicSendEOI(uint8_t irq)
{
	if (irq < 8)
	{
		ckPortOutByte(PIC_MASTER_PORT1, 0x60 | irq);
	}
	else
	{
		ckPortOutByte(PIC_MASTER_PORT1, 0x60 | PIC_IRQ_SLAVE);
		ckPortOutByte(PIC_SLAVE_PORT1, 0x60 | (irq - 8));
	}
}
