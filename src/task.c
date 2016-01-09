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
 * @file task.c
 * @brief multi-tasking & floating-point support
 * @date 2014. 5. 6.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "task.h"
#include "port.h"
#include "gdt.h"
#include "string.h"
#include "timer.h"
#include "assert.h"
#include "cpuid.h"
#include "interrupt.h"
#include "memory.h"
#include "likely.h"
#include "terminal.h"
#include "timer.h"
#include "lock_system.h"

// id로부터 Task *를 얻어옴 - 무효할 경우 NULL
static inline Task *csGetTaskFromId(uint32_t id)
{
	if (id == TASK_INVALID_ID)
		return NULL;

	Task *ptr = &g_pTaskStruct->tasks[id & TASK_ID_IDX_MASK];
	if (ptr->selector != 0 && ptr->id == id && ptr->flag != TASK_FLAG_WAITFOREXIT)
		return ptr;
	else
		return NULL;
}
// id로부터 Process *를 얻어옴 - 무효할 경우 NULL
static inline Process *csGetProcessFromId(uint32_t id)
{
	if (id == PROCESS_INVALID_ID)
		return NULL;

	Process *ptr = &g_pTaskStruct->processes[id & PROCESS_ID_IDX_MASK];
	if (ptr->PageDirectory != NULL && ptr->id == id && ptr->pMainThread != NULL)
		return ptr;
	else
		return NULL;
}


static void csTssInit(Tss *pTss,
	uint32_t cr3, uint16_t ds, uint16_t cs, uint32_t eip, uint8_t ss, uint32_t esp);
static void csTssDescriptorInit(Gdt *pGdt, Tss *pTss, uint8_t dpl);

static Task *ckTaskCreate_unsafe(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize,
	Process *pProcess, TaskPriority priority);

static void csCleanupTask(Task *pTask);
static void csCleanupProcess(Process *pProc);

static void ckTaskTerminate_internal(Task *pTask);
static void ckTaskChangePriority_internal(Task *pTask, TaskPriority priority);
static void ckTaskJoin_internal(Task *pTask);
static void ckTaskSchedule_internal(void);

static void csIdleTask(void);

