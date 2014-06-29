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
 * @file likely.h
 * @date 2014. 6. 1.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef LIKELY_H_
#define LIKELY_H_

/**
 * @brief 주어진 조건문이 참일 확률이 높다고 컴파일러와 CPU에게 힌트를 줍니다.
 * @remark CPU가 분기문을 처리할 때, CPU 파이프라인 처리에 방해를 받아 성능에 영향을 줄 수 있습니다.
 *         이 매크로는 CPU에게 이 조건문이 거의 항상 참이라는 힌트를 주어서 파이프라인 처리가 효율적으로 수행될 수 있게 돕습니다.
 */
#define likely(x)       __builtin_expect((x), 1)

/**
 * @brief 주어진 조건문이 거짓일 확률이 높다고 컴파일러와 CPU에게 힌트를 줍니다.
 *        @ref likely 매크로의 설명을 참조하십시오.
 */
#define unlikely(x)     __builtin_expect((x), 0)

#endif /* LIKELY_H_ */
