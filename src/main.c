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

    num_t test_val = num_new_int(8);
    num_t test_val2 = num_new_int(10);
    num_t l_res = lambda(test_val);
    printf("lambda(%d) = %f\n", test_val.i, l_res.d);
    num_t l_res2 = lambda(test_val2);

    prime_free();
    return 0;
}
