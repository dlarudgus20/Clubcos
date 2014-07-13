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
 * @file memory.c
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "memory.h"
#include "port.h"
#include "string.h"
#include "likely.h"

DynMemStruct g_DynMem;

static void *_memchr_not_without_size(const void *mem, int val)
{
	for (const uint8_t *p = (const uint8_t *)mem; ; p++)
		if (*p != (uint8_t)val)
			return (void *)p;
	return NULL;
}
static inline uint32_t _ceil_uint32(uint32_t num, uint32_t base)
{
	return (num + base - 1) / base * base;
}

static inline void BITMAP_BIT_SET_1(BuddyBitmap *bitmap, uint32_t idx)
{
	bitmap->bits[idx / 8] |= (1 << idx % 8);
}
static inline void BITMAP_BIT_SET_0(BuddyBitmap *bitmap, uint32_t idx)
{
	bitmap->bits[idx / 8] &= ~(1 << idx % 8);
}
static inline uint8_t BITMAP_GET_BIT(BuddyBitmap *bitmap, uint32_t idx)
{
	return (bitmap->bits[idx / 8] & (1 << idx % 8));
}

void ckDynMemInitialize(void)
{
	// 메타데이터의 크기를 계산
	uint8_t *pb = (uint8_t *)DYN_MEMORY_START_ADDRESS;
	g_DynMem.BeginAddr = DYN_MEMORY_START_ADDRESS;
	uint32_t DynSize = g_DynMem.DynMemSize;
	for (uint32_t sz = (DynSize - 1) / DYN_MEM_BUDDY_UNIT_SIZE + 1; ; sz >>= 1)
	{
		pb += sizeof(BuddyBitmap);
		g_DynMem.BeginAddr += sizeof(BuddyBitmap) + (sz - 1) / 8 + 1;

		// 블록이 1개가 되면 최상위 블록.
		if (sz == 1)
			break;
	}
	g_DynMem.BeginAddr = _ceil_uint32(g_DynMem.BeginAddr, DYN_MEM_BUDDY_UNIT_SIZE);

	// 버디블록 초기화

	DynSize -= g_DynMem.BeginAddr - DYN_MEMORY_START_ADDRESS; // 메타데이터의 크기만큼 제외
	g_DynMem.arBitmap = (BuddyBitmap *)DYN_MEMORY_START_ADDRESS;
	g_DynMem.CountOfUnitBlock = (DynSize - 1) / DYN_MEM_BUDDY_UNIT_SIZE + 1;

	BuddyBitmap *pBitmap = g_DynMem.arBitmap;
	for (uint32_t sz = (DynSize - 1) / DYN_MEM_BUDDY_UNIT_SIZE + 1; ; sz >>= 1, pBitmap++)
	{
		pBitmap->bits = pb;

		// 비트맵을 0으로 채움
		memset(pb, 0, (sz - 1) / 8 + 1);
		pb += (sz - 1) / 8 + 1;

		// 블록이 홀수 개면 자투리 블록이 생김
		if (sz % 2 != 0)
		{
			pBitmap->count = 1;
			*(pb - 1) = 1 << (sz % 8 - 1);
		}
		else
		{
			pBitmap->count = 0;
		}

		// 블록이 1개가 되면 최상위 블록.
		if (sz == 1)
			break;
	}
	g_DynMem.BitmapLevel = pBitmap - g_DynMem.arBitmap + 1;

	g_DynMem.UsedSize = 0;
}

void *ckDynMemAllocate(uint32_t size)
{
	if (unlikely(size == 0))
		return NULL;

	size = _ceil_uint32(size, DYN_MEM_BUDDY_UNIT_SIZE);

	uint32_t BitmapIdx;
	for (BitmapIdx = 0; (DYN_MEM_BUDDY_UNIT_SIZE << BitmapIdx) < size; BitmapIdx++) { }

	if (unlikely(BitmapIdx >= g_DynMem.BitmapLevel))
		return NULL;

	BuddyBitmap *start = g_DynMem.arBitmap + BitmapIdx;
	BuddyBitmap *end = g_DynMem.arBitmap + g_DynMem.BitmapLevel;

	INTERRUPT_LOCK();
	uint32_t ret = 0;

	for (BuddyBitmap *now = start; now < end; now++)
	{
		// 블록이 텅 비었으면 상위 블록 검색
		if (now->count == 0)
			continue;

		// 비어있는 블록을 찾아서 할당
		uint8_t *pb = (uint8_t *)_memchr_not_without_size(now->bits, 0);
		uint32_t block = (pb - now->bits) * 8;

		uint32_t num;
		for (num = 0; !(*pb & (1 << num)); num++) { }
		*pb &= ~(1 << num);
		now->count--;
		block += num;

		// 상위 블록에서 할당했으면 남은 블록을 표시함
		if (now != start)
		{
			for (BuddyBitmap *below = now - 1; below >= start; below--)
			{
				block *= 2;
				BITMAP_BIT_SET_1(below, block + 1);
				below->count++;
			}
		}

		g_DynMem.UsedSize += size;

		ret = g_DynMem.BeginAddr + block * (DYN_MEM_BUDDY_UNIT_SIZE << BitmapIdx);
		break;
	}

	INTERRUPT_UNLOCK();

	return (void *)ret;
}

