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

#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <stdbool.h>
#include <stdint.h>

/** Convenience macro for making extern "C" more succinct.
 *
 */
#if defined(__cplusplus)
#  define VECS_EXTERN_C_BEGIN extern "C" {
#  define VECS_EXTERN_C_END }
#else
#  define VECS_EXTERN_C_BEGIN
#  define VECS_EXTERN_C_END
#endif

/** Determines the operating system.
 *
 */
#if defined(__APPLE__) && defined(__MACH__)
#  define PLATFORM_IS_MACOSX
#elif defined(__linux__)
#  define PLATFORM_IS_LINUX
#elif defined(__FreeBSD__)
#  define PLATFORM_IS_BSD
#  define PLATFORM_IS_FREEBSD
#elif defined(__DragonFly__)
#  define PLATFORM_IS_BSD
#  define PLATFORM_IS_DRAGONFLY
#elif defined(__OpenBSD__)
#  define PLATFORM_IS_BSD
#  define PLATFORM_IS_OPENBSD
#elif defined(__EMSCRIPTEN__)
#  define PLATFORM_IS_EMSCRIPTEN
#elif defined(__wasm32__)
#  define PLATFORM_IS_WASM
#  define PLATFORM_IS_WASM32
#elif defined(_WIN32)
#  define PLATFORM_IS_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <Windows.h>
#else
#  error PLATFORM NOT SUPPORTED!
#endif

#if defined(PLATFORM_IS_MACOSX) || defined(PLATFORM_IS_LINUX) \
  || defined(PLATFORM_IS_BSD) || defined(PLATFORM_IS_EMSCRIPTEN)
#  define PLATFORM_IS_POSIX_BASED
#endif

#if defined(PLATFORM_IS_POSIX_BASED) || defined(PLATFORM_IS_WASM) || defined(__MINGW64__)
#  define PLATFORM_IS_CLANG_OR_GCC
#endif

/** The platform's programming model.
 *
 */
#if defined(__LP64__)
#  define PLATFORM_IS_LP64
#elif defined(_WIN64)
#  define PLATFORM_IS_LLP64
#else
#  define PLATFORM_IS_ILP32
#endif

/** ARM architecture flags.
 *
 */
#if defined(__ARM_ARCH_7__) || \
    defined(__ARM_ARCH_7R__) || \
    defined(__ARM_ARCH_7A__)
# define ARMV7 1
#endif

#if defined(ARMV7) || \
    defined(__ARM_ARCH_6__) || \
    defined(__ARM_ARCH_6J__) || \
    defined(__ARM_ARCH_6K__) || \
    defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_6ZK__)
# define ARMV6 1
#endif

#if defined(ARMV6) || \
    defined(__ARM_ARCH_5T__) || \
    defined(__ARM_ARCH_5E__) || \
    defined(__ARM_ARCH_5TE__) || \
    defined(__ARM_ARCH_5TEJ__)
# define ARMV5 1
#endif

#if defined(ARMV5) || \
    defined(__ARM_ARCH_4__) || \
    defined(__ARM_ARCH_4T__)
# define ARMV4 1
#endif

#if defined(ARMV4) || \
    defined(__ARM_ARCH_3__) || \
    defined(__ARM_ARCH_3M__)
# define ARMV3 1
#endif

#if defined(ARMV3) || \
    defined(__ARM_ARCH_2__)
# define ARMV2 1
#endif

/** Architecture flags.
 *
 */
#if defined(ARMV2) || defined(__arm__) || defined(__aarch64__)
# define PLATFORM_IS_ARM
# if defined(__aarch64__)
#  define PLATFORM_IS_ARM64
# else
#  define PLATFORM_IS_ARM32
# endif
#elif defined(__i386__) || defined(_M_IX86) || defined(_X86_) || \
 defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || \
 defined(_M_AMD64)
# define PLATFORM_IS_X86
#elif defined(__riscv)
# define PLATFORM_IS_RISCV
#endif

