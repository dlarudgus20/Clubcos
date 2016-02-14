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

/**
 * @brief 지정한 전경색과 배경색을 조합한 색 코드를 만듭니다.
 * @param fg 전경색입니다.
 * @param bg 배경색입니다.
 */
#define ckMakeTermColor(fg, bg) (((bg) << 4) | (fg))

/**
 * @brief 터미널의 색상 코드입니다.
 */
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

/**
 * @brief 터미널의 상태를 나타내는 열거형입니다.
 * @sa ckTerminalSetStatus
 */
typedef enum tagTermStatus
{
	TERMINAL_STATUS_STRING_INPUTING,	//!< 터미널이 문자열을 입력받고 있는 상태입니다.
	TERMINAL_STATUS_WAITING,			//!< 터미널이 대기 상태입니다. 이 상태에서는 사용자로부터의 입력을 무시합니다.
} TermStatus;

/**
 * @brief 터미널의 커서 모양을 나타내는 열거형입니다.
 * @sa ckTerminalSetCursorType
 */
typedef enum tagTerminalCursorType
{
	TERMINAL_CURSOR_NORMAL,		//!< 일반적인 커서 모양입니다.
	TERMINAL_CURSOR_SOLID,		//!< 꽉 찬 모양의 커서입니다.
	TERMINAL_CURSOR_NOCURSOR,	//!< 커서가 없음을 나타냅니다.
} TerminalCursorType;

/**
 * @brief 터미널을 초기화합니다. 동기화 객체는 초기화하지 않습니다.
 * @remark 동기화 객체를 초기화하지 않기 때문에 @ref ckTerminalInitSyncObjs 를 호출하기 전까진 unsafe 함수와 @ref ckTerminalPanic 함수만 사용 가능합니다.
 */
void ckTerminalInitialize(void);
/** @brief 터미널에서 사용되는 동기화 객체를 초기화합니다. 이 함수를 호출하지 않으면 unsafe 함수와 @ref ckTerminalPanic 만 사용 가능합니다. */
void ckTerminalInitSyncObjs(void);

/** @brief 동기화 없이 터미널의 색을 설정합니다. @param[in] color 터미널의 색 코드입니다. */
void ckTerminalSetColor_unsafe(uint8_t color);
/** @brief 터미널의 색을 설정합니다. @param[in] color 터미널의 색 코드입니다. */
void ckTerminalSetColor(uint8_t color);

/** @brief 터미널의 상태를 설정합니다. @param[in] stat 터미널의 상태입니다. */
void ckTerminalSetStatus(TermStatus stat);

/**
 * @brief 특정 위치에 지정한 색 코드로 문자를 출력합니다.
 * @param[in] c 출력할 문자입니다.
 * @param[in] color 출력할 문자의 색 코드입니다.
 * @param[in] x 출력할 위치의 x 좌표입니다.
 * @param[in] y 출력할 위치의 y 좌표입니다.
 */
void ckTerminalPutEntryAt(char c, uint8_t color, uint16_t x, uint16_t y);
/**
 * @brief 특정 위치에 문자를 출력합니다.
 * @param[in] c 출력할 문자입니다.
 * @param[in] x 출력할 위치의 x 좌표입니다.
 * @param[in] y 출력할 위치의 y 좌표입니다.
 */
void ckTerminalPutCharEntryAt(char c, uint16_t x, uint16_t y);
/**
 * @brief 터미널에 문자를 출력합니다.
 * @param[in] c 출력할 문자입니다.
 */
bool ckTerminalPutChar(char c);

/** @brief 커서가 가리키고 있는 입력 문자를 삭제합니다. 키보드의 [del] 키를 누른 효과가 납니다. */
void ckTerminalDelete(void);
/** @brief 커서 바로 뒤의 입력 문자를 삭제합니다. 키보드의 [BackSpace] 키를 누른 효과가 납니다. */
void ckTerminalBackspace(void);
/** @brief 화면을 지웁니다. */
void ckTerminalCls(void);

