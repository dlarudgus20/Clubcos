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
 * @file array.c
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "array.h"
#include "string.h"
#include "assert.h"

bool ckArray8Insert(Array8 *pAr, uint8_t *ptr, uint8_t data)
{
	assert(pAr->buf <= ptr && ptr <= pAr->now);

	if (pAr->now == pAr->end)
		return false;

	memmove(ptr + 1, ptr, (pAr->now - ptr) * sizeof(uint8_t));
	pAr->now++;
	*ptr = data;

	return true;
}

void ckArray8Erase(Array8 *pAr, uint8_t *ptr)
{
	assert(pAr->buf <= ptr && ptr < pAr->now);

	memcpy(ptr, ptr + 1, (pAr->now - ptr - 1) * sizeof(uint8_t));
	pAr->now--;
}

void ckArray8Append(Array8 *pDest, const Array8 *pSrc)
{
	uint8_t *dest = pDest->now;
	uint8_t *destend = pDest->end;

	uint8_t *src = pSrc->buf;
	uint8_t *srcend = pSrc->now;

	size_t len = min(srcend - src, destend - dest);

	memcpy(dest, src, len);
	pDest->now = dest + len;
}
