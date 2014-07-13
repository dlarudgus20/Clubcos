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
 * @file task.h
 * @date 2014. 5. 6.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef TASK_H_
#define TASK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "memory_map.h"

enum
{
	TASK_QUANTUM = 20,

	MAX_TASK = 2048,

	TASK_GDT_0 = 3,

	TASK_IDMASK_UNIT = MAX_TASK << 1,	// 1비트를 0으로 남겨둠
	TASK_INVALID_ID = 0xffffffff,		// 따라서 모든 비트가 1이 될 수 없음
	TASK_ID_IDX_MASK = MAX_TASK - 1,

	KERNEL_TASK_ID = 0,
	IDLE_TASK_ID = 1,
};
enum
{
	MAX_PROCESS = 512,

	PROCESS_IDMASK_UNIT = MAX_PROCESS << 1,
	PROCESS_INVALID_ID = 0xffffffff,
	PROCESS_ID_IDX_MASK = MAX_PROCESS - 1,

	KERNEL_PROCESS_ID = 0,
};

typedef enum tagTaskPriority TaskPriority;
enum tagTaskPriority
{
	TASK_PRIORITY_REALTIME,
	TASK_PRIORITY_HIGHEST,
	TASK_PRIORITY_HIGH,
	TASK_PRIORITY_ABOVE_NORMAL,
	TASK_PRIORITY_NORMAL,
	TASK_PRIORITY_BELOW_NORMAL,
	TASK_PRIORITY_LOW,
	TASK_PRIORITY_LOWEST,
	TASK_PRIORITY_IDLE,

	COUNT_TASK_PRIORITY,
	MAX_TASK_PRIORITY = COUNT_TASK_PRIORITY - 1,

	KERNEL_TASK_PRIORITY = TASK_PRIORITY_HIGHEST
};

typedef struct tagTss Tss;
struct tagTss
{
	uint32_t backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldtr, iomap;
};

typedef enum tagTaskFlag TaskFlag;
enum tagTaskFlag
{
	TASK_FLAG_RUNNING,
	TASK_FLAG_READY,
	TASK_FLAG_WAIT,
	TASK_FLAG_WAITFOREXIT,
};

typedef struct tagFpuContext FpuContext;
struct tagFpuContext
{
	uint8_t data[512];
} __attribute__((aligned(16)));

typedef struct tagProcessData ProcessData;
struct tagProcessData
{
	// memory I/O mapping address
	uint16_t *TermBuffer;
};

// forward declaration
typedef struct tagTask Task;
typedef struct tagProcess Process;

struct tagTask
{
	LinkedListNode _node;

	LinkedList WaitMeList;

	LinkedListNode WaitNode;
	struct tagTask *WaitObj;

	uint32_t selector;		// 0이면 아직 사용되지 않은 Task 구조체

	TaskFlag flag:2;
	TaskPriority priority:30;

	Tss tss;

	FpuContext fpu_context;
	int bFpuUsed;	// bool

	volatile uint32_t UsedCpuTime;

	Process *pProcess;
	LinkedListNode ThreadNode;

	void *stack;
	uint32_t stacksize;

	uint32_t id;
} __attribute__((aligned(16)));

struct tagProcess
{
	LinkedListNode _node;

	LinkedList ThreadList;
	Task *pMainThread;			// NULL이면 idle에서 cleanup 대기중

	Process *pParentProcess;
	LinkedListNode ChildNode;

	LinkedList ChildProcessList;

	uint32_t **PageDirectory;	// NULL이면 아직 사용되지 않은 Process 구조체
	uint32_t cr3;				// 물리 주소여야 함

	volatile uint32_t UsedCpuTime;

	uint32_t id;

	ProcessData ProcData;
};

typedef struct tagTaskStruct TaskStruct;
struct tagTaskStruct
{
	Task tasks[MAX_TASK];
	uint32_t TaskIdMask;

	Process processes[MAX_PROCESS];
	uint32_t ProcessIdMask;

	LinkedList ReadyList[COUNT_TASK_PRIORITY];
	LinkedList WaitList;
	LinkedList WaitForExitList;

	uint32_t ExecuteCount[COUNT_TASK_PRIORITY];
	Task *pNow;
	ProcessData *pProcData;

	volatile uint32_t ProcessorLoad;

	uint32_t RemainQuantum;

	Task *pLastTaskUsedFPU;

	bool bSSEIsExist;

	uint8_t _padding[3];
};

static TaskStruct * const g_pTaskStruct = (TaskStruct *)TASKSTRUCT_ADDRESS;

void ckTaskStructInitialize(void);

uint32_t ckTaskCreate(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize,
	uint32_t ProcessId, TaskPriority priority);

bool ckTaskTerminate(uint32_t TaskId);
static inline void ckTaskExit(void) { ckTaskTerminate(g_pTaskStruct->pNow->id); }

uint32_t ckProcessCreate(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize, TaskPriority priority,
	uint32_t **PageDirectory, uint32_t cr3, ProcessData ProcData, Process *pParentProcess);

bool ckProcessTerminate(uint32_t ProcessId);
static inline void ckProcessExit(void) { ckProcessTerminate(g_pTaskStruct->pNow->pProcess->id); }

bool ckTaskChangePriority(uint32_t TaskId, TaskPriority priority);

bool ckTaskSuspend(uint32_t TaskId);
bool ckTaskResume(uint32_t TaskId);
bool ckTaskSuspend_byptr(Task *pTask);
bool ckTaskResume_byptr(Task *pTask);

void ckTaskJoin(uint32_t TaskId);
void ckProcessJoin(uint32_t ProcId);

void ckTaskSchedule(void);
void ckTaskScheduleOnTimerInt(void);

static inline Task *ckTaskGetCurrent(void) { return g_pTaskStruct->pNow; }
static inline uint32_t ckTaskGetCurrentId(void) { return g_pTaskStruct->pNow->id; }

static inline Process *ckProcessGetCurrent(void) { return g_pTaskStruct->pNow->pProcess; }
static inline uint32_t ckProcessGetCurrentId(void) { return g_pTaskStruct->pNow->pProcess->id; }
static inline ProcessData *ckProcessGetData(void) { return g_pTaskStruct->pProcData; }

static inline Task *ckTaskGetKernel(void) { return g_pTaskStruct->tasks + 0; }
static inline Task *ckTaskGetIdle(void) { return g_pTaskStruct->tasks + 1; }
static inline Process *ckProcessGetKernel(void) { return g_pTaskStruct->processes + 0; }

static inline void ckTaskSetInitParam(void **pStackTop, void *param)
{
	*pStackTop -= sizeof(void *);
	*(void **)(*pStackTop) = param;
	*pStackTop -= sizeof(void *);
	*(void **)(*pStackTop) = ckTaskExit;
}

static inline void ckLoadTr(uint32_t tr)
{
	__asm__ __volatile__ ( "ltr (%0)" : : "r"(&tr) );
}


// fpu assembly function
static inline void ckFpuFinit()
{
	__asm__ __volatile__ ( "finit" );
}

static inline void ckFpuFxsave(FpuContext *pFpuContext)
{
	__asm__ __volatile__ ( "fxsave %0" : "=m"(*pFpuContext) );
}

static inline void ckFpuFxrstor(const FpuContext *pFpuContext)
{
	__asm__ __volatile__ ( "fxrstor %0" : : "m"(*pFpuContext) );
}

#endif /* TASK_H_ */
