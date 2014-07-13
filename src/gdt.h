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
 * @file gdt.h
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef GDT_H_
#define GDT_H_

#include <stddef.h>
#include <stdint.h>

#include "memory_map.h"

/** @brief NULL 세그먼트 셀렉터입니다. */
#define NULL_SEGMENT 0
/** @brief 커널 데이터 세그먼트 셀렉터입니다. */
#define KERNEL_DATA_SEGMENT (1 * 8)
/** @brief 커널 코드 세그먼트 셀렉터입니다. */
#define KERNEL_CODE_SEGMENT (2 * 8)

/**
 * @brief @ref GdtType1 을 조립하는 매크로 함수입니다.
 * @param[in] SegmentType SegmentType입니다.
 * @param[in] S 0이면 시스템 세그먼트이고, 1이면 코드/데이터 세그먼트입니다. 항상 1로 해둡니다.
 * @param[in] DPL 세그먼트의 DPL입니다.
 * @param[in] P 세그먼트의 present 여부입니다.
 * @sa DATA_SEGMENT_TYPE
 * @sa CODE_SEGMENT_TYPE
 */
#define MAKE_GDT_TYPE1(SegmentType,S,DPL,P) \
	((((P) << 7) & 0x80) | (((DPL) << 5) & 0x60) | (((S) << 4) & 0x10) | ((SegmentType) & 0x0f))

/**
 * @brief 데이터 세그먼트의 SegmentType을 조립하는 매크로 함수입니다.
 * @param[in] expdown Expand Down 여부입니다.
 * @param[in] writable 쓰기 가능 여부입니다.
 * @sa DATA_SEGMENT_TYPE
 */
#define MAKE_DATA_SEGMENT(expdown,writable) \
	(((((expdown) << 2) & 0x04) | (((writable) << 1) & 0x02)) & 0x06)
/**
 * @brief 데이터 세그먼트의 SegmentType을 조립하는 매크로 함수입니다.
 * @param[in] conforming conforming 여부입니다.
 * @param[in] readable 읽기 가능 여부입니다.
 * @sa CODE_SEGMENT_TYPE
 */
#define MAKE_CODE_SEGMENT(conforming,readable) \
	((((((conforming) << 2) & 0x04) | (((readable) << 1) & 0x02)) | 0x08) & 0x0e)

/** @brief Clubcos가 사용하는 데이터 세그먼트의 SegmentType입니다. */
#define DATA_SEGMENT_TYPE MAKE_DATA_SEGMENT(0,1)
/** @brief Clubcos가 사용하는 코드 세그먼트의 SegmentType입니다. */
#define CODE_SEGMENT_TYPE MAKE_CODE_SEGMENT(0,1)

/** @brief @ref Gdt 구조체를 참조하십시오. */
typedef uint8_t GdtType1;
/** @brief @ref Gdt 구조체를 참조하십시오. */
typedef uint8_t GdtType2;

/**
 * @brief GDT를 나타내는 구조체입니다.
 */
typedef struct tagGdt
{
	uint16_t Size_0_15, Address_0_15;

	uint8_t Address_16_23;

	union
	{
		struct
		{
			unsigned SegmentType:4, S:1, DPL:2, P:1;
		};
		GdtType1 type1;
	};

	union
	{
		struct
		{
			unsigned Size_16_19:4;
			unsigned AVL:1, reserved:1, D:1, G:1;
		};
		GdtType2 type2;
	};

	uint8_t Address_24_32;
} Gdt;

/** @brief GDT 테이블입니다. */
static Gdt * const g_pGdtTable = (Gdt *)GDT_TABLE_ADDRESS;

/**
 * @brief @ref Gdt 구조체를 초기화합니다.
 * @param[in] pGdt 초기화할 @ref Gdt 구조체입니다.
 * @param[in] address GDT가 가리키는 선형 주소입니다.
 * @param[in] size GDT가 가리키는 메모리의 크기입니다.
 * @param[in] type1 GDT의 type1입니다.
 */
void ckGdtInit(Gdt *pGdt, uint32_t address, uint32_t size, GdtType1 type1);

/**
 * @brief @ref Gdt 구조체를 NULL GDT로 초기화합니다.
 * @param[in] pGdt 초기화할 @ref Gdt 구조체입니다.
 */
static inline void ckGdtInitNull(Gdt *pGdt)
{
	((int *)pGdt)[1] = ((int *)pGdt)[0] = 0;
}

/**
 * @brief lgdt 명령어로 GDT 테이블을 로드합니다. asmfunc.asm에 구현부가 있습니다.
 */
void ckGdtLoad(uint16_t size, Gdt *address);

/**
 * @brief GDT 테이블을 초기화합니다.
 */
void ckGdtTableInitialize(void);

#endif /* GDT_H_ */
