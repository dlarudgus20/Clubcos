
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
 * @file memory.h
 * @date 2014. 5. 5.
 * @author MINT64OS, by kkamagui (http://kkamagui.tistory.com/)
 *         modified by dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory_map.h"

// TODO: mint64os 고대로 배낌.. 나중에 재작성 해야함

// 버디 블록의 최소 크기, 1KB
enum
{
	DYN_MEMORY_MIN_SIZE = 4 * 1024
};

// 비트맵의 플래그
enum
{
	DYN_MEMORY_EXIST_FLAG = 0x01,
	DYN_MEMORY_EMPTY_FLAG = 0x00
};

typedef struct tagDynMemBitmap DynMemBitmap;
struct tagDynMemBitmap
{
	uint8_t* pbBitmap;
	uint32_t qwExistBitCount;
};

typedef struct tagDynMemStruct DynMemStruct;
struct tagDynMemStruct
{
	uint32_t DynMemSize;

	// 블록 리스트의 총 개수와 가장 크기가 가장 작은 블록의 개수, 그리고 할당된 메모리 크기
	int iMaxLevelCount;
	int iBlockCountOfSmallestBlock;
	uint32_t qwUsedSize;

	// 블록 풀의 시작 어드레스와 마지막 어드레스
	uint32_t qwStartAddress;
	uint32_t qwEndAddress;

	// 할당된 메모리가 속한 블록 리스트의 인덱스를 저장하는 영역과 비트맵 자료구조의
	// 어드레스
	uint8_t* pbAllocatedBlockListIndex;
	DynMemBitmap* pstBitmapOfLevel;
};

extern DynMemStruct g_DynMem;

void ckMemoryInitialize(void);
void* ckMemoryAllocateBuddy(uint32_t qwSize);
bool ckMemoryFreeBuddy(void* pvAddress);

uint32_t ckMemoryCheckSize(void);
static inline uint32_t ckMemoryGetSize(void)
{
	return g_DynMem.DynMemSize;
}

static inline bool ckMemoryPhyIsDynMem(uint32_t phy)
{
	return (DYN_MEMORY_PHYSICAL_ADDRESS < phy && phy < IOMAP_MEMORY_START_ADDRESS);
}

#endif /* MEMORY_H_ */
