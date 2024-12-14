#include "../runtime/share.h"
#include "../runtime/panic.h"
#include "../runtime/object.h"

typedef struct foo_t {
  int x;
  int y;
} foo_t;

const vecs_type_t type_Foo = {
  .type = NULL, // TODO: type_Type
  .data_size = sizeof(foo_t),
};

int start(int x) {
  vecs_object_t* foo = vecs_share_alloc(&type_Foo);
  vecs_share_inc_ref(foo);
  vecs_share_dec_ref(foo);
  vecsint_share_reclaim_local_dead();
  vecsint_share_run_local_finalizers();

  vecs_panic("Goodbye, cruel world!");

  return x;
}
