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
 * @file array.h
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "assert.h"
#include "string.h"

/**
 * @brief 동적 배열 구조체입니다.
 */
typedef struct tagArray8 Array8;
struct tagArray8
{
	uint8_t *buf; /**< 배열의 버퍼입니다. */
	uint8_t *now; /**< 현재 배열의 끝점입니다.*/
	uint8_t *end; /**< 배열 버퍼의 끝점입니다. */
};

/**
 * @brief @ref Array8 구조체를 초기화합니다.
 * @param[out] pAr @ref Array8 구조체입니다.
 * @param[in] buf 동적 배열의 버퍼입니다.
 * @param[in] end 버퍼의 끝점입니다.
 */
static inline void ckArray8Init(Array8 *pAr, uint8_t *buf, uint8_t *end)
{
	pAr->buf = pAr->now = buf;
	pAr->end = end;
}

/**
 * @brief @ref Array8의 최대 크기를 구합니다. 이것은 <c>pAr->end - pAr->buf</c>과 같습니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @return @ref Array8의 최대 크기입니다.
 */
static inline size_t ckArray8MaxSize(Array8 *pAr)
{
	return pAr->end - pAr->buf;
}

/**
 * @brief @ref Array8의 현재 크기를 구합니다. 이것은 <c>pAr->now - pAr->buf</c>과 같습니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @return @ref Array8의 현재 크기입니다.
 */
static inline size_t ckArray8Size(Array8 *pAr)
{
	return pAr->now - pAr->buf;
}

/**
 * @brief @ref Array8 구조체에 요소를 삽입합니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @param[in] ptr 삽입할 위치입니다.
 * @param[in] data 삽입할 요소입니다.
 * @return <c>pAr</c>가 꽉 차 있으면 false입니다. 그렇지 않으면 true입니다.
 */
bool ckArray8Insert(Array8 *pAr, uint8_t *ptr, uint8_t data);
/**
 * @brief @ref Array8 구조체의 특정 위치에 있는 요소를 제거합니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @param[in] ptr 제거할 요소의 위치입니다.
 */
void ckArray8Erase(Array8 *pAr, uint8_t *ptr);

/**
 * @brief 원본 @ref Array8 구조체를 대상 @ref Array8 구조체에 덧붙힙니다.
 * @param[in] pDest 대상 @ref Array8 구조체입니다.
 * @param[in] pSrc 원본 @ref Array8 구조체입니다.
 */
void ckArray8Append(Array8 *pDest, const Array8 *pSrc);

/**
 * @brief @ref Array8 구조체 맨 뒤에 요소를 삽입합니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @param[in] data 삽입할 요소입니다.
 * @return <c>pAr</c>가 꽉 차 있으면 false입니다. 그렇지 않으면 true입니다.
 */
static inline bool ckArray8PushBack(Array8 *pAr, uint8_t data)
{
	return ckArray8Insert(pAr, pAr->now, data);
}

/**
 * @brief @ref Array8 구조체 맨 뒤의 요소 하나를 제거하고, 그 요소를 반환합니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @return 제거된 요소입니다.
 */
static inline uint8_t ckArray8PopBack(Array8 *pAr)
{
	assert(pAr->now > pAr->buf);
	pAr->now--;
	uint8_t ret = *pAr->now;
	ckArray8Erase(pAr, pAr->now);
	return ret;
}

/**
 * @brief @ref Array8 구조체 맨 앞의 요소 하나를 제거하고, 그 요소를 반환합니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @return 제거된 요소입니다.
 */
static inline uint8_t ckArray8PopFront(Array8 *pArr)
{
	uint8_t ret = *pArr->buf;
	ckArray8Erase(pArr, pArr->buf);
	return ret;
}

/**
 * @brief @ref Array8 구조체에서 특정 요소를 찾습니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @param[in] 찾을 요소입니다.
 * @return 찾을 요소의 위치입니다. 그 요소가 존재하지 않으면 <c>NULL</c>입니다.
 */
static inline uint8_t *ckArray8Find(Array8 *pArr, uint8_t data)
{
	return (uint8_t *)memchr(pArr->buf, data, pArr->now - pArr->buf);
}

/**
 * @brief @ref Array8 구조체에서 특정 요소를 찾아 제거합니다.
 * @param[in] pAr @ref Array8 구조체입니다.
 * @param[in] 제거할 요소입니다.
 * @return 제거한 요소가 있던 위치입니다. 그 요소가 존재하지 않으면 <c>NULL</c>입니다.
 */
static inline uint8_t *ckArray8Remove(Array8 *pArr, uint8_t data)
{
	uint8_t *pFound = ckArray8Find(pArr, data);
	if (pFound != NULL)
	{
		ckArray8Erase(pArr, pFound);
	}
	return pFound;
}

#endif /* ARRAY_H_ */
