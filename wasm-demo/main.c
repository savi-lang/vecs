#import "../runtime/pool.h"

void vecsint_harness_panic(const char* message, int message_length);

void vecs_panic(const char* message) {
  // The strlen function is not available on wasm, so we do it manually here.
  int length = 0;
  while (message[length] != '\0') length++;

  vecsint_harness_panic(message, length);
}

int start(int x) {
  vecsint_pool_index(0);

  vecs_panic("Goodbye, cruel world!");

  return x;
}
