#include "transmit.h"
#include <cmath>

void NBTVTransmit::start(nbtvParam params)
{
    mode = params;
    imageSize = params.lines*params.pixels;

    // Reset the scanning beam
    //line = 0;
    //pixel = 0;
    //imgIndex = 0;
}

void NBTVTransmit::step(uint8_t* frame, int numSamps, int16_t* outSamps)
{
    // Generate one frame of samples
    int pixel = 0;
    int imgIndex = 0;
    int samps = 0;
    for(int i = 0; i < numSamps; i++) {
        outSamps[i] = INT16_MAX-(frame[imgIndex]*((INT16_MAX)/384));

        if(pixel == mode.pixels-1) { // End of line
            outSamps[i-1] = ((1*INT16_MAX));
            outSamps[i] = (1*INT16_MAX);
        }

        // Increment pixels
        if(samps == mode.sampsPerPixel) {
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
}

void NBTVTransmit::stop() 
{

}