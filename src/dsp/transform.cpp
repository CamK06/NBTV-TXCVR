#include "transform.h"
#include <fftw3.h>
#include <string.h>
#include <algorithm>
#include <stdint.h>
#include <iostream>

namespace dsp
{

void hilbert(fftw_complex* samps, int len)
{
    // Calculate the FFTW of the samples
    fftw_complex* out = (fftw_complex*)fftw_malloc(len*sizeof(fftw_complex));
    fftw_plan pf = fftw_plan_dft_1d(len, samps, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(pf);
    fftw_destroy_plan(pf);

    // Apply the hilbert transform
    float m = 0;
    for(int i = 0; i < len; i++) {
        if(i >= len/2 || i == 0)
            m = 0;
        else
            m = 1;
        out[i][0] *= m;
        out[i][1] *= m;
    }

    fftw_plan pb = fftw_plan_dft_1d(len, out, samps, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(pb);
    fftw_destroy_plan(pb);
    fftw_free(out);

    // Normalize the level
    for(int i = 0; i < len; i++) {
        samps[i][0] /= len;
        samps[i][1] /= len;
    }
}

}