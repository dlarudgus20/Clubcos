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

/*
 * @file recursive_mutex.h
 * @date 2014. 5. 11.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "task.h"
#include "linkedlist.h"
#include "spinlock.h"

/**
 * @brief recursive mutex 구조체입니다.
 */
typedef struct tagRecursiveMutex
{
	volatile uint32_t TaskId;		//!< 뮤텍스를 소유한 태스크입니다. 없을 경우 @sa TASK_INVALID_ID 입니다.
	uint32_t Recursion;				//!< 이 뮤텍스가 재귀적으로 잠긴 횟수입니다.
	Spinlock WaitLock;				//!< 대기에 사용되는 spinlock입니다.
	LinkedList WaiterList;			//!< 뮤텍스를 대기하는 태스크의 목록입니다.
} RecursiveMutex;

/**
 * @brief @ref RecursiveMutex 구조체를 초기화합니다.
 * @param[in] pMutex 초기화할 @ref RecursiveMutex 구조체입니다.
 */
static inline void ckRecursiveMutexInit(RecursiveMutex *pMutex)
{
	pMutex->TaskId = TASK_INVALID_ID;
	pMutex->Recursion = 0;
	ckSpinlockInit(&pMutex->WaitLock);
	ckLinkedListInit(&pMutex->WaiterList);
}

/**
 * @brief @ref RecursiveMutex 구조체의 임계 영역에 진입합니다.
 * @param[in] pMutex @ref RecursiveMutex 구조체입니다.
 * @return 진입 후 뮤텍스의 LockCount입니다.
 */
uint32_t ckRecursiveMutexLock(RecursiveMutex *pMutex);
/**
 * @brief @ref RecursiveMutex 구조체의 임계 영역에서 빠져나옵니다.
 * @param[in] pMutex @ref RecursiveMutex 구조체입니다.
 * @return 이 뮤텍스를 잠그지 않았으면 <c>false</c>입니다. 그렇지 않으면 <c>true</c>입니다.
 */
bool ckRecursiveMutexUnlock(RecursiveMutex *pMutex);

#endif /* MUTEX_H_ */
