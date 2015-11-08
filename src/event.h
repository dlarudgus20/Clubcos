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
 * @file event.h
 * @date 2015. 11. 8.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "waitable.h"

/**
 * @brief 바이너리 세마포어 구조체입니다.
 */
typedef struct tagEvent
{
	Waitable waitable;

	uint32_t flag;		//!< benaphore의 flag입니다.
	bool bAutoClear;
} Event;

/**
 * @brief 바이너리 세마포어를 초기화합니다.
 * @param[in] bisem 초기화할 @ref Benaphore 구조체입니다.
 * @param[in] 바이너리 세마포어의 초기값입니다.
 */
void ckEventInit(Event *pEvent, bool InitVal, bool bAutoClear);

/**
 * @brief 바이너리 세마포어의 임계 영역에 진입하고, 카운터를 증가시킵니다.
 * @param[in] bisem @ref Benaphore 구조체입니다.
 */
void ckEventWait(Event *pEvent);
/**
 * @brief 바이너리 세마포어의 카운터를 감소시킵니다.
 * @param[in] bisem @ref Benaphore 구조체입니다.
 */
bool ckEventSet(Event *pEvent);
/**
 * @brief 바이너리 세마포어의 카운터를 증가시킵니다.
 * @param[in] bisem @ref Benaphore 구조체입니다.
 */
bool ckEventClear(Event *pEvent);

#endif /* EVENT_H_ */
