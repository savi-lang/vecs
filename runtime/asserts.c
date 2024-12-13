// This particular file is based on the Pony language runtime.
// It has been somewhat modified and adapted to work for our purposes.
// The Pony project's license is reproduced below.
//
// Copyright (C) 2016-2024, The Pony Developers
// Copyright (c) 2014-2015, Causality Ltd.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#define VECS_WANT_ATOMIC_DEFS

#include "asserts.h"

#ifndef PLATFORM_IS_WASM
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#endif

#ifdef PLATFORM_IS_POSIX_BASED
#if defined(__GLIBC__) || defined(PLATFORM_IS_BSD) || defined(ALPINE_LINUX)
#  include <execinfo.h>
#endif
#  include <unistd.h>
#elif defined(PLATFORM_IS_WASM)
// Nothing for WASM.
#else
#  include <Windows.h>
#  include <DbgHelp.h>
#endif

#include "atomics.h"

static VECS_ATOMIC_INIT(bool, assert_guard, false);

#ifdef PLATFORM_IS_POSIX_BASED

#if defined(PLATFORM_IS_BSD) && !defined(PLATFORM_IS_OPENBSD)
typedef size_t stack_depth_t;
#else
typedef int stack_depth_t;
#endif

void vecsint_assert_fail(const char* expr, const char* file, size_t line,
  const char* func)
{
  while(atomic_exchange_explicit(&assert_guard, true, memory_order_acq_rel))
  {
    // If the guard is already set, an assertion fired in another thread. The
    // things here aren't thread safe, so we just start an infinite loop.
    struct timespec ts = {1, 0};
    nanosleep(&ts, NULL);
  }

  fprintf(stderr, "%s:" __zu ": %s: Assertion `%s` failed.\n\n", file, line,
    func, expr);

#if defined(__GLIBC__) || defined(PLATFORM_IS_BSD) || defined(ALPINE_LINUX)
  void* addrs[256];
  stack_depth_t depth = backtrace(addrs, 256);
  char** strings = backtrace_symbols(addrs, depth);

  fputs("Backtrace:\n", stderr);

  for(stack_depth_t i = 0; i < depth; i++)
    printf("  %s\n", strings[i]);

  free(strings);

#else
  fputs("Backtrace functionality not available.\n", stderr);
#endif
  fflush(stderr);
  abort();
}

#elif defined(PLATFORM_IS_WINDOWS)

void vecsint_assert_fail(const char* expr, const char* file, size_t line,
  const char* func)
{
  while(atomic_exchange_explicit(&assert_guard, true, memory_order_acq_rel))
  {
    // If the guard is already set, an assertion fired in another thread. The
    // things here aren't thread safe, so we just start an infinite loop.
    SleepEx(1, true);
  }

  fprintf(stderr, "%s:" __zu ": %s: Assertion `%s` failed.\n\n", file, line,
    func, expr);

  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();

  CONTEXT context;
  memset(&context, 0, sizeof(context));
  context.ContextFlags = CONTEXT_ALL;
  RtlCaptureContext(&context);
  SymInitialize(process, NULL, true);

  STACKFRAME64 frame;
  memset(&frame, 0, sizeof(frame));
  frame.AddrPC.Mode = AddrModeFlat;
  frame.AddrStack.Mode = AddrModeFlat;
  frame.AddrFrame.Mode = AddrModeFlat;

#  ifdef _M_IX86
  frame.AddrPC.Offset = context.Eip;
  frame.AddrStack.Offset = context.Esp;
  frame.AddrFrame.Offset = context.Ebp;
  DWORD machine = IMAGE_FILE_MACHINE_I386;
#  elif defined(_M_X64)
  frame.AddrPC.Offset = context.Rip;
  frame.AddrStack.Offset = context.Rsp;
  frame.AddrFrame.Offset = context.Rbp;
  DWORD machine = IMAGE_FILE_MACHINE_AMD64;
#  endif

  fputs("Backtrace:\n", stderr);

  while(true)
  {
    SetLastError(0);
    if(!StackWalk64(machine, process, thread, &frame, &context, 0,
      SymFunctionTableAccess64, SymGetModuleBase64, 0))
      break;

    DWORD64 addr = frame.AddrFrame.Offset;
    if(addr == 0)
      break;

    union
    {
      SYMBOL_INFO sym;
      BYTE buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    } sym_buf;
    PSYMBOL_INFO symbol = &sym_buf.sym;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    if(SymFromAddr(process, frame.AddrPC.Offset, NULL, symbol))
    {
      fprintf(stderr, "  (%s) [%p]\n", symbol->Name,
        (void*)frame.AddrPC.Offset);
    } else {
      fprintf(stderr, "  () [%p]\n", (void*)frame.AddrPC.Offset);
    }
  }

  if(strcmp(VECS_BUILD_CONFIG, "release") == 0)
  {
    fputs("This is an optimised version of vecs: the backtrace may be "
      "imprecise or incorrect.\nUse a debug version to get more meaningful "
      "information.\n", stderr);
  }

  SymCleanup(process);
  fflush(stderr);
  abort();
}

#endif

void vecsint_assert_disable_popups()
{
  // from LLVM utils/unittest/UnitTestMain/TestMain.cpp
# if defined(_WIN32)
  ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
# endif
}
