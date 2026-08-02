#include "special.h"
#include <math.h>

num_t lambda(num_t x) {
    num_t result = {.type = DOUBLE, .d = 0.0};

    int n;
    if (x.type == INT) {
        n = x.i;
    } else if (x.type == DOUBLE) {
        n = (int)(x.d + 0.5); // nearest integer
    } else {
        return result; // COMPLEX → 0
    }
    if (n <= 1) return result;

    int p = 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            p = i;
            break;
        }
    }
    if (p == 0) p = n;

    int temp = n;
    while (temp % p == 0)
        temp /= p;

    if (temp == 1) result.d = log((double)p);
    return result;
}

num_t sine(num_t x) {
    num_t result = {.type = DOUBLE, .d = 0.0};

    double val = 0.0;
    if (x.type == INT)
        val = (double)x.i;
    else if (x.type == DOUBLE)
        val = x.d;
    else
        return result; // COMPLEX not supported for sine yet

    result.d = sin(val);
    return result;
}

num_t cosine(num_t x) {
    num_t result = {.type = DOUBLE, .d = 0.0};
    double val = 0.0;
    if (x.type == INT)
        val = (double)x.i;
    else if (x.type == DOUBLE)
        val = x.d;
    else
        return result; // COMPLEX not supported for cosine yet
    result.d = cos(val);
    return result;
}
