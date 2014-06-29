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
 * @file coshell_cmd.c
 * @date 2014. 6. 6.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "coshell.h"
#include "terminal.h"
#include "string.h"
#include "keyboard.h"
#include "memory.h"
#include "rtc.h"
#include "timer.h"
#include "task.h"
#include "recursive_mutex.h"
#include "page.h"
#include "memory_map.h"

void ckCoshellCmdReset(const char *param)
{
	ckTerminalPrintString("Are you sure to reset computer? [y/n]");
	if (ckTerminalGetChar() == 'y')
	{
		ckKeyboardResetPC();
	}
	else
	{
		ckTerminalPrintString("Reset canceled.\n");
	}
}

void ckCoshellCmdEcho(const char *param)
{
	if (param != NULL)
	{
		ckTerminalPrintString(param);
	}
	ckTerminalPutChar('\n');
}

void ckCoshellCmdCls(const char *param)
{
	ckTerminalCls();
}

void ckCoshellCmdMemInfo(const char *param)
{
	ckTerminalPrintString ("============ Dynamic Memory Information ============\n");
	ckTerminalPrintStringF("Start Address: [%p]\n",
		g_DynMem.qwStartAddress);
	ckTerminalPrintStringF("Total Size:    [%p]byte, [%u]MB\n",
		g_DynMem.DynMemSize, g_DynMem.DynMemSize / 1024 / 1024);
	ckTerminalPrintStringF("Used Size:     [%p]byte, [%u]KB\n",
		g_DynMem.qwUsedSize, g_DynMem.qwUsedSize / 1024);
}

void ckCoshellCmdTestBuddySeq(const char *param)
{
	DynMemStruct *pstMemory = &g_DynMem;
	uint32_t i, j, k;
	uint32_t *pqwBuffer;

	ckTerminalPrintString("============ Dynamic Memory Test ============\n");

	for (i = 0 ; i < pstMemory->iMaxLevelCount ; i++)
	{
		ckTerminalPrintStringF("Block List [%d] Test Start\n", i);
		ckTerminalPrintString("Allocation And Compare: ");

		// 모든 블록을 할당 받아서 값을 채운 후 검사
		for(j = 0 ; j < pstMemory->iBlockCountOfSmallestBlock >> i; j++)
		{
			pqwBuffer = ckMemoryAllocateBuddy(DYN_MEMORY_MIN_SIZE << i);
			if(pqwBuffer == NULL)
			{
				ckTerminalPrintString("\nAllocation Fail\n");
				return;
			}

			// 값을 채운 후 다시 검사
			for (k = 0 ; k < (DYN_MEMORY_MIN_SIZE << i) / sizeof(k); k++)
			{
				pqwBuffer[k] = k;
			}

			for (k = 0 ; k < (DYN_MEMORY_MIN_SIZE << i) / sizeof(k); k++)
			{
				if (pqwBuffer[k] != k)
				{
					ckTerminalPrintString("\nCompare Fail\n");
					return;
				}
			}
			// 진행 과정을 . 으로 표시
			ckTerminalPrintString(".");
		}

		ckTerminalPrintString("\nFree: ");
		// 할당 받은 블록을 모두 반환
		for (j = 0; j < pstMemory->iBlockCountOfSmallestBlock >> i; j++)
		{
			if (!ckMemoryFreeBuddy(
				(void *)(pstMemory->qwStartAddress + ( DYN_MEMORY_MIN_SIZE << i ) * j)))
			{
				ckTerminalPrintString("\nFree Fail\n");
				return;
			}
			// 진행 과정을 . 으로 표시
			ckTerminalPrintString(".");
		}
		ckTerminalPrintString("\n");
	}
	ckTerminalPrintString("Test Complete~!!!\n");
}

void ckCoshellCmdShowTick(const char *param)
{
	ckTerminalPrintStringF("tick count (low 32bit) : %u [0x%08x]\n",
		g_TimerStruct.TickCountLow, g_TimerStruct.TickCountLow);

	ckTerminalPrintStringF("tick count (64bit) : 0x%08x%08x\n",
		g_TimerStruct.TickCountHigh, g_TimerStruct.TickCountLow);
}

