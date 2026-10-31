#include "bbd_line.h"
#include <algorithm>
#include <cassert>

void BBD_Line::setup(unsigned ns, const BBD_Filter_Coef &fin, const BBD_Filter_Coef &fout)
{
    //[eh2k] mem_.reserve(8192);

    fin_ = &fin;
    fout_ = &fout;

    unsigned Min = fin.M;
    unsigned Mout = fout.M;
    Xin_.reset(new cdouble[Min]);
    Xout_mem_.reset(new cdouble[Mout]);

    set_delay_size(ns);
    clear();
}

void BBD_Line::set_delay_size(unsigned ns)
{
    mem_.clear();
    mem_.resize(ns);
    imem_ = 0;
    ns_ = ns;
}

void BBD_Line::clear()
{
    std::fill(mem_.begin(), mem_.end(), 0);
    imem_ = 0;
    pclk_ = 0;
    ptick_ = 0;
    ybbd_old_ = 0;
    unsigned Min = fin_->M;
    unsigned Mout = fout_->M;
    std::fill(&Xin_[0], &Xin_[Min], 0);
    std::fill(&Xout_mem_[0], &Xout_mem_[Mout], 0);
}

// [eh2k] one audio sample of the BBD and its output filter, Xin = state of the input filter (shared by both lines in
// process2). The filter coefficients G are interpolated by the tick position d; instead of interpolating the row and
// then summing, both rows are summed and the sums interpolated (Gin), or the tick is split between the rows (Gout).
// Min/Mout = 0: the order from the filter coefficients, at runtime
template <unsigned Min, unsigned Mout>
inline float BBD_Line::step(const cdouble *Xin, float fclk)
{
    const BBD_Filter_Coef &fin = *fin_, &fout = *fout_;
    const unsigned mi = Min ? Min : fin.M, mo = Mout ? Mout : fout.M;

    cdouble Xout[BBD_Filter_Coef::max_M];
    for (unsigned m = 0; m < mo; ++m)
        Xout[m] = 0;

    float ybbd_old = ybbd_old_;

    if (fclk > 0) {
        const float pclk_old = pclk_;
        float pclk = pclk_old + fclk;
        const unsigned tick_count = (unsigned)pclk;
        pclk_ = pclk - tick_count;

        const float inv_fclk = 1 / fclk;
        float *mem = mem_.data();
        unsigned imem = imem_;
        unsigned ptick = ptick_;

        for (unsigned tick = 0; tick < tick_count; ++tick) {
            float d = (1 - pclk_old + tick) * inv_fclk;
            d -= (unsigned)d;
            if ((ptick & 1) == 0) {
                const float row = d * (fin.N - 1);
                const unsigned row1 = std::min((unsigned)row, fin.N - 1);
                const unsigned row2 = std::min(row1 + 1, fin.N - 1);
                const float mu = row - (unsigned)row;
                const cdouble *g1 = &fin.G[row1 * mi], *g2 = &fin.G[row2 * mi];
                float s1 = 0, s2 = 0;
                for (unsigned m = 0; m < mi; ++m) {
                    s1 += g1[m].real() * Xin[m].real() - g1[m].imag() * Xin[m].imag();
                    s2 += g2[m].real() * Xin[m].real() - g2[m].imag() * Xin[m].imag();
                }
                mem[imem] = s1 + mu * (s2 - s1);
                imem = ((imem + 1) < ns_) ? (imem + 1) : 0;
            }
            else {
                const float row = d * (fout.N - 1);
                const unsigned row1 = std::min((unsigned)row, fout.N - 1);
                const unsigned row2 = std::min(row1 + 1, fout.N - 1);
                const float mu = row - (unsigned)row;
                const cdouble *g1 = &fout.G[row1 * mo], *g2 = &fout.G[row2 * mo];
                const float ybbd = mem[imem];
                const float delta = ybbd - ybbd_old;
                ybbd_old = ybbd;
                const float c2 = delta * mu, c1 = delta - c2;
                for (unsigned m = 0; m < mo; ++m)
                    Xout[m] += c1 * g1[m] + c2 * g2[m];
            }
            ++ptick;
        }

        imem_ = imem;
        ptick_ = ptick;
    }

    ybbd_old_ = ybbd_old;

    const cdouble *Pout = fout.P.get();
    cdouble *Xout_mem = Xout_mem_.get();
    float y = fout.H * ybbd_old;
    for (unsigned m = 0; m < mo; ++m) {
        cdouble xout = Pout[m] * Xout_mem[m] + Xout[m];
        Xout_mem[m] = xout;
        y += xout.real();
    }
    return y;
}

template <unsigned Min, unsigned Mout>
void BBD_Line::process_t(BBD_Line &a, BBD_Line *b, unsigned n, const float *input, float *output_a, float *output_b,
                         const float *clock_a, const float *clock_b)
{
    const unsigned mi = Min ? Min : a.fin_->M;
    cdouble *Xin = a.Xin_.get();
    const cdouble *Pin = a.fin_->P.get();

    for (unsigned i = 0; i < n; ++i) {
        output_a[i] = a.step<Min, Mout>(Xin, clock_a[i]);
        if (b)
            output_b[i] = b->step<Min, Mout>(Xin, clock_b[i]);

        for (unsigned m = 0; m < mi; ++m)
            Xin[m] = Pin[m] * Xin[m] + input[i];
    }
}

void BBD_Line::process(unsigned n, const float *input, float *output, const float *clock)
{
    if (fin_->M == 3 && fout_->M == 3) // the Juno 60 filters
        process_t<3, 3>(*this, nullptr, n, input, output, nullptr, clock, nullptr);
    else
        process_t<0, 0>(*this, nullptr, n, input, output, nullptr, clock, nullptr);
}

void BBD_Line::process2(BBD_Line &a, BBD_Line &b, unsigned n, const float *input, float *output_a, float *output_b,
                        const float *clock_a, const float *clock_b)
{
    if (a.fin_->M == 3 && a.fout_->M == 3 && b.fin_->M == 3 && b.fout_->M == 3)
        process_t<3, 3>(a, &b, n, input, output_a, output_b, clock_a, clock_b);
    else
        process_t<0, 0>(a, &b, n, input, output_a, output_b, clock_a, clock_b);
}
