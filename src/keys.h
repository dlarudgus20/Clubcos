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
 * @file keys.h
 * @date 2014. 5. 4.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef KEYS_H_
#define KEYS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"

/**
 * @brief 키보드 인터럽트를 받았을 때, 인터럽트 큐에 의해 호출됩니다.
 * @param[in] QueueData 인터럽트 큐에 삽입된 데이터입니다.
 */
void ckOnKeyboardInterrupt(uint32_t QueueData);

/**
 * @brief 키 코드를 cascii 코드로 변환합니다.
 * @param[in] KeyCode 키 코드입니다.
 * @return 변환된 cascii 코드입니다.
 */
uint8_t ckGetCasciiFromKeyCode(uint16_t KeyCode);

/**
 * @brief 해당 키 코드가 눌린 키를 나태내는지 여부를 검사합니다.
 * @return 눌린 키를 나타내면 <c>true</c>이고, 그렇지 않으면 <c>false</c>입니다.
 */
static inline bool ckKeyIsDown(uint16_t KeyCode)
{
	return (KeyCode & KEYCODE_FLAG_UNPUSH) == 0;
}
static inline bool ckKeyIsAscii(uint8_t cascii)
{
	return (cascii & 0x80) == 0;
}

#endif /* KEYS_H_ */