void ckCoshellCmdDateTime(const char *param)
{
	RTCDate date;
	RTCTime time;
	ckRTCReadDate(&date);
	ckRTCReadTime(&time);

	ckTerminalPrintStringF(
		"%04u/%02u/%02u %s\n"
		"%02u:%02u:%02u\n",
		date.year, date.month, date.dayofmonth, ckRTCDayOfWeekStr(date.dayofweek),
		time.hour, time.minute, time.second);
}

void ckCoshellCmdCpuSpeed(const char *param)
{
	uint32_t PrevHigh, PrevLow;
	uint32_t NowHigh, NowLow;

	uint32_t ms;

	if (param == NULL)
	{
		ms = 1 * 1000;	// 1 sec
		ckTerminalPrintString("default: 1000 ms (1 sec)\n");
	}
	else
	{
		ms = atoi(param);
		if (ms == 0)
		{
			ckTerminalPrintString("Usage: cpuspeed [waiting time]\n");
			return;
		}
	}

	if (ms > 3 * 1000)
	{
		ckTerminalPrintStringF("Wait for %u ms... Continue? [y/n]", ms);
		if (ckTerminalGetChar() != 'y')
			return;
	}
	ckTerminalPrintString("Now Measuring......");

	ckAsmGetTimeStamp(&PrevHigh, &PrevLow);

	ckTimerBusyDirectWait_ms(ms);

	ckAsmGetTimeStamp(&NowHigh, &NowLow);

	uint32_t Speed, tmp;
	__asm__
	(
		"sub %5, %%eax		\n"
		"sbb %4, %%edx		\n"
		"divl %6			\n"
		: "=a"(Speed), "=d"(tmp)
		: "d"(NowHigh), "a"(NowLow), "g"(PrevHigh), "g"(PrevLow), "r"(ms * 1000)
	);

	ckTerminalPrintStringF("\nCPU Speed = %d MHz\n", Speed);
}

static void testtask(void)
{
	unsigned count = 0;
	uint32_t id = ckTaskGetCurrent()->id;
	uint32_t y = id & 0x3ff;

	while (count < 0x50000)
	{
		if (count % 50 == 0)
		{
			ckTerminalWriteStringAtF(0, y, TERMINAL_CYAN, "Task %#x Running [0x%08x]", id, count);
		}
		count++;
	}

	ckTerminalWriteStringAtF(0, y, TERMINAL_CYAN, "Task %#x Complete [0x%08x]", id, count);
	if (id == 4)
		ckTaskTerminate(g_pTaskStruct->tasks[2].id);

	ckTaskExit();
}
void ckCoshellCmdTestTask(const char *param)
{
	for (int i = 0; i < 12; i++)
	{
		const TaskPriority t[] =
			{ TASK_PRIORITY_LOW, TASK_PRIORITY_NORMAL, TASK_PRIORITY_HIGH };
		void *stack = ckMemoryAllocateBuddy(4 * 1024);
		ckTaskCreate(
			(uint32_t)testtask, (uint32_t)stack + 4 * 1024, stack,
			ckProcessGetCurrentId(), t[i % 3]);
	}
}

#define TEST_MUTEX_TASK_COUNT 12
static volatile uint32_t count = 0;
static RecursiveMutex TestMutex;
static void increase_count(int i)
{
	if (i % 3000 == 0) ckTaskSchedule();
	volatile uint32_t tmp = count + 2;
	if (i % 3000 == 0) ckTaskSchedule();
	tmp -= 3;
	if (i % 3000 == 0) ckTaskSchedule();
	count = tmp + 2;
	if (i % 3000 == 0) ckTaskSchedule();
}
static void testmutex(void *param)
{
	for (int i = 0; i < 4000; i++)
	{
		ckRecursiveMutexLock(&TestMutex);
		increase_count(i);
		ckRecursiveMutexUnlock(&TestMutex);
	}

	ckTaskExit();
}
static void joinermutextest(void *param)
{
	uint32_t *arTaskId = (uint32_t *)param;
	for (int i = 0; i < TEST_MUTEX_TASK_COUNT; i++)
		ckTaskJoin(arTaskId[i]);

	ckTerminalWriteStringAtF(0, 0, TERMINAL_LIGHT_CYAN, "count: %u", count);
	count = 0;

	ckTaskExit();
}
void ckCoshellCmdTestMutex(const char *param)
{
	uint32_t *arTaskId = (uint32_t *)ckMemoryAllocateBuddy(4 * 1024);
	void *stack, *stack_top;

	ckRecursiveMutexInit(&TestMutex);
	for (int i = 0; i < TEST_MUTEX_TASK_COUNT; i++)
	{
		stack = ckMemoryAllocateBuddy(4 * 1024);
		stack_top = (uint8_t *)stack + 4 * 1024;
		ckTaskSetInitParam(&stack_top, (void *)i);

		arTaskId[i] = ckTaskCreate(
			(uint32_t)testmutex, (uint32_t)stack_top, stack,
			ckProcessGetCurrentId(), TASK_PRIORITY_BELOW_NORMAL);
	}

	stack_top = (uint8_t *)arTaskId + 4 * 1024;
	ckTaskSetInitParam(&stack_top, (void *)arTaskId);

	ckTaskCreate((uint32_t)joinermutextest, (uint32_t)stack_top, arTaskId,
		ckProcessGetCurrentId(), TASK_PRIORITY_NORMAL);
}

