#include "prime.h"
#include "special.h"
#include <stdio.h>

int main(void) {
    prime_init();
    sieve(50);

    printf("Primes up to 50:\n");
    const num_vec_t* primes = prime_list();
    for (size_t i = 0; i < primes->len; i++) {
        printf("%d ", primes->data[i].i);
    }
    printf("\n");

    num_t test_val = { .type = INT, .i = 8 };
    num_t l_res = lambda(test_val);
    printf("lambda(%d) = %f\n", test_val.i, l_res.d);

    prime_free();
    return 0;
}
