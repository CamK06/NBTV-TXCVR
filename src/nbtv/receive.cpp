#include "receive.h"
#include "dsp/transform.h"
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include <algorithm>

void NBTVReceive::start(nbtvParam params, int sampRate)
{
    mode = params;
    imgSize = mode.lines*mode.pixels;
    if(frame == nullptr)
        frame = new uint8_t[imgSize];
    running = true;
    this->sampRate = sampRate;
}

void NBTVReceive::step(int16_t* samps, int numSamps, uint8_t* outFrame)
{
    if(!running)
        return;

    // Shift 400Hz down to recover the baseband video
    fftw_complex* xSamps = (fftw_complex*)fftw_malloc(numSamps*sizeof(fftw_complex));
    fftw_complex* fftSamps = (fftw_complex*)fftw_malloc(numSamps*sizeof(fftw_complex));
    for(int i = 0; i < numSamps; i++) {
        xSamps[i][0] = samps[i];
        xSamps[i][1] = 0;
    }
    dsp::hilbert(xSamps, numSamps);
    int max = 0;
    for(int i = 0; i < numSamps; i++) {
        samps[i] = (xSamps[i][0] * cos(phase/((sampRate/400)/(M_PI*2)))) + ((xSamps[i][1]) * sin(phase/((sampRate/400)/(M_PI*2))));
        if(samps[i] > agcMax) agcMax = samps[i];
        phase++;
    }

    // Calculate average signal level for AGC
    //float newAvg = 0;
    //for(int i = 0; i < numSamps; i++)
    //    newAvg += samps[i];
    //newAvg /= numSamps;
    //agcAvg += max;
    //agcAvg /= 2;
    //agcAvg = max;
    //if(agc < 0) {
    //    agc *= -1;
        //phase+=sampRate/400;
    //}

    for(int i = 0; i < numSamps; i++) {
        // AGC
        samps[i] *= agc;
        
        // Synchronization
        if(i > 0) {
            if(abs(samps[i]) >= (INT16_MAX-8192)) {
                onSyncPulse = true;
                syncCount++;
            }
            else if(onSyncPulse && abs(samps[i]) < (INT16_MAX-8192)) {
                onSyncPulse = false;
                if(syncCount == 2) {
                    noSyncCount = 0;
                    syncCount = 0;
                    synced = true;
                    pixel = 0;
                    ssamps = 0;
                    line++;
                    outFrame[imgIndex] = 0;
                    imgIndex = (line*mode.pixels)+pixel;
                    outFrame[imgIndex] = 0;
                    continue;
                }
                else
                    syncCount = 0;
            }
            else {
                noSyncCount++;
                if(noSyncCount >= mode.sampsPerPixel*mode.lines*mode.pixels) {
                    noSyncCount = 0;
                    syncCount = 0;
                    synced = false;
                }
            }
        }

        // Too lazy to do this another way
        //if(samps[i] >= 0)
        pixVal += abs(samps[i]-16384);
        //else
        //    pixVal -= samps[i];

        if(ssamps >= mode.sampsPerPixel) {
            // Write the pixel to the frame
            pixVal /= mode.sampsPerPixel;
            pixVal = ((pixVal/INT16_MAX)*255);
            outFrame[imgIndex] = pixVal;

            // Advance the beam
            ssamps = 0;
            imgIndex++;
            pixel++;
            if(pixel == mode.pixels) {
                pixel = 0;
                line++;
                imgIndex = (line*mode.pixels)+pixel;
            }
            //line++;
            //if(line == mode.lines)
            //    line = 0;
        }

        // The frame is done, prepare for the next
        if(imgIndex > imgSize) {
            ssamps = 0;
            imgIndex = 0;
            pixel = 0;
            line = 0;
            updateDisplay = true;
            agcFrames++;
            agcAvg += agcMax;
            agcAvg /= 2;
            if(agcFrames > agcDelay) {
                agc = (INT16_MAX)/agcAvg;
                agcAvg = 0;
                agcMax = 0;
                agcFrames = 0;
            }
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