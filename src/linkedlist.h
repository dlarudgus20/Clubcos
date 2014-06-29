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
 * @file linkedlist.h
 * @date 2014. 5. 10.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief 이중 환형 연결 리스트의 한 노드를 나타내는 구조체입니다.
 * @sa LinkedList
 */
typedef struct tagLinkedListNode LinkedListNode;
struct tagLinkedListNode
{
	LinkedListNode * volatile pNext;
	LinkedListNode * volatile pPrev;
};

/**
 * @brief 이중 환형 연결 리스트 구조체입니다.
 */
typedef struct tagLinkedList LinkedList;
struct tagLinkedList
{
	LinkedListNode dummy;

	/**
	 * @brief 연결 리스트의 요소의 갯수입니다.
	 * @remark 이 값은 다른 스레드에서 lock-free 함수가 실행 중일 땐 무효할 수 있습니다.
	 *         하지만 그 함수가 실행을 마친 후에는 유효해야 합니다.
	 */
	uint32_t size;
};

/**
 * @brief @ref LinkedList 구조체를 초기화합니다.
 * @param[in] pList 초기화할 @ref LinkedList 구조체입니다.
 */
static inline void ckLinkedListInit(LinkedList *pList)
{
	// circular linked list
	pList->dummy.pNext = &pList->dummy;
	pList->dummy.pPrev = &pList->dummy;
	pList->size = 0;
}

/**
 * @brief @ref LinkedList 구조체의 첫 노드를 가져옵니다. <c>pList->dummy.pNext</c>와 동일합니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @return 연결 리스트의 첫 노드입니다. 연결 리스트가 비어 있으면 <c>&pList->dummy</c>입니다.
 */
static inline LinkedListNode *ckLinkedListHead(LinkedList *pList) { return pList->dummy.pNext; }
/**
 * @brief @ref LinkedList 구조체의 마지막 노드를 가져옵니다. <c>pList->dummy.pPrev</c>와 동일합니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @return 연결 리스트의 마지막 노드입니다. 연결 리스트가 비어 있으면 <c>&pList->dummy</c>입니다.
 */
static inline LinkedListNode *ckLinkedListTail(LinkedList *pList) { return pList->dummy.pPrev; }

/**
 * @brief 단일 소비자/생산자 환경에서 lock-free하게 요소를 @ref LinkedList 맨 뒤에 삽입합니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @param[in] pNode 삽입할 요소의 노드입니다.
 * @remark 이 함수는 단일 소비자/생산자 환경에서 lock-free하게 연산을 수행할 수 있습니다.
 *         @ref ckLinkedListPopFront_lockfree 와 조합해 lock-free 큐로써 사용됩니다.
 * @sa ckLinkedListPopFront_lockfree
 * @sa ckLinkedListPushBack
 */
void ckLinkedListPushBack_lockfree(LinkedList *pList, LinkedListNode *pNode);
/**
 * @brief 단일 소비자/생산자 환경에서 lock-free하게 @ref LinkedList 의 맨 앞 요소를 빼옵니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @return 빼온 요소입니다. 리스트가 비어 있을 경우 <c>NULL입니다.</c>
 * @remark 이 함수에 대한 설명은 @ref ckLinkedListPushBack_lockfree 함수의 설명을 참조하십시오.
 * @sa ckLinkedListPushBack_lockfree
 * @sa ckLinkedListPopFront
 */
LinkedListNode *ckLinkedListPopFront_lockfree(LinkedList *pList);

/**
 * @brief 요소를 @ref LinkedList 맨 뒤에 삽입합니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @param[in] pNode 삽입할 요소의 노드입니다.
 */
static inline void ckLinkedListPushBack(LinkedList *pList, LinkedListNode *pNode)
{
	pList->size++;

	LinkedListNode *dummy = &pList->dummy;
	LinkedListNode *tail = dummy->pPrev;

	pNode->pNext = dummy;
	pNode->pPrev = tail;
	dummy->pPrev = pNode;
	tail->pNext = pNode;
}

/**
 * @brief @ref LinkedList 의 맨 앞 요소를 빼옵니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @return 빼온 요소입니다. 리스트가 비어 있을 경우 <c>NULL입니다.</c>
 */
static inline LinkedListNode *ckLinkedListPopFront(LinkedList *pList)
{
	LinkedListNode *dummy = &pList->dummy;
	LinkedListNode *ptr = dummy->pNext;
	LinkedListNode *next = ptr->pNext;

	if (ptr == dummy)
		return NULL;

	dummy->pNext = next;
	next->pPrev = dummy;

	pList->size--;
	return ptr;
}

/**
 * @brief @ref LinkedList 에서 노드 하나를 제거합니다.
 * @param[in] pList @ref LinkedList 구조체입니다.
 * @param[in] pNode 제거할 노드입니다.
 */
static inline void ckLinkedListErase(LinkedList *pList, LinkedListNode *pNode)
{
	LinkedListNode *prev = pNode->pPrev;
	LinkedListNode *next = pNode->pNext;

	prev->pNext = next;
	next->pPrev = prev;
	pList->size--;
}

#endif /* LINKEDLIST_H_ */