void ckTaskStructInitialize(void)
{
	memset(g_pTaskStruct, 0, sizeof(TaskStruct));

	// SSE 사용 가능 여부 확인 및 초기화
	CpuidResult cpuid;
	ckAsmCpuid(CPUID_CODE_FEATURES, &cpuid);
	g_pTaskStruct->bSSEIsExist = ((cpuid.edx & CPUID_FEATURE_EDX_SSE) != 0);

	uint32_t cr0 = ckAsmGetCr0();
	cr0 |= CR0_MONITOR_COPROC | CR0_TASK_SWITCHED;
	cr0 &= ~(CR0_FPU_EMULATION);
	ckAsmSetCr0(cr0);

	if (g_pTaskStruct->bSSEIsExist)
	{
		uint32_t cr4 = ckAsmGetCr4();
		cr4 |= CR4_OSFXSR | CR4_OSXMMEXCPT;
		ckAsmSetCr4(cr4);
	}

	g_pTaskStruct->pLastTaskUsedFPU = NULL;

	// 각종 변수들 초기화
	g_pTaskStruct->RemainQuantum = TASK_QUANTUM;
	g_pTaskStruct->TaskIdMask = TASK_IDMASK_UNIT;
	g_pTaskStruct->ProcessIdMask = PROCESS_IDMASK_UNIT;
	g_pTaskStruct->ProcessorLoad = 100;

	for (int i = 0; i < COUNT_TASK_PRIORITY; i++)
	{
		ckLinkedListInit(&g_pTaskStruct->ReadyList[i]);
	}
	ckLinkedListInit(&g_pTaskStruct->WaitForExitList);
	ckLinkedListInit(&g_pTaskStruct->WaitList);

	uint32_t cr3 = ckAsmGetCr3();

	// Kernel Task Init
	Task *pTask = &g_pTaskStruct->tasks[0];
	ckLinkedListInit(&pTask->waitable.listOfWaiters);
	pTask->WaitedObj = NULL;
	pTask->selector = TASK_GDT_0;
	pTask->flag = TASK_FLAG_RUNNING;
	pTask->boost = 0;
	pTask->origin_prior = KERNEL_TASK_PRIORITY;
	pTask->priority = KERNEL_TASK_PRIORITY;
	pTask->bFpuUsed = false;
	pTask->UsedCpuTime = 0;
	pTask->stack = NULL;
	pTask->id = KERNEL_TASK_ID;
	g_pTaskStruct->ExecuteCount[KERNEL_TASK_PRIORITY] = 1;

	// cr3 레지스터는 수동으로 설정해 주어야 하는 것 같다.
	pTask->tss.cr3 = cr3;
	csTssDescriptorInit(g_pGdtTable + TASK_GDT_0, &pTask->tss, 0);

	g_pTaskStruct->pNow = pTask;
	ckLoadTr(TASK_GDT_0 * 8);

	// Idle Task Init
	pTask = &g_pTaskStruct->tasks[1];
	ckLinkedListInit(&pTask->waitable.listOfWaiters);
	pTask->WaitedObj = NULL;
	pTask->selector = TASK_GDT_0 + 1;
	pTask->flag = TASK_FLAG_READY;
	pTask->boost = 0;
	pTask->origin_prior = TASK_PRIORITY_IDLE;
	pTask->priority = TASK_PRIORITY_IDLE;
	pTask->bFpuUsed = false;
	pTask->UsedCpuTime = 0;
	pTask->stack = NULL;
	pTask->id = IDLE_TASK_ID;

	csTssInit(&pTask->tss,
		cr3, KERNEL_DATA_SEGMENT, KERNEL_CODE_SEGMENT,
		(uint32_t)csIdleTask, KERNEL_DATA_SEGMENT, IDLE_TASK_STACK_TOP);
	csTssDescriptorInit(g_pGdtTable + TASK_GDT_0 + 1, &pTask->tss, 0);

	ckLinkedListPushBack_nosync(
		&g_pTaskStruct->ReadyList[TASK_PRIORITY_IDLE],
		&pTask->_node);

	// Kernel Process Init
	Process *pProc = &g_pTaskStruct->processes[0];
	ckLinkedListInit(&pProc->ThreadList);
	ckLinkedListPushBack_nosync(&pProc->ThreadList, &g_pTaskStruct->tasks[0].ThreadNode);
	g_pTaskStruct->tasks[0].pProcess = pProc;
	ckLinkedListPushBack_nosync(&pProc->ThreadList, &g_pTaskStruct->tasks[1].ThreadNode);
	g_pTaskStruct->tasks[1].pProcess = pProc;
	pProc->pMainThread = &g_pTaskStruct->tasks[0];

	pProc->pParentProcess = NULL;
	ckLinkedListInit(&pProc->ChildProcessList);

	pProc->PageDirectory = (uint32_t **)(cr3 - KERNEL_BASE_ADDRESS + KERNEL_PHY_BASE_ADDR);
	pProc->PageDirSize = 0;
	pProc->cr3 = cr3;
	pProc->UsedCpuTime = 0;
	pProc->id = KERNEL_PROCESS_ID;

	g_pTaskStruct->pProcData = &pProc->ProcData;
	pProc->ProcData.TermBuffer = (uint16_t *)IOMAP_MEMORY_START_ADDRESS;
}

