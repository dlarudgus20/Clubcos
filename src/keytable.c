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
 * @file keytable.c
 * @date 2014. 5. 4.
 * @author first version in MINT64OS by kkamagui (http://kkamagui.tistory.com/)
 *         modified & bug fixed by dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "keyboard.h"

KeyStruct g_KeyTable[] =
{
	/*	0x00	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x01	*/	{	CASCII_ESC			,	CASCII_ESC			, false },
	/*	0x02	*/	{	'1'					,	'!'					, false },
	/*	0x03	*/	{	'2'					,	'@'					, false },
	/*	0x04	*/	{	'3'					,	'#'					, false },
	/*	0x05	*/	{	'4'					,	'$'					, false },
	/*	0x06	*/	{	'5'					,	'%'					, false },
	/*	0x07	*/	{	'6'					,	'^'					, false },
	/*	0x08	*/	{	'7'					,	'&'					, false },
	/*	0x09	*/	{	'8'					,	'*'					, false },
	/*	0x0A	*/	{	'9'					,	'('					, false },
	/*	0x0B	*/	{	'0'					,	')'					, false },
	/*	0x0C	*/	{	'-'					,	'_'					, false },
	/*	0x0D	*/	{	'='					,	'+'					, false },
	/*	0x0E	*/	{	CASCII_BACKSPACE	,	CASCII_BACKSPACE	, false },
	/*	0x0F	*/	{	CASCII_TAB			,	CASCII_TAB			, false },
	/*	0x10	*/	{	'q'					,	'Q'					, false },
	/*	0x11	*/	{	'w'					,	'W'					, false },
	/*	0x12	*/	{	'e'					,	'E'					, false },
	/*	0x13	*/	{	'r'					,	'R'					, false },
	/*	0x14	*/	{	't'					,	'T'					, false },
	/*	0x15	*/	{	'y'					,	'Y'					, false },
	/*	0x16	*/	{	'u'					,	'U'					, false },
	/*	0x17	*/	{	'i'					,	'I'					, false },
	/*	0x18	*/	{	'o'					,	'O'					, false },
	/*	0x19	*/	{	'p'					,	'P'					, false },
	/*	0x1A	*/	{	'['					,	'{'					, false },
	/*	0x1B	*/	{	']'					,	'}'					, false },
	/*	0x1C	*/	{	'\n'				,	'\n'				, false },
	/*	0x1D	*/	{	CASCII_LCTRL		,	CASCII_LCTRL		, false },
	/*	0x1E	*/	{	'a'					,	'A'					, false },
	/*	0x1F	*/	{	's'					,	'S'					, false },
	/*	0x20	*/	{	'd'					,	'D'					, false },
	/*	0x21	*/	{	'f'					,	'F'					, false },
	/*	0x22	*/	{	'g'					,	'G'					, false },
	/*	0x23	*/	{	'h'					,	'H'					, false },
	/*	0x24	*/	{	'j'					,	'J'					, false },
	/*	0x25	*/	{	'k'					,	'K'					, false },
	/*	0x26	*/	{	'l'					,	'L'					, false },
	/*	0x27	*/	{	';'					,	':'					, false },
	/*	0x28	*/	{	'\''				,	'\"'				, false },
	/*	0x29	*/	{	'`'					,	'~'					, false },
	/*	0x2A	*/	{	CASCII_LSHIFT		,	CASCII_LSHIFT		, false },
	/*	0x2B	*/	{	'\\'				,	'|'					, false },
	/*	0x2C	*/	{	'z'					,	'Z'					, false },
	/*	0x2D	*/	{	'x'					,	'X'					, false },
	/*	0x2E	*/	{	'c'					,	'C'					, false },
	/*	0x2F	*/	{	'v'					,	'V'					, false },
	/*	0x30	*/	{	'b'					,	'B'					, false },
	/*	0x31	*/	{	'n'					,	'N'					, false },
	/*	0x32	*/	{	'm'					,	'M'					, false },
	/*	0x33	*/	{	','					,	'<'					, false },
	/*	0x34	*/	{	'.'					,	'>'					, false },
	/*	0x35	*/	{	'/'					,	'?'					, false },
	/*	0x36	*/	{	CASCII_RSHIFT		,	CASCII_RSHIFT		, false },
	/*	0x37	*/	{	'*'					,	'*'					, false },
	/*	0x38	*/	{	CASCII_LALT			,	CASCII_LALT			, false },
	/*	0x39	*/	{	' '					,	' '					, false },
	/*	0x3A	*/	{	CASCII_CAPSLOCK		,	CASCII_CAPSLOCK		, false },
	/*	0x3B	*/	{	CASCII_F1			,	CASCII_F1			, false },
	/*	0x3C	*/	{	CASCII_F2			,	CASCII_F2			, false },
	/*	0x3D	*/	{	CASCII_F3			,	CASCII_F3			, false },
	/*	0x3E	*/	{	CASCII_F4			,	CASCII_F4			, false },
	/*	0x3F	*/	{	CASCII_F5			,	CASCII_F5			, false },
	/*	0x40	*/	{	CASCII_F6			,	CASCII_F6			, false },
	/*	0x41	*/	{	CASCII_F7			,	CASCII_F7			, false },
	/*	0x42	*/	{	CASCII_F8			,	CASCII_F8			, false },
	/*	0x43	*/	{	CASCII_F9			,	CASCII_F9			, false },
	/*	0x44	*/	{	CASCII_F10			,	CASCII_F10			, false },
	/*	0x45	*/	{	CASCII_NUMLOCK		,	CASCII_NUMLOCK		, false },
	/*	0x46	*/	{	CASCII_SCROLLLOCK	,	CASCII_SCROLLLOCK	, false },
/*#define KEY_NUM_MIN 0x47 (keytable.h)*/
	/*	0x47	*/	{	CASCII_NUM_HOME		,	'7'					, false },
	/*	0x48	*/	{	CASCII_NUM_UP		,	'8'					, false },
	/*	0x49	*/	{	CASCII_NUM_PAGEUP	,	'9'					, false },
	/*	0x4A	*/	{	'-'					,	'-'					, false },
	/*	0x4B	*/	{	CASCII_NUM_LEFT		,	'4'					, false },
	/*	0x4C	*/	{	CASCII_NUM_CENTER	,	'5'					, false },
	/*	0x4D	*/	{	CASCII_NUM_RIGHT	,	'6'					, false },
	/*	0x4E	*/	{	'+'					,	'+'					, false },
	/*	0x4F	*/	{	CASCII_NUM_END		,	'1'					, false },
	/*	0x50	*/	{	CASCII_NUM_DOWN		,	'2'					, false },
	/*	0x51	*/	{	CASCII_NUM_PAGEDOWN	,	'3'					, false },
	/*	0x52	*/	{	CASCII_NUM_INS		,	'0'					, false },
	/*	0x53	*/	{	CASCII_NUM_DEL		,	'.'					, false },
/*#define KEY_NUM_MAX 0x53 (keytable.h)*/
	/*	0x54	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x55	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x56	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x57	*/	{	CASCII_F11			,	CASCII_F11			, false },
	/*	0x58	*/	{	CASCII_F12			,	CASCII_F12			, false },

	/* 확장키
	 * 확장키 공식 = (키 코드) - 0x1C + 0x59
	 */

	/*	0x59	*/	{	CASCII_NUM_ENTER	,	CASCII_ENTER		, false }, /* 0xE0 0x1C */
	/*	0x5A	*/	{	CASCII_RCTRL		,	CASCII_RCTRL		, false }, /* 0xE0 0x1D */

	/*	0x5B	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x5C	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x5D	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x5E	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x5F	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x60	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x61	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x62	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x63	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x64	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x65	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x66	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x67	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x68	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x69	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x6A	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x6B	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x6C	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x6D	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x6E	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x6F	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x70	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x71	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x72	*/	{	CASCII_NUM_SLASH	,	'/'					, false }, /* 0xE0 0x35 */

	/*	0x73	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x74	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x75	*/	{	CASCII_RALT			,	CASCII_RALT			, false }, /* 0xE0 0x38 */

	/*	0x76	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x77	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x78	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x79	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x7A	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x7B	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x7C	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x7D	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x7E	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x7F	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x80	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x81	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x82	*/	{	CASCII_NONE			,	CASCII_NONE			, false },
	/*	0x83	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x84	*/	{	CASCII_HOME			,	CASCII_HOME			, false }, /* 0xE0 0x47 */
	/*	0x85	*/	{	CASCII_UP			,	CASCII_UP			, false }, /* 0xE0 0x48 */
	/*	0x86	*/	{	CASCII_PAGEUP		,	CASCII_PAGEUP		, false }, /* 0xE0 0x49 */

	/*	0x87	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x88	*/	{	CASCII_LEFT			,	CASCII_LEFT			, false }, /* 0xE0 0x4B */

	/*	0x89	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x8A	*/	{	CASCII_RIGHT		,	CASCII_RIGHT		, false }, /* 0xE0 0x4D */

	/*	0x8B	*/	{	CASCII_NONE			,	CASCII_NONE			, false },

	/*	0x8C	*/	{	CASCII_END			,	CASCII_END			, false }, /* 0xE0 0x4F */
	/*	0x8D	*/	{	CASCII_DOWN			,	CASCII_DOWN			, false }, /* 0xE0 0x50 */
	/*	0x8E	*/	{	CASCII_PAGEDOWN		,	CASCII_PAGEDOWN		, false }, /* 0xE0 0x51 */
	/*	0x8F	*/	{	CASCII_INSERT		,	CASCII_INSERT		, false }, /* 0xE0 0x52 */
	/*	0x90	*/	{	CASCII_DELETE		,	CASCII_DELETE		, false }, /* 0xE0 0x53 */

/*#define KEY_CODE_PAUSE 0x91 (keytable.h)*/
	/*	0x91	*/	{	CASCII_PAUSE		,	CASCII_PAUSE		, false }, /* 0xE1 0x1D 0x45 */
};
