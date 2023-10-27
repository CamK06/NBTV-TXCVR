#include "receive.h"
#include <cmath>
#include <iostream>

void NBTVReceive::start(nbtvParam params)
{
    mode = params;
    imgSize = mode.lines*mode.pixels;
    if(frame == nullptr)
        frame = new uint8_t[imgSize];
    running = true;
}

void NBTVReceive::step(int16_t* samps, int numSamps, uint8_t* outFrame)
{
    if(!running)
        return;

    int syncIdx = 0;
    for(int i = 0; i < numSamps; i++) {

        // AGC
        samps[i] *= agc;

        // Synchronization
        //if(i > 0) {
        //    double diff = samps[i]-samps[i-1];
        //    if(diff < 0)
        //        diff *= -1;
            
        //}

        // Too lazy to do this another way
        if(samps[i] >= 0)
            pixVal += samps[i];
        else
            pixVal -= samps[i];

        if(ssamps >= mode.sampsPerPixel) {
            // Write the pixel to the frame
            pixVal /= mode.sampsPerPixel;
            pixVal = 255-((pixVal/INT16_MAX)*255);
            outFrame[imgIndex] = pixVal;

            // Advance the beam
            ssamps = 0;
            imgIndex++;
            pixel++;
            if(pixel == mode.pixels)
                pixel = 0;
        }

        // The frame is done, prepare for the next
        if(imgIndex > imgSize) {
            ssamps = 0;
            imgIndex = 0;
            pixel = 0;
            updateDisplay = true;
            syncIdx = 0;
        }
        ssamps++;
    }

    outFrame = frame;
}

void NBTVReceive::stop()
{
    running = false;
    //delete[] frame;
}