/** Data types.
 *
 */
#include <stddef.h>

#define VECS_ERRNO uint32_t

#define __zu "%zu"

/** Standard builtins.
 *
 */
#ifdef PLATFORM_IS_ARM32
uint32_t __vecsint_popcount(uint32_t x);
uint32_t __vecsint_ffs(uint32_t x);
uint32_t __vecsint_ffsll(uint64_t x);
uint32_t __vecsint_ctz(uint32_t x);
uint32_t __vecsint_clz(uint32_t x);
uint32_t __vecsint_clzll(uint64_t x);
#else
inline uint32_t __vecsint_popcount(uint32_t x) {
  return (uint32_t)__builtin_popcount(x);
}

inline uint32_t __vecsint_ffs(uint32_t x) {
  return (uint32_t)__builtin_ffs((int)x);
}

inline uint32_t __vecsint_ffsll(uint64_t x) {
  return (uint32_t)__builtin_ffsll((long long int)x);
}

inline uint32_t __vecsint_ctz(uint32_t x) {
  return (uint32_t)__builtin_ctz(x);
}

inline uint32_t __vecsint_clz(uint32_t x) {
  return (uint32_t)__builtin_clz(x);
}

inline uint32_t __vecsint_clzll(uint64_t x) {
  return (uint32_t)__builtin_clzll(x);
}
#endif

inline uint32_t __vecsint_ffszu(size_t x) {
#ifdef PLATFORM_IS_ILP32
  return __vecsint_ffs(x);
#else
  return __vecsint_ffsll(x);
#endif
}

inline uint32_t __vecsint_clzzu(size_t x) {
#ifdef PLATFORM_IS_ILP32
  return __vecsint_clz(x);
#else
  return __vecsint_clzll(x);
#endif
}

inline void* __vecsint_memcpy(void* dest, const void* src, size_t count) {
  return __builtin_memcpy(dest, src, count);
}

/** Static assert
 *
 */

#if defined(__clang__)
#  if __has_feature(cxx_static_assert)
#    define vecsint_static_assert(c, m) static_assert(c, m)
#  elif __has_feature(c_static_assert)
#    define vecsint_static_assert(c, m) _Static_assert(c, m)
#  else
#    error "Clang doesn't support `static_assert` or `_Static_assert`."
#  endif
#else
#  include <assert.h>
#  define vecsint_static_assert(c, m) static_assert(c, m)
#endif

/** Storage class modifiers.
 *
 */
#define __vecsint_spec_malloc__(FUNC) FUNC __attribute__((malloc))

/** Symbol visibility for the LLVM JIT.
 *
 */
#ifdef PLATFORM_IS_WINDOWS
#  define VECS_API __attribute__((dllexport))
#elif defined(PLATFORM_IS_POSIX_BASED)
#  define VECS_API  __attribute__((visibility("default")))
#endif

/** Compile time choose expression.
 *
 *  (void)0 will cause a compile-time error in non-cpp environments, as
 *  __vecsint_choose_expr is based on __builtin_choose_expr.
 */
#if defined(__cplusplus)
#  define EXPR_NONE 0
#  define __vecsint_choose_expr(COND, THEN, ELSE) \
            ((COND) ? (THEN) : (ELSE))
#elif defined(PLATFORM_IS_CLANG_OR_GCC)
#  define EXPR_NONE ((void) 0)
#  define __vecsint_choose_expr(COND, THEN, ELSE) \
            __builtin_choose_expr(COND, THEN, ELSE)
#endif

#ifndef PLATFORM_IS_WASM
#  include "threads.h"
#  include "paths.h"
#endif

#if defined(PLATFORM_IS_WINDOWS)
#  include "vcvars.h"
#endif

#if defined(ARMV7) && !defined(__ARM_NEON) && !defined(__ARM_NEON__)
#  define PLATFORM_IS_ARMHF_WITHOUT_NEON 1
#endif

#endif