#define TEST_FLOAT_TASK_COUNT 12
static void testfloattask(void *param)
{
	const double PI = 3.14159265358979323846;
	const double E =  2.7182818284590452354;

	uint32_t y = (uint32_t)param + 3;
	double pi10 = 1, e10 = 1;
	double result = 1, value;

	for (int i = 0; i < 10; i++) pi10 *= PI;
	for (int i = 0; i < 10; i++) e10 *= E;
	value = pi10 / e10;

	for (int i = 0; i < 20; i++) result *= value;
	result /= 10000 * E;

	ckTerminalWriteStringAtF(0, y, TERMINAL_LIGHT_MAGENTA,
		"Task %#x : (pi^10 / e^10)^20 / (10000*e) = %d.%06u",
		ckTaskGetCurrentId(), (int)result, (int)((result - (int)result) * 1000000));

	ckTaskExit();
}
static void testfloatfreertask(const char *param)
{
	uint32_t *arTaskId = (uint32_t *)param;
	for (int i = 0; i < TEST_FLOAT_TASK_COUNT; i++)
		ckTaskJoin(arTaskId[0]);

	ckTerminalWriteStringAtF(0, 0, TERMINAL_MAGENTA,
		"testfloat : %u ms", g_TimerStruct.TickCountLow - arTaskId[TEST_FLOAT_TASK_COUNT]);

	ckTaskExit();
}
void ckCoshellCmdTestFloat(const char *param)
{
	uint32_t *arTaskId = (uint32_t *)ckMemoryAllocateBuddy(4 * 1024);
	void *stack, *stack_top;

	arTaskId[TEST_FLOAT_TASK_COUNT] = g_TimerStruct.TickCountLow;

	for (int i = 0; i < TEST_FLOAT_TASK_COUNT; i++)
	{
		stack = ckMemoryAllocateBuddy(4 * 1024);
		stack_top = (uint8_t *)stack + 4 * 1024;
		ckTaskSetInitParam(&stack_top, (void *)i);

		arTaskId[i] = ckTaskCreate(
			(uint32_t)testfloattask, (uint32_t)stack_top, stack,
			ckProcessGetCurrentId(), TASK_PRIORITY_BELOW_NORMAL);
	}

	stack_top = (uint8_t *)arTaskId + 4 * 1024;
	ckTaskSetInitParam(&stack_top, (void *)arTaskId);

	ckTaskCreate((uint32_t)testfloatfreertask, (uint32_t)stack_top, arTaskId,
		ckProcessGetCurrentId(), TASK_PRIORITY_IDLE);
}

