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
 * @file page.h
 * @date 2014. 5. 3.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#ifndef PAGE_H_
#define PAGE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
	PAGE_DIRENT_S = 0x080,	//!< 페이지 크기입니다. 0이면 4KB 페이지이고, 1이면 4MB 페이지입니다.
	PAGE_DIRENT_C = 0x010,	//!< 1이면 캐시를 비활성화합니다.
	PAGE_DIRENT_W = 0x008,	//!< 1면 Write-through를 사용하고, 0이면 Write-back을 사용합니다.
	PAGE_DIRENT_U = 0x004,	//!< 1이면 User 페이지이고, 0이면 Supervisor 페이지입니다.
	PAGE_DIRENT_R = 0x002,	//!< 1이면 R/W 모두 가능한 페이지이고, 0이면 Read만 가능한 페이지입니다.
	PAGE_DIRENT_P = 0x001	//!< Present 비트입니다.
};
enum
{
	PAGE_TBLENT_G = 0x100,	//!< Global 페이지입니다.
	PAGE_TBLENT_C = 0x010,	//!< 1이면 캐시를 비활성화합니다.
	PAGE_TBLENT_W = 0x008,	//!< 1면 Write-through를 사용하고, 0이면 Write-back을 사용합니다.
	PAGE_TBLENT_U = 0x004,	//!< 1이면 User 페이지이고, 0이면 Supervisor 페이지입니다.
	PAGE_TBLENT_R = 0x002,	//!< 1이면 R/W 모두 가능한 페이지이고, 0이면 Read만 가능한 페이지입니다.
	PAGE_TBLENT_P = 0x001	//!< Present 비트입니다.
};

#endif /* PAGE_H_ */
