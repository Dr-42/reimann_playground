#ifndef PLOTTER_H
#define PLOTTER_H

#include "num.h"

typedef num_t (*plot_func_t)(num_t x);

void plot_auto(num_t (*f)(num_t), int x_min, int x_max, int height);

#endif // PLOTTER_H
