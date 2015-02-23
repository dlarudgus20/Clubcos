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
 * @file port_impl.h
 * @date 2014. 5. 25.
 * @author dlarudgus20
 */

#ifndef PORT_IMPL_H_
#define PORT_IMPL_H_

static inline uint8_t ckPortInByte(uint16_t port)
{
	uint8_t ret;
	__asm__ __volatile__ ( "in %1, %0" : "=a"(ret) : "d"(port) );
	return ret;
}
static inline void ckPortOutByte(uint16_t port, uint8_t data)
{
	__asm__ __volatile__ ( "out %0, %1" : : "a"(data), "d"(port) );
}

static inline uint16_t ckPortInWord(uint16_t port)
{
	uint16_t ret;
	__asm__ __volatile__ ( "in %1, %0" : "=a"(ret) : "d"(port) );
	return ret;
}
static inline void ckPortOutWord(uint16_t port, uint16_t data)
{
	__asm__ __volatile__ ( "out %0, %1" : : "a"(data), "d"(port) );
}

static inline void ckPortInWordList(uint16_t port, uint16_t *buf, size_t count)
{
	__asm__ __volatile__ ( "cld \n rep insw" : : "d"(port), "D"(buf), "c"(count) : "edi", "ecx", "cc" );
}

static inline void ckAsmGetTimeStamp(uint32_t *high, uint32_t *low)
{
	uint32_t h, l;
	__asm__ __volatile__ ( "rdtsc" : "=d"(h), "=a"(l) );
	*high = h; *low = l;
}

static inline uint32_t ckAsmGetEFlag(void)
{
	uint32_t ret;
	__asm__ __volatile__ ( "pushfl \n pop %0" : "=g"(ret) );
	return ret;
}

static inline void ckAsmSetEFlag(uint32_t eflag)
{
	__asm__ __volatile__ ( "push %0 \n popfl" : : "g"(eflag) : "cc" );
}

static inline void ckAsmCli(void)
{
	__asm__ __volatile__ ( "cli" );
}

static inline void ckAsmSti(void)
{
	__asm__ __volatile__ ( "sti" );
}

static inline void ckAsmHlt(void)
{
	__asm__ __volatile__ ( "hlt" );
}

#endif /* PORT_IMPL_H_ */
