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

/*
 * spinlock.h
 *
 *  Created on: 2015. 3. 31.
 *      Author: dlaru_000
 */

#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include <stddef.h>
#include <stdint.h>

typedef volatile uint32_t Spinlock;

static inline void ckSpinlockInit(Spinlock *pSpinlock)
{
	*pSpinlock = 0;
}

static inline void ckSpinlockLock(Spinlock *pSpinlock)
{
	__asm__ __volatile__
	(
		"	lock btsl $0, (%%eax)	\n\t"
		"	jnc .spin_locked		\n\t"
		".spin_loop:				\n\t"
		"	pause					\n\t"
		"	testl $1, (%%eax)		\n\t"
		"	jnz .spin_loop			\n\t"
		"	lock btsl $0, (%%eax)	\n\t"
		"	jc .spin_loop			\n\t"
		".spin_locked:				\n\t"
		:
		: "a"(pSpinlock)
	);
}

static inline void ckSpinlockUnlock(Spinlock *pSpinlock)
{
	__asm__ __volatile__
	(
		"	movl $0, (%%eax)		\n\t"
		:
		: "a"(pSpinlock)
	);
}

#endif /* SPINLOCK_H_ */
