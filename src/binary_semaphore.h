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
 * @file binary_semaphore.h
 * @date 2014. 6. 1.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef BINARY_SEMAPHORE_H_
#define BINARY_SEMAPHORE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 바이너리 세마포어 구조체입니다.
 */
typedef struct tagBiSem
{
	volatile uint32_t flag;		//!< binary semaphore의 flag입니다.
} BiSem;

/**
 * @brief 바이너리 세마포어를 초기화합니다.
 * @param[in] bisem 초기화할 @ref BiSem 구조체입니다.
 * @param[in] 바이너리 세마포어의 초기값입니다.
 */
static inline bool ckBiSemInit(BiSem *bisem, uint32_t InitVal)
{
	if (InitVal > 1)
		return false;

	bisem->flag = InitVal;

	return true;
}

/**
 * @brief 바이너리 세마포어의 임계 영역에 진입하고, 카운터를 증가시킵니다.
 * @param[in] bisem @ref BiSem 구조체입니다.
 */
void ckBiSemEnter(BiSem *bisem);
/**
 * @brief 바이너리 세마포어의 카운터를 감소시킵니다.
 * @param[in] bisem @ref BiSem 구조체입니다.
 */
bool ckBiSemPost(BiSem *bisem);
/**
 * @brief 바이너리 세마포어의 카운터를 증가시킵니다.
 * @param[in] bisem @ref BiSem 구조체입니다.
 */
bool ckBiSemUnpost(BiSem *bisem);
/**
 * @brief 바이너리 세마포어의 임계 영역에 진입하되, 카운터를 증가시키지 않습니다.
 * @param[in] bisem @ref BiSem 구조체입니다.
 */
void ckBiSemWait(BiSem *bisem);

#endif /* BINARY_SEMAPHORE_H_ */
