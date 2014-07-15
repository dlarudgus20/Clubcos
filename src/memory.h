
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

/** @brief 동적 메모리를 관리하는 buddy block의 최소 크기입니다. 1page 크기와 동일합니다. */
#define DYN_MEM_BUDDY_UNIT_SIZE (4 * 1024u)

/** @brief 동적 메모리를 관리하는 buddy block을 기록하는 비트맵 자료구조입니다. */
typedef struct tagBuddyBitmap
{
	uint8_t *bits;	//!< 비트맵의 포인터입니다.
	uint32_t count;	//!< 비트맵에서 1로 설정된 비트의 갯수입니다.
} BuddyBitmap;

/** @brief 동적 메모리를 관리하는 구조체입니다. */
typedef struct tagDynMemStruct
{
	uint32_t DynMemSize;		//!< 동적 메모리 영역의 크기입니다.

	uint32_t BeginAddr;			//!< buddy block 메타데이터를 제외한 동적 메모리 영역의 시작 주소입니다.
	uint32_t UsedSize;			//!< 메타데이터를 제외한, 현재 할당되어 사용되고 있는 동적 메모리 영역의 크기입니다.

	uint32_t CountOfUnitBlock;	//!< 최소 크기(@ref DYN_MEM_BUDDY_UNIT_SIZE )의 buddy block의 갯수입니다.
	uint32_t BitmapLevel;		//!< buddy block 계층의 갯수입니다.

	BuddyBitmap *arBitmap;		//!< buddy block을 저장하는 비트맵들을 가리키는 포인터입니다.
} DynMemStruct;

/** @brief @ref DynMemStruct 입니다. */
extern DynMemStruct g_DynMem;

/**
 * @brief buddy block 알고리즘을 사용하는 동적 메모리 관리자를 초기화합니다.
 */
void ckDynMemInitialize(void);
/**
 * @brief 동적 메모리에서 메모리를 할당받습니다.
 * @return 할당된 메모리의 시작 주소입니다. @ref DYN_MEM_BUDDY_UNIT_SIZE 단위로 올림 처리됩니다.
 */
void *ckDynMemAllocate(uint32_t size);
/**
 * @brief 동적 메모리에서 할당한 메모리를 해제합니다. size는 @ref DYN_MEM_BUDDY_UNIT_SIZE 단위로 올림 처리됩니다.
 * @param[in] addr 해제할 메모리의 시작 주소입니다. @ref DYN_MEM_BUDDY_UNIT_SIZE 단위로 <i>내림</i> 처리됩니다.
 * @param[in] size 해제할 메모리의 크기입니다. @ref DYN_MEM_BUDDY_UNIT_SIZE 단위로 올림 처리됩니다.
 */
bool ckDynMemFree(void *addr, uint32_t size);

/**
 * @brief 돟적 메모리의 크기를 검사하고 <c>g_Dynmem.DynMemSize</c>에 저장합니다.
 * @return 동적 메모리의 크기입니다.
 */
uint32_t ckDynMemCheckSize(void);
/**
 * @brief 동적 메모리의 크기를 얻어옵니다. <c>g_DynMem.DynMemSize</c>와 동일합니다.
 * @return 동적 메모리의 크기입니다.
 */
static inline uint32_t ckDynMemGetSize(void)
{
	return g_DynMem.DynMemSize;
}

/**
 * @brief 특정 물리 주소가 동적 메모리 영역의 주소인지 검사합니다.
 * @param[in] phy 검사할 물리 주소입니다.
 * @return <c>phy</c>가 동적 메모리 영역 안에 있다면 <c>true</c>이고, 그렇지 않다면 <c>false</c>입니다.
 */
static inline bool ckDynMemPhyIsDynMem(uint32_t phy)
{
	return (DYN_MEMORY_PHYSICAL_ADDRESS < phy && phy < IOMAP_MEMORY_START_ADDRESS);
}

#endif /* MEMORY_H_ */
