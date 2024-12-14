#ifndef VECS_HARNESS_H
#define VECS_HARNESS_H

// For some sandboxed platforms, like WASM, we rely on the host environment
// to provide certain functions as a "harness" layer we can attach to.
//
// Therefore, the following functions are not implemented in our runtime code,
// but are instead declared here for coordination purposes, and are expected
// to be implemented by the host environment.

#if defined(PLATFORM_IS_WASM)
// Allocate a block of memory matching the given number of pages.
//
// The size of a "page" is platform specific:
// - On WASM, a page is 64 KiB.
//
// Returns NULL if the allocation failed.
void* vecs_harness_page_alloc(size_t page_count);
#endif

#if defined(PLATFORM_IS_WASM)
// Immediately terminate the current program with a panic message.
//
// This is the underlying implementation of `vecs_panic` on these platforms.
// See the documentation of that function for more details.
void vecs_harness_panic(const char* message, size_t message_length);
#endif



#endif
