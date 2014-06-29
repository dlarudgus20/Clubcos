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
#include "pic.h"
#include "terminal.h"
#include "task.h"
#include "memory_map.h"
#include "string.h"
#include "assert.h"

TimerStruct g_TimerStruct = { 0 };

static TimeOut * const TimeOutTable = (TimeOut *)TIMEOUT_TABLE_ADDRESS;
static const size_t CountOfTimeOutTable = 4096;
static TimeOut *pTimeOutEnd;

static void ckTimerReinitialize(void);
static void ckTimerInit_internal(uint16_t count, bool periodic);

static void ckWaitThroughPITCounter(uint16_t count);
static uint16_t ckTimerReadPITCounter(void);

void ckTimerInitialize(void)
{
	pTimeOutEnd = TimeOutTable;
	memset(TimeOutTable, 0, CountOfTimeOutTable * sizeof(TimeOut));

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

bool ckTimerSetFor(unsigned timespan, uint32_t code)
{
	assert((code & 0xff000000) == 0);

	bool bRet = true;

	INTERRUPT_LOCK();

	if (pTimeOutEnd < TimeOutTable + CountOfTimeOutTable && timespan != 0)
	{
		*pTimeOutEnd++ = (TimeOut){ g_TimerStruct.TickCountLow + timespan, code };
	}
	else
	{
		bRet = false;
	}

	INTERRUPT_UNLOCK();
	return bRet;
}

void ckOnTimerInterrupt(uint32_t QueueData)
{
}

void ck_TimerIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_TIMER);

	// 타이머
	uint32_t low = ++g_TimerStruct.TickCountLow;
	if (low == 0)
		g_TimerStruct.TickCountHigh++;

	TimeOut *pTimeOut = TimeOutTable;
	while (pTimeOut->timeout == low && pTimeOut != pTimeOutEnd)
	{
		ckCircularQueue32Put(&g_InterruptQueue, pTimeOut->code | INTERRUPT_QUEUE_FLAG_TIMER);
		pTimeOut++;
	}

	memmove(TimeOutTable, pTimeOut, (pTimeOutEnd - pTimeOut) * sizeof(TimeOut));

	uint32_t diff = (uint32_t)(pTimeOut - TimeOutTable);
	pTimeOutEnd -= diff;
	memset(pTimeOutEnd, 0, diff * sizeof(TimeOut));

	// 태스크 스케쥴링
	ckTaskScheduleOnTimerInt();
}

#define MS_TO_COUNT(ms) (PIT_FREQUENCY * (ms) / 1000)
void ckTimerBusyDirectWait_ms(uint32_t milli)
{
	INTERRUPT_LOCK();

	while (milli > 30)		// 30ms 단위로 쉼.
	{
		ckWaitThroughPITCounter(MS_TO_COUNT(30));
		milli -= 30;
	}
	ckWaitThroughPITCounter(MS_TO_COUNT(milli));

	ckTimerReinitialize();

	INTERRUPT_UNLOCK();
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

