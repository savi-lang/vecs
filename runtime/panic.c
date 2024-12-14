#include "panic.h"
#include "platform.h"
#include "harness.h"

#ifdef PLATFORM_IS_WASM
void vecs_panic(const char* message) {
  // The strlen function is not available on wasm, so we do it manually here.
  int length = 0;
  while (message[length] != '\0') length++;

  vecs_harness_panic(message, length);
}
#else
// On other platforms, we can just use the standard library to print the error
// and abort the program directly.
#include <stdio.h>
void vecs_panic(const char* message) {
  perror(message);
  abort();
}
#endif
