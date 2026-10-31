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

// complex exp/pow written out: libstdc++ >= 12 calls clogf/cexpf for these, which the loader doesn't provide
static inline cdouble c_exp(cdouble z)
{
    return std::polar(expf(z.real()), z.imag());
}

// z^d = |z|^d e^(i d arg z): log |z| and arg z of a pole once, not for each of its steps
struct c_log
{
    cdouble::value_type lr, arg;
};

static inline c_log c_logof(cdouble z)
{
    const auto r = sqrtf(z.real() * z.real() + z.imag() * z.imag());
    return {logf(r), atan2f(z.imag(), z.real())};
}

static inline cdouble c_pow(const c_log &z, cdouble::value_type d)
{
    return std::polar(expf(d * z.lr), d * z.arg);
}

BBD_Filter_Coef BBD::compute_filter(float fs, unsigned steps, const BBD_Filter_Spec &spec)
{
    BBD_Filter_Coef coef;
    cdouble::value_type ts = 1.0 / fs;

    // [eh2k] keep real poles and one pole of each conjugate pair (weight 2: Re(g*x) + Re(conj(g)*conj(x)))
    unsigned keep[BBD_Filter_Coef::max_M];
    cdouble::value_type weight[BBD_Filter_Coef::max_M];
    unsigned M = 0;
    for (unsigned m = 0; m < spec.M && M < BBD_Filter_Coef::max_M; ++m) {
        const cdouble p = spec.P[m], r = spec.R[m];
        bool partner_kept = false, partner_later = false;
        for (unsigned k = 0; k < spec.M; ++k)
            if (k != m && p.imag() != 0 && spec.P[k] == std::conj(p) && spec.R[k] == std::conj(r))
                (k < m ? partner_kept : partner_later) = true;
        if (partner_kept)
            continue;
        keep[M] = m;
        weight[M++] = partner_later ? 2 : 1;
    }

    coef.M = M;
    coef.N = steps;
    coef.G.reset(new cdouble[M * steps]);
    coef.P.reset(new cdouble[M]);

    cdouble *pm = coef.P.get();
    c_log lm[BBD_Filter_Coef::max_M];
    for (unsigned m = 0; m < M; ++m) {
        pm[m] = c_exp(ts * spec.P[keep[m]]);
        lm[m] = c_logof(pm[m]);
    }

    for (unsigned step = 0; step < steps; ++step)  {
        auto d = (cdouble::value_type)step / (steps - 1);
        cdouble *gm = &coef.G[step * M];
        switch (spec.kind) {
        case BBD_Filter_Kind::Input:
            for (unsigned m = 0; m < M; ++m)
                gm[m] = weight[m] * ts * spec.R[keep[m]] * c_pow(lm[m], d);
            break;
        case BBD_Filter_Kind::Output:
            for (unsigned m = 0; m < M; ++m)
                gm[m] = weight[m] * (spec.R[keep[m]] / spec.P[keep[m]]) * c_pow(lm[m], 1 - d);
            break;
        }
    }

    cdouble H = 0;
    for (unsigned m = 0; m < spec.M; ++m)
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