void ckCoshellCmdTestProcess(const char *param)
{
	uint8_t *code = (uint8_t *)ckMemoryAllocateBuddy(4 * 1024);
	uint8_t *stack = (uint8_t *)ckMemoryAllocateBuddy(4 * 1024);

	uint32_t code_phy = ckDynMemLogicalToPhysical((uint32_t)code);
	uint32_t stack_phy = ckDynMemLogicalToPhysical((uint32_t)stack);

	uint32_t **PageDirectory = (uint32_t **)ckMemoryAllocateBuddy(4 * 1024);
	uint32_t *PageTable = (uint32_t *)ckMemoryAllocateBuddy(4 * 1024);

	memset(PageDirectory, 0, 4 * 1024);
	memset(PageTable, 0, 4 * 1024);

	// init kernel area
	memcpy(PageDirectory + KERNEL_PAGE_DIR_NUM,
		(uint32_t *)PAGE_DIRECTORY_ADDRESS + KERNEL_PAGE_DIR_NUM,
		KERNEL_PAGE_DIR_ENT_COUNT * 4);

	PageDirectory[0x000] =
		(uint32_t *)(ckDynMemLogicalToPhysical((uint32_t)&PageTable[0x00000])
			| PAGE_DIRENT_R | PAGE_DIRENT_P
			);

	// code
	// [0x00100000 ~ 0x00101000) D#0x000, T#0x00100
	PageTable[0x00100] = code_phy | PAGE_DIRENT_R | PAGE_DIRENT_P;
	// stack
	// [0x00101000 ~ 0x00102000) D#0x000, T#0x00101
	PageTable[0x00101] = stack_phy | PAGE_DIRENT_R | PAGE_DIRENT_P;
	// text video memory mapping
	// [0x00102000 ~ 0x00103000) D#0x000, T#0x00102
	PageTable[0x00102] = 0xb8000 | PAGE_DIRENT_R | PAGE_DIRENT_P;

	// TODO: User-mode process

	// write code
	static uint8_t c[] = {
		0x6a, (uint8_t)'p',				// push 'p'
		0xb9, 0x00, 0x00, 0x00, 0x00,	// mov ecx, ckTerminalPutChar
		0xff, 0xd1,						// call ecx
		0x83, 0xc4, 0x04,				// add esp, 4
		0xeb, 0xfe,						// jmp $ ; required user-mode and virtual memory swapping
		0xb9, 0x00, 0x00, 0x00, 0x00,	// mov ecx, ckTaskExit
		0xff, 0xd1,						// call ecx
	};

	c[3] = (uint8_t)((uint32_t)ckTerminalPutChar >> 0);
	c[4] = (uint8_t)((uint32_t)ckTerminalPutChar >> 8);
	c[5] = (uint8_t)((uint32_t)ckTerminalPutChar >> 16);
	c[6] = (uint8_t)((uint32_t)ckTerminalPutChar >> 24);

	c[14] = (uint8_t)((uint32_t)ckTaskExit >> 0);
	c[15] = (uint8_t)((uint32_t)ckTaskExit >> 8);
	c[16] = (uint8_t)((uint32_t)ckTaskExit >> 16);
	c[17] = (uint8_t)((uint32_t)ckTaskExit >> 24);

	memcpy(code, c, sizeof(c));

	uint32_t id = ckProcessCreate(0x00100000, 0x00102000, NULL, TASK_PRIORITY_NORMAL,
		PageDirectory, ckDynMemLogicalToPhysical((uint32_t)PageDirectory),
		(ProcessData) { .TermBuffer = (uint16_t *)0x00102000 }, ckProcessGetCurrent());

	ckTerminalPrintStringF("process id : %u", id);
}

void ckCoshellCmdKillTask(const char *param)
{
	if (param != NULL)
	{
		int id = atoi(param);

		if (id > 2)
			if (ckTaskTerminate((uint32_t)id))
				ckTerminalPrintStringF("Task [%d] is terminated successfully.", id);
			else
				ckTerminalPrintStringF("Failed to terminate task [%d].", id);
		else
			ckTerminalPrintStringF("%d: Invalid Id", id);
	}
	else
		ckTerminalPrintString("Usage) killtask (task id)");
}

void ckCoshellCmdKillProc(const char *param)
{
	if (param != NULL)
	{
		int id = atoi(param);

		if (id > 2)
			if (ckProcessTerminate((uint32_t)id))
				ckTerminalPrintStringF("Process [%d] is terminated successfully.", id);
			else
				ckTerminalPrintStringF("Failed to terminate process [%d].", id);
		else
			ckTerminalPrintStringF("%d: Invalid Id", id);
	}
	else
		ckTerminalPrintString("Usage) killproc (process id)");
}
