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
 * @file circular_queue.c
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "circular_queue.h"

void ckCircularQueue32Init(CircularQueue32 *pQueue, uint32_t *pQueueBuf, size_t QueueSize)
{
	pQueue->pQueueBuf = pQueueBuf;
	pQueue->pFirst = pQueue->pLast = pQueueBuf;
	pQueue->size = QueueSize;
	pQueue->bEmpty = true;
}
bool ckCircularQueue32Put(CircularQueue32 *pQueue, uint32_t data)
{
	if(pQueue->bEmpty || (pQueue->pFirst != pQueue->pLast))
	{
		*pQueue->pLast = data;
		pQueue->pLast++;

		if(pQueue->pLast >= &pQueue->pQueueBuf[pQueue->size])
		{
			pQueue->pLast = pQueue->pQueueBuf;
		}

		pQueue->bEmpty = false;

		return true;
	}
	else
	{
		return false;
	}
}
uint32_t ckCircularQueue32Get(CircularQueue32 *pQueue, bool bPeek, bool *pbSuccess)
{
	uint32_t nRet;
	bool b;

	if (pbSuccess == NULL)
		pbSuccess = &b;

	if(!pQueue->bEmpty)
	{
		nRet = *pQueue->pFirst;

		if (!bPeek)
			ckCircularQueue32Remove(pQueue);

		*pbSuccess = true;
		return nRet;
	}
	else
	{
		*pbSuccess = false;
		return 0;
	}
}

bool ckCircularQueue32Remove(CircularQueue32 *pQueue)
{
	if (!pQueue->bEmpty)
	{
		pQueue->pFirst++;

		if(pQueue->pFirst >= &pQueue->pQueueBuf[pQueue->size])
		{
			pQueue->pFirst = pQueue->pQueueBuf;
		}

		if(pQueue->pFirst == pQueue->pLast)
		{
			pQueue->bEmpty = true;
		}

		return true;
	}
	else
	{
		return false;
	}
}

/* CircularQueue8 */

void ckCircularQueue8Init(CircularQueue8 *pQueue, uint8_t *pQueueBuf, size_t QueueSize)
{
	pQueue->pQueueBuf = pQueueBuf;
	pQueue->pFirst = pQueue->pLast = pQueueBuf;
	pQueue->size = QueueSize;
	pQueue->bEmpty = true;
}
bool ckCircularQueue8Put(CircularQueue8 *pQueue, uint8_t data)
{
	if(pQueue->bEmpty || (pQueue->pFirst != pQueue->pLast))
	{
		*pQueue->pLast = data;
		pQueue->pLast++;

		if(pQueue->pLast >= &pQueue->pQueueBuf[pQueue->size])
		{
			pQueue->pLast = pQueue->pQueueBuf;
		}

		pQueue->bEmpty = false;

		return true;
	}
	else
	{
		return false;
	}
}
uint8_t ckCircularQueue8Get(CircularQueue8 *pQueue, bool bPeek, bool *pbSuccess)
{
	uint8_t nRet;
	bool b;

	if (pbSuccess == NULL)
		pbSuccess = &b;

	if(!pQueue->bEmpty)
	{
		nRet = *pQueue->pFirst;

		if (!bPeek)
			ckCircularQueue8Remove(pQueue);

		*pbSuccess = true;
		return nRet;
	}
	else
	{
		*pbSuccess = false;
		return 0;
	}
}
bool ckCircularQueue8Remove(CircularQueue8 *pQueue)
{
	if (!pQueue->bEmpty)
	{
		pQueue->pFirst++;

		if(pQueue->pFirst >= &pQueue->pQueueBuf[pQueue->size])
		{
			pQueue->pFirst = pQueue->pQueueBuf;
		}

		if(pQueue->pFirst == pQueue->pLast)
		{
			pQueue->bEmpty = true;
		}

		return true;
	}
	else
	{
		return false;
	}
}

