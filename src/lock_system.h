// Copyright (c) 2014, 임경현
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
 * @file lock_system.h
 * @date 2015. 11. 1.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef LOCK_SYSTEM_H_
#define LOCK_SYSTEM_H_

#include <stdbool.h>
#include "port.h"

typedef struct tagLockSystemObject
{
	bool prev_IF;
} LockSystemObject;

static inline void ckLockSystem(LockSystemObject *pObj)
{
	pObj->prev_IF = ((ckAsmGetEFlag() & EFLAG_IF) != 0);
	ckAsmCli();
	__sync_synchronize();
}

static inline void ckUnlockSystem(LockSystemObject *pObj)
{
	if (pObj->prev_IF)
	{
		ckAsmSti();
		__sync_synchronize();
	}
}

#endif /* LOCK_SYSTEM_H_ */
