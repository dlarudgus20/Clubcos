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
 * @file binary_semaphore.c
 * @date 2014. 6. 1.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "binary_semaphore.h"
#include "task.h"
#include "likely.h"

void ckBiSemEnter(BiSem *bisem)
{
	while (!__sync_bool_compare_and_swap(&bisem->flag, 0, 1))
	{
		ckTaskSchedule();
	}
}

bool ckBiSemPost(BiSem *bisem)
{
	return __sync_bool_compare_and_swap(&bisem->flag, 1, 0);
}

bool ckBiSemUnpost(BiSem *bisem)
{
	return __sync_bool_compare_and_swap(&bisem->flag, 0, 1);
}

void ckBiSemWait(BiSem *bisem)
{
	while (bisem->flag != 0)
	{
		ckTaskSchedule();
	}
}

