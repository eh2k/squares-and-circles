#pragma once
#include "bbd_filter.h"
#include <algorithm>
#include <vector>
#include <memory>
#include <complex>

class BBD_Line {
public:

    BBD_Line():mem_(0)
    {}
    
    /**
     * Initialize a delay line with the specified parameters. (non-RT)
     * @param fs audio sampling rate
     * @param ns number of stages / length of the virtual capacitor array
     * @param fsin analog specification of the input filter
     * @param fsout analog specification of the output filter
     */
    void setup(unsigned ns, const BBD_Filter_Coef &fin, const BBD_Filter_Coef &fout);

    /**
     * Reinitialize all the internal state to zero. (RT)
     */
    void clear();

    /**
     * Process a block of audio signal. (RT)
     * @note The clock input is defined as \f$F_{clk}/F_{s}\f$, where
     *       \f$F_{clk}\f$ is the desired BBD instantaneous clock rate. It is
     *       valid to have \f$F_{clk}>F_{s}/2\f$.
     * @param n number of frames to process
     * @param input input buffer of size @p n
     * @param output output buffer of size @p n
     * @param clock clock input buffer of size @p n
     */
    void process(unsigned n, const float *input, float *output, const float *clock);

    /**
     * Get the discretization of the input filter. (RT)
     * @return digital filter model
     */
    const BBD_Filter_Coef &filter_in() const noexcept { return *fin_; }

    /**
     * Get the discretization of the output filter. (RT)
     * @return digital filter model
     */
    const BBD_Filter_Coef &filter_out() const noexcept { return *fout_; }

    /**
     * Determine the BBD clock rate \f$F_{clk}\f$ which obtains a given delay. (RT)
     * @param delay delay in seconds
     * @param ns number of stages / length of the virtual capacitor array
     * @return BBD clock rate in Hz
     */
    static inline float hz_rate_for_delay(float delay, unsigned ns)
        { return 2 * ns / delay; }

    /**
     * Determine the delay obtained for a given BBD clock rate \f$F_{clk}\f$. (RT)
     * @param rate BBD clock rate in Hz
     * @param ns number of stages / length of the virtual capacitor array
     * @return delay in seconds
     */
    static inline cdouble::value_type delay_for_hz_rate(cdouble::value_type rate, unsigned ns)
        { return 2 * ns / rate; }

private:
    unsigned ns_; // delay size
    std::vector<float> mem_; // delay memory
    unsigned imem_; // delay memory index
    cdouble::value_type pclk_; // clock phase
    unsigned ptick_; // clock tick counter
    cdouble::value_type ybbd_old_;
    const BBD_Filter_Coef *fin_;
    const BBD_Filter_Coef *fout_;
    std::unique_ptr<cdouble[]> Xin_;
    std::unique_ptr<cdouble[]> Xout_;
    std::unique_ptr<cdouble[]> Xout_mem_; // sample memory of output filter
    std::unique_ptr<cdouble[]> Gin_;
    std::unique_ptr<cdouble[]> Gout_;
};
