#include "object.h"
#include "panic.h"

///
// Public API (see header for docs)

void vecs_object_fini(const vecs_object_t* object) {
  // TODO: Implement this function
  vecs_panic("Object finalizers are not yet implemented");
}

bool vecs_type_needs_fini(const vecs_type_t* type) {
  return false; // TODO: add finalizer function to type descriptor struct
}
