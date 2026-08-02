#include "num_vec.h"
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define MAX_PRIMES 100000000000000

num_vec_t known_primes = {0};

// Checks if a number is prime
bool is_prime(int n) {
    // If n is less than 1 something is wrong
    if (n < 1) {
        fprintf(stderr, "Error: n must be greater than 0\n");
        return false;
    }

    // todo

    return true;
}

// Sieve of Eratosthenes
void sieve(num_t ceiling, num_vec_t primes) {
    if (ceiling.type != INT) {
        fprintf(stderr, "Error: ceiling must be an integer\n");
        return;
    }
    num_t largest_prime = num_vec_get(&primes, primes.len - 1);
    if (ceiling.i < largest_prime.i) {
        return;
    }

    // Start seive after largest prime
    int start = largest_prime.i + 1;
    for (int i = start; i <= ceiling.i; i++) {
        // If i is prime, add it to the list
        if (is_prime(i)) {
            num_t data = {INT, {i}};
            num_vec_push(&primes, data);
        }
    }
}

// Mangoldt Lambda Function
num_t lambda(num_t x) {
    // lambda(n) = ln(p) if n == p^k (p, k belongs to I) else 0
}

int main() {
    printf("Hello World!\n");
    return 0;
}
