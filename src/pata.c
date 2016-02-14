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

static void csPATAInit(bool bPrimary);

static bool csWaitForNoBusy(bool bPrimary);
static bool csWaitForDriveReady(bool bPrimary);
static bool csWaitForInterrupt(bool bPrimary);

void ckPATAInitialize(void)
{
	ckSimpleMutexInit(&g_PATAStruct.mutex);

	g_PATAStruct.DeviceInfo[0].bExist = g_PATAStruct.DeviceInfo[1].bExist = false;
	g_PATAStruct.DeviceInfo[2].bExist = g_PATAStruct.DeviceInfo[3].bExist = false;
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
		&g_PATAStruct.DeviceInfo[(bPrimary ? 0 : 2)],
		&g_PATAStruct.DeviceInfo[(bPrimary ? 0 : 2) + 1]
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

		if (!csWaitForNoBusy(bPrimary))
			continue;

		ckPortOutByte(portbase + PATA_PORTIDX_DRV_HEAD,
			(bMaster ? 0 : PATA_DRVHEAD_SLAVE) | PATA_DRVHEAD_LBA | PATA_DRVHEAD_MASK);

		if (!csWaitForDriveReady(bPrimary))
			continue;

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

		memcpy(pInfoAr[mas]->ModelNumber, idrs.vwModelNumber, sizeof(idrs.vwModelNumber));
		pInfoAr[mas]->ModelNumber[sizeof(pInfoAr[mas]->ModelNumber) - 1] = '\0';
		strtrimend(pInfoAr[mas]->ModelNumber);

		memcpy(pInfoAr[mas]->SerialNumber, idrs.vwSerialNumber, sizeof(idrs.vwSerialNumber));
		pInfoAr[mas]->SerialNumber[sizeof(pInfoAr[mas]->SerialNumber) - 1] = '\0';
		strtrimend(pInfoAr[mas]->SerialNumber);
	}
}

void ckPATAOutputInfo(bool bPrimary, bool bMaster)
{
	PATADeviceInfo *pInfo = &g_PATAStruct.DeviceInfo[(bPrimary ? 0 : 2) + (bMaster ? 0 : 1)];

	const char *strpm = (const char *[]){ "hda", "hdb", "hdc", "hdd" }[(bPrimary ? 0 : 2) + (bMaster ? 0 : 1)];

	if (pInfo->bExist)
	{
		ckTerminalPrintStringF("PATA [%s]: serial[%s], model[%s], sectors[%#x]\n",
			strpm, pInfo->SerialNumber, pInfo->ModelNumber, pInfo->dwCountOfTotalSector);
	}
	else
	{
		ckTerminalPrintStringF("PATA [%s]: not exist\n", strpm);
	}
}

int ckPATAReadSector(bool bPrimary, bool bMaster, uint32_t lba, uint8_t count, void *buf)
{
	PATADeviceInfo *pInfo = &g_PATAStruct.DeviceInfo[(bPrimary ? 0 : 2) + (bMaster ? 0 : 1)];
	int ret = 0;

	if (pInfo->bExist)
	{
		if (lba + count < pInfo->dwCountOfTotalSector)
		{
			uint16_t portbase = csPortBase(bPrimary);

			ckSimpleMutexLock(&g_PATAStruct.mutex);

			if (csWaitForNoBusy(bPrimary))
			{
				uint8_t drvflag = (bMaster ? 0 : PATA_DRVHEAD_SLAVE) | PATA_DRVHEAD_LBA | PATA_DRVHEAD_MASK;

				ckPortOutByte(portbase + PATA_PORTIDX_SEC_COUNT, count);
				ckPortOutByte(portbase + PATA_PORTIDX_SEC_NUM, (uint8_t)lba);
				ckPortOutByte(portbase + PATA_PORTIDX_CYL_LSB, (uint8_t)(lba >> 8));
				ckPortOutByte(portbase + PATA_PORTIDX_CYL_MSB, (uint8_t)(lba >> 16));
				ckPortOutByte(portbase + PATA_PORTIDX_DRV_HEAD, drvflag | ((lba >> 24) & 0x0f));

				if (csWaitForDriveReady(bPrimary))
				{
					ckPortOutByte(portbase + PATA_PORTIDX_COMMAND, PATA_CMD_READ);

					int i, bufidx = 0;
					for (i = 0; i < count; i++)
					{
						uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);
						if (status & PATA_STATUS_ERROR)
						{
							break;
						}
						else if (!(status & PATA_STATUS_DATA_REQUEST))
						{
							if (!csWaitForInterrupt(bPrimary))
								break;
						}

						for (int j = 0; j < 512 / 2; j++)
						{
							((uint16_t *)buf)[bufidx++] = ckPortInWord(portbase + PATA_PORTIDX_DATA);
						}
					}

					ret = i;
				}
			}

			ckSimpleMutexUnlock(&g_PATAStruct.mutex);
		}
	}

	return ret;
}

