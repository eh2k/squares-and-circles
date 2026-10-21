#pragma once
#include <memory>
#include <complex>
typedef std::complex<float> cdouble;

enum class BBD_Filter_Kind {
    Input,
    Output,
};

/*
  Analog specifications of BBD filters, input and output.
  M=order R=numerator P=denominator
  Analog transfer: H(s)=sum(m:1→M) (R[m]/(s-P[m]))
*/

struct BBD_Filter_Spec {
    BBD_Filter_Kind kind;
    unsigned M;
    const cdouble* R;/*[M]*/
    const cdouble* P;/*[M]*/
    //
    cdouble transfer(cdouble::value_type frequency) const noexcept;
};

/*
  Discretized matrix of filters coefficients.
  M=order, N=interpolation steps, H=feedback factor

  [eh2k] Input and output of the filters are real, so the state of a pole is the conjugate of its partner's: only one
  pole of each conjugate pair is kept (M is the reduced order), with G doubled. The real part of the sum stays exact.
*/
struct BBD_Filter_Coef {
    static constexpr unsigned max_M = 8;
    unsigned M;
    unsigned N;
    std::unique_ptr<cdouble[]> G;/*[M*N]*/
    std::unique_ptr<cdouble[]> P;/*[M]*/
    cdouble::value_type H;
};

namespace BBD {
BBD_Filter_Coef compute_filter(float fs, unsigned steps, const BBD_Filter_Spec &spec);
} // namespace BBD

/*
  The model of BBD input and output filters from Juno 60.
*/
extern const BBD_Filter_Spec bbd_fin_j60;
extern const BBD_Filter_Spec bbd_fout_j60;
