#include "bbd_filter.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

#ifdef M_PI
cdouble BBD_Filter_Spec::transfer(cdouble::value_type frequency) const noexcept
{
    cdouble j(0.0, 1.0);
    cdouble s = j * (cdouble::value_type)(2.0 * M_PI) * frequency;
    cdouble h = 0.0;
    for (unsigned i = 0; i < M; ++i)
        h += R[i] / (s - P[i]);
    return h;
}
#endif

template <class T>
static void interpolate_row(cdouble::value_type d, unsigned rows, unsigned cols, const T *src, T *dst)
{
    assert(d >= 0);
    auto row = d * (rows - 1);
    unsigned row1 = std::min((unsigned)row, rows - 1);
    unsigned row2 = std::min(row1 + 1, rows - 1);
    auto mu = row - (unsigned)row;
    for (unsigned i = 0; i < cols; ++i)
        dst[i] = (1 - mu) * src[row1 * cols + i] + mu * src[row2 * cols + i];
}

void BBD_Filter_Coef::interpolate_G(cdouble::value_type d, cdouble *g/*[M]*/) const noexcept
{
    interpolate_row(d, N, M, G.get(), g);
}

BBD_Filter_Coef BBD::compute_filter(float fs, unsigned steps, const BBD_Filter_Spec &spec)
{
    BBD_Filter_Coef coef;
    cdouble::value_type ts = 1.0 / fs;
    unsigned M = spec.M;

    coef.M = M;
    coef.N = steps;
    coef.G.reset(new cdouble[M * steps]);
    coef.P.reset(new cdouble[M]);

    cdouble *pm = coef.P.get();
    for (unsigned m = 0; m < M; ++m)
        pm[m] = std::exp(ts * spec.P[m]);

    for (unsigned step = 0; step < steps; ++step)  {
        auto d = (cdouble::value_type)step / (steps - 1);
        cdouble *gm = &coef.G[step * M];
        switch (spec.kind) {
        case BBD_Filter_Kind::Input:
            for (unsigned m = 0; m < M; ++m)
                gm[m] = ts * spec.R[m] * std::pow(pm[m], d);
            break;
        case BBD_Filter_Kind::Output:
            for (unsigned m = 0; m < M; ++m)
                gm[m] = (spec.R[m] / spec.P[m]) * std::pow(pm[m], 1 - d);
            break;
        }
    }

    cdouble H = 0;
    for (unsigned m = 0; m < M; ++m)
        H -= spec.R[m] / spec.P[m];
    coef.H = H.real();

    return coef;
}

namespace j60 {
static constexpr unsigned M_in = 5;
static constexpr cdouble R_in[M_in] = {{251589, 0}, {-130428, -4165}, {-130428, 4165}, {4634, -22873}, {4634, 22873}};
static constexpr cdouble P_in[M_in] = {{-46580, 0}, {-55482, 25082}, {-55482, -25082}, {-26292, -59437}, {-26292, 59437}};
static constexpr unsigned M_out = 5;
static constexpr cdouble R_out[M_out] = {{5092, 0}, {11256, -99566}, {11256, 99566}, {-13802, -24606}, {-13802, 24606}};
static constexpr cdouble P_out[M_out] = {{-176261, 0}, {-51468, 21437}, {-51468, -21437}, {-26276, -59699}, {-26276, 59699}};
} // namespace j60

const BBD_Filter_Spec bbd_fin_j60 = {BBD_Filter_Kind::Input, j60::M_in, j60::R_in, j60::P_in};
const BBD_Filter_Spec bbd_fout_j60 = {BBD_Filter_Kind::Output, j60::M_out, j60::R_out, j60::P_out};
