// aanl_test.cpp: checks aanl.h in double: F1' = f and F2' = x f(x) everywhere (central differences), and each
// saturator's output for a slow input equals f of the input half a sample (first-order) or one sample (second-order)
// earlier. g++ -O2 -std=c++17 lib/aanl/aanl_test.cpp -o aanl_test && ./aanl_test
#include <stdio.h>
#include <math.h>
#define AANL_DOUBLE
#include "aanl.h"
using namespace aanl;

static int fails = 0;
template <class F, class G>
void check1(const char *name, F f, G F1)
{
    double worst = 0, wx = 0;
    for (double x = -6; x <= 6; x += 0.00137)
    {
        const double h = 1e-6, d = (F1(x + h) - F1(x - h)) / (2 * h);
        const double e = fabs(d - f(x));
        if (e > worst)
            worst = e, wx = x;
    }
    const bool ok = worst < 1e-6;
    fails += !ok;
    printf("%-22s F1' vs f   max err %.2e at x=%.3f %s\n", name, worst, wx, ok ? "ok" : "FAIL");
}
template <class F, class H>
void check2(const char *name, F f, H F2)
{
    double worst = 0, wx = 0;
    for (double x = -6; x <= 6; x += 0.00137)
    {
        const double h = 1e-6, d = (F2(x + h) - F2(x - h)) / (2 * h);
        const double e = fabs(d - x * f(x));
        if (e > worst)
            worst = e, wx = x;
    }
    const bool ok = worst < 1e-5;
    fails += !ok;
    printf("%-22s F2' vs x f max err %.2e at x=%.3f %s\n", name, worst, wx, ok ? "ok" : "FAIL");
}
template <class S, class F>
void checkSlow(const char *name, S s, F f, double delay)
{
    // a slow sine through the saturator against f of the input delayed by `delay` samples
    double worst = 0;
    for (int n = 0; n < 20000; n++)
    {
        const double w = 2 * M_PI * 20 / 96000, x = 4 * sin(w * n), y = s(x);
        const double ref = f(4 * sin(w * (n - delay)));
        if (n > 100 && fabs(y - ref) > worst)
            worst = fabs(y - ref);
    }
    const bool ok = worst < 1e-3;
    fails += !ok;
    printf("%-22s slow sine vs f(delayed) max err %.2e %s\n", name, worst, ok ? "ok" : "FAIL");
}

int main()
{
    check1("hardclip", hardclip::f, hardclip::F1);
    check2("hardclip2", hardclip::f, hardclip2::F2);
    check1("cubic1", cubic1::f, cubic1::F1);
    check1("parabolic", parabolic::f, parabolic::F1);
    check2("parabolic2", parabolic::f, parabolic2::F2);
    check1("hyperbolic", hyperbolic::f, hyperbolic::F1);
    check2("hyperbolic2", hyperbolic::f, hyperbolic2::F2);
    check1("sinarctan", sinarctan::f, sinarctan::F1);
    check2("sinarctan2", sinarctan::f, sinarctan2::F2);
    check1("softclipQuadratic1", softclipQuadratic1::f, softclipQuadratic1::F1);
    check2("softclipQuadratic2", softclipQuadratic1::f, softclipQuadratic2::F2);
    check1("tanh1", tanh1::f, tanh1::F1);
    check1("arctan", arctan::f, arctan::F1);
    check2("arctan2", arctan::f, arctan2::F2);
    check1("asinh1", asinh1::f, asinh1::F1);
    check2("asinh2", asinh1::f, asinh2::F2);
    auto sat = [](double x) { return x / sqrt(1 + x * x); };
    checkSlow("hardclip", hardclip(), hardclip::f, 0.5);
    checkSlow("hardclip2", hardclip2(), hardclip::f, 1.0);
    checkSlow("cubic1", cubic1(), cubic1::f, 0.5);
    checkSlow("parabolic2", parabolic2(), parabolic::f, 1.0);
    checkSlow("hyperbolic2", hyperbolic2(), hyperbolic::f, 1.0);
    checkSlow("sinarctan2", sinarctan2(), sinarctan::f, 1.0);
    checkSlow("sinarctanStable", sinarctanStable(), sat, 0.5);
    checkSlow("softclipQuadratic2", softclipQuadratic2(), softclipQuadratic1::f, 1.0);
    checkSlow("tanh1", tanh1(), tanh1::f, 0.5);
    checkSlow("arctan2", arctan2(), arctan::f, 1.0);
    checkSlow("asinh2", asinh2(), asinh1::f, 1.0);
    // the transparent one: an allpass in the linear part, about 0.71 samples late at low frequencies
    checkSlow("sinarctanTransparent", sinarctanTransparent(), sat, 0.7071);
    // tanh1 far out: the quotient must stay at +-1 (aanl.lib's Rcosh would give 0 beyond |x| = 89 in float)
    tanh1 t;
    t(100);
    const double y = t(101);
    printf("tanh1 at x = 100..101: %.6f %s\n", y, fabs(y - 1) < 1e-9 ? "ok" : "FAIL");
    fails += fabs(y - 1) >= 1e-9;
    printf("%d failures\n", fails);
    return fails != 0;
}
