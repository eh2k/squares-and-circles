
/*
 * Based on: https://medium.com/@martinmikkelsen/cubic-splines-in-the-c-language-4463fe9a3a83
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int N;    // number of intervals (points - 1)
    float *x; // x points
    float *y; // y points (a[i])
    float *b; // .
    float *c; // .
    float *d; // .
} cspline;

// Initialize spline with given (x, y) points
void cspline_init(cspline *s, const float *x, const float *y, int N, int stride)
{
    s->N = N;
    s->x = (float *)malloc(N * sizeof(float));
    s->y = (float *)malloc(N * sizeof(float));
    s->b = (float *)malloc(N * sizeof(float));
    s->c = (float *)malloc((N - 1) * sizeof(float));
    s->d = (float *)malloc((N - 1) * sizeof(float));

    for (int i = 0; i < N; ++i)
    {
        s->x[i] = x[i * stride];
        s->y[i] = y[i * stride];
    }
    int i;
    float dx[N - 1];
    float p[N - 1];
    for (i = 0; i < N - 1; ++i)
    {
        dx[i] = x[(i + 1) * stride] - x[i * stride];

        if (dx[i] < 0.1f)
            dx[i] = 0.1f;

        p[i] = (y[(i + 1) * stride] - y[i * stride]) / dx[i];
    }
    float D[N];
    float B[N];
    float Q[N - 1];
    D[0] = 2;
    D[N - 1] = 2;
    B[0] = 3 * p[0];
    B[N - 1] = 3 * p[N - 2];
    Q[0] = 1;

    for (i = 0; i < N - 2; ++i)
    {
        D[i + 1] = 2 * dx[i] / dx[i + 1] + 2;
        B[i + 1] = 3 * (p[i] + p[i + 1] * dx[i] / dx[i + 1]);
        Q[i + 1] = dx[i] / dx[i + 1];
    }
    for (i = 1; i < N; ++i)
    {
        D[i] -= Q[i - 1] / D[i - 1];
        B[i] -= B[i - 1] / D[i - 1];
    }
    s->b[N - 1] = B[N - 1] / D[N - 1];

    for (i = N - 2; i >= 0; --i)
    {
        s->b[i] = (B[i] - Q[i] * s->b[i + 1]) / D[i];
    }

    for (i = 0; i < N - 1; ++i)
    {
        s->c[i] = (-2 * s->b[i] - s->b[i + 1] + 3 * p[i]) / dx[i];
        s->d[i] = (s->b[i] + s->b[i + 1] - 2 * p[i]) / dx[i] / dx[i];
    }
}

float cspline_eval(const cspline *s, float x)
{
    int i;

    if (x <= s->x[0])
    {
        i = 0;
    }
    else if (x >= s->x[s->N - 1])
    {
        i = s->N - 1;
    }
    else
    {
        int low = 0, high = s->N - 1;
        while (low < high - 1)
        {
            int mid = (low + high) / 2;
            if (x < s->x[mid])
            {
                high = mid;
            }
            else
            {
                low = mid;
            }
        }
        i = low;
    }

    auto dx = x - s->x[i];
    auto y_new = s->y[i] + s->b[i] * dx + s->c[i] * powf(dx, 2) + s->d[i] * powf(dx, 3);
    return y_new;
}

void cspline_free(cspline *spline)
{
    free(spline->x);
    free(spline->y);
    free(spline->b);
    free(spline->c);
    free(spline->d);
}
