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
 * @file event.c
 * @date 2015. 11. 8.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "event.h"

#include "lock_system.h"
#include "task.h"
#include "assert.h"

void ckEventInit(Event *pEvent, bool InitVal, bool bAutoClear)
{
	pEvent->flag = (InitVal ? 1 : 0);
	pEvent->bAutoClear = bAutoClear;
	ckLinkedListInit(&pEvent->waitable.listOfWaiters);
}

void ckEventWait(Event *pEvent)
{
	// fastpath
	if (pEvent->bAutoClear)
	{
		if (__sync_bool_compare_and_swap(&pEvent->flag, 1, 0))
			return;
	}
	else
	{
		if (__sync_fetch_and_or(&pEvent->flag, 0) == 1)
			return;
	}

	LockSystemObject lso;
	ckLockSystem(&lso);

	if (pEvent->flag == 0)
	{
		Task *pTask = ckTaskGetCurrent();
		ckLinkedListPushBack_nosync(&pEvent->waitable.listOfWaiters, &pTask->nodeOfWaitedObj);
		ckTaskSuspend_byptr(pTask);
	}

	ckUnlockSystem(&lso);
}

bool ckEventSet(Event *pEvent)
{
	bool bRet = true;

	LockSystemObject lso;
	ckLockSystem(&lso);

	if (pEvent->flag == 1)
	{
		bRet = false;
	}
	else if (pEvent->waitable.listOfWaiters.size != 0)
	{
		if (pEvent->bAutoClear)
		{
			LinkedListNode *node = ckLinkedListPopFront_nosync(&pEvent->waitable.listOfWaiters);
			Task *pNodeTask = (Task *)((uint32_t)node - offsetof(Task, nodeOfWaitedObj));

			pNodeTask->WaitedObj = NULL;
			ckTaskResume_byptr(pNodeTask);
		}
		else
		{
			pEvent->flag = 1;

			LinkedList *pList = &pEvent->waitable.listOfWaiters;
			for (LinkedListNode *node = ckLinkedListHead(pList);
				node != (LinkedListNode *)pList;
				node = node->pNext)
			{
				Task *pNodeTask = (Task *)((uint32_t)node - offsetof(Task, nodeOfWaitedObj));
				pNodeTask->WaitedObj = NULL;
				ckTaskResume_byptr(pNodeTask);
			}
		}
	}
	else
	{
		pEvent->flag = 1;
	}

	ckUnlockSystem(&lso);

	return bRet;
}

bool ckEventClear(Event *pEvent)
{
	return __sync_bool_compare_and_swap(&pEvent->flag, 1, 0);
}
