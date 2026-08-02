#include "prime.h"
#include "num.h"
#include <stdio.h>
#include <stdlib.h>

static num_vec_t known_primes = {0};

void prime_init(void) {
    if (known_primes.cap == 0) {
        known_primes.cap = 10;
        known_primes.data = malloc(sizeof(num_t) * known_primes.cap);
        known_primes.len = 0;
    }
}

void prime_free(void) {
    free(known_primes.data);
    known_primes.data = NULL;
    known_primes.cap = 0;
    known_primes.len = 0;
}

const num_vec_t* prime_list(void) {
    return &known_primes;
}

bool is_prime(int n) {
    if (n < 1) {
        fprintf(stderr, "Error: n must be greater than 0\n");
        return false;
    }
    if (n == 1) return false;

    if (known_primes.len > 0) {
        int largest_known = known_primes.data[known_primes.len - 1].i;

        if (n <= largest_known) {
            size_t left = 0;
            size_t right = known_primes.len - 1;
            while (left <= right) {
                size_t mid = left + (right - left) / 2;
                int p = known_primes.data[mid].i;
                if (p == n) return true;
                if (p < n) {
                    left = mid + 1;
                } else {
                    if (mid == 0) break;
                    right = mid - 1;
                }
            }
            return false;
        }
    }

    for (size_t i = 0; i < known_primes.len; i++) {
        int p = known_primes.data[i].i;
        if (p * p > n) return true;
        if (n % p == 0) return false;
    }

    int start = 2;
    if (known_primes.len > 0) {
        start = known_primes.data[known_primes.len - 1].i + 1;
    }
    for (int i = start; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

void sieve(int ceiling) {
    if (known_primes.cap == 0) {
        prime_init();
    }

    if (known_primes.len == 0) {
        if (ceiling >= 2) {
            num_t first = num_new_int(2);
            num_vec_push(&known_primes, first);
        } else {
            return;
        }
    }

    num_t largest = num_vec_get(&known_primes, known_primes.len - 1);
    if (ceiling <= largest.i) return;

    int start = largest.i + 1;
    for (int i = start; i <= ceiling; i++) {
        if (is_prime(i)) {
            num_t data = num_new_int(i);
            num_vec_push(&known_primes, data);
        }
    }
}
