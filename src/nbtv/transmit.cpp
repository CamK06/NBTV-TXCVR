#include "transmit.h"
#include <cmath>
#include <complex>
#include "dsp/transform.h"
#include <fftw3.h>
#include <iostream>

void NBTVTransmit::start(nbtvParam params, int sampRate)
{
    mode = params;
    imageSize = params.lines*params.pixels;
    running = true;
    this->sampRate = sampRate;
}

void NBTVTransmit::step(uint8_t* frame, int numSamps, int16_t* outSamps)
{
    if(!running)
        return;

    // Generate one frame of samples
    fftw_complex* xSamps = (fftw_complex*)fftw_malloc(numSamps*sizeof(fftw_complex));
    int pixel = 0;
    int imgIndex = 0;
    int samps = 0;
    for(int i = 0; i < numSamps; i++) {
        xSamps[i][0] = ((frame[imgIndex]*(((INT16_MAX)-8192)/255)));
        xSamps[i][1] = 0;
        if(pixel >= mode.pixels-1) { // End of line
            xSamps[i-1][0] = (INT16_MIN/2)-8192;
            xSamps[i-1][1] = 0;
            // xSamps[i][0] = (INT16_MIN/2)-8192;
            // xSamps[i][1] = 0;
        }

        // Increment pixels
        if(samps >= mode.sampsPerPixel) {
            samps = 0;
            imgIndex++;
            pixel++;
            if(pixel == mode.pixels)
                pixel = 0;
        }
        if(imgIndex > imageSize) {
            samps = 0;
            imgIndex = 0;
            pixel = 0;
        }
        samps++;
    }

    // Shift everything up 400Hz
    dsp::hilbert(xSamps, numSamps);
    for(int i = 0; i < numSamps; i++) {
        outSamps[i] = (xSamps[i][0] * cos(phase/((sampRate/400)/(M_PI*2)))) - (xSamps[i][1] * sin(phase/((sampRate/400)/(M_PI*2))));
        outSamps[i] += (cos(phase/((sampRate/400)/(M_PI*2)))*((INT16_MAX/2)-8192))/2;
        phase++;
    }

    fftw_free(xSamps);
}

void NBTVTransmit::stop() 
{
    running = false;
}