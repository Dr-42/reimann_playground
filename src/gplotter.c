#include "gplotter.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

/* ---------- image buffer ---------- */
typedef struct {
    uint8_t* data;
    int w, h;
} Image;

static void img_set(Image* im, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= im->w || y < 0 || y >= im->h) return;
    int i = (y * im->w + x) * 3;
    im->data[i + 0] = r;
    im->data[i + 1] = g;
    im->data[i + 2] = b;
}

static Image* img_new(int w, int h) {
    Image* im = malloc(sizeof(Image));
    im->w = w;
    im->h = h;
    im->data = malloc(w * h * 3);
    memset(im->data, 255, w * h * 3);
    return im;
}

static void img_free(Image* im) {
    free(im->data);
    free(im);
}

/* ---------- 3×5 bitmap font ---------- */
#define FONT_W 3
#define FONT_H 5
#define FONT_GAP 1

static const uint8_t font_data[128][5] = {
    ['0'] = {0x07, 0x05, 0x05, 0x05, 0x07}, ['1'] = {0x02, 0x06, 0x02, 0x02, 0x07},
    ['2'] = {0x07, 0x01, 0x07, 0x04, 0x07}, ['3'] = {0x07, 0x01, 0x07, 0x01, 0x07},
    ['4'] = {0x05, 0x05, 0x07, 0x01, 0x01}, ['5'] = {0x07, 0x04, 0x07, 0x01, 0x07},
    ['6'] = {0x07, 0x04, 0x07, 0x05, 0x07}, ['7'] = {0x07, 0x01, 0x01, 0x01, 0x01},
    ['8'] = {0x07, 0x05, 0x07, 0x05, 0x07}, ['9'] = {0x07, 0x05, 0x07, 0x01, 0x07},
    ['-'] = {0x00, 0x00, 0x07, 0x00, 0x00}, ['.'] = {0x00, 0x00, 0x00, 0x00, 0x02},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00}, ['e'] = {0x03, 0x05, 0x07, 0x04, 0x03},
    ['+'] = {0x00, 0x02, 0x07, 0x02, 0x00},
};

static void img_char(Image* im, int x, int y, char c, uint8_t r, uint8_t g, uint8_t b) {
    if (c < 0 || c > 127) c = ' ';
    const uint8_t* glyph = font_data[(int)c];
    for (int row = 0; row < FONT_H; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_W; col++) {
            if (bits & (1 << col)) img_set(im, x + col, y + row, r, g, b);
        }
    }
}

static void img_text(Image* im, int x, int y, const char* s, uint8_t r, uint8_t g, uint8_t b) {
    int cx = x;
    for (const char* p = s; *p; p++) {
        img_char(im, cx, y, *p, r, g, b);
        cx += FONT_W + FONT_GAP;
    }
}

static int text_width(const char* s) {
    int len = (int)strlen(s);
    return len * (FONT_W + FONT_GAP) - (len > 0 ? FONT_GAP : 0);
}

