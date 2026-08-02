#include "special.h"

#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "num.h"
#include "reimann_zeros.h"

int spectral_N = 1000;
int hardy_M = 20000;

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

// Hardy Z-function: the 1D adapter for the rotating vector zeta(1/2 + i t).
// Input  x.d = t  (the imaginary part; the real part is fixed at 1/2).
// Output Z(t), a real scalar whose zero-crossings are the Riemann zeros.
//
// Pipeline:
//   1. zeta(1/2+it) = eta(1/2+it) / (1 - 2^{1-(1/2+it)})   [Chapter 1 bridge]
//   2. multiply by e^{i theta(t)} to cancel the background spin
//   3. read the real part  ->  a 1D wave your renderer can draw

num_t hardy_z(num_t x) {
    num_t result = num_new_double(0.0);
    double t = 0.0;
    if (x.type == INT)
        t = (double)x.i;
    else if (x.type == DOUBLE)
        t = x.d;
    else
        return result; // COMPLEX not supported

    // theta asymptotic is meaningless at t ~ 0; the first zero is at 14.13, so just blank the low end.
    if (t < 1.0) {
        result.d = NAN; // render() skips NaN -> clean line break, no spurious segment to the axis
        return result;
    }

    // --- 1a. eta(1/2 + i t) by direct alternating sum ---
    // term_n = (-1)^{n-1} n^{-(1/2+it)} = (-1)^{n-1} n^{-1/2} (cos(t ln n) - i sin(t ln n))
    double eta_re = 0.0, eta_im = 0.0;
    double sgn = 1.0;
    for (int n = 1; n <= hardy_M; n++) {
        double inv = 1.0 / sqrt((double)n);
        double tl = t * log((double)n);
        eta_re += sgn * inv * cos(tl);
        eta_im += sgn * inv * (-sin(tl));
        sgn = -sgn;
    }

    // --- 1b. denominator 1 - 2^{1-s} with s = 1/2 + it  =>  1 - sqrt(2)(cos(t ln2) - i sin(t ln2)) ---
    double ln2 = log(2.0);
    double sq2 = sqrt(2.0);
    double c2 = cos(t * ln2);
    double s2 = sin(t * ln2);
    double den_re = 1.0 - sq2 * c2;
    double den_im = sq2 * s2; // note the sign: -( -i sin ) = +i sin
    double den2 = den_re * den_re + den_im * den_im;

    // --- 1c. complex divide  z = eta / den  =  zeta(1/2 + it) ---
    double z_re = (eta_re * den_re + eta_im * den_im) / den2;
    double z_im = (eta_im * den_re - eta_re * den_im) / den2;

    // --- 2. the camera rotation theta(t); asymptotic, error O(1/t^3), exact enough for t >= ~5 ---
    double theta = 0.5 * t * log(t / (2.0 * M_PI)) - 0.5 * t - M_PI / 8.0 + 1.0 / (48.0 * t);

    // --- 3. project onto the real axis: Z = Re( e^{i theta} z ) ---
    result.d = cos(theta) * z_re - sin(theta) * z_im;
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
