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
 * @file terminal.c
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include <stdarg.h>
#include "terminal.h"
#include "coshell.h"
#include "keys.h"
#include "port.h"
#include "string.h"
#include "array.h"
#include "circular_queue.h"
#include "event.h"
#include "fast_mutex.h"
#include "memory_map.h"
#include "task.h"
#include "stackdump.h"

static FastMutex s_TermMutex;

// TODO: 매크로 치워버리고 코드를 직접 수정
#define s_TermBuffer (((const ProcessData * restrict)g_pTaskStruct->pProcData)->TermBuffer)

static uint16_t s_CursorRow = 0, s_CursorColumn = 0;
static uint16_t s_EndRow = 0, s_EndColumn = 0;
static uint8_t s_TermColor, s_AntiColor;
static TermStatus s_TermStat = TERMINAL_STATUS_WAITING;

static char s_LineBuffer[250];
static char *s_LineBufPtr;
static Array8 s_LineBufArray;

static char s_InputBuffer[1024];
static Array8 s_InputArray;

static char s_HitKeyBuffer[1024];
static CircularQueue8 s_HitKeyQueue;

static Event s_HitEvent, s_LineEvent;

static const uint16_t s_BaseVideoPort = 0x3d4;

// TERMINAL_CURSOR_NORMAL
static uint8_t s_DefaultBeginHeight, s_DefaultEndHeight;

static inline uint8_t csAntiColor(uint8_t color)
{
	return ~color ^ 0x8080;
}
static inline uint16_t csMakeVGAEntry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

// subroutine
static void csNewLine(void);
static bool csAddChar(char c);
static void csCancelInputing(void);

static uint16_t csFindRightNotNullCol(uint16_t row);
static void csScrollScreen(void);

static void csGetCursorWidthDefault(void);

// static unsafe
static bool ckTerminalPutChar_unsafe(char c);
static void ckTerminalBackspace_unsafe(void);
static void ckTerminalDelete_unsafe(void);
static void ckTerminalCls_unsafe(void);
static void ckTerminalClearStatusBar_unsafe(void);
static void ckTerminalClearLineBuffer_unsafe(void);
static void ckTerminalClearInputBuffer_unsafe(void);
static void ckTerminalClearHitKeyBuffer_unsafe(void);
static void ckTerminalUpdateCursor_unsafe(void);

static void csLockForTerminal(void);
static void csUnlockForTerminal(void);

void ckTerminalInitialize(void)
{
	csGetCursorWidthDefault();

	uint8_t color = TERMINAL_COLOR_DEFAULT;
	s_TermColor = color;
	s_AntiColor = ~color ^ 0x8080;

	ckTerminalClearInputBuffer_unsafe();
	ckTerminalClearHitKeyBuffer_unsafe();
	ckArray8Init(&s_InputArray, (uint8_t *)s_InputBuffer, (uint8_t *)s_InputBuffer + sizeof(s_InputBuffer));

	ckTerminalCls_unsafe();

	ckTerminalClearStatusBar_unsafe();
}
void ckTerminalInitSyncObjs(void)
{
	ckFastMutexInit(&s_TermMutex);

	ckEventInit(&s_HitEvent, false, true);
	ckEventInit(&s_LineEvent, false, true);
}

static void csLockForTerminal(void)
{
	bool lock_for_terminal_result = ckFastMutexLock(&s_TermMutex);
	assert(lock_for_terminal_result);
}

static void csUnlockForTerminal(void)
{
	bool unlock_for_terminal_result = ckFastMutexUnlock(&s_TermMutex);
	assert(unlock_for_terminal_result);
}

void ckTerminalSetColor(uint8_t color)
{
	csLockForTerminal();
	ckTerminalSetColor_unsafe(color);
	csUnlockForTerminal();
}
void ckTerminalSetColor_unsafe(uint8_t color)
{
	s_TermColor = color;
	s_AntiColor = ~color ^ 0x8080;
}

void ckTerminalSetStatus(TermStatus stat)
{
	csLockForTerminal();
	s_TermStat = stat;
	csUnlockForTerminal();
}