/**
 * @brief 사용자의 입력을 터미널로 전송합니다.
 * @param[in] cascii 사용자가 입력한 키의 cascii 코드입니다.
 * @remark 사용자가 키를 입력했을 때, 사용자의 입력을 처리하는 함수입니다.
 *         이 함수는 키보드 처리기에서 호출됩니다.
 */
void ckTerminalOnInput(uint8_t cascii);

/**
 * @brief 사용자가 현재 입력한 내용을 비웁니다.
 * @remark 이 함수를 호출하면 사용자가 입력했던 내용이 모두 무효화됩니다.
 *         화면에 echo된 문자가 지워지지 않는다는 점에 유의하십시오.
 */
void ckTerminalClearLineBuffer(void);

/**
 * @brief 특정 위치에 문자열을 출력합니다.
 * @param[in] x 출력할 위치의 x 좌표입니다.
 * @param[in] y 출력할 위치의 y 좌표입니다.
 * @param[in] color 출력할 문자열의 색 코드입니다.
 * @param[in] str 출력할 문자열입니다.
 */
void ckTerminalWriteStringAt(uint16_t x, uint16_t y, uint8_t color, const char *str);
/**
 * @brief 특정 위치에 서식 문자열을 조립해 출력합니다.
 * @param[in] x 출력할 위치의 x 좌표입니다.
 * @param[in] y 출력할 위치의 y 좌표입니다.
 * @param[in] color 출력할 문자열의 색 코드입니다.
 * @param[in] format 출력할 서식 문자열입니다.
 */
void ckTerminalWriteStringAtF(uint16_t x, uint16_t y, uint8_t color, const char *format, ...);

/**
 * @brief 동기화 없이 상태 바에 문자열을 출력합니다.
 * @param[in] str 상태 바에 출력할 문자열입니다. @ref TERMINAL_WIDTH 보다 길이가 클 경우 짤라서 출력됩니다.
 */
void ckTerminalPrintStatusBar_unsafe(const char *str);
/**
 * @brief 상태 바에 문자열을 출력합니다.
 * @param[in] str 상태 바에 출력할 문자열입니다. @ref TERMINAL_WIDTH 보다 길이가 클 경우 짤라서 출력됩니다.
 */
void ckTerminalPrintStatusBar(const char *str);
/**
 * @brief 상태 바에 서식 문자열을 조립해 출력합니다.
 * @param[in] format 상태 바에 출력할 서식 문자열입니다. @ref TERMINAL_WIDTH 보다 길이가 클 경우 짤라서 출력됩니다.
 */
void ckTerminalPrintStatusBarF(const char *format, ...);
/**
 * @brief 상태 바를 비웁니다.
 */
void ckTerminalClearStatusBar(void);

/**
 * @brief panic 메시지를 출력하고 시스템을 panic 상태로 만듭니다. @ref assert 매크로에서 사용됩니다. 이 함수는 @ref ckTerminalInitSyncObjs 함수를 호출하지 않고도 사용 가능합니다.
 * @param[in] str 출력할 panic 문자열입니다.
 */
void ckTerminalPanic(const char *str);

/**
 * @brief "---Type <return> to continue, or q <return> to quit---" 메시지를 띄우고 입력을 받습니다.
 * @return 계속이면 <c>true</c>, 종료이면 <c>false</c>입니다.
 */
bool ckTerminalAskContinue(void);

/**
 * @brief 커서의 모양을 설정합니다.
 * @param[in] BeginHeight 커서의 시작 높이입니다. 0 ~ 0x1f 사이의 값입니다.
 * @param[in] EndHeight 커서의 끝 높이입니다. 0 ~ 0x1f 사이의 값입니다.
 * @remark 이 함수는 VGA 포트에 직접 커서의 높이를 전달합니다.
 *         일반적인 경우엔 @ref ckTerminalSetCursorType 함수의 사용를 권장합니다.
 */
void ckTerminalSetCursorWidth(uint8_t BeginHeight, uint8_t EndHeight);
/**
 * @brief 커서의 모양을 지정된 타입으로 설정합니다.
 * @param[in] cur 커서의 모양 타입입니다.
 */
