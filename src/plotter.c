#include "plotter.h"
#include <locale.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define TERM_MAX_COLS 220

static double extract_double(num_t val) {
    if (val.type == INT) return (double)val.i;
    if (val.type == DOUBLE) return val.d;
    return sqrt(val.real * val.real + val.imag * val.imag);
}

static int braille_bit(int sub_col, int sub_row) {
    return (sub_col == 0) ? sub_row : sub_row + 4;
}

void plot_auto(num_t (*f)(num_t), int x_min, int x_max, int height) {
    setlocale(LC_CTYPE, "");

    int range = x_max - x_min + 1;
    int eff_width = (range <= TERM_MAX_COLS) ? range : TERM_MAX_COLS;

    /* ---------- sample ---------- */
    double* y_vals = malloc(eff_width * sizeof(double));
    int* x_vals = malloc(eff_width * sizeof(int));

    double step = (eff_width > 1 && range > 1) ? (double)(range - 1) / (double)(eff_width - 1) : 0.0;
    for (int col = 0; col < eff_width; col++) {
        int x = x_min + (int)(col * step + 0.5);
        num_t in = {.type = INT, .i = x}; // num_new_int would be cleaner
        num_t out = f(in);
        x_vals[col] = x;
        y_vals[col] = extract_double(out);
    }

    /* ---------- auto‑scale ---------- */
    double y_min = INFINITY;
    double y_max = -INFINITY;
    for (int i = 0; i < eff_width; i++) {
        if (!isnan(y_vals[i]) && !isinf(y_vals[i])) {
            if (y_vals[i] < y_min) y_min = y_vals[i];
            if (y_vals[i] > y_max) y_max = y_vals[i];
        }
    }
    if (y_min == INFINITY) {
        y_min = 0.0;
        y_max = 1.0;
    }
    double margin = (y_max - y_min) * 0.1;
    if (margin <= 0.0) margin = 1.0;
    y_min -= margin;
    y_max += margin;
    double y_range = y_max - y_min;
    if (y_range <= 0.0) y_range = 1.0;

    /* ---------- Braille canvas ---------- */
    int char_height = height;
    int char_width = (eff_width + 1) / 2;
    int pixel_height = char_height * 4;

    uint8_t** dots = malloc(char_height * sizeof(uint8_t*));
    for (int r = 0; r < char_height; r++)
        dots[r] = calloc(char_width, sizeof(uint8_t));

    for (int col = 0; col < eff_width; col++) {
        if (isnan(y_vals[col]) || isinf(y_vals[col])) continue;

        double t = (y_vals[col] - y_min) / y_range;
        int sub_row = (int)((1.0 - t) * (pixel_height - 1));
        if (sub_row < 0) sub_row = 0;
        if (sub_row >= pixel_height) sub_row = pixel_height - 1;

        int sub_col = col;
        int char_row = sub_row / 4;
        int char_col = sub_col / 2;
        int local_row = sub_row % 4;
        int local_col = sub_col % 2;

        if (char_row >= 0 && char_row < char_height && char_col >= 0 && char_col < char_width) {
            int bit = braille_bit(local_col, local_row);
            dots[char_row][char_col] |= (uint8_t)(1 << bit);
        }
    }

    /* ---------- print ---------- */
    printf("\n");
    for (int r = 0; r < char_height; r++) {
        /* label aligned to the *pixel* centre of each character row */
        double y = y_max - (y_range * (r * 4 + 2) / (pixel_height - 1));
        if (r == 0 || r == char_height - 1 || r % 5 == 0)
            printf("% 7.2f │", y);
        else
            printf("        │");

        for (int c = 0; c < char_width; c++)
            putwchar(dots[r][c] ? (wchar_t)(0x2800 + dots[r][c]) : L' ');
        putwchar(L'\n');
    }

    /* x‑axis line */
    printf("        └");
    for (int c = 0; c < char_width; c++)
        putwchar(L'─');
    putwchar(L'\n');

    printf("        x: [%d .. %d]  (%d samples, step ≈ %.1f)\n\n", x_min, x_max, eff_width,
           (eff_width > 1) ? (double)(range - 1) / (eff_width - 1) : 1.0);

    /* ---------- cleanup ---------- */
    for (int r = 0; r < char_height; r++)
        free(dots[r]);
    free(dots);
    free(y_vals);
    free(x_vals);
}
