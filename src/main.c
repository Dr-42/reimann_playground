#include "special.h"
#include "gplotter.h"

int main(void) {
    /* two traces on the same axes */
    plot_trace_t traces[] = {
        {lambda, 50, 100, 220}, // blue
        // {sine, 220, 60, 60},    // red
        // {cosine, 60, 220, 60},  // green
        {psi, 220, 60, 60},      // red
        {identity, 60, 220, 60}, // green
    };
    plot_jpg(traces, 3, -10, 1000, 900, 500, "overlay.jpg", 90);
    return 0;
}