uint32_t ckTaskCreate(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize,
	uint32_t ProcessId, TaskPriority priority)
{
	uint32_t ret_id;

	LockSystemObject lso;
	ckLockSystem(&lso);

	Process *pProcess = csGetProcessFromId(ProcessId);
	if (pProcess != NULL)
	{
		Task *pTask = ckTaskCreate_unsafe(eip, esp, stack, stacksize, pProcess, priority);
		if (pTask != NULL)
		{
			ret_id = pTask->id;
			goto ret_label;
		}
	}
	ret_id = TASK_INVALID_ID;

ret_label:
	ckUnlockSystem(&lso);

	return ret_id;
}
static Task *ckTaskCreate_unsafe(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize,
	Process *pProcess, TaskPriority priority)
{
	Task *ret = NULL;

	for (int i = 2; i < MAX_TASK; i++)
	{
		ret = &g_pTaskStruct->tasks[i];
		if (ret->selector == 0)
		{
			ret->WaitedObj = NULL;
			ckLinkedListInit(&ret->waitable.listOfWaiters);

			ret->bFpuUsed = false;
			ret->UsedCpuTime = 0;

			ret->selector = TASK_GDT_0 + i;
			ret->flag = TASK_FLAG_READY;
			ret->boost = 0;
			ret->origin_prior = priority;
			ret->priority = priority;

			ret->pProcess = pProcess;
			ckLinkedListPushBack_nosync(&pProcess->ThreadList, &ret->ThreadNode);

			ret->stack = stack;
			ret->stacksize = stacksize;

			ret->id = g_pTaskStruct->TaskIdMask | i;
			g_pTaskStruct->TaskIdMask += TASK_IDMASK_UNIT;

			csTssInit(&ret->tss, (uint32_t)pProcess->cr3,
				KERNEL_DATA_SEGMENT, KERNEL_CODE_SEGMENT,
				eip, KERNEL_DATA_SEGMENT, esp);

			csTssDescriptorInit(g_pGdtTable + TASK_GDT_0 + i, &ret->tss, 0);

			ckLinkedListPushBack_nosync(&g_pTaskStruct->ReadyList[priority], &ret->_node);

			break;
		}
	}

	return ret;
}

uint32_t ckProcessCreate(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize, TaskPriority priority,
	uint32_t **PageDirectory, uint32_t PageDirSize,
	uint32_t cr3, ProcessData ProcData, uint32_t ParentProcessId)
{
	uint32_t ret_id = PROCESS_INVALID_ID;
	Process *ret;

	LockSystemObject lso;
	ckLockSystem(&lso);

	Process *pParentProcess = csGetProcessFromId(ParentProcessId);
	if (pParentProcess != NULL)
	{
		for (int i = 0; i < MAX_PROCESS; i++)
		{
			ret = &g_pTaskStruct->processes[i];
			if (ret->PageDirectory == NULL)
			{
				ckLinkedListInit(&ret->ThreadList);

				ret->pParentProcess = pParentProcess;
				ckLinkedListInit(&ret->ChildProcessList);

				ret->PageDirectory = PageDirectory;
				ret->PageDirSize = PageDirSize;

				ret->cr3 = cr3;
				ret->UsedCpuTime = 0;

				ret_id = ret->id = g_pTaskStruct->ProcessIdMask | i;
				g_pTaskStruct->ProcessIdMask += PROCESS_IDMASK_UNIT;

				ckLinkedListPushBack_nosync(&pParentProcess->ChildProcessList, &ret->ChildNode);

				ret->ProcData = ProcData;

				ret->pMainThread = ckTaskCreate_unsafe(eip, esp, stack, stacksize, ret, priority);

				break;
			}
		}
	}

	ckUnlockSystem(&lso);

	return ret_id;
}

