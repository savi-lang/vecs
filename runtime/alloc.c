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

#ifdef __linux__
#define _GNU_SOURCE
#endif
#include "platform.h"
#include "panic.h"

// We use the `perror` function to print an out of memory error,
// but this isn't supported for the WASM target.
#ifndef PLATFORM_IS_WASM
#include <stdio.h>
#endif

#ifdef PLATFORM_IS_POSIX_BASED
#include <sys/mman.h>
#endif

#if defined(PLATFORM_IS_MACOSX)
#include <mach/vm_statistics.h>
#endif

void* vecsint_virt_alloc(size_t bytes)
{
  void* p;
  bool ok = true;

#if defined(PLATFORM_IS_WINDOWS)
  p = VirtualAlloc(NULL, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if(p == NULL)
    ok = false;
#elif defined(PLATFORM_IS_POSIX_BASED)
#if defined(PLATFORM_IS_LINUX) || defined(PLATFORM_IS_EMSCRIPTEN)
  p = mmap(0, bytes, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
#elif defined(PLATFORM_IS_MACOSX)
  p = mmap(0, bytes, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANON, -1, 0);
#elif defined(PLATFORM_IS_DRAGONFLY)
  p = mmap(0, bytes, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANON, -1, 0);
#elif defined(PLATFORM_IS_OPENBSD)
  p = mmap(0, bytes, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANON, -1, 0);
#elif defined(PLATFORM_IS_BSD)
#ifndef MAP_ALIGNED_SUPER
#define MAP_ALIGNED_SUPER 0
#endif
  p = mmap(0, bytes, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANON | MAP_ALIGNED_SUPER, -1, 0);
#endif
  if(p == MAP_FAILED)
    ok = false;
#endif

  if(!ok) vecs_panic("out of memory");

  return p;
}

void vecsint_virt_free(void* p, size_t bytes)
{
#if defined(PLATFORM_IS_WINDOWS)
  VirtualFree(p, 0, MEM_RELEASE);
#elif defined(PLATFORM_IS_POSIX_BASED)
  munmap(p, bytes);
#endif
}
