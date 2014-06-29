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
 * @file recursive_mutex.c
 * @date 2014. 5. 11.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "recursive_mutex.h"
#include "port.h"
#include "task.h"

uint32_t ckRecursiveMutexLock(RecursiveMutex *pMutex)
{
	if (!__sync_bool_compare_and_swap(&pMutex->bLocked, false, true))
	{
		Task *pCur = ckTaskGetCurrent();

		if (pMutex->TaskId == pCur->id)
		{
			return ++pMutex->LockCount;
		}
		else
		{
			while (!__sync_bool_compare_and_swap(&pMutex->bLocked, false, true))
			{
				ckTaskSchedule();
			}
		}
	}

	pMutex->LockCount = 1;
	pMutex->TaskId = ckTaskGetCurrentId();
	return 1;
}

bool ckRecursiveMutexUnlock(RecursiveMutex *pMutex)
{
	if (pMutex->bLocked && pMutex->TaskId == ckTaskGetCurrentId())
	{
		uint32_t LockCount = pMutex->LockCount;
		if (LockCount > 1)
		{
			pMutex->LockCount = --LockCount;
		}
		else
		{
			pMutex->LockCount = 0;
			pMutex->TaskId = TASK_INVALID_ID;
			__sync_synchronize();
			pMutex->bLocked = false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

