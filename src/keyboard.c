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
 * @file keyboard.c
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include <stdbool.h>
#include "keyboard.h"
#include "keys.h"
#include "port.h"
#include "lock_system.h"
#include "interrupt.h"
#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "likely.h"

KeyboardStruct g_Keyboard = { true, false, false, false };

// 키보드 LED를 갱신합니다. ACK를 기다릴 때 주어진 함수를 이용합니다.
static bool csKeyboardUpdateLED_internal(bool (*WaitForACK)(void));

bool ckKeyboardInitialize(void)
{
	ckPortOutByte(KEYBOARD_PORT_CONTROL, KEYBOARD_CONTROL_ACTIVATE_KEYBOARD);
	ckKeyboardWaitForInput();
	ckPortOutByte(KEYBOARD_PORT_INPUT, KEYBOARD_COMMAND_ACTIVATE);
	if (unlikely(!ckKeyboardWaitForACK()))
		return false;

	csKeyboardUpdateLED_internal(ckKeyboardWaitForACK);	// LED 조정은 실패해도 그리 큰 상관 없음

	ckIdtInit(g_pIdtTable + PIC_INTERRUPT_KEYBOARD, ckKeyboardIntHandler, KERNEL_CODE_SEGMENT, 0);

	return true;
}

bool ckKeyboardUpdateLED(void)
{
	return csKeyboardUpdateLED_internal(ckKeyboardWaitForACKAndPutQueue);
}
static bool csKeyboardUpdateLED_internal(bool (*WaitForACK)(void))
{
	bool bRet = false;

	LockSystemObject lso;
	ckLockSystem(&lso);

	ckKeyboardWaitForInput();

	ckPortOutByte(KEYBOARD_PORT_INPUT, KEYBOARD_COMMAND_LED);
	ckKeyboardWaitForInput();
	if (likely(WaitForACK()))
	{
		uint8_t code = (g_Keyboard.bCapsLock << 2) | (g_Keyboard.bNumLock << 1) | g_Keyboard.bScrollLock;
		ckPortOutByte(KEYBOARD_PORT_INPUT, code);
		ckKeyboardWaitForInput();
		if (likely(WaitForACK()))
		{
			bRet = true;
		}
	}

	ckUnlockSystem(&lso);

	return bRet;
}

#define ACK_TRY_COUNT 100
bool ckKeyboardWaitForACK(void)
{
	for (int i = 0; i < ACK_TRY_COUNT; i++)
	{
		for (int j = 0; j < 0xffff; j++)	// 0xffff번 돌아도 비어있으면 무시하고 진행
		{
			if (ckKeyboardIsOutputBufferFull())
			{
				break;
			}
		}

		if (ckPortInByte(KEYBOARD_PORT_OUTPUT) == KEYBOARD_ACK)
			return true;
	}
	return false;
}
bool ckKeyboardWaitForACKAndPutQueue(void)
{
	uint8_t data;

	for (int i = 0; i < ACK_TRY_COUNT; i++)
	{
		for(int j = 0; j < 0xffff; j++)
		{
			if (ckKeyboardIsOutputBufferFull())
			{
				break;
			}
		}

		data = ckPortInByte(KEYBOARD_PORT_OUTPUT);
		if (data == KEYBOARD_ACK)
			return true;
		else
			ckInterruptQueuePut(data | INTERRUPT_QUEUE_FLAG_KEYBOARD);
	}
	return false;
}

void ckKeyboardWaitForInput(void)
{
	for (int i = 0; i < 0xffff; i++)
	{
		if (!ckKeyboardIsInputBufferFull())
			return;
	}
}

void ckKeyboardResetPC(void)
{
	ckKeyboardWaitForInput();
	ckPortOutByte(KEYBOARD_PORT_CONTROL, KEYBOARD_CONTROL_WRITE_OUTP);
	ckPortOutByte(KEYBOARD_PORT_INPUT, 0 /* reset */);
	while (1) { } /* no return */
}

/** @brief C로 짜여진 키보드 인터럽트 ISR입니다. */
void ck_KeyboardIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_KEYBOARD);
	uint8_t code = ckPortInByte(KEYBOARD_PORT_OUTPUT);

	if (likely(code != KEYBOARD_ACK))
		ckInterruptQueuePut(code | INTERRUPT_QUEUE_FLAG_KEYBOARD);
}
