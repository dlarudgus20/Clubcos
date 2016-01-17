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
 * @file kernel_queue.c
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "terminal.h"
#include "kernel_queue.h"
#include "lock_system.h"

static Event s_QueueEvent;
static CircularQueue32 s_Queue;
static uint32_t s_QueueBuf[1024];

void ckKernelQueueInitialize(void)
{
	ckEventInit(&s_QueueEvent, false, true);
	ckCircularQueue32Init(&s_Queue, s_QueueBuf, sizeof(s_QueueBuf) / sizeof(s_QueueBuf[0]));
}

bool ckKernelQueuePut(uint32_t data)
{
	if (ckCircularQueue32Put(&s_Queue, data))
	{
		ckEventSet(&s_QueueEvent);
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t ckKernelQueueWaitAndGet(void)
{
	uint32_t ret;
	bool bSuccess;

	do
	{
		bool bEmpty = ((volatile CircularQueue32 *)&s_Queue)->bEmpty;
		if (bEmpty)
			ckEventWait(&s_QueueEvent);

		LockSystemObject lso;
		ckLockSystem(&lso);
		ret = ckCircularQueue32Get((CircularQueue32 *)&s_Queue, false, &bSuccess);
		ckUnlockSystem(&lso);
	} while (!bSuccess);

	return ret;
}
