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
#include "waitable.h"
#include "memory_map.h"

enum
{
	TASK_QUANTUM = 20,					//<! 태스크가 한번에 할당받는 CPU 시간입니다.

	MAX_TASK = 2048,					//<! 태스크의 최대 갯수입니다.

	TASK_GDT_0 = 3,						//<! 태스크 디스크럽터의 시작 번호입니다.

	// 1비트를 0으로 남겨둠
	TASK_IDMASK_UNIT = MAX_TASK << 1,	//<! 태스크 id 중복 방지를 위해 사용되는 시작 비트입니다.
	// 따라서 모든 비트가 1이 될 수 없음
	TASK_INVALID_ID = 0xffffffff,		//<! 무효한 태스크 ID입니다.
	TASK_ID_IDX_MASK = MAX_TASK - 1,	//<! 태스크 ID에서 태스크 인덱스를 얻어올 때 사용되는 마스크 비트입니다.

	KERNEL_TASK_ID = 0,					//<! 커널 태스크 ID입니다.
	IDLE_TASK_ID = 1,					//<! idle 태스크 ID입니다.
};
enum
{
	MAX_PROCESS = 512,						//<! 프로세스의 최대 갯수입니다.

	PROCESS_IDMASK_UNIT = MAX_PROCESS << 1,	//<! 프로세스 id 중복 방지를 위해 사용되는 시작 비트입니다.
	PROCESS_INVALID_ID = 0xffffffff,		//<! 무효한 프로세스 ID입니다.
	PROCESS_ID_IDX_MASK = MAX_PROCESS - 1,	//<! 프로세스 ID에서 태스크 인덱스를 얻어올 때 사용되는 마스크 비트입니다.

	KERNEL_PROCESS_ID = 0,					//<! 커널 프로세스 ID입니다.
};

/** @brief 태스크의 스케쥴 우선순위를 나타내는 열거형입니다. */
typedef enum tagTaskPriority
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
} TaskPriority;

/** @brief TSS를 나타내는 구조체입니다. */
typedef struct tagTss
{
	uint32_t backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldtr, iomap;
} Tss;

/**
 * @brief 태스크의 현재 상태를 나타내는 열거형입니다.
 */
typedef enum tagTaskFlag
{
	TASK_FLAG_RUNNING,		//!< 태스크가 현재 실행 중입니다.
	TASK_FLAG_READY,		//!< 태스크가 실행 대기중입니다.
	TASK_FLAG_WAIT,			//!< 태스크가 waitable object를 기다리고 있습니다.
	TASK_FLAG_WAITFOREXIT,	//!< 태스크가 종료된 후 idle 태스크에 의해 정리되기를 기다리고 있습니다.
	TASK_FLAG_ZOMBIE,		//!< 태스크가 waitable object를 기다리던 도중 terminate됬습니다.
} TaskFlag;

/** @brief FPU 콘텍스트를 나타내는 구조체입니다. */
typedef struct tagFpuContext
{
	uint8_t data[512];
} __attribute__((aligned(16))) FpuContext;

/** @brief 프로세스 별로 가지는 데이터입니다. */
typedef struct tagProcessData
{
	uint16_t *TermBuffer;	//!< 터미널 버퍼의 linear address입니다.
} ProcessData;

// forward declaration
struct tagProcess;

