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
 * @file rtc.h
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef RTC_H_
#define RTC_H_

#include <stddef.h>
#include <stdint.h>

enum
{
	RTC_CMOSADDRESS			= 0x70,
	RTC_CMOSDATA			= 0x71,

	RTC_ADDRESS_SECOND		= 0x00,
	RTC_ADDRESS_MINUTE		= 0x02,
	RTC_ADDRESS_HOUR		= 0x04,
	RTC_ADDRESS_DAYOFWEEK	= 0x06,
	RTC_ADDRESS_DAYOFMONTH	= 0x07,
	RTC_ADDRESS_MONTH		= 0x08,
	RTC_ADDRESS_YEAR		= 0x09,
	RTC_ADDRESS_STATUS_A	= 0x0a,
	RTC_ADDRESS_STATUS_B	= 0x0b,
	RTC_ADDRESS_STATUS_C	= 0x0c,
	RTC_ADDRESS_STATUS_D	= 0x0d,
};

/**
 * @brief @ref ckRTCReadTime 함수의 결과값을 나타냅니다.
 */
typedef struct tagRTCTime RTCTime;
struct tagRTCTime
{
	uint8_t hour, minute, second;
};

/**
 * @brief @ref ckRTCReadDate 함수의 결과값을 나타냅니다.
 */
typedef struct tagRTCDate RTCDate;
struct tagRTCDate
{
	uint16_t year;
	uint8_t month, dayofmonth, dayofweek;
};

/**
 * @brief 8바이트 BCD 포맷을 이진 포맷으로 변환합니다.
 * @param[in] b 8바이트 BCD 포맷입니다.
 * @return 변환된 이진 포맷입니다.
 */
static inline uint8_t ckBCD8toBinary(uint8_t b)
{
	return ((b >> 4) * 10) + (b & 0x0f);
}

/**
 * @brief RTC를 초기화합니다.
 */
void ckRTCInitialize(void);

/**
 * @brief RTC를 통해 현재 시간을 읽습니다.
 * @param[out] pTime 읽어온 현재 시간입니다.
 */
void ckRTCReadTime(RTCTime *pTime);
/**
 * @brief RTC를 통해 현재 날짜를 읽어옵니다.
 * @param[out] pDate 읽어온 현재 날짜입니다.
 */
void ckRTCReadDate(RTCDate *pDate);

/**
 * @brief @ref RTCDate::dayofweek 을 문자열로 변환합니다.
 * @param[in] dayofweek dayofweek
 * @return 변환된 문자열입니다.
 */
const char *ckRTCDayOfWeekStr(uint8_t dayofweek);

#endif /* RTC_H_ */
