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
 * @file rtc.c
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "rtc.h"
#include "port.h"
#include "fast_mutex.h"

static FastMutex s_RtcMutex;
static bool s_IsBCD;
static bool s_Is12Hour;

void ckRTCInitialize(void)
{
	ckFastMutexInit(&s_RtcMutex);

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_STATUS_B);
	uint8_t StatusB = ckPortInByte(RTC_CMOSDATA);
	s_IsBCD = ((StatusB & 0x04) == 0);
	s_Is12Hour = ((StatusB & 0x02) == 0);
}

void ckRTCReadTime(RTCTime *pTime)
{
	bool Is12HourPM;

	ckFastMutexLock(&s_RtcMutex);

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_HOUR);
	pTime->hour = ckPortInByte(RTC_CMOSDATA);
	Is12HourPM = ((pTime->hour & 0x80) != 0);
	pTime->hour &= 0x7f;
	if (s_IsBCD)
		pTime->hour = ckBCD8toBinary(pTime->hour);

	if (Is12HourPM)
		pTime->hour += 12;
	else if (s_Is12Hour && pTime->hour == 12)
		pTime->hour = 0;	// midnight

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_MINUTE);
	pTime->minute = ckPortInByte(RTC_CMOSDATA);
	if (s_IsBCD)
		pTime->minute = ckBCD8toBinary(pTime->minute);

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_SECOND);
	pTime->second = ckPortInByte(RTC_CMOSDATA);
	if (s_IsBCD)
		pTime->second = ckBCD8toBinary(pTime->second);

	ckFastMutexUnlock(&s_RtcMutex);
}

void ckRTCReadDate(RTCDate *pDate)
{
	ckFastMutexLock(&s_RtcMutex);

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_YEAR);
	pDate->year = ckPortInByte(RTC_CMOSDATA);
	if (s_IsBCD)
		pDate->year = ckBCD8toBinary(pDate->year) + 2000;
	else
		pDate->year += 2000;

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_MONTH);
	pDate->month = ckPortInByte(RTC_CMOSDATA);
	if (s_IsBCD)
		pDate->month = ckBCD8toBinary(pDate->month);

	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFMONTH);
	pDate->dayofmonth = ckPortInByte(RTC_CMOSDATA);
	if (s_IsBCD)
		pDate->dayofmonth = ckBCD8toBinary(pDate->dayofmonth);

//	ckPortOutByte(RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFWEEK);
//	pDate->dayofweek = ckPortInByte(RTC_CMOSDATA);
//	if (s_IsBCD)
//		pDate->dayofweek = ckBCD8toBinary(pDate->dayofweek);
	/* CMOS의 day of week은 믿을 수 없음 - 직접 계산 */

	ckFastMutexUnlock(&s_RtcMutex);

	// http://en.wikipedia.org/wiki/Weekday_determination#A_tabular_method_to_calculate_the_day_of_the_week
	static uint8_t MonthsTable[] = {
		0, 3, 3, 6,	1, 4, 6, 2, 5, 0, 3, 5
	};
	static uint8_t CenturyNumTable[] = {
		6, 4, 2, 0
	};

	if ((pDate->year % 4 == 0 && pDate->year % 100 != 0) || pDate->year % 400 == 0)
	{
		MonthsTable[0] = 6;
		MonthsTable[1] = 2;
	}

	uint8_t y = pDate->year % 100;

	uint32_t formula = pDate->dayofmonth + MonthsTable[pDate->month - 1]
		+ y + y / 4 + CenturyNumTable[(y / 100) % 4];

	pDate->dayofweek = formula % 7 + 1;
}

const char *ckRTCDayOfWeekStr(uint8_t dayofweek)
{
	static char *arstr[] = {
		"Error",
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday"
	};
	return arstr[(dayofweek < 8) ? dayofweek : 0];
}

