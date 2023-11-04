#pragma once

#include <fftw3.h>

namespace dsp
{

void hilbert(fftw_complex* samps, int len);

}