#include "special.h"
#include "gplotter.h"

int main(void) {
    /* two traces on the same axes */
    plot_trace_t traces[] = {
        // {sine, 220, 60, 60},    // red
        // {cosine, 60, 220, 60},  // green
        {spectral_psi, 50, 100, 220}, // blue
        {psi, 220, 60, 60},           // red
        {identity, 60, 220, 60},      // green
        {hardy_z, 120, 20, 180},      // purple
    };
    int scalar = 5;

    plot_settings_t settings = {
        .x_min = -2,
        .x_max = 300,
        .img_width = 1024 * scalar,
        .img_height = 1024 * scalar,
        .filename = "overlay.jpg",
        .quality = 90,
    };

    plot_jpg(traces, 4, &settings);
    return 0;
}
