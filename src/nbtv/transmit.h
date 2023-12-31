#pragma once

#include <QObject>
#include "modes.h"

class NBTVTransmit : public QObject
{
    Q_OBJECT
public:
    void start(nbtvParam params, int sampRate);
    void step(uint8_t* frame, int numSamps, int16_t* outSamps);
    void stop();

private:
    //int line = 0;
    //int pixel = 0;
    //int imgIndex = 0;
    int phase = 0;
    int sampRate = 0;
    int imageSize = 0;
    nbtvParam mode;
    bool running = false;
};