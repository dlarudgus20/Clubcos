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
 * @file assert.h
 * @date 2014. 5. 5.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#ifndef NDEBUG

/**
 * @brief 단언 실패 메시지와 함께 PANIC합니다. <c>NDEBUG</c> 매크로가 선언되지 않았을 경우에만 존재합니다.
 * @param[in] str 단언문입니다.
 * @param[in] file 단언문이 있는 파일입니다.
 * @param[in] func 단언문이 있는 함수입니다.
 * @param[in] line 단언문이 있는 줄번호입니다.
 * @sa ckTerminalPanic
 */
void ckAssertMessage(const char *str, const char *file, const char *func, unsigned line);

/**
 * @brief 단언문을 확인합니다. 단언문이 실패할 경우 @ref ckAssertMessage 를 통해 PANIC합니다.<br/>
 *        <c>NDEBUG</c> 매크로가 선언되어있다면 아무 일도 하지 않습니다.
 * @param exp 단언문입니다.
 */
# define assert(exp) ((void)((exp) || (ckAssertMessage(#exp, __FILE__, __func__, __LINE__), 1)))

#else

# define assert(exp) ((void)0)

#endif

#endif /* ASSERT_H_ */
