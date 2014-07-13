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
 * @file circular_queue.h
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 요소의 크기가 4바이트인 환형 큐 구조체입니다.
 */
typedef struct tagCircularQueue32
{
	uint32_t *pQueueBuf;
	uint32_t *pFirst, *pLast;
	size_t size;
	bool bEmpty;
} CircularQueue32;

/**
 * @brief @ref CircularQueue32 구조체를 초기화합니다.
 * @param[in] pQueue 초기화할 @ref CircularQueue32 구조체입니다.
 * @param[in] pQueueBuf 환형 큐에서 사용할 버퍼입니다.
 * @param[in] QueueSize 저장할 수 있는 요소의 최대 수입니다.
 */
void ckCircularQueue32Init(CircularQueue32 *pQueue, uint32_t *pQueueBuf, size_t QueueSize);

/**
 * @brief @ref CircularQueue32 구조체에 요소를 하나 넣습니다.
 * @param[in] pQueue @ref CircularQueue32 구조체입니다.
 * @param[in] data 넣을 요소입니다.
 * @return 큐가 꽉 차 있다면 false입니다. 그렇지 않으면 true입니다.
 */
bool ckCircularQueue32Put(CircularQueue32 *pQueue, uint32_t data);

/**
 * @brief @ref CircularQueue32 구조체에서 요소를 하나 빼옵니다.
 * @param[in] pQueue @ref CircularQueue32 구조체입니다.
 * @param[in] bPeek 이 값이 <c>true</c>이면 요소를 빼지 않습니다.
 * @param[out] pbSuccess 큐가 텅 비어 있으면 이 값이 <c>false</c>가 됩니다. 그렇지 않으면 <c>true</c>가 됩니다.
 *             필요치 않으면 <c>NULL</c>을 줄 수도 있습니다.
 * @return 빼온 요소입니다.
 */
uint32_t ckCircularQueue32Get(CircularQueue32 *pQueue, bool bPeek, bool *pbSuccess);

/**
 * @brief @ref CircularQueue32 구조체에서 요소를 하나 빼오고, 빼온 요소는 버립니다.
 * @param[in] pQueue @ref CircularQueue32 구조체입니다.
 * @return 큐가 텅 비어 있으면 <c>false</c>입니다. 그렇지 않으면 <c>true</c>입니다.
 */
bool ckCircularQueue32Remove(CircularQueue32 *pQueue);


/**
 * @brief 요소의 크기가 1바이트인 환형 큐 구조체입니다.
 */
typedef struct tagCircularQueue8
{
	uint8_t *pQueueBuf;
	uint8_t *pFirst, *pLast;
	size_t size;
	bool bEmpty;
} CircularQueue8;
/** @brief @ref ckCircularQueue32Init 를 참조하십시오. */
void ckCircularQueue8Init(CircularQueue8 *pQueue, uint8_t *pQueueBuf, size_t QueueSize);
/** @brief @ref ckCircularQueue32Put 을 참조하십시오. */
bool ckCircularQueue8Put(CircularQueue8 *pQueue, uint8_t data);
/** @brief @ref ckCircularQueue32Get 을 참조하십시오. */
uint8_t ckCircularQueue8Get(CircularQueue8 *pQueue, bool bPeek, bool *pbSuccess);
/** @brief @ref ckCircularQueue32Remove 을 참조하십시오. */
bool ckCircularQueue8Remove(CircularQueue8 *pQueue);

#endif /* QUEUE_H_ */
