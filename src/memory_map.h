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
 * @file memory_map.h
 * @date 2014. 5. 3.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef MEMORY_MAP_H_
#define MEMORY_MAP_H_

// 참조: misc/docs/memory_map.txt
// 이곳의 address는 특별한 언급이 없는 한 전부 logical address입니다.

/** @brief Clubcos에서 요구하는 최소 메모리 크기입니다. */
#define MINIMAL_REQUIRE_MEMORY 0x02000000 // 32MB

/** @brief 커널 영역이 있는 선형 주소입니다. */
#define KERNEL_BASE_ADDRESS 0xfe000000
/** @brief 커널 영역이 있는 물리 주소입니다. */
#define KERNEL_PHY_BASE_ADDR 0x00100000

/** @brief 커널 영역을 나타내는 페이지 디렉토리 엔트리의 번호입니다. */
#define KERNEL_PAGE_DIR_NUM 0x3f8
/** @brief 커널 영역을 나타내는 페이지 디렉토리 엔트리의 갯수입니다. */
#define KERNEL_PAGE_DIR_ENT_COUNT (0x400 - KERNEL_PAGE_DIR_NUM)

/** @brief 페이지 디렉토리의 선형 주소입니다. */
#define PAGE_DIRECTORY_ADDRESS 0xfe200000
/** @brief 페이지 테이블의 선형 주소입니다. */
#define PAGE_TABLE_ADDRESS 0xfe201000

/** @brief I/O 맵핑 영역을 나타내는 페이지 테이블 엔트리의 번호입니다. */
#define PAGE_TABLE_OF_SHARED_MEMORY 0xbf800
/** @brief I/O 맵핑 영역을 나타내는 페이지 테이블 엔트리의 선형 주소입니다. */
#define PAGE_TABLE_ADDR_OF_SHARED_MEMORY (PAGE_TABLE_ADDRESS + PAGE_TABLE_OF_SHARED_MEMORY * 4)

/** @brief GDT 테이블의 선형 주소입니다. @sa g_pGdtTable */
#define GDT_TABLE_ADDRESS 0xfe601000
/** @brief IDT 테이블의 선형 주소입니다. @sa g_pIdtTable */
#define IDT_TABLE_ADDRESS 0xfe611000

/** @brief TimeOut 테이블의 선형 주소입니다. @sa TimeOut */
#define TIMEOUT_TABLE_ADDRESS 0xfe611800

/** @brief TaskStruct 구조체의 선형 주소입니다. @sa g_pTaskStruct */
#define TASKSTRUCT_ADDRESS 0xfe619800

/** @brief Coshell 태스크의 스택 최상단의 선형 주소입니다. */
#define COSHELL_TASK_STACK_TOP 0xfe7ad0c4

/** @brief 아이들 태스크의 스택 최상단의 선형 주소입니다. */
#define IDLE_TASK_STACK_TOP 0xfe800000


/** @brief 동적 메모리 영역의 선형 시작 주소입니다. */
#define DYN_MEMORY_START_ADDRESS 0x00100000
/** @brief I/O 맵핑 메모리 영역의 선형 시작 주소입니다. */
#define IOMAP_MEMORY_START_ADDRESS 0xbf800000

/** @brief 동적 메모리 영역의 물리 시작 주소입니다. */
#define DYN_MEMORY_PHYSICAL_ADDRESS 0x00900000


/**
 * @brief 동적 메모리 영역에 있는 선형 주소를 물리 주소로 변환합니다.
 * @param[in] address 동적 메모리 영역에 있는 선형 주소입니다.
 * @return 변환된 물리 주소입니다.
 */
static inline uint32_t ckDynMemLogicalToPhysical(uint32_t address)
{
	return address - DYN_MEMORY_START_ADDRESS + DYN_MEMORY_PHYSICAL_ADDRESS;
}
/**
 * @brief 동적 메모리 영역에 있는 물리 주소를 선형 주소로 변환합니다.
 * @param[in] address 동적 메모리 영역에 있는 물리 주소입니다.
 * @return 변환된 선형 주소입니다.
 */
static inline uint32_t ckDynMemPhysicalToLogical(uint32_t address)
{
	return address + DYN_MEMORY_START_ADDRESS - DYN_MEMORY_PHYSICAL_ADDRESS;
}

#endif /* MEMORY_MAP_H_ */
