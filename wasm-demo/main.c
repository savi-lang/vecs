#include "../runtime/pool.h"
#include "../runtime/panic.h"

int start(int x) {
  vecsint_pool_index(0);

  vecs_panic("Goodbye, cruel world!");

  return x;
}