int ckPATAWriteSector(bool bPrimary, bool bMaster, uint32_t lba, uint8_t count, const void *buf)
{
	PATADeviceInfo *pInfo = &g_PATAStruct.DeviceInfo[(bPrimary ? 0 : 2) + (bMaster ? 0 : 1)];
	int ret = 0;

	if (pInfo->bExist)
	{
		if (lba + count < pInfo->dwCountOfTotalSector)
		{
			uint16_t portbase = csPortBase(bPrimary);

			ckSimpleMutexLock(&g_PATAStruct.mutex);

			if (csWaitForNoBusy(bPrimary))
			{
				uint8_t drvflag = (bMaster ? 0 : PATA_DRVHEAD_SLAVE) | PATA_DRVHEAD_LBA | PATA_DRVHEAD_MASK;

				ckPortOutByte(portbase + PATA_PORTIDX_SEC_COUNT, count);
				ckPortOutByte(portbase + PATA_PORTIDX_SEC_NUM, (uint8_t)lba);
				ckPortOutByte(portbase + PATA_PORTIDX_CYL_LSB, (uint8_t)(lba >> 8));
				ckPortOutByte(portbase + PATA_PORTIDX_CYL_MSB, (uint8_t)(lba >> 16));
				ckPortOutByte(portbase + PATA_PORTIDX_DRV_HEAD, drvflag | ((lba >> 24) & 0x0f));

				if (csWaitForDriveReady(bPrimary))
				{
					ckPortOutByte(portbase + PATA_PORTIDX_COMMAND, PATA_CMD_WRITE);

					int i = 0, bufidx = 0;

					// wait for DRQ
					while (1)
					{
						uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);

						if (status & PATA_STATUS_ERROR)
						{
							goto end;
						}
						else if (status & PATA_STATUS_DATA_REQUEST)
						{
							break;
						}

						ckTaskSleep(1);
					}

					for (; i < count; i++)
					{
						for (int j = 0; j < 512 / 2; j++)
						{
							ckPortOutWord(portbase + PATA_PORTIDX_DATA, ((uint16_t *)buf)[bufidx++]);
						}

						uint8_t status = ckPortInByte(portbase + PATA_PORTIDX_STATUS);
						if (status & PATA_STATUS_ERROR)
						{
							break;
						}
						else if (!(status & PATA_STATUS_DATA_REQUEST))
						{
							if (!csWaitForInterrupt(bPrimary))
								break;
						}
					}

				end:
					ret = i;
				}
			}

			ckSimpleMutexUnlock(&g_PATAStruct.mutex);
		}
	}

	return ret;
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
	int idx = bPrimary ? 0 : 1;

	uint32_t start = ckTimerGetTickCount();
	while (g_TimerStruct.TickCountLow - start <= WAITTIME_FOR_PATA)
	{
		if (g_PATAStruct.bInterruptOccurred[idx])
		{
			g_PATAStruct.bInterruptOccurred[idx] = false;
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
