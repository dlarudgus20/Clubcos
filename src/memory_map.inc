; Copyright (c) 2014, 임경현 (dlarudgus20)
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; * Redistributions of source code must retain the above copyright notice, this
;   list of conditions and the following disclaimer.
;
; * Redistributions in binary form must reproduce the above copyright notice,
;   this list of conditions and the following disclaimer in the documentation
;   and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; @file memory_map.inc
; @author dlarudgus20
; @copyright The BSD (2-Clause) License

; 참조: misc/docs/memory_map.txt
; 이곳의 address는 특별한 언급이 없는 한 전부 physical address입니다.

%define KERNEL_BASE_ADDRESS 0x00100000

%define PAGE_DIRECTORY_ADDRESS 0x00300000
%define PAGE_TABLE_ADDRESS 0x00301000

%define PAGE_TABLE_LOGICAL_ADDRESS 0xfe201000

%define KERNEL_STACK_END_LOGICAL_ADDR 0xfe200000
%define KERNEL_STACK_END_PHYSICAL_ADDR 0x00300000

%define DYN_MEMORY_START_ADDRESS 0x00900000
%define IOMAP_MEMORY_START_ADDRESS 0xbf800000

%define DYM_MEMORY_METADATA_PAGE_COUNT 49

%define PAGE_TABLE_DYNMEM_METADATA (PAGE_TABLE_ADDRESS + 0xfffcf * 4)
