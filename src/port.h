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
 * @file port.h
 * @brief several assembly function about port and other miscellaneous things.
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef PORT_H_
#define PORT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
	EFLAG_2		= 0x00000002,	// eflag에는 이 비트가 활성화되야 하는 것 같다.
	EFLAG_IF	= 0x00000200,
	EFLAG_AC	= 0x00040000,
};

/**
 * @brief far jmp를 수행하는 어셈블리 함수입니다.
 * @param[in] eip far jmp할 오프셋입니다.
 * @param[in] cs far jmp할 세그먼트입니다.
 */
void ckAsmFarJmp(uint32_t eip, uint16_t cs);

/** @brief 특정 포트에서 1바이트를 읽어오는 어셈블리 함수 */
static inline uint8_t ckPortInByte(uint16_t port);
/** @brief 특정 포트에 1바이트를 쓰는 어셈블리 함수 */
static inline void ckPortOutByte(uint16_t port, uint8_t data);

/**
 * @brief CPU의 타임스탬프 카운터를 읽어옵니다.
 * @param[out] high 타임스탬프 카운터의 상위 32비트입니다.
 * @param[out] low 타임스탬프 카운터의 하위 32비트입니다.
 */
static inline void ckAsmGetTimeStamp(uint32_t *high, uint32_t *low);

/** @brief EFLAG를 읽어오는 어셈블리 함수 */
static inline uint32_t ckAsmGetEFlag(void);
/** @brief EFLAG에 쓰는 어셈블리 함수 */
static inline void ckAsmSetEFlag(uint32_t eflag);

/** @brief <c>cli</c> 어셈블리 함수 */
static inline void ckAsmCli(void);
/** @brief <c>sti</c> 어셈블리 함수 */
static inline void ckAsmSti(void);
/** @brief <c>hlt</c> 어셈블리 함수 */
static inline void ckAsmHlt(void);

/**
 * @brief 인터럽트를 금지시킴으로써 lock을 겁니다.
 * @sa INTERRUPT_UNLOCK
 */
#define INTERRUPT_LOCK() \
	uint32_t _eflag = ckAsmGetEFlag(); \
	ckAsmCli()

/**
 * @brief 인터럽트 금지 lock을 해제합니다.
 * @sa INTERRUPT_LOCK
 */
#define INTERRUPT_UNLOCK() \
	ckAsmSetEFlag(_eflag)

#include "port_impl.h"

#include "control_register.h"

#endif /* PORT_H_ */