/** @brief 태스크를 나타내는 구조체입니다. */
typedef struct tagTask
{
	union
	{
		LinkedListNode _node; // waitable.nodeOfOwner
		Waitable waitable;
	};

	LinkedListNode nodeOfWaitedObj;	//!< waitable object를 기다릴 때 사용되는 노드입니다.
	Waitable *WaitedObj;			//!< 이 값이 <c>NULL</c>이 아닐 경우 태스크가 이 waitable object를 기다리고 있습니다.

	uint32_t selector;				//!< 태스크 디스크럽터 셀렉터입니다. 0이면 사용되지 않은 <c>Task</c> 구조체입니다.

	TaskFlag flag:3;				//!< 태스크의 상태 flag입니다.
	TaskPriority priority:29;		//!< 태스크의 우선순위입니다.

	Tss tss;						//!< 태스크의 TSS입니다.

	FpuContext fpu_context;			//!< 태스크의 FPU 콘텍스트입니다.
	int bFpuUsed;					//!< 이 태스크가 최근 FPU를 사용했는지 나타내는 진위형입니다.
									//!<정렬을 위해 4byte <c>int</c>형으로 선언되 있습니다.

	volatile uint32_t UsedCpuTime;	//!< 이 태스크가 사용한 CPU 시간입니다.

	struct tagProcess *pProcess;	//!< 이 태스크를 가지는 프로세스입니다.
	LinkedListNode ThreadNode;		//!< 프로세스가 이 태스크를 관리하기 위해 사용하는 노드입니다.

	void *stack;					//!< 동적으로 할당된 이 태스크의 스택입니다. NULL일 경우 정적 할당된 스택을 사용합니다.
	uint32_t stacksize;				//!< 이 태스크의 스택의 크기입니다. 동적으로 할당된 스택이 아닐 경우 무효한 값입니다.

	uint32_t id;					//!< 이 태스크의 id입니다.
} __attribute__((aligned(16))) Task;

/** @brief 프로세스를 나타내는 구조체입니다. */
typedef struct tagProcess
{
	LinkedListNode _node;

	LinkedList ThreadList;				//!< 이 프로세스가 가지는 태스크의 리스트입니다.
	Task *pMainThread;					//!< 이 프로세스의 메인 태스크입니다. <c>NULL</c>이면 idle에서 정리해주기를 기다리고 있습니다.

	struct tagProcess *pParentProcess;	//!< 부모 프로세스입니다. 최상위 부모 프로세스이면 <c>NULL</c>입니다.
	LinkedListNode ChildNode;			//!< 부모 프로세스가 이 프로세스를 관리하기 위해 사용하는 노드입니다.

	LinkedList ChildProcessList;		//!< 자식 프로세스의 리스트입니다.

	uint32_t **PageDirectory;			//!< 이 프로세스의 페이지 디렉토리의 커널 프로세스의 선형 주소입니다.
										//!< <c>NULL</c> 이면 아직 사용되지 않은 <c>Process</c> 구조체입니다.
	/**
	 * @brief @ref PageDirectory 의 크기입니다. 이 값은 동적으로 할당된 @ref PageDirectory 를 해제할 떄 사용됩니다.
	 *        0이면 할당 해제를 수행하지 않습니다.
	 */
	uint32_t PageDirSize;

	uint32_t cr3;						//!< 페이지 디렉토리의 물리 주소입니다.

	volatile uint32_t UsedCpuTime;		//!< 이 프로세스가 사용한 CPU 시간입니다.

	uint32_t id;						//!< 이 프로세스의 id입니다.

	ProcessData ProcData;				//!< 이 프로세스의 data입니다.
} Process;

