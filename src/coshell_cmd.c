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
#include "simple_mutex.h"
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
		ckTerminalPrintString("Reset cancelled.\n");
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
	ckTerminalPrintString ("==========================================\n");
	ckTerminalPrintStringF("DynMem BeginAddr : 0x%08x\n", g_DynMem.BeginAddr);
	ckTerminalPrintString ("==========================================\n");
	ckTerminalPrintStringF("CountOfUnitBlock : %#x\n", g_DynMem.CountOfUnitBlock);
	ckTerminalPrintStringF("BitmapLevel      : %u\n", g_DynMem.BitmapLevel);
	ckTerminalPrintString ("==========================================\n");
	ckTerminalPrintStringF("Raw Size         : 0x%08x [%u MB]\n",
		g_DynMem.DynMemSize, g_DynMem.DynMemSize / 1024 / 1024);

	uint32_t meta = g_DynMem.BeginAddr - DYN_MEMORY_START_ADDRESS;
	ckTerminalPrintStringF("Metadata Size    : 0x%08x [%u KB]\n",
		meta, meta / 1024);

	uint32_t dmsize = DYN_MEMORY_START_ADDRESS + g_DynMem.DynMemSize - g_DynMem.BeginAddr;
	ckTerminalPrintStringF("DynMem Size      : 0x%08x [%u MB]\n",
		dmsize, dmsize / 1024 / 1024);
	ckTerminalPrintStringF("Used Size        : 0x%08x [%u KB]\n",
		g_DynMem.UsedSize, g_DynMem.UsedSize / 1024);

	ckTerminalPrintString ("==========================================\n");
}

void ckCoshellCmdTestDynSeq(const char *param)
{
	uint32_t *mem;

	for (uint32_t i = 0; i < g_DynMem.BitmapLevel; i++)
	{
		ckTerminalPrintStringF("[%u] Bitmap Level Test Start\n", i);

		ckTerminalPrintString("Allocation & Compare : ");

		for (uint32_t j = 0; j < (g_DynMem.CountOfUnitBlock >> i); j++)
		{
			mem = (uint32_t *)ckDynMemAllocate(DYN_MEM_BUDDY_UNIT_SIZE << i);
			if (mem == NULL)
			{
				ckTerminalPrintStringF(
					"\nAllocation Fail : Level[%u] Size[%u] Allocation[%u]\n",
					i, DYN_MEM_BUDDY_UNIT_SIZE << i, j);
				return;
			}

			for (uint32_t k = 0; k < ((DYN_MEM_BUDDY_UNIT_SIZE << i) / sizeof(uint32_t)); k++)
			{
				mem[k] = k;
			}
			for (uint32_t k = 0; k < ((DYN_MEM_BUDDY_UNIT_SIZE << i) / sizeof(uint32_t)); k++)
			{
				if (mem[k] != k)
				{
					ckTerminalPrintStringF(
						"\nCompare Fail : Level[%u] Size[%u] Allocation[%u] Compare[%u]\n",
						i, DYN_MEM_BUDDY_UNIT_SIZE << i, j, k);
					return;
				}
			}

			ckTerminalPutChar('.');
		}

		ckTerminalPrintString("\nFree : ");
		for (uint32_t j = 0; j < (g_DynMem.CountOfUnitBlock >> i); j++)
		{
			uint32_t free_addr = g_DynMem.BeginAddr + (DYN_MEM_BUDDY_UNIT_SIZE << i) * j;
			ckDynMemFree((void *)free_addr, DYN_MEM_BUDDY_UNIT_SIZE << i);
			ckTerminalPutChar('.');
		}

		ckTerminalPutChar('\n');
	}

	ckTerminalPrintString("`testdynseq` completed.\n");
}

