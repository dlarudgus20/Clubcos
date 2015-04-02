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
#include "assert.h"

uint32_t ckRecursiveMutexLock(RecursiveMutex *pMutex)
{
	Task *pCur = ckTaskGetCurrent();
	uint32_t CurId = pCur->id;

	assert(pCur->WaitObj == NULL);
	pCur->WaitObj = pMutex;

	if (!__sync_bool_compare_and_swap(&pMutex->TaskId, TASK_INVALID_ID, CurId))
	{
		if (__sync_bool_compare_and_swap(&pMutex->TaskId, CurId, CurId))
		{
			pCur->WaitObj = NULL;
			return ++pMutex->Recursion;
		}
		else
		{
			ckSpinlockLock(&pMutex->WaitLock);
			INTERRUPT_LOCK();
			ckSpinlockUnlock(&pMutex->WaitLock);

			ckLinkedListPushBack_nosync(&pMutex->WaiterList, pCur->WaitNode);
			ckTaskSuspend_byptr(pCur);

			INTERRUPT_UNLOCK();
		}
	}

	return 1;
}

bool ckRecursiveMutexUnlock(RecursiveMutex *pMutex)
{
	if (__atomic_load_n(&pMutex->TaskId, __ATOMIC_SEQ_CST) == ckTaskGetCurrentId())
	{
		if (pMutex->Recursion != 0)
		{
			pMutex->Recursion--;
		}
		else
		{
			ckSpinlockLock(&pMutex->WaitLock);

			Task *pNodeTask;
			do
			{
				LinkedListNode node = ckLinkedListPopFront_nosync(&pMutex->WaiterList);
				pNodeTask = (Task *)((uintptr_t)node - offsetof(Task, WaitNode));
			} while (pNodeTask->flag == TASK_FLAG_WAITFOREXIT);

			ckTaskResume_byptr(pNodeTask);

			ckSpinlockUnlock(&pMutex->WaitLock);
		}

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

