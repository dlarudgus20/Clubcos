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
 * @file kernel_queue.h
 * @date 2016. 1. 16.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef KERNEL_QUEUE_H_
#define KERNEL_QUEUE_H_

#include <stddef.h>
#include <stdint.h>
#include "circular_queue.h"
#include "event.h"
#include "pic.h"

enum
{
	KERNEL_QUEUE_FLAG_KEYBOARD	= PIC_IRQ_KEYBOARD << 24,

	KERNEL_QUEUE_FLAG_PROCLOAD	= 16 << 24,
};

/**
 * @brief 인터럽트 큐를 초기화합니다.
 */
void ckKernelQueueInitialize(void);

/**
 * @brief 인터럽트 큐에 데이터를 넣습니다.
 * @param[in] data 넣을 데이터입니다.
 * @return 큐가 꽉 차 있다면 false입니다. 그렇지 않으면 true입니다.
 */
bool ckKernelQueuePut(uint32_t data);

/**
 * @brief 인터럽트 큐에 데이터가 생길 때까지 기다린 후, 데이터를 가져옵니다.
 * @return 가져온 데이터입니다.
 */
uint32_t ckKernelQueueWaitAndGet(void);

#endif /* KERNEL_QUEUE_H_ */