void ckTerminalPutEntryAt(char c, uint8_t color, uint16_t x, uint16_t y)
{
	assert(x < 80 && y < 25);
	s_TermBuffer[y * TERMINAL_WIDTH + x] = c | (color << 8);
}

void ckTerminalPutCharEntryAt(char c, uint16_t x, uint16_t y)
{
	assert(x < 80 && y < 25);
	ckTerminalPutEntryAt(c, s_TermColor, x, y);
}

bool ckTerminalPutChar(char c)
{
	bool bRet;
	csLockForTerminal();
	bRet = ckTerminalPutChar_unsafe(c);
	csUnlockForTerminal();
	return bRet;
}
static bool ckTerminalPutChar_unsafe(char c)
{
	bool bRet = true;
	if (c == '\n')
	{
		csNewLine();
	}
	else if (c == '\b')
	{
		ckTerminalBackspace_unsafe();
	}
	else if (c != '\0')
	{
		bRet = csAddChar(c);
	}
	return bRet;
}

void ckTerminalDelete(void)
{
	csLockForTerminal();
	ckTerminalDelete_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalDelete_unsafe(void)
{
	uint16_t *CursorPtr = s_TermBuffer + s_CursorRow * TERMINAL_WIDTH + s_CursorColumn;
	uint16_t *EndPtr = s_TermBuffer + s_EndRow * TERMINAL_WIDTH + s_EndColumn;

	if (CursorPtr != EndPtr)
	{
		memmove(CursorPtr, CursorPtr + 1, (EndPtr - CursorPtr) * sizeof(uint16_t));
		*(uint8_t *)EndPtr = '\0';

		if (s_EndColumn != 0)
		{
			s_EndColumn--;
		}
		else
		{
			s_EndRow--;
			s_EndColumn = csFindRightNotNullCol(s_EndRow);
		}
	}

	ckTerminalUpdateCursor_unsafe();
}

void ckTerminalBackspace(void)
{
	csLockForTerminal();
	ckTerminalBackspace_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalBackspace_unsafe(void)
{
	if (s_CursorColumn != 0)
	{
		s_CursorColumn--;
		ckTerminalDelete_unsafe();
	}
	else
	{
		if (s_CursorRow != 0)
		{
			uint16_t *OldCursorPtr = s_TermBuffer + s_CursorRow * TERMINAL_WIDTH + s_CursorColumn;
			uint16_t *EndPtr = s_TermBuffer + s_EndRow * TERMINAL_WIDTH + s_EndColumn;

			s_CursorRow--;
			s_CursorColumn = csFindRightNotNullCol(s_CursorRow);

			uint16_t *NewCursorPtr = s_TermBuffer + s_CursorRow * TERMINAL_WIDTH + s_CursorColumn;
			uint16_t diff = EndPtr - OldCursorPtr;

			memmove(NewCursorPtr, OldCursorPtr, diff * sizeof(uint16_t));
			_memset_2(NewCursorPtr + diff, csMakeVGAEntry('\0', s_TermColor), EndPtr - NewCursorPtr - diff);

			s_EndRow = s_CursorRow;
			s_EndColumn = s_CursorColumn + diff;

			while (s_CursorColumn < TERMINAL_WIDTH)
			{
				s_EndRow++;
				s_EndColumn -= TERMINAL_WIDTH;
			}
		}
		ckTerminalUpdateCursor_unsafe();
	}
}

static void csNewLine(void)
{
	s_EndColumn = 0;
	if (++s_EndRow == TERMINAL_HEIGHT)
	{
		csScrollScreen();
		s_EndRow--;
	}

	s_CursorColumn = s_EndColumn;
	s_CursorRow = s_EndRow;
	ckTerminalUpdateCursor_unsafe();
}

static bool csAddChar(char c)
{
	bool bRet = true;

	if (s_EndRow == TERMINAL_HEIGHT - 1 && s_EndColumn == TERMINAL_WIDTH - 1)
	{
		if (s_CursorRow == 0)
		{
			bRet = false;
			goto ret;	// cannot scroll
		}

		csScrollScreen();
		s_EndRow--;
		s_CursorRow--;
	}

	uint16_t *CursorPtr = s_TermBuffer + s_CursorRow * TERMINAL_WIDTH + s_CursorColumn;
	uint16_t *EndPtr = s_TermBuffer + s_EndRow * TERMINAL_WIDTH + s_EndColumn;

	memmove(CursorPtr + 1, CursorPtr, (EndPtr - CursorPtr) * sizeof(uint16_t));
	*CursorPtr = csMakeVGAEntry(c, s_TermColor);

	if (++s_CursorColumn == TERMINAL_WIDTH)
	{
		s_CursorColumn = 0;
		s_CursorRow++;
	}
	if (++s_EndColumn == TERMINAL_WIDTH)
	{
		s_EndColumn = 0;
		s_EndRow++;
	}

ret:
	ckTerminalUpdateCursor_unsafe();
	return bRet;
}

void ckTerminalOnInput(uint8_t cascii)
{
	csLockForTerminal();

	ckCircularQueue8Put(&s_HitKeyQueue, cascii);
	ckEventSet(&s_HitEvent);

	if (s_TermStat != TERMINAL_STATUS_STRING_INPUTING)
		goto Ret;

	TermStatus OldStat = s_TermStat;
	s_TermStat = TERMINAL_STATUS_WAITING;

	if (ckKeyIsAscii(cascii))
	{
		if (cascii == '\n')
		{
			csNewLine();

			//*LineBufArray.now = '\0'; // in doubt?
			ckArray8Append(&s_InputArray, &s_LineBufArray);

			if (s_LineBufArray.end == s_LineBufArray.now)	// 버퍼가 넘칠 경우 마지막 문자 하나를 제거.
				s_LineBufArray.now--;
			*s_InputArray.now++ = '\n';

			ckEventSet(&s_LineEvent);

			ckTerminalClearLineBuffer_unsafe();
		}
		else if (cascii == CASCII_ESC)
		{
			csCancelInputing();
		}
		else if (cascii == '\b')
		{
			if (s_LineBufPtr != (char *)s_LineBufArray.buf)
			{
				s_LineBufPtr--;
				ckArray8Erase(&s_LineBufArray, (uint8_t *)s_LineBufPtr);
				ckTerminalBackspace_unsafe();
			}
		}
		else if (!iscntrl(cascii))
		{
			if (ckArray8Insert(&s_LineBufArray, (uint8_t *)s_LineBufPtr, (uint8_t)cascii))
			{
				s_LineBufPtr++;
				if (!ckTerminalPutChar_unsafe((char)cascii))
				{
					ckArray8Erase(&s_LineBufArray, (uint8_t *)(--s_LineBufPtr));
				}
			}
		}
	}
	else if (cascii == CASCII_DELETE || cascii == CASCII_NUM_DEL)
	{
		if (s_LineBufPtr != (char *)s_LineBufArray.now)
		{
			ckArray8Erase(&s_LineBufArray, (uint8_t *)s_LineBufPtr);
			ckTerminalDelete_unsafe();
		}
	}
	else if (cascii == CASCII_LEFT || cascii == CASCII_NUM_LEFT)
	{
		if (s_LineBufPtr != (char *)s_LineBufArray.buf)
		{
			s_LineBufPtr--;
			if (s_CursorColumn == 0)
			{
				s_CursorRow--;
				s_CursorColumn = TERMINAL_WIDTH - 1;
			}
			else
			{
				s_CursorColumn--;
			}
			ckTerminalUpdateCursor_unsafe();
		}
	}
	else if (cascii == CASCII_RIGHT || cascii == CASCII_NUM_RIGHT)
	{
		if (s_LineBufPtr != (char *)s_LineBufArray.now)
		{
			s_LineBufPtr++;
			if (++s_CursorColumn == TERMINAL_WIDTH)
			{
				s_CursorRow++;
				s_CursorColumn = 0;
			}
			ckTerminalUpdateCursor_unsafe();
		}
	}

	s_TermStat = OldStat;

Ret:
	csUnlockForTerminal();
}

static void csCancelInputing(void)
{
	uint16_t gap = (uint16_t)(s_LineBufArray.now - s_LineBufArray.buf);

	int col = (int)s_CursorColumn - gap;
	uint16_t row = s_CursorRow;
	while (col < 0)
	{
		row--;
		col += TERMINAL_WIDTH;
	}
	s_CursorColumn = col;
	s_CursorRow = row;

	uint16_t *CursorPtr = s_TermBuffer + s_CursorRow * TERMINAL_WIDTH + s_CursorColumn;
	uint16_t *EndPtr = s_TermBuffer + s_EndRow * TERMINAL_WIDTH + s_EndColumn;
	_memset_2(CursorPtr, csMakeVGAEntry('\0', s_TermColor), EndPtr - CursorPtr);

	ckTerminalClearLineBuffer_unsafe();
	ckTerminalUpdateCursor_unsafe();
}

void ckTerminalClearLineBuffer(void)
{
	csLockForTerminal();
	ckTerminalClearLineBuffer_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalClearLineBuffer_unsafe(void)
{
	s_EndColumn = s_CursorColumn;
	s_EndRow = s_CursorRow;

	ckArray8Init(&s_LineBufArray, (uint8_t *)s_LineBuffer,
		(uint8_t *)s_LineBuffer + sizeof(s_LineBuffer) - 1);
	s_LineBufPtr = s_LineBuffer;
}

void ckTerminalWriteStringAt(uint16_t x, uint16_t y, uint8_t color, const char *str)
{
	assert(x < 80 && y < 25);

	while (*str != '\0')
	{
		ckTerminalPutEntryAt(*str++, color, x, y);

		if (++x >= TERMINAL_WIDTH)
		{
			x = 0;
			if (++y >= TERMINAL_HEIGHT)
				return;
		}
	}
}

void ckTerminalWriteStringAtF(uint16_t x, uint16_t y, uint8_t color, const char *format, ...)
{
	assert(x < 80 && y < 25);

	va_list va;
	char buf[1024];
	va_start(va, format);
	vsnprintf(buf, sizeof(buf), format, va);
	ckTerminalWriteStringAt(x, y, color, buf);
	va_end(va);
}

void ckTerminalPrintStatusBar(const char *str)
{
	csLockForTerminal();
	ckTerminalPrintStatusBar_unsafe(str);
	csUnlockForTerminal();
}
void ckTerminalPrintStatusBar_unsafe(const char *str)
{
	char ch;

	for (unsigned i = 0; i < TERMINAL_WIDTH; i++)
	{
		ch = *str;
		if (ch != '\0')
			str++;

		ckTerminalPutEntryAt(ch, s_AntiColor, i, TERMINAL_HEIGHT);
	}
}

void ckTerminalPrintStatusBarF(const char *format, ...)
{
	va_list va;
	char buf[1024];
	va_start(va, format);
	vsnprintf(buf, sizeof(buf), format, va);
	ckTerminalPrintStatusBar(buf);
	va_end(va);
}

void ckTerminalClearStatusBar(void)
{
	csLockForTerminal();
	ckTerminalClearStatusBar_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalClearStatusBar_unsafe(void)
{
	_memset_2(s_TermBuffer + TERMINAL_HEIGHT * TERMINAL_WIDTH,
			csMakeVGAEntry('\0', csAntiColor(s_TermColor)), TERMINAL_WIDTH);
}

//static void panic_stackdump(void *fn, uint32_t num)
//{
//	ckTerminalPrintStringF_unsafe("\n[%02d] : %p", num, fn);
//}
void ckTerminalPanic(const char *str)
{
	ckAsmCli(); // panic

	s_TermColor = TERMINAL_COLOR_PANIC;
	ckTerminalPrintString_unsafe("\n[!!PANIC!!] ");
	ckTerminalPrintString_unsafe(str);

	// buggy for #PF :(
	// ckStackDump(panic_stackdump, 0, 0xfffffff);

	while (1) ckAsmHlt();
}

bool ckTerminalAskContinue(void)
{
	ckTerminalPrintString("---Type <return> to continue, or q <return> to quit---");

	while (1)
	{
		switch (ckTerminalGetChar())
		{
			case '\n':
				return true;
			case 'q':
				return false;
		}
	}
}

void ckTerminalCls(void)
{
	csLockForTerminal();
	ckTerminalCls_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalCls_unsafe(void)
{
	_memset_2(s_TermBuffer, csMakeVGAEntry('\0', s_TermColor), TERMINAL_HEIGHT * TERMINAL_WIDTH);
	ckTerminalClearLineBuffer_unsafe();
	ckTerminalUpdateCursor_unsafe();

	s_CursorRow = s_CursorColumn = s_EndRow = s_EndColumn = 0;
}

void ckTerminalSetCursorWidth(uint8_t BeginHeight, uint8_t EndHeight)
{
	csLockForTerminal();

	ckPortOutByte(s_BaseVideoPort, 0x0a);
	ckPortOutByte(s_BaseVideoPort + 1, BeginHeight & 0x1f);
	ckPortOutByte(s_BaseVideoPort, 0x0b);
	ckPortOutByte(s_BaseVideoPort + 1, EndHeight & 0x1f);

	csUnlockForTerminal();
}

void ckTerminalSetCursorType(TerminalCursorType cur)
{
	switch (cur)
	{
		case TERMINAL_CURSOR_NORMAL:
			ckTerminalSetCursorWidth(s_DefaultBeginHeight, s_DefaultEndHeight);
			break;
		case TERMINAL_CURSOR_SOLID:
		{
			csLockForTerminal();

			// http://forum.osdev.org/viewtopic.php?f=1&t=26573&p=222179&hilit=text+mode+cursor+solid#p222179
			ckPortOutByte(s_BaseVideoPort, 0x0a);
			ckPortOutByte(s_BaseVideoPort + 1, 0);

			csUnlockForTerminal();
			break;
		}
		case TERMINAL_CURSOR_NOCURSOR:
			ckTerminalSetCursorWidth(1, 0);
			break;
	}
}

static void csGetCursorWidthDefault(void)
{
	ckPortOutByte(s_BaseVideoPort, 0x0a);
	s_DefaultBeginHeight = ckPortInByte(s_BaseVideoPort + 1) & 0x1f;
	ckPortOutByte(s_BaseVideoPort, 0x0b);
	s_DefaultEndHeight = ckPortInByte(s_BaseVideoPort + 1) & 0x1f;
}

uint16_t ckTerminalWhereX(void)
{
	return s_CursorColumn;
}

uint16_t ckTerminalWhereY(void)
{
	return s_CursorRow;
}

void ckTerminalGotoXY(uint16_t x, uint16_t y)
{
	assert(x < 80 && y < 25);

	csLockForTerminal();

	ckTerminalClearLineBuffer_unsafe();
	ckTerminalClearInputBuffer_unsafe();

	s_EndColumn = s_CursorColumn = x;
	s_EndRow = s_CursorRow = y;

	ckTerminalUpdateCursor_unsafe();

	csUnlockForTerminal();
}

void ckTerminalPrintString_unsafe(const char *str)
{
	while (*str != '\0')
		ckTerminalPutChar_unsafe(*str++);
}

void ckTerminalPrintString(const char *str)
{
	csLockForTerminal();

	while (*str != '\0')
		ckTerminalPutChar_unsafe(*str++);

	csUnlockForTerminal();
}

void ckTerminalPrintStringF(const char *format, ...)
{
	va_list va;
	char buf[1024];
	va_start(va, format);
	vsnprintf(buf, sizeof(buf), format, va);
	ckTerminalPrintString(buf);
	va_end(va);
}
void ckTerminalPrintStringF_unsafe(const char *format, ...)
{
	va_list va;
	char buf[1024];
	va_start(va, format);
	vsnprintf(buf, sizeof(buf), format, va);
	ckTerminalPrintString_unsafe(buf);
	va_end(va);
}

size_t ckTerminalGetLine(char *buf, size_t size)
{
	ckTerminalSetStatus(TERMINAL_STATUS_STRING_INPUTING);

	ckEventWait(&s_LineEvent);

	csLockForTerminal();

	char *end = strchr(s_InputBuffer, '\n');
	assert(end != NULL);

	size_t len = min((size_t)(end - s_InputBuffer), size - 1);
	memcpy(buf, s_InputBuffer, len);
	buf[len] = '\0';

	memmove(s_InputArray.buf, s_InputArray.buf + len,
		(s_InputArray.now - s_InputArray.buf) - len);
	s_InputArray.now -= len + 1;

	ckTerminalClearHitKeyBuffer_unsafe();

	if (s_InputArray.buf != s_InputArray.now)
		ckEventSet(&s_LineEvent);

	csUnlockForTerminal();

	return len;
}

char ckTerminalGetChar(void)
{
	ckTerminalSetStatus(TERMINAL_STATUS_STRING_INPUTING);

	ckEventWait(&s_LineEvent);

	csLockForTerminal();

	char ret = (char)ckArray8PopFront(&s_InputArray);

	ckTerminalClearHitKeyBuffer_unsafe();

	if (s_InputArray.buf != s_InputArray.now)
		ckEventSet(&s_LineEvent);

	csUnlockForTerminal();

	return ret;
}

uint8_t ckTerminalGetch(void)
{
	ckEventWait(&s_HitEvent);

	csLockForTerminal();

	uint8_t ret = ckCircularQueue8Get(&s_HitKeyQueue, false, NULL);

	if (!s_HitKeyQueue.bEmpty)
		ckEventSet(&s_HitEvent);

	csUnlockForTerminal();

	return ret;
}

void ckTerminalClearInputBuffer(void)
{
	csLockForTerminal();
	ckTerminalClearInputBuffer_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalClearInputBuffer_unsafe(void)
{
	ckArray8Init(&s_InputArray, (uint8_t *)s_InputBuffer, (uint8_t *)s_InputBuffer + sizeof(s_InputBuffer));
	ckEventClear(&s_LineEvent);
}

void ckTerminalClearHitKeyBuffer(void)
{
	csLockForTerminal();
	ckTerminalClearHitKeyBuffer_unsafe();
	csUnlockForTerminal();
}
static void ckTerminalClearHitKeyBuffer_unsafe(void)
{
	ckCircularQueue8Init(&s_HitKeyQueue, (uint8_t *)s_HitKeyBuffer,
		sizeof(s_HitKeyBuffer) / sizeof(s_HitKeyBuffer[0]));

	ckEventClear(&s_HitEvent);
}

bool ckTerminalIsHitKeyBufferFull(void)
{
	csLockForTerminal();
	bool ret = !s_HitKeyQueue.bEmpty;
	csUnlockForTerminal();
	return ret;
}

void ckTerminalClearAllBuffers(void)
{
	csLockForTerminal();
	ckTerminalClearHitKeyBuffer_unsafe();
	ckTerminalClearInputBuffer_unsafe();
	ckTerminalClearLineBuffer_unsafe();
	csUnlockForTerminal();
}

static void ckTerminalUpdateCursor_unsafe(void)
{
	uint16_t pos = s_CursorRow * TERMINAL_WIDTH + s_CursorColumn;
	ckPortOutByte(s_BaseVideoPort, 0x0f);
	ckPortOutByte(s_BaseVideoPort + 1, (uint8_t)pos);
	ckPortOutByte(s_BaseVideoPort, 0x0e);
	ckPortOutByte(s_BaseVideoPort + 1, (uint8_t)(pos >> 8));
}

static uint16_t csFindRightNotNullCol(uint16_t row)
{
	const uint16_t *rbegin = s_TermBuffer + (row + 1) * TERMINAL_WIDTH - 1;
	const uint16_t *rend = s_TermBuffer + row * TERMINAL_WIDTH - 1;

	do
	{
		if (*((const char *)rbegin) != '\0')
			break;
	} while (--rbegin != rend);

	// found.
	return (uint16_t)(rbegin - rend);
}

static void csScrollScreen(void)
{
	// scroll screen
	memcpy(s_TermBuffer,
			s_TermBuffer + TERMINAL_WIDTH,
			((TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH) * sizeof(uint16_t));

	// clear last row
	_memset_2(s_TermBuffer + (TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH,
			csMakeVGAEntry('\0', s_TermColor),
			TERMINAL_WIDTH);
}

