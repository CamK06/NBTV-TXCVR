#pragma once

#include <QObject>
#include "modes.h"

class NBTVReceive : public QObject
{
    Q_OBJECT
public:
    void start(nbtvParam params);
    void step(int16_t* samps, int numSamps, uint8_t* outFrame);
    void stop();

private:
    nbtvParam mode;
    uint8_t* frame = nullptr;
    int frameIndex = 0;
    bool running = false;
};