bool ckTaskTerminate(uint32_t TaskId)
{
	bool bRet = false;
	assert(TaskId != KERNEL_TASK_ID && TaskId != IDLE_TASK_ID);

	LockSystemObject lso;
	ckLockSystem(&lso);

	Task *pTask = csGetTaskFromId(TaskId);
	if (pTask != NULL)
	{
		ckTaskTerminate_internal(pTask);
		bRet = true;
	}

	ckUnlockSystem(&lso);

	return bRet;
}
static void ckTaskTerminate_internal(Task *pTask)
{
	LinkedList *pList = &pTask->waitable.listOfWaiters;
	for (LinkedListNode *node = ckLinkedListHead(pList);
		node != (LinkedListNode *)pList;
		node = node->pNext)
	{
		Task *pNodeTask = (Task *)((uint32_t)node - offsetof(Task, nodeOfWaitedObj));
		pNodeTask->WaitedObj = NULL;
		ckTaskResume_byptr(pNodeTask);
	}

	if (g_pTaskStruct->pLastTaskUsedFPU == pTask)
	{
		g_pTaskStruct->pLastTaskUsedFPU = NULL;
	}

	Process *pProc = pTask->pProcess;
	// 메인 스레드라면 프로세스와 스레드를 같이 죽임
	// 그렇지 않으면 바로 ThreadList에서 제거
	if (pProc->pMainThread != pTask)
	{
		ckLinkedListErase(&pProc->ThreadList, &pTask->ThreadNode);
		pProc = pTask->pProcess = NULL;
	}
	else
	{
		pProc->pMainThread = NULL;
	}

	if (pTask == g_pTaskStruct->pNow)
	{
		assert(pTask->WaitedObj == NULL);

		pTask->flag = TASK_FLAG_WAITFOREXIT;
		ckLinkedListPushBack_nosync(&g_pTaskStruct->WaitForExitList, &pTask->_node);

		ckTaskSchedule_internal();
		while (1) { } /* 이 코드는 실행되지 않음 */
	}
	else
	{
		if (pTask->WaitedObj != NULL)
		{
			//ckLinkedListErase(&pTask->WaitedObj->listOfWaiters, &pTask->nodeOfWaitedObj);
			//pTask->WaitedObj = NULL;

			assert(pTask->flag == TASK_FLAG_WAIT);
			ckLinkedListErase(&g_pTaskStruct->WaitList, &pTask->_node);

			pTask->flag = TASK_FLAG_ZOMBIE;
		}
		else
		{
			assert(pTask->flag == TASK_FLAG_READY);
			ckLinkedListErase(&g_pTaskStruct->ReadyList[pTask->priority], &pTask->_node);

			csCleanupTask(pTask);
		}
	}
}

bool ckProcessTerminate(uint32_t ProcessId)
{
	bool bRet = false;
	assert(ProcessId != KERNEL_PROCESS_ID);

	LockSystemObject lso;
	ckLockSystem(&lso);

	Process *pProc = csGetProcessFromId(ProcessId);
	if (pProc != NULL)
	{
		ckTaskTerminate_internal(pProc->pMainThread);
		bRet = true;
	}

	ckUnlockSystem(&lso);

	return bRet;
}

static void csCleanupTask(Task *pTask)
{
	Process *pProc = pTask->pProcess;

	if (pProc != NULL)
		csCleanupProcess(pProc);

	if (pTask->stack != NULL)
		ckDynMemFree(pTask->stack, pTask->stacksize);

	ckGdtInitNull(g_pGdtTable + pTask->selector);
	pTask->selector = 0;
}

static void csCleanupProcess(Process *pProc)
{
	ckLinkedListErase(&pProc->pParentProcess->ChildProcessList, &pProc->ChildNode);

	uint32_t **pdir = pProc->PageDirectory;
	if (pdir != NULL)
	{
		uint32_t **dirend = pdir + KERNEL_PAGE_DIR_NUM;
		for (uint32_t **dirptr = pdir; dirptr < dirend; dirptr++)
		{
			if (*dirptr != NULL)
			{
				uint32_t *ptbl = (uint32_t *)ckDynMemPhysicalToLogical((uint32_t)*dirptr & ~0x3ff);

				uint32_t *tblend = ptbl + 1024;
				for (uint32_t *tblptr = ptbl; tblptr < tblend; tblptr++)
				{
					if (*tblptr != 0 && ckDynMemPhyIsDynMem(*tblptr & ~0x3ff))
					{
						uint32_t addr = ckDynMemPhysicalToLogical(*tblptr & ~0x3ff);
						ckDynMemFree((void *)addr, DYN_MEM_BUDDY_UNIT_SIZE);
						// 중복 해제를 방지하기 위해
						*tblptr = 0;
					}
				}

				ckDynMemFree(ptbl, DYN_MEM_BUDDY_UNIT_SIZE);
				// 중복 해제를 방지하기 위해
				*dirptr = 0;
			}
		}

		if (pProc->PageDirSize != 0)
			ckDynMemFree(pProc->PageDirectory, pProc->PageDirSize);
	}
}

