#ifndef NUM_H
#define NUM_H

typedef enum num_type_t {
    INT,
    DOUBLE,
    COMPLEX,
} num_type_t;

typedef struct num_t {
    num_type_t type;
    union {
        int i;
        double d;
        struct {
            double real;
            double imag;
        };
    };
} num_t;

num_t num_new_int(int i);
num_t num_new_double(double d);
num_t num_new_complex(double real, double imag);

#endif // NUM_H
