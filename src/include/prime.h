#ifndef PRIME_H
#define PRIME_H

#include "num_vec.h"
#include <stdbool.h>

void prime_init(void);
void prime_free(void);
bool is_prime(int n);
void sieve(int ceiling);
const num_vec_t* prime_list(void);

#endif // PRIME_H
