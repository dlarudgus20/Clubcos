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
 * @file pata.c
 * @date 2014. 7. 29.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "pata.h"
#include "port.h"
#include "gdt.h"
#include "idt.h"
#include "interrupt.h"
#include "pic.h"
#include "task.h"
#include "timer.h"

#include "terminal.h"

PATAStruct g_PATAStruct;

enum
{
	WAITTIME_FOR_PATA = 500
};

static inline uint16_t csPortBase(bool bPrimary)
{
	return bPrimary ? PATA_PORTBASE_PRIMARY : PATA_PORTBASE_SECONDARY;
}
static inline PATADeviceInfo *csGetDeviceInfo(bool bPrimary, bool bMaster)
{
	return &g_PATAStruct.DeviceInfo[(bPrimary ? 0 : 2) + (bMaster ? 0 : 1)];
}

static void csPATAInit(bool bPrimary);
static void csSelectDevice(bool bPrimary, bool bMaster);

typedef enum tagWaitResult { WAIT_OK, WAIT_ERROR, WAIT_TIMEOUT } WaitResult;
static WaitResult csWaitForNoBusy(bool bPrimary);

void ckPATAInitialize(void)
{
	ckBenaphoreInit(&g_PATAStruct.sem, 0);
	g_PATAStruct.IsSlaveSelected[0] = g_PATAStruct.IsSlaveSelected[1] = -1;

	ckIdtInit(g_pIdtTable + PIC_INTERRUPT_HARDDISK1, ckPATAPrimaryIntHandler, KERNEL_CODE_SEGMENT, 0);
	ckIdtInit(g_pIdtTable + PIC_INTERRUPT_HARDDISK2, ckPATASecondaryIntHandler, KERNEL_CODE_SEGMENT, 0);

	csPATAInit(false);
	csPATAInit(true);

	// 테스트용 코드
	while (1) ckAsmHlt();
}

static void csPATAInit(bool bPrimary)
{
	uint16_t portbase = csPortBase(bPrimary);

	//PATAIdentifyResult idrs;

	// regular status register가 0xff면 아예 존재하지 않음
	if (ckPortInByte(portbase + PATA_PORTIDX_STATUS) == 0xff)
	{
		csGetDeviceInfo(bPrimary, true)->bExist = false;
		csGetDeviceInfo(bPrimary, false)->bExist = false;
		return;
	}

	// 인터럽트 비활성화
	ckPortOutByte(portbase + PATA_PORTIDX_CONTROL, PATA_CTRL_INT_DISABLE);

	for (int i = 0; i < 2; i++)
	{
		bool bMaster = (i == 1);

		PATADeviceInfo *pInfo = csGetDeviceInfo(bPrimary, bMaster);
		csSelectDevice(bPrimary, bMaster, true);

		ckPortOutByte(portbase + PATA_PORTIDX_SEC_COUNT, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_SEC_NUM, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_CYL_LSB, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_CYL_MSB, 0);

		ckPortOutByte(portbase + PATA_PORTIDX_COMMAND, PATA_CMD_IDENTIFY);
		//csWaitForReady();
	}
}

static void csSelectDevice(bool bPrimary, bool bMaster)
{
	uint16_t portbase = csPortBase(bPrimary);

	int pri = bPrimary ? 0 : 1;
	int mas = bMaster ? 0 : 1;

	// 이미 select 되있다면 그냥 return
	if (g_PATAStruct.IsSlaveSelected[pri] == mas)
		return;

	ckPortOutByte(portbase + PATA_PORTIDX_DRV_HEAD,
		(bMaster ? 0 : PATA_DRVHEAD_SLAVE) | PATA_DRVHEAD_LBA | PATA_DRVHEAD_MASK);
	g_PATAStruct.IsSlaveSelected[pri] = mas;

	// 400ns delay
	for (int i = 0; i < 4; i++)
		ckPortInByte(portbase + PATA_PORTIDX_ALT_STATUS);
}

static WaitResult csWaitForNoBusy(bool bPrimary)
{
	uint16_t portbase = csPortBase(bPrimary);

	uint32_t start = g_TimerStruct.TickCountLow;
	while (g_TimerStruct.TickCountLow - start <= WAITTIME_FOR_PATA)
	{
		uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);
		if (!(status & PATA_STATUS_BUSY))
		{
			return WAIT_OK;
		}

		ckTaskSleep(1);
	}
	return WAIT_TIMEOUT;
}

/** @brief C로 짜여진 PATA Primary ISR입니다. */
void ck_PATAPrimaryIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_HARDDISK1);
}

/** @brief C로 짜여진 PATA Secondary ISR입니다. */
void ck_PATASecondaryIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_HARDDISK2);
}