bool ckTaskChangePriority(uint32_t TaskId, TaskPriority priority)
{
	bool bRet = false;

	LockSystemObject lso;
	ckLockSystem(&lso);

	Task *pTask = csGetTaskFromId(TaskId);
	if (pTask != NULL)
	{
		ckTaskChangePriority_internal(pTask, priority);
		bRet = true;
	}

	ckUnlockSystem(&lso);

	return bRet;
}
static void ckTaskChangePriority_internal(Task *pTask, TaskPriority priority)
{
	if (pTask->flag != TASK_FLAG_READY)
	{
		pTask->priority = priority;
	}
	else if (pTask->priority != priority)
	{
		ckLinkedListErase(&g_pTaskStruct->ReadyList[pTask->priority], &pTask->_node);
		ckLinkedListPushBack_nosync(&g_pTaskStruct->ReadyList[priority], &pTask->_node);
		pTask->priority = priority;
	}
}

bool ckTaskSuspend(uint32_t TaskId)
{
	bool bRet = false;
	assert(TaskId != KERNEL_TASK_ID && TaskId != IDLE_TASK_ID);

	LockSystemObject lso;
	ckLockSystem(&lso);

	Task *pTask = csGetTaskFromId(TaskId);
	if (pTask != NULL)
	{
		bRet = ckTaskSuspend_byptr(pTask);
	}

	ckUnlockSystem(&lso);

	return bRet;
}

bool ckTaskSuspend_byptr(Task *pTask)
{
	bool bRet = false;

	LockSystemObject lso;
	ckLockSystem(&lso);

	TaskFlag flag = pTask->flag;

	if (flag != TASK_FLAG_WAIT)
	{
		if (flag == TASK_FLAG_READY)
		{
			ckLinkedListErase(&g_pTaskStruct->ReadyList[pTask->priority], &pTask->_node);
		}

		ckLinkedListPushBack_nosync(&g_pTaskStruct->WaitList, &g_pTaskStruct->pNow->_node);

		pTask->flag = TASK_FLAG_WAIT;
		if (flag == TASK_FLAG_RUNNING)
		{
			// 스케쥴링 함수에서 Suspend
			ckTaskSchedule_internal();

			// 다시 Resume됨.
		}
		bRet = true;
	}

	ckUnlockSystem(&lso);

	return bRet;
}

bool ckTaskResume(uint32_t TaskId)
{
	bool bRet = false;
	assert(TaskId != KERNEL_TASK_ID && TaskId != IDLE_TASK_ID);

	LockSystemObject lso;
	ckLockSystem(&lso);

	Task *pTask = csGetTaskFromId(TaskId);
	if (pTask != NULL)
	{
		bRet = ckTaskResume_byptr(pTask);
	}

	ckUnlockSystem(&lso);

	return bRet;
}
bool ckTaskResume_byptr(Task *pTask)
{
	bool bRet = false;

	LockSystemObject lso;
	ckLockSystem(&lso);

	if (pTask->flag == TASK_FLAG_WAIT)
	{
		ckLinkedListErase(&g_pTaskStruct->WaitList, &pTask->_node);

		pTask->flag = TASK_FLAG_READY;
		pTask->boost = TASK_BOOST_COUNT;
		pTask->priority = (pTask->priority > TASK_PRIORITY_HIGHEST + 2) ? pTask->priority : TASK_PRIORITY_HIGHEST;
		ckLinkedListPushBack_nosync(&g_pTaskStruct->ReadyList[pTask->priority], &pTask->_node);
		bRet = true;
	}
	else if (pTask->flag == TASK_FLAG_ZOMBIE)
	{
		csCleanupTask(pTask);
	}

	ckUnlockSystem(&lso);

	return bRet;
}

void ckTaskJoin(uint32_t TaskId)
{
	LockSystemObject lso;
	ckLockSystem(&lso);

	Task *pTask = csGetTaskFromId(TaskId);
	if (pTask != NULL)
		ckTaskJoin_internal(pTask);

	ckUnlockSystem(&lso);
}

static void ckTaskJoin_internal(Task *pTask)
{
	Task *pNow = ckTaskGetCurrent();

	assert(pNow->WaitedObj == NULL);

	if (pTask->flag != TASK_FLAG_WAITFOREXIT)
	{
		pNow->WaitedObj = &pTask->waitable;
		ckLinkedListPushBack_nosync(&pTask->waitable.listOfWaiters, &pNow->nodeOfWaitedObj);
		ckTaskSuspend_byptr(pNow);
	}
}

