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
 * @file interrupt.h
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <stddef.h>
#include <stdint.h>
#include "circular_queue.h"
#include "pic.h"

// Interrupt Queue
enum
{
	INTERRUPT_QUEUE_FLAG_TIMER		= PIC_IRQ_TIMER << 24,
	INTERRUPT_QUEUE_FLAG_KEYBOARD	= PIC_IRQ_KEYBOARD << 24,
};


/** @brief 인터럽트 큐입니다. */
extern CircularQueue32 g_InterruptQueue;
/** @brief 인터럽트 큐를 초기화합니다. @sa g_InterruptQueue */
void ckInterruptQueueInitialize(void);

/**
 * @brief 인터럽트 콘텍스트입니다. C로 짜여진 ISR에 어셈블리로 짜여진 ISR이 인수로 이 콘텍스트의 포인터를 넘깁니다.
 */
typedef struct tagInterruptContext
{
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;

	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	union
	{
		struct
		{
			uint32_t eip;
			uint32_t cs;
			uint32_t eflag;
			uint32_t esp2;
			uint32_t ss2;
		};
		struct
		{
			uint32_t err_code;
			uint32_t err_eip;
			uint32_t err_cs;
			uint32_t err_eflag;
			uint32_t err_esp2;
			uint32_t err_ss2;
		};
	};
} InterruptContext;

/* CPU 예외 핸들러 */
void ckExceptIntHandler00();
void ckExceptIntHandler01();
void ckExceptIntHandler02();
void ckExceptIntHandler03();
void ckExceptIntHandler04();
void ckExceptIntHandler05();
void ckExceptIntHandler06();
void ckExceptIntHandler07();
void ckExceptIntHandler08();
void ckExceptIntHandler09();
void ckExceptIntHandler0A();
void ckExceptIntHandler0B();
void ckExceptIntHandler0C();
void ckExceptIntHandler0D();
void ckExceptIntHandler0E();
void ckExceptIntHandler0F();
void ckExceptIntHandler10();
void ckExceptIntHandler11();
void ckExceptIntHandler12();
void ckExceptIntHandler13();
void ckExceptIntHandler14();

void ckUnknownIntHandler();

#endif /* INTERRUPT_H_ */
