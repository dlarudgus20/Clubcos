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
 * @file timer.c
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "timer.h"
#include "port.h"
#include "idt.h"
#include "gdt.h"
#include "interrupt.h"
#include "kernel_queue.h"
#include "pic.h"
#include "terminal.h"
#include "task.h"
#include "lock_system.h"
#include "memory_map.h"
#include "string.h"
#include "assert.h"

TimerStruct g_TimerStruct = { 0 };

static TimeOut s_SentinelTimeOut = {
	.timeout = 0xffffffff,
	.NoticeQueue = NULL,
	.pNextTimeOut = &s_SentinelTimeOut
};

static void ckTimerReinitialize(void);
static void ckTimerInit_internal(uint16_t count, bool periodic);

static void ckWaitThroughPITCounter(uint16_t count);
static uint16_t ckTimerReadPITCounter(void);

void ckTimerInitialize(void)
{
	g_TimerStruct.pTimeOutHead = &s_SentinelTimeOut;

	ckTimerReinitialize();

	ckIdtInit(g_pIdtTable + PIC_INTERRUPT_TIMER, ckTimerIntHandler, KERNEL_CODE_SEGMENT, 0);
}
static void ckTimerReinitialize(void)
{
	ckTimerInit_internal(PIT_FREQUENCY / TIMER_FREQUENCY, true);
}

static void ckTimerInit_internal(uint16_t count, bool periodic)
{
	ckPortOutByte(PIT_CONTROL_PORT,
		periodic ? PIT_CONTROL_COUNTER0_PERIODIC : PIT_CONTROL_COUNTER0_ONCE);

	ckPortOutByte(PIT_COUNTER0_PORT, (uint8_t)count);
	ckPortOutByte(PIT_COUNTER0_PORT, (uint8_t)(count >> 8));
}

bool ckTimerSet(TimeOut *pTimeOut)
{
	if (pTimeOut->NoticeQueue == NULL)
		return false;

	LockSystemObject lso;
	ckLockSystem(&lso);

	uint32_t tick = g_TimerStruct.TickCountLow;

	s_SentinelTimeOut.timeout = tick + 0xffffffff;

	TimeOut *before = (TimeOut *)&g_TimerStruct.pTimeOutHead;
	TimeOut *node = before->pNextTimeOut;
	while (1)
	{
		if ((pTimeOut->timeout - tick) <= (node->timeout - tick))
		{
			before->pNextTimeOut = pTimeOut;
			pTimeOut->pNextTimeOut = node;
			break;
		}
		else
		{
			before = node;
			node = node->pNextTimeOut;
		}
	}

	ckUnlockSystem(&lso);

	return true;
}

void ck_TimerIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_TIMER);

	// tick count
	uint32_t low = ++g_TimerStruct.TickCountLow;
	if (low == 0)
		g_TimerStruct.TickCountHigh++;

	// TimeOut
	TimeOut *pTimeOut = g_TimerStruct.pTimeOutHead;
	while (1)
	{
		if (pTimeOut->timeout == low)
		{
			g_TimerStruct.pTimeOutHead = pTimeOut->pNextTimeOut;

			if (pTimeOut->NoticeQueue != NULL)
			{
				ckLinkedListPushBack_mpsc(pTimeOut->NoticeQueue, &pTimeOut->NoticeNode);
			}
			else // sentinal
			{
				pTimeOut->timeout += 0xffffffff;
			}

			pTimeOut = pTimeOut->pNextTimeOut;
		}
		else
		{
			break;
		}
	}

	if (low % 125 == 0)
	{
		ckKernelQueuePut(KERNEL_QUEUE_FLAG_PROCLOAD);
	}

	// 태스크 스케쥴링
	ckTaskScheduleOnTimerInt();
}

//TimeOut *ckTimerAllocTimeOut(uint32_t timeout, uint32_t code, LinkedList *NoticeQueue)
//{
//
//}

#define MS_TO_COUNT(ms) (PIT_FREQUENCY * (ms) / 1000)
void ckTimerBusyDirectWait_ms(uint32_t milli)
{
	LockSystemObject lso;
	ckLockSystem(&lso);

	while (milli > 30)		// 30ms 단위로 쉼.
	{
		ckWaitThroughPITCounter(MS_TO_COUNT(30));
		milli -= 30;
	}
	ckWaitThroughPITCounter(MS_TO_COUNT(milli));

	ckTimerReinitialize();

	ckUnlockSystem(&lso);
}

static void ckWaitThroughPITCounter(uint16_t count)
{
	ckTimerInit_internal(0xffff, false);

	uint16_t prev = ckTimerReadPITCounter();

	while (((prev - ckTimerReadPITCounter()) & 0xffff) < count)
	{
		// busy waiting...
	}
}

static uint16_t ckTimerReadPITCounter(void)
{
	ckPortOutByte(PIT_CONTROL_PORT, PIT_CONTROL_COUNTER0 | PIT_CONTROL_LATCH);
	uint8_t low = ckPortInByte(PIT_COUNTER0_PORT);
	uint8_t high = ckPortInByte(PIT_COUNTER0_PORT);

	return (high << 8) | low;
}

