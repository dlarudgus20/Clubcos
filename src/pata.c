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
#include "string.h"

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
static inline bool csIsMasterSelected(bool bPrimary)
{
	return (g_PATAStruct.IsSlaveSelected[bPrimary ? 0 : 1] == 0);
}

static void csPATAInit(bool bPrimary);
static bool csSelectDevice(bool bPrimary, bool bMaster);

static bool csWaitForNoBusy(bool bPrimary);
static bool csWaitForDriveReady(bool bPrimary);
static bool csWaitForInterrupt(bool bPrimary);

void ckPATAInitialize(void)
{
	ckBenaphoreInit(&g_PATAStruct.sem, 0);

	g_PATAStruct.DeviceInfo[0].bExist = g_PATAStruct.DeviceInfo[1].bExist = false;
	g_PATAStruct.DeviceInfo[2].bExist = g_PATAStruct.DeviceInfo[3].bExist = false;
	g_PATAStruct.IsSlaveSelected[0] = g_PATAStruct.IsSlaveSelected[1] = -1;
	g_PATAStruct.bInterruptOccurred[0] = g_PATAStruct.bInterruptOccurred[1] = false;

	ckIdtInit(g_pIdtTable + PIC_INTERRUPT_HARDDISK1, ckPATAPrimaryIntHandler, KERNEL_CODE_SEGMENT, 0);
	ckIdtInit(g_pIdtTable + PIC_INTERRUPT_HARDDISK2, ckPATASecondaryIntHandler, KERNEL_CODE_SEGMENT, 0);

	csPATAInit(false);
	csPATAInit(true);
}
#include"terminal.h"
static void csPATAInit(bool bPrimary)
{
	uint16_t portbase = csPortBase(bPrimary);

	PATADeviceInfo *pInfoAr[2] = {
		csGetDeviceInfo(bPrimary, true),
		csGetDeviceInfo(bPrimary, false)
	};

	PATAIdentifyResult idrs;

	// regular status register가 0xff면 아예 존재하지 않음
	if (ckPortInByte(portbase + PATA_PORTIDX_STATUS) == 0xff)
		return;

	// 인터럽트 활성화
	ckPortOutByte(portbase + PATA_PORTIDX_CONTROL, 0);

	// slave부터 초기화
	// 자주 쓰이는 master가 selected 상태가 되도록
	for (int mas = 1; mas >= 0; mas--)
	{
		bool bMaster = (mas == 0);

		csSelectDevice(bPrimary, bMaster);

		g_PATAStruct.bInterruptOccurred[bPrimary ? 0 : 1] = false;
		ckPortOutByte(portbase + PATA_PORTIDX_SEC_COUNT, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_SEC_NUM, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_CYL_LSB, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_CYL_MSB, 0);
		ckPortOutByte(portbase + PATA_PORTIDX_COMMAND, PATA_CMD_IDENTIFY);
		if (!csWaitForInterrupt(bPrimary))
			continue;

		uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);
		if (status & PATA_STATUS_ERROR)
			continue;

		for (int i = 0; i < 512 / 2; i++)
		{
			((uint16_t *)&idrs)[i] = ckPortInWord(portbase + PATA_PORTIDX_DATA);
		}

		swap_endian_of_shorts(idrs.vwModelNumber,
				sizeof(idrs.vwModelNumber) / sizeof(idrs.vwModelNumber[0]));
		swap_endian_of_shorts(idrs.vwSerialNumber,
				sizeof(idrs.vwSerialNumber) / sizeof(idrs.vwSerialNumber[0]));

		pInfoAr[mas]->bExist = true;
		pInfoAr[mas]->dwCountOfTotalSector = idrs.dwCountOfTotalSector;

		ckTerminalSetColor(TERMINAL_COLOR_PANIC);
		idrs.wControllerType = idrs._reserved2[0] = 0;
		ckTerminalPrintStringF("\n [%s]: config[%#x], serial[%s], model[%s], sectors[%#x]",
				(const char *[]){ "hda", "hdb", "hdc", "hdd" }[(bPrimary ? 0 : 2) + mas],
				idrs.wConfiguration, (const char *)idrs.vwModelNumber, (const char *)idrs.vwSerialNumber,
				idrs.dwCountOfTotalSector);
		ckTerminalSetColor(TERMINAL_COLOR_LOG);
	}
}

static bool csSelectDevice(bool bPrimary, bool bMaster)
{
	uint16_t portbase = csPortBase(bPrimary);

	// 이미 select 되있다면 그냥 return
	if (csIsMasterSelected(bPrimary) == bMaster)
		return true;

	if (!csWaitForNoBusy(bPrimary))
		return false;

	ckPortOutByte(portbase + PATA_PORTIDX_DRV_HEAD,
		(bMaster ? 0 : PATA_DRVHEAD_SLAVE) | PATA_DRVHEAD_LBA | PATA_DRVHEAD_MASK);
	g_PATAStruct.IsSlaveSelected[bPrimary ? 0 : 1] = (bMaster ? 0 : 1);

	// 400ns delay
	// http://wiki.osdev.org/ATA_PIO_Mode#400ns_delays
	for (int i = 0; i < 4; i++)
		ckPortInByte(portbase + PATA_PORTIDX_ALT_STATUS);

	if (!csWaitForDriveReady(bPrimary))
		return false;

	return true;
}

static bool csWaitForNoBusy(bool bPrimary)
{
	uint16_t portbase = csPortBase(bPrimary);

	uint32_t start = ckTimerGetTickCount();
	while (g_TimerStruct.TickCountLow - start <= WAITTIME_FOR_PATA)
	{
		uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);
		if (!(status & PATA_STATUS_BUSY))
		{
			return true;
		}

		ckTaskSleep(1);
	}
	return false;
}
static bool csWaitForDriveReady(bool bPrimary)
{
	uint16_t portbase = csPortBase(bPrimary);

	uint32_t start = ckTimerGetTickCount();
	while (g_TimerStruct.TickCountLow - start <= WAITTIME_FOR_PATA)
	{
		uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);
		if (status & PATA_STATUS_DRV_READY)
		{
			return true;
		}

		ckTaskSleep(1);
	}
	return false;
}
static bool csWaitForInterrupt(bool bPrimary)
{
	uint32_t start = ckTimerGetTickCount();
	while (g_TimerStruct.TickCountLow - start <= WAITTIME_FOR_PATA)
	{
		if (g_PATAStruct.bInterruptOccurred[bPrimary ? 0 : 1])
		{
			return true;
		}

		ckTaskSleep(1);
	}
	return false;
}

/** @brief C로 짜여진 PATA Primary ISR입니다. */
void ck_PATAPrimaryIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_HARDDISK1);
	g_PATAStruct.bInterruptOccurred[0] = true;
}

/** @brief C로 짜여진 PATA Secondary ISR입니다. */
void ck_PATASecondaryIntHandler(InterruptContext *pContext)
{
	ckPicSendEOI(PIC_IRQ_HARDDISK2);
	g_PATAStruct.bInterruptOccurred[1] = true;
}