/* ---------- Bresenham line ---------- */
static void img_line(Image* im, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (1) {
        img_set(im, x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/* ---------- nice tick step ---------- */
static double nice_step(double range, int max_ticks) {
    if (range <= 0) return 1;
    double exp = pow(10, floor(log10(range)));
    double residual = range / (exp * max_ticks) * 10; // scale up
    if (residual <= 1.5) return exp * 0.1;
    if (residual <= 3.5) return exp * 0.2;
    if (residual <= 7.5) return exp * 0.5;
    return exp;
}

static double first_tick(double vmin, double step) {
    return ceil(vmin / step) * step;
}

/* ---------- value → pixel (equal‑aspect mapping) ---------- */
typedef struct {
    double x_c, y_c; // data centre
    double ppu;      // pixels per data unit (same for x and y)
    int px0, py0;    // plot area pixel origin (top‑left of plot area)
    int pw, ph;      // plot area pixel dimensions
} Mapping;

static Mapping compute_mapping(Image* im, double x_min, double x_max, double y_min, double y_max, int ml, int mr,
                               int mt, int mb) {
    Mapping m;
    m.pw = im->w - ml - mr;
    m.ph = im->h - mt - mb;
    double xr = x_max - x_min;
    double yr = y_max - y_min;
    if (xr <= 0) xr = 1;
    if (yr <= 0) yr = 1;

    double ppu_x = m.pw / xr;
    double ppu_y = m.ph / yr;
    m.ppu = ppu_x < ppu_y ? ppu_x : ppu_y; // pick smaller → fits both

    m.x_c = (x_min + x_max) / 2.0;
    m.y_c = (y_min + y_max) / 2.0;
    m.px0 = ml + m.pw / 2;
    m.py0 = mt + m.ph / 2;
    return m;
}

static int map_x(Mapping* m, double x) {
    return (int)(m->px0 + (x - m->x_c) * m->ppu + 0.5);
}
static int map_y(Mapping* m, double y) {
    return (int)(m->py0 - (y - m->y_c) * m->ppu + 0.5);
}

/* ---------- workhorse ---------- */
static Image* render(plot_trace_t* traces, int n_traces, int x_min, int x_max, int img_w, int img_h) {
    /* --- margins --- */
    int ML = 70, MR = 20, MT = 18, MB = 40;

/* --- sample all traces into a single array for auto‑scale --- */
#define MAX_SAMPLES 2048
    int nsamp = img_w;
    if (nsamp > MAX_SAMPLES) nsamp = MAX_SAMPLES;
    if (nsamp < 100) nsamp = 100;

    double** y_all = malloc(n_traces * sizeof(double*));
    double* x_val = malloc(nsamp * sizeof(double));
    for (int t = 0; t < n_traces; t++)
        y_all[t] = malloc(nsamp * sizeof(double));

    double step = (nsamp > 1) ? (double)(x_max - x_min) / (nsamp - 1) : 0.0;
    for (int i = 0; i < nsamp; i++) {
        double xv = x_min + i * step;
        x_val[i] = xv;
        num_t in = {.type = DOUBLE, .d = xv};
        for (int t = 0; t < n_traces; t++) {
            num_t out = traces[t].f(in);
            double yd = 0;
            if (out.type == INT)
                yd = (double)out.i;
            else if (out.type == DOUBLE)
                yd = out.d;
            else
                yd = sqrt(out.real * out.real + out.imag * out.imag);
            y_all[t][i] = yd;
        }
    }

    /* --- global y range --- */
    double gy_min = INFINITY, gy_max = -INFINITY;
    for (int t = 0; t < n_traces; t++) {
        for (int i = 0; i < nsamp; i++) {
            double v = y_all[t][i];
            if (!isnan(v) && !isinf(v)) {
                if (v < gy_min) gy_min = v;
                if (v > gy_max) gy_max = v;
            }
        }
    }
    if (gy_min == INFINITY) {
        gy_min = -1;
        gy_max = 1;
    }
    double mrg = (gy_max - gy_min) * 0.1;
    if (mrg <= 0) mrg = 1;
    gy_min -= mrg;
    gy_max += mrg;

    /* --- mapping --- */
    Image* im = img_new(img_w, img_h);
    Mapping map = compute_mapping(im, x_min, x_max, gy_min, gy_max, ML, MR, MT, MB);

    /* --- grid lines (light grey dashed) --- */
    double xs = nice_step(x_max - x_min, 10);
    double ys = nice_step(gy_max - gy_min, 8);
    for (double xg = first_tick(x_min, xs); xg <= x_max + xs * 0.1; xg += xs) {
        int px = map_x(&map, xg);
        for (int py = MT; py < im->h - MB; py += 4)
            img_set(im, px, py, 225, 225, 225);
    }
    for (double yg = first_tick(gy_min, ys); yg <= gy_max + ys * 0.1; yg += ys) {
        int py = map_y(&map, yg);
        for (int px = ML; px < im->w - MR; px += 4)
            img_set(im, px, py, 225, 225, 225);
    }

    /* --- axis lines --- */
    if (0 >= x_min && 0 <= x_max) {
        int ax = map_x(&map, 0);
        img_line(im, ax, MT, ax, im->h - MB, 120, 120, 120);
    }
    if (0 >= gy_min && 0 <= gy_max) {
        int ay = map_y(&map, 0);
        img_line(im, ML, ay, im->w - MR, ay, 120, 120, 120);
    }

    /* --- tick marks & labels --- */
    for (double xg = first_tick(x_min, xs); xg <= x_max + xs * 0.1; xg += xs) {
        int px = map_x(&map, xg);
        int ay = (0 >= gy_min && 0 <= gy_max) ? map_y(&map, 0) : im->h - MB;
        img_line(im, px, ay - 3, px, ay + 3, 80, 80, 80);
        char buf[32];
        snprintf(buf, sizeof(buf), "%.4g", xg);
        int tw = text_width(buf);
        img_text(im, px - tw / 2, im->h - MB + 8, buf, 60, 60, 60);
    }
    for (double yg = first_tick(gy_min, ys); yg <= gy_max + ys * 0.1; yg += ys) {
        int py = map_y(&map, yg);
        int ax = (0 >= x_min && 0 <= x_max) ? map_x(&map, 0) : ML;
        img_line(im, ax - 3, py, ax + 3, py, 80, 80, 80);
        char buf[32];
        snprintf(buf, sizeof(buf), "%.4g", yg);
        int tw = text_width(buf);
        img_text(im, ML - tw - 8, py - FONT_H / 2, buf, 60, 60, 60);
    }

    /* --- plot traces --- */
    for (int t = 0; t < n_traces; t++) {
        uint8_t rr = traces[t].r, gg = traces[t].g, bb = traces[t].b;
        int prev_px = -1, prev_py = -1, prev_ok = 0;
        for (int i = 0; i < nsamp; i++) {
            double v = y_all[t][i];
            if (isnan(v) || isinf(v)) {
                prev_ok = 0;
                continue;
            }
            int px = map_x(&map, x_val[i]);
            int py = map_y(&map, v);
            /* marker */
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                    img_set(im, px + dx, py + dy, rr, gg, bb);
            /* line segment */
            if (prev_ok && abs(px - prev_px) <= 25) {
                img_line(im, prev_px, prev_py, px, py, rr, gg, bb);
            }
            prev_px = px;
            prev_py = py;
            prev_ok = 1;
        }
    }

    /* --- cleanup --- */
    for (int t = 0; t < n_traces; t++)
        free(y_all[t]);
    free(y_all);
    free(x_val);
    return im;
}

/* ---------- public API ---------- */

void plot_jpg(plot_trace_t* traces, int n_traces, int x_min, int x_max, int img_w, int img_h, const char* filename,
              int quality) {
    Image* im = render(traces, n_traces, x_min, x_max, img_w, img_h);
    if (!im) return;
    if (!stbi_write_jpg(filename, img_w, img_h, 3, im->data, quality))
        fprintf(stderr, "plot_jpg: failed to write %s\n", filename);
    else
        printf("plot_jpg: wrote %s (%d×%d q=%d)\n", filename, img_w, img_h, quality);
    img_free(im);
}

void plot_jpg1(plot_func_t f, int x_min, int x_max, int img_w, int img_h, const char* filename, int quality) {
    plot_trace_t tr = {f, 50, 100, 220};
    plot_jpg(&tr, 1, x_min, x_max, img_w, img_h, filename, quality);
}

void plot_ppm(plot_func_t f, int x_min, int x_max, int img_w, int img_h, const char* filename) {
    plot_trace_t tr = {f, 50, 100, 220};
    Image* im = render(&tr, 1, x_min, x_max, img_w, img_h);
    if (!im) return;
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror(filename);
        img_free(im);
        return;
    }
    fprintf(fp, "P6\n%d %d\n255\n", img_w, img_h);
    fwrite(im->data, 1, img_w * img_h * 3, fp);
    fclose(fp);
    printf("plot_ppm: wrote %s (%d×%d)\n", filename, img_w, img_h);
    img_free(im);
}
