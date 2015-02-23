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
 * @file ckMain.c
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"
#include "keys.h"
#include "coshell.h"
#include "memory.h"
#include "timer.h"
#include "rtc.h"
#include "task.h"
#include "pata.h"
#include "likely.h"
#include "string.h"

/**
 * @mainpage Clubcos 소스 레퍼런스
 * @author dlarudgus20 (임경현)
 * @version v0.1.0
 * @section intro_sec 소개
 * the source reference of Clubcos - Clubc Operating System<br/>
 * visit <a href="http://dlarudgus20.github.io/Clubcos/">github site</a> or
 * <a href="https://github.com/dlarudgus20/Clubcos">github project</a> for more information.
 */

/**
 * @brief Clubcos 커널의 시작 부분입니다. boot.asm에서 호출됩니다.
 */
void ckMain(void)
{
	// 터미널 초기화를 위해 ProcData.TermBuffer를 사용 가능한 상태로 만듬.
	g_pTaskStruct->pProcData = &g_pTaskStruct->processes[0].ProcData;
	g_pTaskStruct->pProcData->TermBuffer = (uint16_t *)IOMAP_MEMORY_START_ADDRESS;

	// 터미널 초기화
	ckTerminalInitialize();
	ckTerminalSetColor(TERMINAL_COLOR_LOG);

	{
		char buf[1024];
		snprintf(buf, sizeof(buf), "sizeof(TaskStruct) == %d (0x%08x)\n",
			sizeof(TaskStruct), sizeof(TaskStruct));
		ckTerminalPrintString_unsafe(buf);
		snprintf(buf, sizeof(buf), "sizeof(Task) == %d (0x%08x)\n",
			sizeof(Task), sizeof(Task));
		ckTerminalPrintString_unsafe(buf);
	}

	// 메모리 최소사양 점검
	ckTerminalPrintString_unsafe("Checking memory size...");
	ckDynMemCheckSize();
	if (unlikely(g_DynMem.DynMemSize + 0x900000 < MINIMAL_REQUIRE_MEMORY))
		ckTerminalPanic("Clubcos requires 32MB RAM at least.");
	ckTerminalPrintString_unsafe(" [OK]\n");

	// GDT & IDT 초기화
	ckTerminalPrintString_unsafe("Initializing GDT Table...");
	ckGdtTableInitialize();
	ckTerminalPrintString_unsafe(" [OK]\n");
	ckTerminalPrintString_unsafe("Initializing IDT Table...");
	ckIdtTableInitialize();
	ckTerminalPrintString_unsafe(" [OK]\n");

	// 동적 메모리 초기화
	ckTerminalPrintString_unsafe("Initializing Dynamic Memory Manager...");
	ckDynMemInitialize();
	ckTerminalPrintString_unsafe(" [OK]\n");

	// 인터럽트 큐 초기화
	ckInterruptQueueInitialize();

	// PIC 초기화
	ckTerminalPrintString_unsafe("Initializing PIC...");
	ckPicInitialize();
	ckTerminalPrintString_unsafe(" [OK]\n");

	// 타이머 & RTC 초기화
	ckTerminalPrintString_unsafe("Initializing Timer & RTC...");
	ckTimerInitialize();
	ckRTCInitialize();

	// 멀티태스킹 초기화 (타이머 이후)
	ckTerminalPrintString_unsafe("Initializing Tasking...");
	ckTaskStructInitialize();
	ckTerminalPrintString_unsafe(" [OK]\n");

	// 키보드 초기화
	ckTerminalPrintString_unsafe("Initializing Keyboard...");
	if (unlikely(!ckKeyboardInitialize()))
		ckTerminalPanic("Keyboard initializing is failed.");
	ckTerminalPrintString_unsafe(" [OK]\n");

	// PATA 초기화
	//ckTerminalPrintString_unsafe("Initializeing PATA...");
	//ckPATAInitialize();
	//ckTerminalPrintString_unsafe(" [OK]\n");

	// PIC의 마스크를 해제하고 인터럽트를 허용
	ckPicMaskInterrupt(
		~(PIC_MASKBIT_SLAVE | PIC_MASKBIT_TIMER | PIC_MASKBIT_KEYBOARD
			| PIC_MASKBIT_HARDDISK1 | PIC_MASKBIT_HARDDISK2)
		);
	ckAsmSti();

	// Coshell 초기화
	ckCoshellInitialize();


	// 인터럽트 큐 처리 루프

	volatile CircularQueue32 *pQueue = &g_InterruptQueue;
	uint32_t QueueData, data;

	while (1)
	{
		while (!pQueue->bEmpty)
		{
			INTERRUPT_LOCK();
			QueueData = ckCircularQueue32Get((CircularQueue32 *)pQueue, false, NULL);
			INTERRUPT_UNLOCK();

			data = QueueData & 0xffffff;
			switch (QueueData & 0xff000000)
			{
				case INTERRUPT_QUEUE_FLAG_KEYBOARD:
					ckOnKeyboardInterrupt(data);
					break;
			}
		}

		// 모두 처리했으면 CPU 시간을 반납합니다.
		ckTaskSchedule();
	}
}
