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
 * @file linkedlist.c
 * @date 2014. 5. 10.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "linkedlist.h"

void ckLinkedListPushBack_mpsc(LinkedList *pList, LinkedListNode *pNode)
{
	__sync_add_and_fetch(&pList->size, 1);

	LinkedListNode *dummy = &pList->dummy;
	LinkedListNode *tail;

	pNode->pNext = dummy;

	while (1)
	{
		tail = dummy->pPrev;
		pNode->pPrev = tail;
		if (__sync_bool_compare_and_swap(&dummy->pPrev, tail, pNode))
		{
			tail->pNext = pNode;
			break;
		}
	}
}

LinkedListNode *ckLinkedListPopFront_mpsc(LinkedList *pList)
{
	LinkedListNode *dummy = &pList->dummy;

	LinkedListNode *ptr, *next;

	ptr = dummy->pNext;
	if (ptr == dummy)
		return NULL;

	while (1)
	{
		next = ptr->pNext;
		if (__sync_bool_compare_and_swap(&next->pPrev, ptr, dummy))
		{
			dummy->pNext = next;
			break;
		}
	}

	__sync_sub_and_fetch(&pList->size, 1);
	return ptr;
}

