#ifndef VECS_OBJECT_H
#define VECS_OBJECT_H

#include "platform.h"

typedef struct vecs_type_t {
  // Pointer to the type called Type. With this, we can treat types as objects.
  // That is, a vecs_type_t* can be cast to a vecs_object_t*.
  const struct vecs_type_t* type;

  // The size (bytes) to be allocated for data fields in each instance object.
  size_t data_size;

  // TODO: Add the rest of the type descriptor fields.
} vecs_type_t;

typedef struct vecs_object_t {
  const vecs_type_t* type;

  // Data fields come next, but those vary from object type to object type,
  // so we don't define them here.
} vecs_object_t;

// Run any finalizer(s) defined for the given object.
//
// This may include either or both of a compiler-generated finalizer
// (to call `vecs_share_dec_ref` and/or `vecs_object_fini` on all the data
// fields of the object that are themselves objects), and/or a user-defined
// finalizer function (to run custom FFI-related cleanup code).
void vecs_object_fini(const vecs_object_t* object);

// Return true if the given type needs to run finalizers.
//
// If this returns false, there is no need to run `vecs_object_fini` on any
// instances of the given type.
bool vecs_type_needs_fini(const vecs_type_t* type);

#endif
