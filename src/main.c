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
        {hardy_z, 60, 220, 60},       // purple
    };
    plot_jpg(traces, 4, -2, 100, 1024, 1080, "overlay.jpg", 90);
    return 0;
}