/** @brief 태스크 관리 구조체입니다. */
typedef struct tagTaskStruct
{
	Task tasks[MAX_TASK];						//!< 태스크의 배열입니다.
	uint32_t TaskIdMask;						//!< 태스크 id 중복 방지를 위해 사용되는 비트 마스크입니다.
												//!< 한번 사용될 때마다 @ref TASK_IDMASK_UNIT 씩 증가합니다.

	Process processes[MAX_PROCESS];				//!< 프로세스의 배열입니다.
	uint32_t ProcessIdMask;						//!< 프로세스 id 중복 방지를 위해 사용되는 비트 마스크입니다.
												//!< 한번 사용될 때마다 @ref PROCESS_IDMASK_UNIT 씩 증가합니다.

	LinkedList ReadyList[COUNT_TASK_PRIORITY];	//!< @ref TASK_FLAG_RUNNING 상태의 태스크의 우선순위 큐입니다.

	LinkedList WaitList;						//!< @ref TASK_FLAG_WAIT 상태의 태스크의 목록입니다.
	LinkedList WaitForExitList;					//!< @ref TASK_FLAG_WAITFOREXIT 상태의 태스크의 목록입니다. idle에서의 처리를 위해 mpcs로 동기화가 필요합니다.

	uint32_t ExecuteCount[COUNT_TASK_PRIORITY];	//!< 각 우선순위별로 실행된 횟수를 기록하는 배열입니다.

	Task *pNow;									//!< 현재 실행중인 태스크입니다.
	ProcessData *pProcData;						//!< 현재 실행중인 프로세스의 data입니다. <c>pNow->pProcess->ProcData</c>와 동일합니다.

	volatile uint32_t ProcessorLoad;			//!< 현재 프로세스 점유율입니다. 단위는 %입니다.

	uint32_t RemainQuantum;						//!< 태스크 스위칭까지 남은 CPU 시간입니다.

	Task *pLastTaskUsedFPU;						//!< 마지막으로 FPU를 사용한 태스크입니다.

	bool bSSEIsExist;							//!< SSE의 사용 가능 여부입니다.

	uint8_t _padding[3];
} TaskStruct;

/** @brief @ref TaskStruct 입니다. */
static TaskStruct * const g_pTaskStruct = (TaskStruct *)TASKSTRUCT_ADDRESS;

/** @brief 멀티태스킹을 초기화합니다. */
void ckTaskStructInitialize(void);

/**
 * @brief 태스크를 생성합니다.
 * @param[in] eip 태스크의 초기 eip입니다. 태스크가 실행할 코드입니다.
 * @param[in] esp 태스크의 초기 esp입니다. 스택의 최상단입니다.
 * @param[in] stack 동적으로 할당된 스택입니다. 정적으로 할당한 스택을 사용하면 <c>NULL</c>을 지정하십시오. 참고:@ref Task::stack
 * @param[in] stacksize <c>stack</c>의 크기입니다. 참고:@ref Task::stacksize
 * @param[in] ProcessId 생성될 태스크를 가지는 프로세스의 id입니다.
 * @param[in] priority 태스크의 스케쥴 우선순위입니다.
 * @return 생성된 태스크의 id입니다.
 *         <c>ProcessId</c>가 무효하거나, 태스크를 더 이상 생성할 수 없으면 @ref TASK_INVALID_ID 가 반환됩니다.
 */
uint32_t ckTaskCreate(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize,
	uint32_t ProcessId, TaskPriority priority);

/**
 * @brief 태스크를 종료시킵니다.
 * @param[in] TaskId 종료시킬 태스크입니다.
 * @return <c>TaskId</c>가 무효하다면 (이미 종료됬다면) <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckTaskTerminate(uint32_t TaskId);
/** @brief 자기 자신의 태스크를 종료합니다. <c>ckTaskTerminate(g_pTaskStruct->pNow->id);</c>의 래퍼입니다. */
static inline void ckTaskExit(void) { ckTaskTerminate(g_pTaskStruct->pNow->id); }

/**
 * @brief 프로세스를 생성합니다.
 * @param[in] eip 메인 태스크의 초기 eip입니다. 태스크가 실행할 코드입니다.
 * @param[in] esp 메인 태스크의 초기 esp입니다. 스택의 최상단입니다.
 * @param[in] stack 메인 태스크의 동적으로 할당된 스택입니다. 정적으로 할당한 스택을 사용하면 <c>NULL</c>을 지정하십시오. 참고:@ref Task::stack
 * @param[in] stacksize <c>stack</c>의 크기입니다. 참고:@ref Task::stacksize
 * @param[in] priority 태스크의 스케쥴 우선순위입니다.
 * @param[in] PageDirectory 페이지 디렉토리입니다. 참고:@ref Process::PageDirectory
 * @param[in] PageDirSize <c>PageDirectory</c>의 크기입니다. 참고:@ref Process::PageDirSize
 * @param[in] cr3 페이지 디렉토리의 물리 주소입니다. 참고:@ref Process::cr3
 * @param[in] ProcData 이 프로세스의 data입니다.
 * @param[in] ParentProcessId 부모 프로세스의 ID입니다.
 * @return 생성된 프로세스의 id입니다.
 *         <c>ParentProcessId</c>가 무효하거나, 프로세스를 더 이상 생성할 수 없으면 @ref PROCESS_INVALID_ID 가 반환됩니다.
 */