void ckProcessJoin(uint32_t ProcId)
{
	LockSystemObject lso;
	ckLockSystem(&lso);

	Process *pProc = csGetProcessFromId(ProcId);
	if (pProc != NULL)
		ckTaskJoin_internal(pProc->pMainThread);

	ckUnlockSystem(&lso);
}

void ckTaskSleep(uint32_t milisecond)
{
	uint32_t start = g_TimerStruct.TickCountLow;

	// milisecond가 0이여도 최소 한번은 스케쥴링을 발생시킴.
	do
	{
		ckTaskSchedule();
	} while (g_TimerStruct.TickCountLow - start < milisecond);
}

void ckTaskSchedule(void)
{
	LockSystemObject lso;
	ckLockSystem(&lso);

	ckTaskSchedule_internal();

	ckUnlockSystem(&lso);
}

void ckTaskScheduleOnTimerInt(void)
{
	if (--g_pTaskStruct->RemainQuantum == 0)
	{
		ckTaskSchedule_internal();
	}
}

static void ckTaskSchedule_internal(void)
{
	Task *pNow = NULL;

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < COUNT_TASK_PRIORITY; i++)
		{
			if (g_pTaskStruct->ExecuteCount[i] < g_pTaskStruct->ReadyList[i].size)
			{
				pNow = (Task *)ckLinkedListPopFront_nosync(&g_pTaskStruct->ReadyList[i]);
				g_pTaskStruct->ExecuteCount[i]++;
				break;
			}
			else
			{
				g_pTaskStruct->ExecuteCount[i] = 0;
			}
		}

		if (pNow != NULL)
			break;
	}

	if (pNow != NULL)
	{
		Task *pPrev = g_pTaskStruct->pNow;

		if (pPrev->flag == TASK_FLAG_RUNNING)
		{
			ckLinkedListPushBack_nosync(&g_pTaskStruct->ReadyList[pPrev->priority], &pPrev->_node);
			pPrev->flag = TASK_FLAG_READY;
		}

		if (pPrev->boost != 0)
		{
			if (--pPrev->boost == 0)
			{
				ckTaskChangePriority_internal(pPrev, pPrev->origin_prior);
			}
		}

		// 태스크가 사용한 CPU 퀀텀 계산
		Process *pProc = pPrev->pProcess;
		if (likely(pProc != NULL))
		{
			uint32_t UsedCpuTime = TASK_QUANTUM - g_pTaskStruct->RemainQuantum;
			pProc->UsedCpuTime += UsedCpuTime;
			pPrev->UsedCpuTime += UsedCpuTime;
		}

		// ProcData 갱신
		g_pTaskStruct->pProcData = &pNow->pProcess->ProcData;

		// 전환할 태스크가 마지막으로 fpu를 사용한 태스크가 아니라면 TS = 1
		// 그렇지 않으면 TS = 0 (FPU 콘텍스트를 교체할 필요가 없음)
		if (pNow != g_pTaskStruct->pLastTaskUsedFPU)
		{
			ckAsmSetCr0(ckAsmGetCr0() | CR0_TASK_SWITCHED);
		}
		else
		{
			ckAsmClearTS();
		}

		g_pTaskStruct->pNow = pNow;
		g_pTaskStruct->pNow->flag = TASK_FLAG_RUNNING;

		g_pTaskStruct->RemainQuantum = TASK_QUANTUM;

		ckAsmFarJmp(0, g_pTaskStruct->pNow->selector * 8);
	}
}

// #NM Device No Available
void ck_ExceptIntHandler07(InterruptContext *pContext)
{
	// warning: 멀티코어라면 어떨까?

	ckAsmClearTS();

	Task *pNow = g_pTaskStruct->pNow;
	Task *pLastTaskUsedFPU = g_pTaskStruct->pLastTaskUsedFPU;

	if (pNow != pLastTaskUsedFPU)
	{
		if (pLastTaskUsedFPU != NULL)
		{
			ckFpuFxsave(&pLastTaskUsedFPU->fpu_context);
		}

		if (pNow->bFpuUsed)
		{
			ckFpuFxrstor(&pNow->fpu_context);
		}
		else
		{
			ckFpuFinit();
			pNow->bFpuUsed = true;
		}

		g_pTaskStruct->pLastTaskUsedFPU = pNow;
	}
}

