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
 * @file string.h
 * @brief some os-independent functions
 * @date 2014. 4. 26.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

size_t strlen(const char* str);
size_t strnlen(const char *str, size_t size);
char *strcpy(char * restrict dest, const char * restrict src);
char *strncpy(char * restrict dest, const char * restrict src, size_t count);
char *strcat(char * restrict dest, const char * restrict src);

int strcmp(const char *lhs, const char *rhs);
char *strchr(const char *str, int ch);
char *_strchr_not(const char *str, int ch);

void *_memcpy_forward(void *dest, const void *src, size_t count); /* asmfunc */
void *_memcpy_reverse(void *dest, const void *src, size_t count); /* asmfunc */
static inline void *memcpy(void * restrict dest, const void * restrict src, size_t count)
{
	return _memcpy_forward((void *)dest, (const void *)src, count);
}
static inline void *memmove(void *dest, const void *src, size_t count)
{
	char *d = (char *)dest;
	const char *s = (const char *)src;
	if (s < d && d < s + count)
		return _memcpy_reverse(dest, src, count);
	else
		return _memcpy_forward(dest, src, count);
}

void *memset(void *dest, int ch, size_t count); /* asmfunc */
void *_memset_2(void *dest, int ch, size_t count); /* asmfunc */
void *memchr(const void *ptr, int ch, size_t count); /* asmfunc */
void *_memchr_4(const void *ptr, int ch, size_t count); /* asmfunc */

int atoi(const char *str);

int snprintf(char *buf, size_t size, const char *format, ...);
int vsnprintf(char *buf, size_t size, const char *format, va_list va);

void srand(unsigned int seed);
int rand(void);

static inline int isalpha(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
static inline int isdigit(int c) { return c >= '0' && c <= '9'; }
static inline int iscntrl(int c) { return (c <= 0x1f && c == 0x7f); }
static inline int isspace(int c) { return strchr(" \t\n\v\f\r", c) != NULL; }

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define range(a, x, b) (((x) < (a)) ? (a) : (min(x, b)))

void swap_endian_of_shorts(uint16_t *ar, size_t count);

#endif /* STRING_H_ */
