#ifndef GPLOTTER_H
#define GPLOTTER_H
#include "num.h"

#include "num.h"

typedef num_t (*plot_func_t)(num_t x);

typedef struct {
    plot_func_t f;
    unsigned char r, g, b;
} plot_trace_t;

/* multi‑trace JPEG – all traces share the same auto‑scaled axes */
void plot_jpg(plot_trace_t* traces, int n_traces, int x_min, int x_max, int img_width, int img_height,
              const char* filename, int quality);

/* convenience single‑trace (blue) */
void plot_jpg1(plot_func_t f, int x_min, int x_max, int img_width, int img_height, const char* filename, int quality);

/* single‑trace PPM (kept for quick viewing) */
void plot_ppm(plot_func_t f, int x_min, int x_max, int img_width, int img_height, const char* filename);

#endif // GPLOTTER_H
