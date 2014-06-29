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
 * @file keyboard.h
 * @date 2014. 4. 27.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "port.h"

enum
{
	KEYBOARD_PORT_CONTROL = 0x64,
	KEYBOARD_PORT_STATUS = 0x64,
	KEYBOARD_PORT_INPUT = 0x60,
	KEYBOARD_PORT_OUTPUT = 0x60
};

enum
{
	KEYBOARD_CONTROL_ACTIVATE_MOUSE = 0xa7,
	KEYBOARD_CONTROL_DEACTIVATED_MOUSE = 0xa8,
	KEYBOARD_CONTROL_ACTIVATE_KEYBOARD = 0xae,
	KEYBOARD_CONTROL_DEACTIVATE_KEYBOARD = 0xad,
	KEYBOARD_CONTROL_READ_OUTP = 0xd0,
	KEYBOARD_CONTROL_WRITE_OUTP = 0xd1
};
enum
{
	KEYBOARD_STATUS_PARE = 0x80,
	KEYBOARD_STATUS_TIM = 0x40,
	KEYBOARD_STATUS_AUXB = 0x20,
	KEYBOARD_STATUS_KEYL = 0x10,
	KEYBOARD_STATUS_C_D = 0x08,
	KEYBOARD_STATUS_SYSF = 0x04,
	KEYBOARD_STATUS_INPB = 0x02,
	KEYBOARD_STATUS_OUTB = 0x01
};

enum
{
	KEYBOARD_COMMAND_LED = 0xed,
	KEYBOARD_COMMAND_ACTIVATE = 0xf4,

	KEYBOARD_ACK = 0xfa
};

/**
 * @brief 키 하나를 나타내는 구조체입니다.
 * @sa g_KeyTable
 */
typedef struct tagKeyStruct KeyStruct;
struct tagKeyStruct
{
	uint8_t NormalKey;		/**< 키가 다른 키와 조합되지 않았을 때의 cascii 코드입니다. */
	uint8_t CombinedKey;	/**< 키가 Shift, Caps Lock 혹은 NumLock과 조합될 때의 cascii 코드입니다. */
	bool bPushedKey;		/**< 키가 눌려 있는지의 여부입니다. */
};

/**
 * @brief 키보드 자료구조입니다.
 */
typedef struct tagKeyboardStruct KeyboardStruct;
struct tagKeyboardStruct
{
	bool bNumLock;			/**< NumLock 활성화 여부입니다. */
	bool bCapsLock;			/**< CapsLock 활성화 여부입니다. */
	bool bScrollLock;		/**< ScrollLock 활성화 여부입니다. */
	bool bShift;			/**< Shift키 활성화 여부입니다. */
};

/**
 * @brief 키 테이블입니다.
 * @remark 키 테이블에는 키 코드 순서대로 @ref KeyStruct 가 배치되어 있습니다.<br/>
 *         예를 들자면, `i` 키의 키 코드는 0x17입니다.
 *         따라서 g_KeyTable[0x17].NormalKey는 'i'가, g_KeyTable[0x17].CombinedKey는 'I'입니다.<br/>
 *         왼쪽 방향키의 키 코드는 0x88입니다.
 *         따라서 g_KeyTable[0x88].NormalKey와 g_KeyTable[0x88]은 CASCII_LEFT입니다.
 * @sa KeyStruct
 */
extern KeyStruct g_KeyTable[];

/** @brief 키보드 자료구조입니다. */
extern KeyboardStruct g_Keyboard;

#include "keytable.h"

/**
 * @brief 키보드를 초기화합니다.
 * @return 실패했을 경우 <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckKeyboardInitialize(void);
/**
 * @brief 키보드 LED를 갱신합니다.
 * @return 실패했을 경우 <c>false</c>입니다. 그렇지 않다면 <c>true</c>입니다.
 */
bool ckKeyboardUpdateLED(void);

/**
 * @brief 키보드의 ACK를 기다립니다. 기다리는 동안 입력된 스캔 코드는 무시합니다.
 */
bool ckKeyboardWaitForACK(void);
/**
 * @brief 키보드의 ACK를 기다립니다. 기다리는 동안 입력된 스캔 코드를 인터럽트 큐에 삽입합니다.
 */
bool ckKeyboardWaitForACKAndPutQueue(void);

/**
 * @brief 입력 버퍼가 비어 있을 때까지 기다립니다.
 */
void ckKeyboardWaitForInput(void);

/**
 * @brief 키보드 컨트롤러를 통해 PC를 재부팅 시킵니다.
 */
void ckKeyboardResetPC(void);

/**
 * @brief 키보드 컨트롤러의 입력 버퍼가 채워져 있는지의 여부를 조사합니다.
 * @return 입력 버퍼가 채워져 있으면 <c>true</c>입니다. 그렇지 않으면 <c>false</c>입니다.
 */
static inline bool ckKeyboardIsInputBufferFull(void)
{
	return (ckPortInByte(KEYBOARD_PORT_STATUS) & KEYBOARD_STATUS_INPB) != 0;
}
/**
 * @brief 키보드 컨트롤러의 출력 버퍼가 채워져 있는지의 여부를 조사합니다.
 * @return 출력 버퍼가 채워져 있으면 <c>true</c>입니다. 그렇지 않으면 <c>false</c>입니다.
 */
static inline bool ckKeyboardIsOutputBufferFull(void)
{
	return (ckPortInByte(KEYBOARD_PORT_STATUS) & KEYBOARD_STATUS_OUTB) != 0;
}

/**
 * @brief 어셈블리로 짜여진 키보드 인터럽트 ISR입니다.
 */
void ckKeyboardIntHandler();

#endif /* KEYBOARD_H_ */
