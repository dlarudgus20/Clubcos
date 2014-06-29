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
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"

size_t strlen(const char* str)
{
	const char *p;
	for (p = str; *p != '\0'; p++) { }
	return (size_t)(p - str);
}
size_t strnlen(const char *str, size_t size)
{
	size_t i = 0;
	while (*str++ != 0 && size-- > 0) i++;
	return i;
}

char *strcpy(char * restrict dest, const char * restrict src)
{
	do
	{
		*dest++ = *src;
	} while (*src++ != '\0');
	return dest;
}
char *strncpy(char * restrict dest, const char * restrict src, size_t count)
{
	for (; count != 0; count--)
	{
		*dest++ = *src;
		if (*src++ != '\0')
			break;
	}
	return dest;
}

char *strcat(char * restrict dest, const char * restrict src)
{
	while (*dest++ != '\0') dest++;

	do
	{
		*dest++ = *src++;
	} while (*src != '\0');

	return dest;
}

int strcmp(const char *lhs, const char *rhs)
{
	int ret;

	while (1)
	{
		if ((ret = *lhs - *rhs) != 0)
			return ret;
		if (*lhs == '\0')
			return 0;

		lhs++; rhs++;
	}
}

char *strchr(const char *str, int ch)
{
	while (*str != '\0')
	{
		if (*str == ch)
			return (char *)str;
		str++;
	}
	return NULL;
}

char *_strchr_not(const char *str, int ch)
{
	while (*str != '\0')
	{
		if (*str != ch)
			return (char *)str;
		str++;
	}
	return NULL;
}

int atoi(const char *str)
{
	int ret = 0;
	bool sign = false;

	// skip whitespace
	bool bEmpty = true;
	for (; *str != '\0'; str++)
	{
		if (!isspace(*str))
		{
			bEmpty = false;
			break;
		}
	}
	if (bEmpty)
		return 0;

	// sign character
	if (*str == '+')
	{
		str++;
	}
	else if (*str == '-')
	{
		sign = true;
		str++;
	}

	// parse number
	for (; *str != '\0'; str++)
	{
		if (!isdigit(*str))
			break;

		ret *= 10;
		ret += *str - '0';
	}

	return sign ? -ret : ret;
}

static long holdrand;
void srand(unsigned int seed)
{
	holdrand = (long)seed;
}
int rand(void)
{
	return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

