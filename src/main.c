#include "num_vec.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

num_vec_t known_primes = {0};

// Checks if a number is prime using the known_primes cache
bool is_prime(int n) {
    if (n < 1) {
        fprintf(stderr, "Error: n must be greater than 0\n");
        return false;
    }

    if (n == 1) {
        return false;
    }

    if (known_primes.len > 0) {
        int largest_known = known_primes.data[known_primes.len - 1].i;

        // Fast-path: binary search when n is within the cached range
        if (n <= largest_known) {
            size_t left = 0;
            size_t right = known_primes.len - 1;
            while (left <= right) {
                size_t mid = left + (right - left) / 2;
                int p = known_primes.data[mid].i;

                if (p == n) {
                    return true;
                }

                if (p < n) {
                    left = mid + 1;
                } else {
                    if (mid == 0) break; // prevent underflow
                    right = mid - 1;
                }
            }
            return false;
        }
    }

    // Trial division using cached primes up to sqrt(n)
    for (size_t i = 0; i < known_primes.len; i++) {
        int p = known_primes.data[i].i;

        if (p * p > n) {
            return true; // no factor ≤ sqrt(n) → n is prime
        }

        if (n % p == 0) {
            return false;
        }
    }

    // Fallback: continue from the number after the last known prime
    int start = 2;
    if (known_primes.len > 0) {
        start = known_primes.data[known_primes.len - 1].i + 1;
    }

    for (int i = start; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

// Builds a list of primes up to ceiling (trial‑division, not a real Eratosthenes sieve)
void sieve(num_t ceiling, num_vec_t* primes) {
    if (ceiling.type != INT) {
        fprintf(stderr, "Error: ceiling must be an integer\n");
        return;
    }

    // Ensure the vector has a valid buffer
    if (primes->cap == 0) {
        primes->cap = 10;
        primes->data = malloc(sizeof(num_t) * primes->cap);
        primes->len = 0;
    }

    if (primes->len == 0) {
        if (ceiling.i >= 2) {
            num_t first_prime;
            first_prime.type = INT;
            first_prime.i = 2;
            num_vec_push(primes, first_prime);
        } else {
            return;
        }
    }

    num_t largest_prime = num_vec_get(primes, primes->len - 1);
    if (ceiling.i <= largest_prime.i) {
        return;
    }

    int start = largest_prime.i + 1;
    for (int i = start; i <= ceiling.i; i++) {
        if (is_prime(i)) {
            num_t data;
            data.type = INT;
            data.i = i;
            num_vec_push(primes, data);
        }
    }
}

// von Mangoldt function Λ(n)
num_t lambda(num_t x) {
    num_t result;
    result.type = DOUBLE;
    result.d = 0.0;

    if (x.type != INT || x.i <= 1) {
        return result;
    }

    int n = x.i;
    int p = 0;

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            p = i;
            break;
        }
    }

    if (p == 0) {
        p = n; // n itself is prime
    }

    int temp = n;
    while (temp % p == 0) {
        temp /= p;
    }

    if (temp == 1) { // n = p^k
        result.d = log((double)p);
    }

    return result;
}

int main(void) {
    // Initialize the global prime cache
    known_primes.cap = 10;
    known_primes.data = malloc(sizeof(num_t) * known_primes.cap);
    known_primes.len = 0;

    num_t ceil;
    ceil.type = INT;
    ceil.i = 50;

    sieve(ceil, &known_primes);

    printf("Primes up to %d:\n", ceil.i);
    for (size_t i = 0; i < known_primes.len; i++) {
        printf("%d ", known_primes.data[i].i);
    }
    printf("\n");

    num_t test_val;
    test_val.type = INT;
    test_val.i = 8;

    num_t test_val1;
    test_val1.type = INT;
    test_val1.i = 10;

    num_t l_res = lambda(test_val);
    num_t l_res1 = lambda(test_val1);

    printf("lambda(%d) = %f\n", test_val.i, l_res.d);
    printf("lambda(%d) = %f\n", test_val1.i, l_res1.d);

    free(known_primes.data);

    return 0;
}
