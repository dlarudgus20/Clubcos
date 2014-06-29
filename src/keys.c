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
 * @filekeys.c
 * @date 2014. 5. 4.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "keyboard.h"
#include "keys.h"
#include "terminal.h"
#include "string.h"

// ckGetKeyCodeFromScanCode 함수를 이융해 키보드로부터 전송된 스캔 코드를 키 코드로 조립합니다.
// ckOnKeyboardInterrupt로부터 호출됩니다.
// 키 코드가 조립되었으면 true를 리턴하고, 조립 중이면 false를 리턴합니다.
static bool csAssembleScanCodeToKeyCode(uint8_t ScanCode, uint16_t *pKeyCode);

// 조립된 스캔 코드, 확장 키 여부, Pause키 여부로부터 키 코드를 만들어 냅니다.
static uint16_t csGetKeyCodeFromScanCode(uint8_t ScanCode, bool bExtern, bool bPause);

void ckOnKeyboardInterrupt(uint32_t QueueData)
{
	uint16_t KeyCode;
	uint8_t cascii;

	if (csAssembleScanCodeToKeyCode(QueueData, &KeyCode))
	{
		cascii = ckGetCasciiFromKeyCode(KeyCode);
		g_KeyTable[KeyCode & 0xff].bPushedKey = ckKeyIsDown(KeyCode);

		if (!ckKeyIsAscii(cascii))
		{
			switch (cascii)
			{
				case CASCII_LSHIFT:
				case CASCII_RSHIFT:
					g_Keyboard.bShift =
						(g_KeyTable[SCAN_CODE_LSHIFT].bPushedKey || g_KeyTable[SCAN_CODE_RSHIFT].bPushedKey);
					break;
				case CASCII_NUMLOCK:
					if (!ckKeyIsDown(KeyCode))
					{
						g_Keyboard.bNumLock = !g_Keyboard.bNumLock;
						ckKeyboardUpdateLED();
					}
					break;
				case CASCII_CAPSLOCK:
					if (!ckKeyIsDown(KeyCode))
					{
						g_Keyboard.bCapsLock = !g_Keyboard.bCapsLock;
						ckKeyboardUpdateLED();
					}
					break;
				case CASCII_SCROLLLOCK:
					if(!ckKeyIsDown(KeyCode))
					{
						g_Keyboard.bScrollLock = !g_Keyboard.bScrollLock;
						ckKeyboardUpdateLED();
					}
					break;
			}
		}

		// TODO: Keyboard Event
		if (cascii != 0 && ckKeyIsDown(KeyCode))
			ckTerminalOnInput(cascii);
	}
}

static bool csAssembleScanCodeToKeyCode(uint8_t ScanCode, uint16_t *pKeyCode)
{
	static bool bExternCode = false, bPause = false;
	static unsigned SkipForPause = 0;

	if (bPause)
	{
		SkipForPause--;
		if(SkipForPause != 0)
		{
			return false;
		}
		else
		{
			bPause = false;
			*pKeyCode = KEYCODE_PAUSE;
			return true;
		}
	}
	else if (bExternCode)
	{
		bExternCode = false;
		*pKeyCode = csGetKeyCodeFromScanCode(ScanCode, true, false);
		return true;
	}
	else
	{
		if(ScanCode == 0xE1)
		{
			bPause = true;
			SkipForPause = 2;
			return false;
		}
		else if(ScanCode == 0xE0)
		{
			bExternCode = true;
			return false;
		}
		else
		{
			*pKeyCode = csGetKeyCodeFromScanCode(ScanCode, false, false);
			return true;
		}
	}
}

static uint16_t csGetKeyCodeFromScanCode(uint8_t ScanCode, bool bExtern, bool bPause)
{
	bool bPushKey = true;
	uint16_t ret;

	if ((ScanCode & 0x80) != 0)
	{
		bPushKey = false;
		ScanCode &= 0x7f;
	}

	if (bPause)
	{
		ret = KEYCODE_PAUSE;
	}
	else if (bExtern)
	{
		ret = ScanCode - 0x1C + 0x59;
	}
	else
	{
		ret = ScanCode;
	}

	return bPushKey ? ret : (ret | KEYCODE_FLAG_UNPUSH);
}

uint8_t ckGetCasciiFromKeyCode(uint16_t KeyCode)
{
	KeyStruct key;

	KeyCode &= 0xff;

	key = g_KeyTable[KeyCode];
	if (KeyCode >= KEYCODE_NUM_MIN && KeyCode <= KEYCODE_NUM_MAX)
	{
		if(g_Keyboard.bNumLock)
		{
			return key.CombinedKey;
		}
		else
		{
			return key.NormalKey;
		}
	}
	else if (isalpha(key.NormalKey))
	{
		if (g_Keyboard.bCapsLock)
		{
			if (g_Keyboard.bShift)
			{
				return key.NormalKey;
			}
			else
			{
				return key.CombinedKey;
			}
		}
		else if (g_Keyboard.bShift)
		{
			return key.CombinedKey;
		}
		else
		{
			return key.NormalKey;
		}
	}
	else
	{
		if (g_Keyboard.bShift)
		{
			return key.CombinedKey;
		}
		else
		{
			return key.NormalKey;
		}
	}
}

