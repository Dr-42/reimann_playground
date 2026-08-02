#include "num.h"

num_t num_new_int(int i) {
    num_t result = {.type = INT, .i = i};
    return result;
}

num_t num_new_double(double d) {
    num_t result = {.type = DOUBLE, .d = d};
    return result;
}

num_t num_new_complex(double real, double imag) {
    num_t result = {.type = COMPLEX, .real = real, .imag = imag};
    return result;
}
