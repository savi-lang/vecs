#ifndef VECS_ATOMICS_H
#define VECS_ATOMICS_H

#if !defined(__ARM_ARCH_2__) && !defined(__arm__) && !defined(__aarch64__) && \
 !defined(__i386__) && !defined(_M_IX86) && !defined(_X86_) && \
 !defined(__amd64__) && !defined(__x86_64__) && !defined(_M_X64) && \
 !defined(_M_AMD64) && !defined(__EMSCRIPTEN__) && !defined(__riscv) && \
 !defined(__wasm32__)
#  error "Unsupported platform"
#endif

#ifndef __cplusplus
#  include <stdalign.h>
#endif

#if defined(__clang__)
#  if __clang_major__ >= 4 || (__clang_major__ == 3 && __clang_minor__ >= 6)
#    include <stdatomic.h>
#    define VECS_ATOMIC(T) T _Atomic
#    define VECS_ATOMIC_RVALUE(T) T _Atomic
#    define VECS_ATOMIC_INIT(T, N, V) T _Atomic N = V
#  elif __clang_major__ >= 3 && __clang_minor__ >= 4
#    define VECS_ATOMIC(T) alignas(sizeof(T)) T
#    define VECS_ATOMIC_RVALUE(T) T
#    define VECS_ATOMIC_INIT(T, N, V) T N = V
#    define VECS_ATOMIC_BUILTINS
#  else
#    error "Please use Clang >= 3.4"
#  endif
#else
#  error "Unsupported compiler"
#endif

#if defined(__LP64__) || defined(_WIN64)
#  define VECS_DOUBLEWORD __int128_t
#else
#  define VECS_DOUBLEWORD int64_t
#endif
#define VECS_ABA_PROTECTED_PTR_DECLARE(T) \
  typedef union \
  { \
    struct \
    { \
      T* object; \
      uintptr_t counter; \
    }; \
    VECS_DOUBLEWORD raw; \
  } aba_protected_##T;
#define VECS_ABA_PROTECTED_PTR(T) aba_protected_##T

// We provide our own implementation of big atomic objects (larger than machine
// word size) because we need special functionalities that aren't provided by
// standard atomics. In particular, we need to be able to do both atomic and
// non-atomic operations on big objects since big atomic operations (e.g.
// CMPXCHG16B on x86_64) are very expensive.
#define VECS_ATOMIC_ABA_PROTECTED_PTR(T) \
    alignas(sizeof(VECS_ABA_PROTECTED_PTR(T))) VECS_ABA_PROTECTED_PTR(T)

#define bigatomic_load_explicit(PTR, MO) \
    ({ \
      _Static_assert(sizeof(*(PTR)) == (2 * sizeof(void*)), ""); \
      (__typeof__(*(PTR)))__atomic_load_n(&(PTR)->raw, MO); \
    })

#define bigatomic_store_explicit(PTR, VAL, MO) \
    ({ \
      _Static_assert(sizeof(*(PTR)) == (2 * sizeof(void*)), ""); \
      __atomic_store_n(&(PTR)->raw, (VAL).raw, MO); \
    })

#define bigatomic_compare_exchange_strong_explicit(PTR, EXP, DES, SUCC, FAIL) \
    ({ \
      _Static_assert(sizeof(*(PTR)) == (2 * sizeof(void*)), ""); \
      __atomic_compare_exchange_n(&(PTR)->raw, &(EXP)->raw, (DES).raw, false, \
        SUCC, FAIL); \
    })

#ifdef VECS_ATOMIC_BUILTINS
#  define memory_order_relaxed __ATOMIC_RELAXED
#  define memory_order_consume __ATOMIC_CONSUME
#  define memory_order_acquire __ATOMIC_ACQUIRE
#  define memory_order_release __ATOMIC_RELEASE
#  define memory_order_acq_rel __ATOMIC_ACQ_REL
#  define memory_order_seq_cst __ATOMIC_SEQ_CST

#  define atomic_load_explicit(PTR, MO) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_load_n(PTR, MO); \
      })

#  define atomic_store_explicit(PTR, VAL, MO) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_store_n(PTR, VAL, MO); \
      })

#  define atomic_exchange_explicit(PTR, VAL, MO) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_exchange_n(PTR, VAL, MO); \
      })

#  define atomic_compare_exchange_weak_explicit(PTR, EXP, DES, SUCC, FAIL) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_compare_exchange_n(PTR, EXP, DES, true, SUCC, FAIL); \
      })

#  define atomic_compare_exchange_strong_explicit(PTR, EXP, DES, SUCC, FAIL) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_compare_exchange_n(PTR, EXP, DES, false, SUCC, FAIL); \
      })

#  define atomic_fetch_add_explicit(PTR, VAL, MO) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_fetch_add(PTR, VAL, MO); \
      })

#  define atomic_fetch_sub_explicit(PTR, VAL, MO) \
      ({ \
        _Static_assert(sizeof(PTR) <= sizeof(void*), ""); \
        __atomic_fetch_sub(PTR, VAL, MO); \
      })

#  define atomic_thread_fence(MO) \
      __atomic_thread_fence(MO)

#  undef VECS_ATOMIC_BUILTINS
#endif

#if defined(PLATFORM_IS_X86)
#  define VECS_CPU_RELAX() asm volatile("pause" ::: "memory")
#elif defined(PLATFORM_IS_WASM32)
#  define VECS_CPU_RELAX() error "On WASM, use __builtin_wasm_memory_atomic_* functions instead of VECS_CPU_RELAX";
#else
#  define VECS_CPU_RELAX()
#endif

#endif
