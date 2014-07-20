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
#include "linkedlist.h"

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

/** @brief 타임아웃을 나타내는 구조체입니다. */
typedef struct tagTimeOut
{
	union
	{
		struct tagTimeOut *pNextTimeOut;	//!< Timer 내부에서 사용하는 순방향 연결 리스트의 노드입니다.
		LinkedListNode NoticeNode;			//!< @ref NoticeQueue 에 push하는 데 사용되는 노드입니다.
	};

	uint32_t timeout;						//!< 타임아웃이 발생할 tick count입니다.
	uint32_t code;							//!< 사용자가 마음대로 사용할 수 있는 추가 정보입니다.

	/**
	 * @brief 타임아웃이 발생한 후에 이 구조체가 push될 연결 리스트입니다.
	 * @remark Push/Pop 연산 시에 lockfree 함수를 사용해야 합니다.
	 */
	LinkedList *NoticeQueue;
} TimeOut;

/** @brief 타이머 관리 구조체입니다. */
typedef struct tagTimerStruct
{
	volatile uint32_t TickCountLow;		//!< 틱 카운트의 하위 32비트입니다.
	volatile uint32_t TickCountHigh;	//!< 틱 카운트의 상위 32비트입니다.

	TimeOut *pTimeOutHead;				//!< @ref TimeOut 구조체 리스트의 head입니다.
} TimerStruct;

/** @brief @ref TimerStruct 입니다. */
extern TimerStruct g_TimerStruct;

/** @brief 타이머를 초기화합니다. */
void ckTimerInitialize(void);
/**
 * @brief 타임아웃을 설정합니다.
 * @param[in] pTimeOut 설정할 타임아웃입니다.
 */
bool ckTimerSet(TimeOut *pTimeOut);

//TimeOut *ckTimerAllocTimeOut(uint32_t timeout, uint32_t code, LinkedList *NoticeQueue);

/** @brief 타이머 인터럽트 ISR입니다. */
void ckTimerIntHandler(void);

/**
 * @brief 인터럽트를 금지한 후 PIT 카운터를 읽어 지정된 시간동안 바쁜 대기를 수행합니다.
 * @param[in] milli 기다릴 시간입니다. 단위는 ms입니다.
 */
void ckTimerBusyDirectWait_ms(uint32_t milli);

#endif /* TIMER_H_ */
