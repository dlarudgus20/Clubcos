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
 * @file control_register.h
 * @date 2014. 6. 14.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef CONTROL_REGISTER_H_
#define CONTROL_REGISTER_H_

/** @brief <c>cr0</c> 레지스터의 플래그입니다. */
enum tagCr0Flags
{
	CR0_MONITOR_COPROC	= 1 << 1,
	CR0_FPU_EMULATION	= 1 << 2,
	CR0_TASK_SWITCHED	= 1 << 3,
	CR0_WRITE_BACK		= 1 << 29,
	CR0_DISABLE_CACHE	= 1 << 30,
	CR0_PAGING			= 1 << 31,
};

/** @brief <c>cr4</c> 레지스터의 플래그입니다. */
enum tagCr3Flags
{
	CR4_OSFXSR			= 1 << 9,
	CR4_OSXMMEXCPT		= 1 << 10,
};

// Special Functions

/**
 * @brief <c>clts</c> 어셈블리 명령으로 CR0의 TS비트를 0으로 만듭니다.<br/>
 *        psudo code:<code>CR0.TS = 0</code>
 */
static inline void ckAsmClearTS(void)
{
	__asm__ __volatile__ ( "clts" );
}

// Get/Set

/** @brief cr0 레지스터를 얻어옵니다. */
static inline uint32_t ckAsmGetCr0(void)
{
	uint32_t ret;
	__asm__ __volatile__ ( "mov %%cr0, %0" : "=r"(ret) );
	return ret;
}
/** @brief cr0 레지스터에 값을 넣습니다. */
static inline void ckAsmSetCr0(uint32_t value)
{
	__asm__ __volatile__ ( "mov %0, %%cr0" : : "r"(value) );
}

/** @brief cr3 레지스터를 얻어옵니다. */
static inline uint32_t ckAsmGetCr3(void)
{
	uint32_t ret;
	__asm__ __volatile__ ( "mov %%cr3, %0 " : "=r"(ret) );
	return ret;
}
/** @brief cr3 레지스터에 값을 넣습니다. */
static inline void ckAsmSetCr3(uint32_t value)
{
	// 페이징이 바뀌기 때문에 memory를 clobber함
	__asm__ __volatile__ ( "mov %0, %%cr3" : : "r"(value) : "memory" );
}

/** @brief cr4 레지스터를 얻어옵니다. */
static inline uint32_t ckAsmGetCr4(void)
{
	uint32_t ret;
	__asm__ __volatile__ ( "mov %%cr4, %0 " : "=r"(ret) );
	return ret;
}
/** @brief cr4 레지스터에 값을 넣습니다. */
static inline void ckAsmSetCr4(uint32_t value)
{
	__asm__ __volatile__ ( "mov %0, %%cr4" : : "r"(value) );
}

#endif /* CONTROL_REGISTER_H_ */