static void csIdleTask(void)
{
	Task *pTaskMe = ckTaskGetCurrent();

	uint32_t LastTickCount = g_TimerStruct.TickCountLow;
	uint32_t LastIdleTime = pTaskMe->UsedCpuTime;

	uint32_t TickCount, DiffTick;
	uint32_t IdleTime, DiffIdleTime;
	uint32_t ProcessorLoad;
	Task *pTask;

	LinkedList NoticeQueue;
	TimeOut timeout;

	ckLinkedListInit(&NoticeQueue);
	timeout.timeout = g_TimerStruct.TickCountLow + 500;
	timeout.NoticeQueue = &NoticeQueue;
	ckTimerSet(&timeout);

	while (1)
	{
		/* 프로세서 사용률 측정*/

		TickCount = g_TimerStruct.TickCountLow;
		DiffTick = TickCount - LastTickCount;

		IdleTime = pTaskMe->UsedCpuTime;
		DiffIdleTime = IdleTime - LastIdleTime;

		if (DiffTick == 0)
		{
			g_pTaskStruct->ProcessorLoad = ProcessorLoad = 0;
		}
		else
		{
			g_pTaskStruct->ProcessorLoad = ProcessorLoad = 100 - (DiffIdleTime * 100) / DiffTick;
		}

		LastTickCount = TickCount;
		LastIdleTime = IdleTime;

		/* 프로세서 사용률에 따라 hlt함 */

		if (ProcessorLoad < 40)
			ckAsmHlt();
		if (ProcessorLoad < 80)
			ckAsmHlt();
		if (ProcessorLoad < 95)
			ckAsmHlt();

		/* TASK_WAITFOREXIT 상태의 태스크 처리 */

		while (1)
		{
			pTask = (Task *)ckLinkedListPopFront_mpsc(&g_pTaskStruct->WaitForExitList);
			if (pTask == NULL)
				break;

			csCleanupTask(pTask);
		}

		/* 0.5초마다 processor load 출력 */

		if (ckLinkedListPopFront_mpsc(&NoticeQueue) != NULL)
		{
			ckTerminalPrintStatusBarF("ProcessorLoad : %03u%%", ProcessorLoad);

			timeout.timeout = g_TimerStruct.TickCountLow + 500;
			timeout.NoticeQueue = &NoticeQueue;
			ckTimerSet(&timeout);
		}

		/* 끝 - 다른 태스크에게 시간 양보 */

		ckTaskSchedule();
	}
}

static void csTssInit(Tss *pTss,
	uint32_t cr3, uint16_t ds, uint16_t cs, uint32_t eip, uint8_t ss, uint32_t esp)
{
	pTss->backlink = pTss->esp0 = pTss->ss0 = pTss->esp1 = pTss->ss1 = pTss->esp2 = pTss->ss2 = 0;

	pTss->cr3 = cr3;

	pTss->eip = eip;
	pTss->eflags = EFLAG_IF | EFLAG_2;

	pTss->eax = pTss->ecx = pTss->edx = pTss->ebx = pTss->esi = pTss->edi = 0;

	pTss->ebp = pTss->esp = esp;

	pTss->ds = pTss->es = pTss->fs = pTss->gs = ds;
	pTss->cs = cs;
	pTss->ss = ss;

	pTss->ldtr = 0;
	pTss->iomap = 0x40000000;
}

static void csTssDescriptorInit(Gdt *pGdt, Tss *pTss, uint8_t dpl)
{
	pGdt->Size_0_15 = 0x68;
	pGdt->Size_16_19 = 0;
	pGdt->G = 0;

	pGdt->Address_0_15 = (uint16_t)(uint32_t)pTss;
	pGdt->Address_16_23 = (uint8_t)((uint32_t)pTss >> 16);
	pGdt->Address_24_32 = (uint8_t)((uint32_t)pTss >> 24);

	pGdt->SegmentType = 0x9;
	pGdt->S = 0;
	pGdt->DPL = dpl;
	pGdt->P = 1;

	pGdt->reserved = pGdt->D = 0;
}

