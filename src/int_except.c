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
 * @file int_except.c
 * @brief CPU 예외 핸들러입니다.
 * @date 2014. 5. 25.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "interrupt.h"
#include "string.h"
#include "terminal.h"

// InterruptContext를 문자열로 덤프합니다.
static void csDumpContext(char *buf, size_t bufsize,
	const char *msg, InterruptContext *pContext);
static void csDumpContextWithErr(char *buf, size_t bufsize,
	const char *msg, InterruptContext *pContext, const char *err_desc);

// 예외로 인해 PANIC 상태에 빠집니다.
static void csPanicException(InterruptContext *pContext, const char *msg);
static void csPanicExceptionWithErr(InterruptContext *pContext, const char *msg, const char *err_desc);

// selector error code의 설명 문자열을 조립합니다.
// http://wiki.osdev.org/Exceptions#Selector_Error_Code
static const char *csSelectorErrorMsg(uint32_t err_code);
// page fault error code의 설명 문자열을 조립합니다.
// http://wiki.osdev.org/Exceptions#Page_Fault
static const char *csPageFaultErrorMsg(uint32_t err_code);

void ck_UnknownIntHandler(InterruptContext *pContext)
{
	csPanicException(pContext, "unknown exception");
}

void ck_ExceptIntHandler00(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x00] `Divide-by-Zero`");
}

void ck_ExceptIntHandler01(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x01] `Debug`");
}

void ck_ExceptIntHandler02(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x02] `NMI`");
}

void ck_ExceptIntHandler03(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x03] `Breakpoint`");
}

void ck_ExceptIntHandler04(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x04] `Overflow`");
}

void ck_ExceptIntHandler05(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x05] `Bound Range Exceeded`");
}

void ck_ExceptIntHandler06(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x06] `Invalid Opcode`");
}

/*
 * ck_ExceptIntHandler07 (#NM, Device Not Available)는 floating-point 처리를 위해 task.c에 있습니다.
 */

void ck_ExceptIntHandler08(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x08] `Double Fault`", "");
}

void ck_ExceptIntHandler09(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x09] `Coprocessor Segment Overrun`");
}

void ck_ExceptIntHandler0A(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x0A] `Invalid TSS`",
		csSelectorErrorMsg(pContext->err_code));
}

void ck_ExceptIntHandler0B(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x0B] `Segment Not Present`",
		csSelectorErrorMsg(pContext->err_code));
}

void ck_ExceptIntHandler0C(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x0C] `Stack-Segment Fault`",
		csSelectorErrorMsg(pContext->err_code));
}

void ck_ExceptIntHandler0D(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x0D] `General Protection Fault`",
		csSelectorErrorMsg(pContext->err_code));
}

void ck_ExceptIntHandler0E(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x0E] `Page Fault`",
		csPageFaultErrorMsg(pContext->err_code));
}

void ck_ExceptIntHandler0F(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x0F] `?? (reserved) ??`");
}

void ck_ExceptIntHandler10(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x10] `x87 Floating-Point Exception`");
}

void ck_ExceptIntHandler11(InterruptContext *pContext)
{
	csPanicExceptionWithErr(pContext, "[0x11] `Alignment Check`", "");
}

void ck_ExceptIntHandler12(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x12] `Machine Check`");
}

void ck_ExceptIntHandler13(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x13] `SIMD Floating-Point Exception`");
}

void ck_ExceptIntHandler14(InterruptContext *pContext)
{
	csPanicException(pContext, "[0x14] `Virtualization Exception`");
}

static const char *csSelectorErrorMsg(uint32_t err_code)
{
	static const char *tbl[] = { "GDT", "IDT", "LDT", "IDT" };

	static char msg[1024];

	snprintf(msg, sizeof(msg), "0x%04x %s%s : ",
		err_code & 0xfff8, tbl[(err_code & 0x6) >> 1],
		(err_code & 0x1) ? " externally to the processor" : "");

	// 어차피 panic이기 때문에 static 지역변수를 return해도 안전함.
	return msg;
}

static const char *csPageFaultErrorMsg(uint32_t err_code)
{
	static char msg[1024];

	if (err_code & 0x1)
		strcpy(msg, "page-protection, ");
	else
		strcpy(msg, "non-present, ");

	if (err_code & 0x2)
		strcat(msg, "during writing, ");
	else
		strcat(msg, "during reading, ");

	if (err_code & 0x4)
		strcat(msg, "by user, ");

	if (err_code & 0x8)
		strcat(msg, "read from reserved field, ");

	if (err_code & 0x10)
		strcat(msg, "instruction fetch, ");

	strcpy(msg + strlen(msg) - 2, " : ");
	return msg;
}

static void csPanicException(InterruptContext *pContext, const char *msg)
{
	char buf[1024];
	ckTerminalPrintStatusBar_unsafe(msg);
	csDumpContext(buf, sizeof(buf), msg, pContext);
	ckTerminalPanic(buf);
}

static void csPanicExceptionWithErr(InterruptContext *pContext, const char *msg, const char *err_desc)
{
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s Exception occur!!!", msg);
	ckTerminalPrintStatusBar_unsafe(buf);
	csDumpContextWithErr(buf, sizeof(buf), msg, pContext, err_desc);
	ckTerminalPanic(buf);
}

static void csDumpContext(char *buf, size_t bufsize,
	const char *msg, InterruptContext *pContext)
{
	snprintf(buf, bufsize,
		"%s Exception occur!!!\n"
		"cs(%p) eip(%p) eflag(%p) ss2(%p) esp2(%p) "
		"ds(%p) es(%p) fs(%p) gs(%p) "
		"eax(%p) ebx(%p) ecx(%p) edx(%p) esi(%p) edi(%p) esp(%p) ebp(%p)"
		, msg
		, pContext->cs, pContext->eip, pContext->eflag, pContext->ss2, pContext->esp2
		, pContext->ds, pContext->es, pContext->fs, pContext->gs
		, pContext->eax, pContext->ebx, pContext->ecx, pContext->edx
		, pContext->esi, pContext->edi, pContext->esp, pContext->ebp);
}

static void csDumpContextWithErr(char *buf, size_t bufsize,
	const char *msg, InterruptContext *pContext, const char *err_desc)
{
	snprintf(buf, bufsize,
		"%s Exception occur!!!\n"
		"cs(%p) eip(%p) eflag(%p) ss2(%p) esp2(%p) "
		"ds(%p) es(%p) fs(%p) gs(%p) "
		"eax(%p) ebx(%p) ecx(%p) edx(%p) esi(%p) edi(%p) esp(%p) ebp(%p) "
		"err_code(%s%p)"
		, msg
		, pContext->err_cs, pContext->err_eip, pContext->err_eflag, pContext->err_ss2, pContext->err_esp2
		, pContext->ds, pContext->es, pContext->fs, pContext->gs
		, pContext->eax, pContext->ebx, pContext->ecx, pContext->edx
		, pContext->esi, pContext->edi, pContext->esp, pContext->ebp
		, err_desc, pContext->err_code);
}