uint32_t ckProcessCreate(uint32_t eip, uint32_t esp,
	void *stack, uint32_t stacksize, TaskPriority priority,
	uint32_t **PageDirectory, uint32_t PageDirSize,
	uint32_t cr3, ProcessData ProcData, uint32_t ParentProcessId);

/**
 * @brief 프로세스를 종료시킵니다.
 * @param[in] ProcessId 종료시킬 태스크입니다.
 * @return <c>ProcessId</c>가 무효하다면 (이미 종료됬다면) <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckProcessTerminate(uint32_t ProcessId);
/** @brief 자기 자신의 프로세스를 종료합니다. <c>ckProcessTerminate(g_pTaskStruct->pNow->pProcess->id);</c>의 래퍼입니다. */
static inline void ckProcessExit(void) { ckProcessTerminate(g_pTaskStruct->pNow->pProcess->id); }

/**
 * @brief 태스크의 스케쥴 우선순위를 변경합니다.
 * @param[in] TaskId 우선순위를 변경할 태스크입니다.
 * @param[in] priority 이 우선순위로 변경합니다.
 * @return <c>TaskId</c>가 무효하다면 <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckTaskChangePriority(uint32_t TaskId, TaskPriority priority);

/**
 * @brief 태스크를 @ref TASK_FLAG_WAIT 상태로 만듭니다.
 * @param[in] TaskId 일시 정지시킬 태스크입니다.
 * @return <c>TaskId</c>가 무효하거나 이미 TASK_FLAG_WAIT 상태의 태스크라면 <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckTaskSuspend(uint32_t TaskId);
/**
 * @brief @ref TASK_FLAG_WAIT 상태의 태스크를 재개시킵니다.
 * @param[in] TaskId 재개시킬 태스크입니다.
 * @return <c>TaskId</c>가 무효하거나 TASK_FLAG_WAIT 상태가 아닌 태스크라면 <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckTaskResume(uint32_t TaskId);

/** @brief @ref tagTask 에 대한 포인터로 @ref ckTaskSuspend 함수를 호출합니다. */
bool ckTaskSuspend_byptr(Task *pTask);
/** @brief @ref tagTask 에 대한 포인터로 @ref ckTaskResume 함수를 호출합니다. */
bool ckTaskResume_byptr(Task *pTask);

/**
 * @brief 지정된 태스크가 종료될 때까지 기다립니다. 지정한 태스크 id가 무효하다면 (이미 종료됬다면) 그냥 리턴합니다.
 * @param[in] TaskId 기다릴 태스크입니다.
 */
void ckTaskJoin(uint32_t TaskId);
/**
 * @brief 지정된 프로세스가 종료될 때까지 기다립니다. 지정한 프로세스 id가 무효하다면 (이미 종료됬다면) 그냥 리턴합니다.
 * @param[in] ProcId 기다릴 프로세스입니다.
 */
void ckProcessJoin(uint32_t ProcId);

/**
 * @brief 지정된 시간동안 cpu 시간을 양보합니다.
 * @param[in] milisecond 양보할 cpu 시간입니다. 단위는 ms입니다.
 * @remark <c>milisecond</c>가 <c>0</c>이면 태스크 스케쥴링을 한번 발생시킵니다.
 */
void ckTaskSleep(uint32_t milisecond);

