
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

#define DYN_MEM_BUDDY_UNIT_SIZE (4 * 1024u)

typedef struct tagBuddyBitmap BuddyBitmap;
struct tagBuddyBitmap
{
	uint8_t *bits;
	uint32_t count;
};

typedef struct tagDynMemStruct DynMemStruct;
struct tagDynMemStruct
{
	uint32_t DynMemSize;

	uint32_t BeginAddr;
	uint32_t UsedSize;

	uint32_t CountOfUnitBlock;
	uint32_t BitmapLevel;

	BuddyBitmap *arBitmap;
};

extern DynMemStruct g_DynMem;

void ckDynMemInitialize(void);
void *ckDynMemAllocate(uint32_t size);
bool ckDynMemFree(void *addr, uint32_t size);

uint32_t ckDynMemCheckSize(void);
static inline uint32_t ckDynMemGetSize(void)
{
	return g_DynMem.DynMemSize;
}

static inline bool ckDynMemPhyIsDynMem(uint32_t phy)
{
	return (DYN_MEMORY_PHYSICAL_ADDRESS < phy && phy < IOMAP_MEMORY_START_ADDRESS);
}

#endif /* MEMORY_H_ */
