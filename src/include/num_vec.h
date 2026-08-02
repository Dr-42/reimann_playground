#ifndef NUM_VEC_H
#define NUM_VEC_H

#include <stddef.h>

#include "num.h"

typedef struct num_vec_t {
    size_t len;
    size_t cap;
    num_t* data;
} num_vec_t;

num_vec_t* num_vec_new(size_t cap);
num_t* num_vec_push(num_vec_t* vec, num_t val);
num_t num_vec_pop(num_vec_t* vec);
void num_vec_free(num_vec_t* vec);
num_t num_vec_get(num_vec_t* vec, size_t index);
void num_vec_set(num_vec_t* vec, size_t index, num_t val);

#endif // NUM_VEC_H
