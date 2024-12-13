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

#ifndef VECS_ASSERTS_H
#define VECS_ASSERTS_H

#include "platform.h"

VECS_EXTERN_C_BEGIN

#if !defined(VECS_NDEBUG) && !defined(VECS_ALWAYS_ASSERT) && defined(NDEBUG)
#  define VECS_NDEBUG
#endif

#if defined(VECS_NDEBUG)
#  define vecsint_assert(expr) ((void)0)
#else
#  define vecsint_assert(expr) \
    ((expr) ? (void)0 : \
      vecsint_assert_fail(#expr, __FILE__, __LINE__, __func__))
#endif

void vecsint_assert_fail(const char* expr, const char* file, size_t line,
  const char* func);

void vecsint_assert_disable_popups();

VECS_EXTERN_C_END

#endif
