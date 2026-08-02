#include "special.h"

#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "num.h"
#include "reimann_zeros.h"

int spectral_N = 1000;

// Identity function
num_t identity(num_t x) {
    num_t result = num_new_double(0.0);
    if (x.type == INT)
        result.d = (double)x.i;
    else if (x.type == DOUBLE)
        result.d = x.d;
    else
        result.d = sqrt(x.real * x.real + x.imag * x.imag);
    return result;
}

// Chebyshev function
num_t psi(num_t x) {
    num_t result = num_new_double(0.0);
    double val = 0.0;
    if (x.type == INT)
        val = (double)x.i;
    else if (x.type == DOUBLE)
        val = x.d;
    else
        return result; // COMPLEX not supported for psi yet

    for (int i = 1; i <= val; i++) {
        num_t input = {.type = INT, .i = i};
        result.d += lambda(input).d;
    }
    return result;
}

// Spectral function
num_t spectral_psi(num_t x) {
    num_t result = num_new_double(0.0);

    num_t val = num_new_double(0.0);
    if (x.type == INT) {
        fprintf(stderr, "Error: Spectral Psi not supported for integers yet");
    } else if (x.type == DOUBLE) {
        val.d = x.d;
    } else {
        return result; // COMPLEX not supported for spectral_psi yet
    }

    if (x.d < 2.0) return result;
    double sum = 0.0;
    double sx = sqrt(x.d);
    double lx = log(x.d);
    for (int n = 0; n < spectral_N; n++) {
        double t = reimann_zeros[n];
        double tl = t * lx;
        sum += sx * (cos(tl) + 2.0 * t * sin(tl)) / (t * t + 0.25);
    }
    double trivial = -0.5 * log(1.0 - 1.0 / (x.d * x.d)); // tiny for x>3, keep or drop
    result.d = x.d - log(2.0 * M_PI) - sum + trivial;     // = the explicit formula
    return result;
}

num_t lambda(num_t x) {
    num_t result = num_new_double(0.0);

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
    num_t result = num_new_double(0.0);

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
    num_t result = num_new_double(0.0);
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
