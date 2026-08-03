#ifndef GPLOTTER_H
#define GPLOTTER_H
#include "num.h"

#include "num.h"

typedef num_t (*plot_func_t)(num_t x);

typedef struct {
    plot_func_t f;
    unsigned char r, g, b;
} plot_trace_t;

typedef struct {
    int x_min, x_max;
    int img_width, img_height;
    char* filename;
    int quality;
} plot_settings_t;

/* multi‑trace JPEG – all traces share the same auto‑scaled axes */
void plot_jpg(plot_trace_t* traces, int num_traces, plot_settings_t* settings);

/* convenience single‑trace (blue) */
void plot_jpg1(plot_func_t f, plot_settings_t* settings);

/* single‑trace PPM (kept for quick viewing) */
void plot_ppm(plot_func_t f, int num_traces, plot_settings_t* settings);

#endif // GPLOTTER_H