static uint32_t testdynran_number;
static void testdynran_task(void *param)
{
	uint16_t y = (uint16_t)((uint32_t)param);

	uint32_t size;
	uint8_t *mem;

	for (uint32_t i = 0; i < testdynran_number; i++)
	{
		while (1)
		{
			size = rand() % 32768 * 4096;
			mem = ckDynMemAllocate(size);
			if (mem != NULL)
				break;
			ckTaskSchedule();
		}

		ckTerminalWriteStringAtF(12, y, TERMINAL_CYAN,
			"|addr:[%p] size:[%p] alloc okay", mem, size);

		for (uint32_t j = 0; j < size / 2; j++)
		{
			mem[j + size / 2] = mem[j] = rand() % 0xff;
		}

		for (uint32_t j = 0; j < size / 2; j++)
		{
			if (mem[j] != mem[j + size / 2])
			{
				ckTerminalWriteStringAt(60, y, TERMINAL_RED, ">>verify fail");
				ckTaskExit();
			}
		}

		ckDynMemFree(mem, size);
	}

	ckTaskExit();
}
void ckCoshellCmdTestDynRan(const char *param)
{
	void *stack, *stack_top;

	if (param != NULL)
		testdynran_number = atoi(param);
	if (param == NULL || testdynran_number == 0)
		testdynran_number = 50;

	srand(ckTimerGetTickCount());

	for (int i = 0; i < 11; i++)
	{
		stack = ckDynMemAllocate(4 * 1024);
		stack_top = (void *)((uint32_t)stack + 4 * 1024);
		ckTaskSetInitParam(&stack_top, (void *)i);

		ckTaskCreate(
			(uint32_t)testdynran_task, (uint32_t)stack_top,
			stack, 4 * 1024,
			ckProcessGetCurrentId(), TASK_PRIORITY_NORMAL);
	}
}

