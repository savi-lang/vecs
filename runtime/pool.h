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

#ifndef mem_pool_h
#define mem_pool_h

#include <stdbool.h>
#include <stddef.h>

#include "platform.h"

VECS_EXTERN_C_BEGIN

/* Because of the way free memory is reused as its own linked list container,
 * the minimum allocation size is 32 bytes.
 */

#define POOL_MIN_BITS 5
#define POOL_MAX_BITS 20
#define POOL_ALIGN_BITS 10

#define POOL_MIN (1 << POOL_MIN_BITS)
#define POOL_MAX (1 << POOL_MAX_BITS)
#define POOL_ALIGN (1 << POOL_ALIGN_BITS)
#define POOL_COUNT (POOL_MAX_BITS - POOL_MIN_BITS + 1)

__vecsint_spec_malloc__(void* vecsint_pool_alloc(size_t index));
void vecsint_pool_free(size_t index, void* p);

__vecsint_spec_malloc__(void* vecsint_pool_alloc_size(size_t size));
void vecsint_pool_free_size(size_t size, void* p);

void* vecsint_pool_realloc_size(size_t old_size, size_t new_size, void* p);

void vecsint_pool_thread_cleanup();

size_t vecsint_pool_index(size_t size);

size_t vecsint_pool_used_size(size_t index);

size_t vecsint_pool_adjust_size(size_t size);

#define POOL_INDEX(SIZE) \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 0)), 0, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 1)), 1, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 2)), 2, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 3)), 3, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 4)), 4, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 5)), 5, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 6)), 6, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 7)), 7, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 8)), 8, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 9)), 9, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 10)), 10, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 11)), 11, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 12)), 12, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 13)), 13, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 14)), 14, \
  __vecsint_choose_expr(SIZE <= (1 << (POOL_MIN_BITS + 15)), 15, \
    EXPR_NONE \
    ))))))))))))))))

#define POOL_ALLOC(TYPE) \
  (TYPE*) vecsint_pool_alloc(POOL_INDEX(sizeof(TYPE)))

#define POOL_FREE(TYPE, VALUE) \
  vecsint_pool_free(POOL_INDEX(sizeof(TYPE)), VALUE)

#define POOL_SIZE(INDEX) \
  ((size_t)1 << (POOL_MIN_BITS + INDEX))

#ifdef USE_RUNTIMESTATS
#define POOL_ALLOC_SIZE(TYPE) \
  POOL_SIZE(POOL_INDEX(sizeof(TYPE)))
#endif

VECS_EXTERN_C_END

#endif
