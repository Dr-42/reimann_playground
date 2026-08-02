#include "num_vec.h"

#include <stdlib.h>

#define DEFAULT_CAP 10

num_vec_t* num_vec_new(size_t cap) {
    if (cap == 0) {
        cap = DEFAULT_CAP;
    }

    num_vec_t* vec = malloc(sizeof(num_vec_t));
    vec->len = 0;
    vec->cap = cap;
    vec->data = malloc(sizeof(num_t) * cap);
    return vec;
}

num_t* num_vec_push(num_vec_t* vec, num_t val) {
    if (vec->len == vec->cap) {
        vec->cap *= 2;
        vec->data = realloc(vec->data, sizeof(num_t) * vec->cap);
    }
    vec->data[vec->len] = val;
    vec->len++;
    return &vec->data[vec->len - 1];
}

num_t num_vec_pop(num_vec_t* vec) {
    num_t val = vec->data[vec->len - 1];
    vec->len--;
    return val;
}

void num_vec_free(num_vec_t* vec) {
    free(vec->data);
    free(vec);
}

num_t num_vec_get(num_vec_t* vec, size_t index) {
    return vec->data[index];
}

void num_vec_set(num_vec_t* vec, size_t index, num_t val) {
    vec->data[index] = val;
}