void ckCoshellCmdShowTick(const char *param)
{
	uint32_t high, low;
	ckTimerGetTickCount64(&high, &low);

	ckTerminalPrintStringF("tick count (low 32bit) : %u [0x%08x]\n", low, low);

	ckTerminalPrintStringF("tick count (64bit) : 0x%08x%08x\n", high, low);
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

static void testtask(void *param)
{
	unsigned count = 0;
	uint32_t id = ckTaskGetCurrent()->id;
	uint32_t y = (uint32_t)param + 3;

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
	const TaskPriority t[] =
		{ TASK_PRIORITY_LOW, TASK_PRIORITY_NORMAL, TASK_PRIORITY_HIGH };

	void *stack, *stack_top;

	for (int i = 0; i < 12; i++)
	{
		stack = ckDynMemAllocate(4 * 1024);
		stack_top = (uint8_t *)stack + 4 * 1024;
		ckTaskSetInitParam(&stack_top, (void *)i);

		ckTaskCreate(
			(uint32_t)testtask, (uint32_t)stack_top,
			stack, 4 * 1024,
			ckProcessGetCurrentId(), t[i % 3]);
	}
}

#define TEST_SIM_MUTEX_TASK_COUNT 12
static volatile uint32_t sim_count = 0;
static SimpleMutex TestSimMutex;
static void testsimmutex(void *param)
{
	for (int i = 0; i < 4000; i++)
	{
		ckSimpleMutexLock(&TestSimMutex);

		if (i % 3000 == 0) ckTaskSchedule();
		volatile uint32_t tmp = sim_count + 2;
		if (i % 3000 == 0) ckTaskSchedule();
		tmp -= 3;
		if (i % 3000 == 0) ckTaskSchedule();
		sim_count = tmp + 2;
		if (i % 3000 == 0) ckTaskSchedule();

		ckSimpleMutexUnlock(&TestSimMutex);
	}

	ckTaskExit();
}
static void joinersimmutextest(void *param)
{
	uint32_t *arTaskId = (uint32_t *)param;
	for (int i = 0; i < TEST_SIM_MUTEX_TASK_COUNT; i++)
		ckTaskJoin(arTaskId[i]);

	ckTerminalWriteStringAtF(0, 0, TERMINAL_LIGHT_CYAN, "count: %u [%u ms]", sim_count,
		ckTimerGetTickCount() - arTaskId[TEST_SIM_MUTEX_TASK_COUNT]);
	sim_count = 0;

	ckTaskExit();
}
void ckCoshellCmdTestSimMutex(const char *param)
{
	uint32_t *arTaskId = (uint32_t *)ckDynMemAllocate(4 * 1024);
	void *stack, *stack_top;

	arTaskId[TEST_SIM_MUTEX_TASK_COUNT] = ckTimerGetTickCount();

	ckSimpleMutexInit(&TestSimMutex);
	for (int i = 0; i < TEST_SIM_MUTEX_TASK_COUNT; i++)
	{
		stack = ckDynMemAllocate(4 * 1024);
		stack_top = (uint8_t *)stack + 4 * 1024;
		ckTaskSetInitParam(&stack_top, (void *)i);

		arTaskId[i] = ckTaskCreate(
			(uint32_t)testsimmutex, (uint32_t)stack_top,
			stack, 4 * 1024,
			ckProcessGetCurrentId(), TASK_PRIORITY_BELOW_NORMAL);
	}

	stack_top = (uint8_t *)arTaskId + 4 * 1024;
	ckTaskSetInitParam(&stack_top, (void *)arTaskId);

	ckTaskCreate((uint32_t)joinersimmutextest, (uint32_t)stack_top,
		arTaskId, 4 * 1024,
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
		"testfloat : %u ms", ckTimerGetTickCount() - arTaskId[TEST_FLOAT_TASK_COUNT]);

	ckTaskExit();
}
void ckCoshellCmdTestFloat(const char *param)
{
	uint32_t *arTaskId = (uint32_t *)ckDynMemAllocate(4 * 1024);
	void *stack, *stack_top;

	arTaskId[TEST_FLOAT_TASK_COUNT] = ckTimerGetTickCount();

	for (int i = 0; i < TEST_FLOAT_TASK_COUNT; i++)
	{
		stack = ckDynMemAllocate(4 * 1024);
		stack_top = (uint8_t *)stack + 4 * 1024;
		ckTaskSetInitParam(&stack_top, (void *)i);

		arTaskId[i] = ckTaskCreate(
			(uint32_t)testfloattask, (uint32_t)stack_top,
			stack, 4 * 1024,
			ckProcessGetCurrentId(), TASK_PRIORITY_BELOW_NORMAL);
	}

	stack_top = (uint8_t *)arTaskId + 4 * 1024;
	ckTaskSetInitParam(&stack_top, (void *)arTaskId);

	ckTaskCreate((uint32_t)testfloatfreertask, (uint32_t)stack_top,
		arTaskId, 4 * 1024,
		ckProcessGetCurrentId(), TASK_PRIORITY_IDLE);
}

void ckCoshellCmdTestProcess(const char *param)
{
	uint8_t *code = (uint8_t *)ckDynMemAllocate(4 * 1024);
	uint8_t *stack = (uint8_t *)ckDynMemAllocate(4 * 1024);

	uint32_t code_phy = ckDynMemLogicalToPhysical((uint32_t)code);
	uint32_t stack_phy = ckDynMemLogicalToPhysical((uint32_t)stack);

	uint32_t **PageDirectory = (uint32_t **)ckDynMemAllocate(4 * 1024);
	uint32_t *PageTable = (uint32_t *)ckDynMemAllocate(4 * 1024);

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

	// write code
	static uint8_t c[] = {
		0x6a, (uint8_t)'p',				// push 'p'
		0xb9, 0x00, 0x00, 0x00, 0x00,	// mov ecx, ckTerminalPutChar
		0xff, 0xd1,						// call ecx
		0x83, 0xc4, 0x04,				// add esp, 4
		0x90, 0x90,						// nop / nop
		0xb9, 0x00, 0x00, 0x00, 0x00,	// mov ecx, ckTaskExit
		0xff, 0xd1,						// call ecx
	};

	c[3] = (uint8_t)((uint32_t)ckTerminalPutChar >> 0);
	c[4] = (uint8_t)((uint32_t)ckTerminalPutChar >> 8);
	c[5] = (uint8_t)((uint32_t)ckTerminalPutChar >> 16);
	c[6] = (uint8_t)((uint32_t)ckTerminalPutChar >> 24);

	c[15] = (uint8_t)((uint32_t)ckTaskExit >> 0);
	c[16] = (uint8_t)((uint32_t)ckTaskExit >> 8);
	c[17] = (uint8_t)((uint32_t)ckTaskExit >> 16);
	c[18] = (uint8_t)((uint32_t)ckTaskExit >> 24);

	memcpy(code, c, sizeof(c));

	uint32_t id = ckProcessCreate(
		0x00100000, 0x00102000,
		NULL, 0, TASK_PRIORITY_NORMAL,
		PageDirectory, 4 * 1024,
		ckDynMemLogicalToPhysical((uint32_t)PageDirectory),
		(ProcessData) { .TermBuffer = (uint16_t *)0x00102000 },
		ckProcessGetCurrentId());

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