void ckTerminalSetCursorType(TerminalCursorType cur);

/** @brief 커서의 현재 위치의 x 좌표를 구합니다. @return 커서의 현재 x 좌표입니다. */
uint16_t ckTerminalWhereX(void);
/** @brief 커서의 현재 위치의 y 좌표를 구합니다. @return 커서의 현재 y 좌표입니다. */
uint16_t ckTerminalWhereY(void);
/**
 * @brief 커서를 이동합니다.
 * @param[in] x 커서가 이동될 위치의 x 좌표입니다.
 * @param[in] y 커서가 이동될 위치의 y 좌표입니다.
 * @remark 이 함수가 커서를 이동할 때
 *         @ref ckTerminalClearLineBuffer 함수를 호출해서 사용자의 입력을 비우고,
 *         @ref ckTerminalClearInputBuffer 함수를 호출해 입력 문자 버퍼의 내용을 비웁니다.
 */
void ckTerminalGotoXY(uint16_t x, uint16_t y);

/**
 * @brief 동기화 없이 문자열을 출력합니다.
 * @param[in] str 출력할 문자열입니다.
 */
void ckTerminalPrintString_unsafe(const char *str);
/**
 * @brief 문자열을 출력합니다.
 * @param[in] str 출력할 문자열입니다.
 * @sa ckTerminalPutChar
 */
void ckTerminalPrintString(const char *str);
/**
 * @brief 동기화 없이 서식 문자열을 조립해 출력합니다.
 * @param[in] format 출력할 서식 문자열입니다.
 */
void ckTerminalPrintStringF_unsafe(const char *format, ...);
/**
 * @brief 서식 문자열을 조립해 출력합니다.
 * @param[in] format 출력할 서식 문자열입니다.
 */
void ckTerminalPrintStringF(const char *format, ...);

/**
 * @brief 입력 문자 버퍼에서 한 행을 꺼냅니다. 표준 C의 <c>gets()</c> 함수와 같은 역할을 합니다.
 * @param[out] buf 입력한 문자열입니다.
 * @param[in] size 버퍼의 크기입니다.
 * @return 입력된 문자열의 길이입니다.
 * @remark 이 함수는 @ref ckTerminalClearHitKeyBuffer 함수를 호출해서 입력 키 버퍼를 비웁니다.
 */
size_t ckTerminalGetLine(char *buf, size_t size);
/**
 * @brief 입력 문자 버퍼에서 한 문자를 꺼냅니다. 표준 C의 <c>getchar()</c> 함수와 같은 역할을 합니다.
 * @return 입력된 문자입니다.
 * @remark 이 함수는 @ref ckTerminalClearHitKeyBuffer 함수를 호출해서 입력 키 버퍼를 비웁니다.
 */
char ckTerminalGetChar(void);
/**
 * @brief 입력 키 버퍼에서 cascii 문자 하나를 꺼냅니다.
 * @return 입력된 키의 cascii 코드입니다.
 */
uint8_t ckTerminalGetch(void);

/** @brief 입력 문자 버퍼를 지웁니다. */
void ckTerminalClearInputBuffer(void);
/** @brief 입력 키 버퍼를 지웁니다. */
void ckTerminalClearHitKeyBuffer(void);
/**
 * @brief 사용자의 입력 내용, 입력 문자 버퍼, 입력 키 버퍼를 모두 비웁니다.
 * @remark 이 함수는 @ref ckTerminalClearHitKeyBuffer @ref ckTerminalClearInputBuffer
 *         @ref ckTerminalClearLineBuffer 를 차례대로 호출합니다.
 */
void ckTerminalClearAllBuffers(void);

/**
 * @brief 입력 키 버퍼에 키가 있는지의 여부를 검사합니다. <c>kbhit()</c> 함수와 같은 역할을 합니다.
 * @return 입력 키 버퍼에 키가 있다면 <c>true</c>고, 그렇지 않으면 <c>false</c>입니다.
 */
bool ckTerminalIsHitKeyBufferFull(void);

#endif /* TERMINAL_H_ */
