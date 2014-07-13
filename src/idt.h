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
 * @file idt.h
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef IDT_H_
#define IDT_H_

#include <stddef.h>
#include <stdint.h>

#include "memory_map.h"

/** @brief @ref Idt 구조체를 참조하십시오. */
typedef uint8_t IdtType;

/**
 * @brief IDT를 나타내는 구조체입니다.
 */
typedef struct tagIdt
{
	uint16_t HandlerAddress_0_15;

	uint16_t HandlerSegment;

	uint8_t ReservedZero1;

	union
	{
		struct
		{
			unsigned ReservedZero2:1, ReservedOne1:1, ReservedOne2:1;

			unsigned D:1;

			unsigned ReservedZero3:1;

			unsigned DPL:2, P:1;
		};
		IdtType type;
	};

	uint16_t HandlerAddress_16_31;
} Idt;

/** @brief IDT 테이블입니다. */
static Idt * const g_pIdtTable = (Idt *)IDT_TABLE_ADDRESS;

/**
 * @brief @ref Idt 구조체를 초기화합니다.
 * @param[in] pIdt 초기화할 @ref Idt 구조체입니다.
 * @param[in] HandlerAddress 어셈블리로 짜여진 ISR입니다.
 * @param[in] HandlerSegment <c>HandlerAddress</c>의 코드 세그먼트 셀렉터입니다.
 * @param[in] DPL 이 IDT의 DPL입니다.
 */
void ckIdtInit(Idt *pIdt, void (*HandlerAddress)(), uint16_t HandlerSegment, uint8_t DPL);

/**
 * @brief lidt 명령어로 IDT 테이블을 로드합니다. asmfunc.asm에 구현부가 있습니다.
 */
void ckIdtLoad(uint16_t size, Idt *address);

/**
 * @brief IDT 테이블을 초기화합니다.
 */
void ckIdtTableInitialize(void);

#endif /* IDT_H_ */
