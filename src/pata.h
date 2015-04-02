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
 * @file pata.h
 * @date 2014. 7. 29.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef PATA_H_
#define PATA_H_

// P.M = Primary Master
// P.S = Primary Slave
// S.M = Secondary Master
// S.S = Secondary Slave

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "benaphore.h"

enum
{
	PATA_PORTBASE_PRIMARY	= 0x1f0,	//!< PATA Port Base primary
	PATA_PORTBASE_SECONDARY	= 0x170,	//!< PATA Port Base secondary

	PATA_PORTIDX_DATA		= 0,		//!< PATA data register (r/w, 2byte)
	PATA_PORTIDX_ERROR		= 1,		//!< PATA error register (r/w)
	PATA_PORTIDX_SEC_COUNT	= 2,		//!< PATA sector count register (r/w)
	PATA_PORTIDX_SEC_NUM	= 3,		//!< PATA sector number register (r/w)
	PATA_PORTIDX_CYL_LSB	= 4,		//!< PATA cylinder number LSB(byte) register (r/w)
	PATA_PORTIDX_CYL_MSB	= 5,		//!< PATA cylinder number MSB(byte) register (r/w)
	PATA_PORTIDX_DRV_HEAD	= 6,		//!< PATA drive/head number register (r/w)
	/* STATE와 COMMAND가 같은 건 오타가 아닙니다 - 읽을땐 STATE이고, 쓸 때는 COMMAND입니다. */
	PATA_PORTIDX_STATUS		= 7,		//!< PATA status register (read-only)
	PATA_PORTIDX_COMMAND	= 7,		//!< PATA command register (write-only)
	/* CONTROL과 ALT_STATUS가 같은 건 오타가 아닙니다 - 읽을 땐 ALT_STATUS이고, 쓸 땐 CONTROL입니다. */
	/* CONTROL 레지스터는 MINT64책에서는 디지털 출력 레지스터라고 나와 있습니다. */
	PATA_PORTIDX_CONTROL	= 0x206,	//!< PATA device control register (w)
	PATA_PORTIDX_ALT_STATUS	= 0x206,	//!< PATA alternative status register (r), which does not affect interrupt.
	PATA_PORTIDX_DRV_ADD	= 0x207,	//!< PATA drive address register (r)
};
enum
{
	PATA_CMD_READ			= 0x20,		//!< PATA Command read
	PATA_CMD_WRITE			= 0x30,		//!< PATA Command write
	PATA_CMD_FLUSH_CACHE	= 0xe7,		//!< PATA Command flush cache
	PATA_CMD_IDENTIFY		= 0xec,		//!< PATA Command identify drive
};
enum
{
	PATA_STATUS_BUSY		= 0x80,		//!< PATA Status busy
	PATA_STATUS_DRV_READY	= 0x40,		//!< PATA Status device ready
	PATA_STATUS_DRV_FAULT	= 0x20,		//!< PATA Status device fault
	PATA_STATUS_DRVSEK_COMPL= 0x10,		//!< PATA Status device seek complete
	PATA_STATUS_DATA_REQUEST= 0x08,		//!< PATA Status data request
	PATA_STATUS_COR_DATA_ERR= 0x04,		//!< PATA Status correctable data error
	PATA_STATUS_INDEX		= 0x02,		//!< PATA Status index
	PATA_STATUS_ERROR		= 0x01,		//!< PATA Status error
};
enum
{
	PATA_CTRL_RESET			= 0x04,		//!< PATA Control software reset
	PATA_CTRL_INT_ENABLE	= 0x02,		//!< PATA Control interrupt enable
	/* 0x01이 없는 건 오타가 아닙니다 - 원래 미사용이에요. */
};
enum
{
	PATA_DRVHEAD_SLAVE		= 0x10,		//!< PATA Drv/Head slave flag
	PATA_DRVHEAD_LBA		= 0x40,		//!< PATA Drv/Head LBA enable flag
	PATA_DRVHEAD_MASK		= 0xa0,		//!< PATA Drv/Head bitmask
};

/** @brief @ref PATA_CMD_IDENTIFY 커맨드의 반환값입니다. */
typedef struct tagPATAEntifyResult
{
	uint16_t wConfiguration;

	uint16_t wCountOfCylinder;
	uint16_t _reserved1;

	uint16_t wCountOfHead;
	uint16_t wUnformattedBytesPerTrack;
	uint16_t wUnformattedBytesPerSector;

	uint16_t wCountOfSectorPerCylinder;
	uint16_t wInterSectorGap;
	uint16_t wBytesInPhaseLock;
	uint16_t wCountOfVendorUniqueStatusWord;

	uint16_t vwSerialNumber[10];
	uint16_t wControllerType;
	uint16_t wBufferSize;
	uint16_t wCountOfECCBytes;
	uint16_t vwFirmwareRevision[4];

	uint16_t vwModelNumber[20];
	uint16_t _reserved2[13];

	uint32_t dwCountOfTotalSector;
	uint16_t _reserved3[196];
} PATAIdentifyResult;

/** @brief PATA 디바이스의 정보입니다. @ref PATAIdentifyResult 중 필요한 것만 뽑은 것들입니다. */
typedef struct tagPATADeviceInfo
{
	bool bExist;
	uint32_t dwCountOfTotalSector;
} PATADeviceInfo;

/** @brief PATA 하드디스크 자료구조입니다. */
typedef struct tagPATAStruct
{
	Benaphore sem;

	PATADeviceInfo DeviceInfo[4];	//!< 순서대로 P.M - P.S - S.M - S.S의 @ref PATADeviceInfo
	int IsSlaveSelected[2];			//!< 0이면 Master, 1이면 Slave, -1이면 선택되지 않음. [0]:Primary, [1]:Secondary
} PATAStruct;

extern PATAStruct g_PATAStruct;

void ckPATAInitialize(void);

/** @brief PATA Primary 인터럽트 ISR입니다. */
void ckPATAPrimaryIntHandler();
/** @brief PATA Secondary 인터럽트 ISR입니다. */
void ckPATASecondaryIntHandler();

#endif /* PATA_H_ */
