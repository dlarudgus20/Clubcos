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
 * @file terminal.h
 * @date 2014. 4. 26.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
	TERMINAL_WIDTH = 80,
	TERMINAL_HEIGHT = 24
};

#define ckMakeTermColor(fg, bg) (((bg) << 4) | (fg))

typedef enum tagTerminalColor
{
	TERMINAL_BLACK = 0,
	TERMINAL_BLUE = 1,
	TERMINAL_GREEN = 2,
	TERMINAL_CYAN = 3,
	TERMINAL_RED = 4,
	TERMINAL_MAGENTA = 5,
	TERMINAL_BROWN = 6,
	TERMINAL_LIGHT_GREY = 7,
	TERMINAL_DARK_GREY = 8,
	TERMINAL_LIGHT_BLUE = 9,
	TERMINAL_LIGHT_GREEN = 10,
	TERMINAL_LIGHT_CYAN = 11,
	TERMINAL_LIGHT_RED = 12,
	TERMINAL_LIGHT_MAGENTA = 13,
	TERMINAL_LIGHT_BROWN = 14,
	TERMINAL_WHITE = 15,

	TERMINAL_COLOR_DEFAULT = ckMakeTermColor(TERMINAL_LIGHT_GREY, TERMINAL_BLACK),
	TERMINAL_COLOR_LOG = ckMakeTermColor(TERMINAL_LIGHT_GREEN, TERMINAL_BLACK),
	TERMINAL_COLOR_PANIC = ckMakeTermColor(TERMINAL_RED, TERMINAL_WHITE),

} TerminalColor;

typedef enum tagTermStatus
{
	TERMINAL_STATUS_STRING_INPUTING,
	TERMINAL_STATUS_WAITING,
} TermStatus;

typedef enum tagTerminalCursorType
{
	TERMINAL_CURSOR_NORMAL,
	TERMINAL_CURSOR_SOLID,
	TERMINAL_CURSOR_NOCURSOR,
} TerminalCursorType;

void ckTerminalInitialize(void);
void ckTerminalSetColor(uint8_t color);
void ckTerminalSetStatus(TermStatus stat);
void ckTerminalPutEntryAt(char c, uint8_t color, uint16_t x, uint16_t y);
void ckTerminalPutCharEntryAt(char c, uint16_t x, uint16_t y);
bool ckTerminalPutChar(char c);
void ckTerminalDelete(void);
void ckTerminalBackspace(void);

void ckTerminalOnInput(uint8_t cascii);
void ckTerminalClearLineBuffer(void);

void ckTerminalWriteStringAt(uint16_t x, uint16_t y, uint8_t color, const char *str);
void ckTerminalWriteStringAtF(uint16_t x, uint16_t y, uint8_t color, const char *format, ...);
void ckTerminalPrintStatusBar_unsafe(const char *str);
void ckTerminalPrintStatusBar(const char *str);
void ckTerminalPrintStatusBarF(const char *format, ...);
void ckTerminalClearStatusBar(void);
void ckTerminalPanic(const char *str);
void ckTerminalCls(void);
void ckTerminalSetCursorWidth(uint8_t BeginHeight, uint8_t EndHeight);
void ckTerminalSetCursorType(TerminalCursorType cur);

uint16_t ckTerminalWhereX(void);
uint16_t ckTerminalWhereY(void);
void ckTerminalGotoXY(uint16_t x, uint16_t y);

void ckTerminalPrintString_unsafe(const char *str);
void ckTerminalPrintString(const char *str);
void ckTerminalPrintStringF(const char *format, ...);

size_t ckTerminalGetLine(char *buf, size_t size);
char ckTerminalGetChar(void);
uint8_t ckTerminalGetch(void);
void ckTerminalClearInputBuffer(void);
void ckTerminalClearHitKeyBuffer(void);
bool ckTerminalIsHitKeyBufferFull(void);

void ckTerminalClearAllBuffers(void);

#endif /* TERMINAL_H_ */