bool ckDynMemFree(void *addr, uint32_t size)
{
	if (unlikely(size == 0))
			return false;

	if (unlikely((uint32_t)addr < g_DynMem.BeginAddr))
		return false;

	uint32_t rel_addr = _ceil_uint32((uint32_t)addr - g_DynMem.BeginAddr, DYN_MEM_BUDDY_UNIT_SIZE);
	size = _ceil_uint32(size, DYN_MEM_BUDDY_UNIT_SIZE);

	uint32_t BitmapIdx;
	for (BitmapIdx = 0; (DYN_MEM_BUDDY_UNIT_SIZE << BitmapIdx) < size; BitmapIdx++) { }

	if (unlikely(BitmapIdx >= g_DynMem.BitmapLevel))
		return false;

	BuddyBitmap *start = g_DynMem.arBitmap + BitmapIdx;
	uint32_t block = rel_addr / (DYN_MEM_BUDDY_UNIT_SIZE << BitmapIdx);

	INTERRUPT_LOCK();

	// 할당되지 않은 블록이면 false 리턴
	if (BITMAP_GET_BIT(start, block))
	{
		INTERRUPT_UNLOCK();
		return false;
	}

	// 블록 해제
	BITMAP_BIT_SET_1(start, block);
	start->count++;

	// 인접 블록이 있으면 병합해 상위 블록으로 올림
	BuddyBitmap *end = g_DynMem.arBitmap + g_DynMem.BitmapLevel;
	for (BuddyBitmap *now = start; ; )
	{
		uint32_t buddy = block ^ 1; // 2n -> 2n+1, 2n+1 -> 2n

		if (BITMAP_GET_BIT(now, buddy))
		{
			// 상위 블록이 존재하지 않으면 그대로 끝
			if (now + 1 >= end)
				break;

			BITMAP_BIT_SET_0(now, buddy);
			BITMAP_BIT_SET_0(now, block);
			now->count -= 2;

			now++;
			block /= 2;
			BITMAP_BIT_SET_1(now, block);
			now->count++;
		}
		else
		{
			// 인접 블록이 없음
			break;
		}
	}

	g_DynMem.UsedSize -= size;

	INTERRUPT_UNLOCK();
	return true;
}

static bool ckDynMemCheckSizeSub(volatile uint32_t *ptr)
{
	volatile uint32_t old = *ptr;
	bool ret = false;
	*ptr = 0x1234abcd;
	*ptr ^= 0xffffffff;
	if (*ptr == ~0x1234abcdU)
	{
		*ptr ^= 0xffffffff;
		if (*ptr == 0x1234abcdU)
		{
			ret = true;
		}
	}
	*ptr = old;
	return ret;
}
uint32_t ckDynMemCheckSize(void)
{
	uint32_t cr0;

	volatile uint32_t ptr;

	cr0 = ckAsmGetCr0();
	ckAsmSetCr0(cr0 | CR0_DISABLE_CACHE);

	for (ptr = DYN_MEMORY_START_ADDRESS; ptr < IOMAP_MEMORY_START_ADDRESS; ptr += 0x100000)
	{
		if (!ckDynMemCheckSizeSub((volatile uint32_t *)ptr))
			break;
	}
	if (!ckDynMemCheckSizeSub((volatile uint32_t *)(ptr - 4)))
	{
		ptr -= 0x100000;
	}

	ckAsmSetCr0(cr0);

	g_DynMem.DynMemSize = ptr - DYN_MEMORY_START_ADDRESS;
	return g_DynMem.DynMemSize;
}