/** @brief CPU 시간을 양보하고 다른 태스크로 스위칭합니다. */
void ckTaskSchedule(void);
/**
 * @brief 타이머 인터럽트가 발생했을 때 호출해 태스크를 스위칭합니다.
 * @remark 타이머 인터럽트 핸들러에서 1ms마다 이 함수를 호출합니다. 현재 cpu시간이 남아있다면 이를 감소시키기만 합니다.
 *         현재 cpu 시간이 남아있지 않다면 태스크가 스위치됩니다. */
void ckTaskScheduleOnTimerInt(void);

/** @brief 현재 태스크를 얻어옵니다. @return 현재 태스크입니다. */
static inline Task *ckTaskGetCurrent(void) { return g_pTaskStruct->pNow; }
/** @brief 현재 태스크의 id를 얻어옵니다. @return 현재 태스크의 id입니다. */
static inline uint32_t ckTaskGetCurrentId(void) { return g_pTaskStruct->pNow->id; }

/** @brief 현재 프로세스를 얻어옵니다. @return 현재 프로세스입니다. */
static inline Process *ckProcessGetCurrent(void) { return g_pTaskStruct->pNow->pProcess; }
/** @brief 현재 프로세스의 id를 얻어옵니다. @return 현재 프로세스의 id입니다. */
static inline uint32_t ckProcessGetCurrentId(void) { return g_pTaskStruct->pNow->pProcess->id; }
/** @brief 현재 프로세스의 data를 얻어옵니다. <c>g_pTaskStruct->pProcData</c>와 동일합니다. @return 현재 프로세스의 data입니다. */
static inline ProcessData *ckProcessGetData(void) { return g_pTaskStruct->pProcData; }

/** @brief 커널 태스크를 얻어옵니다. @return 커널 태스크입니다. */
static inline Task *ckTaskGetKernel(void) { return g_pTaskStruct->tasks + 0; }
/** @brief idle 태스크를 얻어옵니다. @return idle 태스크입니다. */
static inline Task *ckTaskGetIdle(void) { return g_pTaskStruct->tasks + 1; }
/** @brief 커널 프로세스를 얻어옵니다. @return 커널 프로세스입니다. */
static inline Process *ckProcessGetKernel(void) { return g_pTaskStruct->processes + 0; }

/**
 * @brief 태스크를 생성하기 전 스택을 조작해 태스크에 parameter를 넘겨줍니다.
 * @param[out] pStackTop 스택의 최상단입니다.
 * @param[in] param 넘겨줄 parameter입니다.
 * @remark 이 함수는 return address를 @ref ckTaskExit 함수로 조작해 태스크 함수가 <c>return;</c> 하면 종료되도록 만듭니다.
 */
static inline void ckTaskSetInitParam(void **pStackTop, void *param)
{
	*pStackTop -= sizeof(void *);
	*(void **)(*pStackTop) = param;
	*pStackTop -= sizeof(void *);
	*(void **)(*pStackTop) = ckTaskExit;
}

/** @brief <c>ltr</c> 명령어로 TR 레지스터에 값을 넣는 어셈블리 함수 */
static inline void ckLoadTr(uint32_t tr)
{
	__asm__ __volatile__ ( "ltr (%0)" : : "r"(&tr) );
}


// fpu assembly function

/** @brief <c>finit</c> 어셈블리 함수 */
static inline void ckFpuFinit()
{
	__asm__ __volatile__ ( "finit" );
}

/** @brief <c>fxsave</c> 어셈블리 함수 */
static inline void ckFpuFxsave(FpuContext *pFpuContext)
{
	__asm__ __volatile__ ( "fxsave %0" : "=m"(*pFpuContext) );
}

/** @brief <c>fxrstor</c> 어셈블리 함수 */
static inline void ckFpuFxrstor(const FpuContext *pFpuContext)
{
	__asm__ __volatile__ ( "fxrstor %0" : : "m"(*pFpuContext) );
}

#endif /* TASK_H_ */
