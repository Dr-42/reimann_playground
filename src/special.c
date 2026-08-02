#include "special.h"
#include <math.h>

num_t lambda(num_t x) {
    num_t result = num_new_double(0.0);

    if (x.type != INT || x.i <= 1) return result;

    int n = x.i;
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
