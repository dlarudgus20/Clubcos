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
 * @file keytable.h
 * @date 2014. 5. 4.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef KEYTABLE_H_
#define KEYTABLE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief cascii 코드를 나타내는 열거형입니다.
 * @remark cascii (Clubcos ASCII)란, ascii코드를 확장한 키 코드로, clubcos 내부에서 키를 처리하는 데 사용됩니다.<br/>
 *         키보드로부터 스캔 코드를 읽어들인 이후에, clubcos는 이 스캔 코드를 조립해 키 코드를 만듭니다.<br/>
 *         키 코드는 가변 길이의 스캔 코드를 1바이트만으로 표현하는 코드입니다.
 *         구제적으로, 0x00 ~ 0x58 사이의 스캔 코드는 그 자신이 스캔 코드와 같고, 0x59 이후에는 확장키가 배열되는 코드입니다.
 *         또한, 만일 눌렸던 키가 unpush되는 것을 의미하는 스캔 코드였을 경우,
 *         키 코드의 32번째 비트 (@ref KEYCODE_FLAG_UNPUSH )를 1로 만드는 것으로 표시합니다.
 *         때문에 키 코드는 <c>uint16_t</c>형으로 나타냅니다.<br/>
 *         변환된 스캔 코드는 @ref ckGetCasciiFromKeyCode 에서 @ref g_KeyTable 를 통해 cascii코드로 변환됩니다.<br/>
 *         자세한 정보는 keys.c 파일을 참조하십시오.
 */
enum tagCascii
{
/* ascii */
	CASCII_NONE				= 0x00,
	CASCII_ENTER			= '\n',
	CASCII_TAB				= '\t',
	CASCII_ESC				= 0x1B,
	CASCII_BACKSPACE		= 0x08,

/* cascii */
	CASCII_LCTRL			= 0x81,
	CASCII_LSHIFT			= 0x82,
	CASCII_RSHIFT			= 0x83,
	CASCII_PRINTSCREEN		= 0x84,
	CASCII_LALT				= 0x85,
	CASCII_CAPSLOCK			= 0x86,
	CASCII_F1				= 0x87,
	CASCII_F2				= 0x88,
	CASCII_F3				= 0x89,
	CASCII_F4				= 0x8A,
	CASCII_F5				= 0x8B,
	CASCII_F6				= 0x8C,
	CASCII_F7				= 0x8D,
	CASCII_F8				= 0x8E,
	CASCII_F9				= 0x8F,
	CASCII_F10				= 0x90,
	CASCII_NUMLOCK			= 0x91,
	CASCII_SCROLLLOCK		= 0x92,
	CASCII_NUM_HOME			= 0x93,
	CASCII_NUM_UP			= 0x94,
	CASCII_NUM_PAGEUP		= 0x95,
	CASCII_NUM_LEFT			= 0x96,
	CASCII_NUM_CENTER		= 0x97,
	CASCII_NUM_RIGHT		= 0x98,
	CASCII_NUM_END			= 0x99,
	CASCII_NUM_DOWN			= 0x9A,
	CASCII_NUM_PAGEDOWN		= 0x9B,
	CASCII_NUM_INS			= 0x9C,
	CASCII_NUM_DEL			= 0x9D,
	CASCII_F11				= 0x9E,
	CASCII_F12				= 0x9F,

/* cascii (확장키) */
	CASCII_NUM_ENTER		= 0xA0,
	CASCII_RCTRL			= 0xA1,
	CASCII_NUM_SLASH		= 0xA2,
	CASCII_RALT				= 0xA3,
	CASCII_HOME				= 0xA4,
	CASCII_UP				= 0xA5,
	CASCII_PAGEUP			= 0xA6,
	CASCII_LEFT				= 0xA7,
	CASCII_RIGHT			= 0xA8,
	CASCII_END				= 0xA9,
	CASCII_DOWN				= 0xAA,
	CASCII_PAGEDOWN			= 0xAB,
	CASCII_INSERT			= 0xAC,
	CASCII_DELETE			= 0xAD,

	CASCII_PAUSE			= 0xAE,
};

// 키 코드
enum
{
	KEYCODE_NUM_MIN			= 0x47,
	KEYCODE_NUM_MAX			= 0x53,

	KEYCODE_PAUSE			= 0x91,

	KEYCODE_FLAG_UNPUSH		= 0x100,
};

// 스캔 코드
enum
{
	SCAN_CODE_RSHIFT		= 0x36,
	SCAN_CODE_LSHIFT		= 0x2A,
};

#endif /* KEYTABLE_H_ */
