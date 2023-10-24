#include "receive.h"

void NBTVReceive::start(nbtvParam params)
{
    mode = params;
    if(frame == nullptr)
        frame = new uint8_t[mode.lines*mode.pixels];
    running = true;
}

void NBTVReceive::step(int16_t* samps, int numSamps, uint8_t* outFrame)
{
    if(!running)
        return;

    // TODO: Process

    outFrame = frame;
}

void NBTVReceive::stop()
{
    running = false;
    delete[] frame;
}