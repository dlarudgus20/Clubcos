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
 * @file timer.h
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
	PIT_FREQUENCY = 1193180,
	TIMER_FREQUENCY = 1000,

	PIT_CONTROL_PORT = 0x43,
	PIT_COUNTER0_PORT = 0x40,

	PIT_CONTROL_COUNTER0 = 0x00,
	PIT_CONTROL_LSBMSBRW = 0x30,
	PIT_CONTROL_LATCH = 0x00,
	PIT_CONTROL_MODE0 = 0x00,	// Interrupt during counting
	PIT_CONTROL_MODE2 = 0x04,	// Clock rate generator
	PIT_CONTROL_BINARYCOUNTER = 0x00,
	PIT_CONTROL_BCDCOUNTER = 0x01,

	PIT_CONTROL_COUNTER0_ONCE =
		PIT_CONTROL_COUNTER0 | PIT_CONTROL_LSBMSBRW | PIT_CONTROL_MODE0 | PIT_CONTROL_BINARYCOUNTER,
	PIT_CONTROL_COUNTER0_PERIODIC =
		PIT_CONTROL_COUNTER0 | PIT_CONTROL_LSBMSBRW | PIT_CONTROL_MODE2 | PIT_CONTROL_BINARYCOUNTER,
};

typedef struct tagTimeOut
{
	uint32_t timeout;
	uint32_t code;
} TimeOut;

typedef struct tagTimerStruct
{
	volatile uint32_t TickCountLow, TickCountHigh;
} TimerStruct;

extern TimerStruct g_TimerStruct;

void ckTimerInitialize(void);
bool ckTimerSetFor(unsigned timespan, uint32_t code);

void ckOnTimerInterrupt(uint32_t QueueData);
void ckTimerIntHandler(void);

void ckTimerBusyDirectWait_ms(uint32_t milli);

#endif /* TIMER_H_ */
