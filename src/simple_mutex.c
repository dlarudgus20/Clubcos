// Copyright (c) 2014, 임경현
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
 * @file simple_mutex.c
 * @date 2015. 11. 1.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "simple_mutex.h"
#include "task.h"
#include "lock_system.h"

void ckSimpleMutexInit(SimpleMutex *pMutex)
{
	pMutex->owner = ckTaskGetCurrentId();
	pMutex->locker = TASK_INVALID_ID;
}

void ckSimpleMutexLock(SimpleMutex *pMutex)
{
	Task *pTask = ckTaskGetCurrent();

	// fastpath
	if (__sync_bool_compare_and_swap(&pMutex->locker, TASK_INVALID_ID, pTask->id))
		return;

	LockSystemObject lso;
	ckLockSystem(&lso);

	if (pMutex->locker == TASK_INVALID_ID)
	{
		pMutex->locker = pTask->id;
	}
	else
	{
		;
	}

	ckUnlockSystem(&lso);
}

bool ckSimpleMutexUnlock(SimpleMutex *pMutex)
{
	Task *pTask = ckTaskGetCurrent();

	LockSystemObject lso;
	ckLockSystem(&lso);

	if (pMutex->locker != pTask->id)
		return false;
	pMutex->locker = TASK_INVALID_ID;

	;

	ckUnlockSystem(&lso);

	return true;
}
