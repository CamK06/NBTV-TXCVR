#pragma once

#include <QObject>
#include "modes.h"

class NBTVReceive : public QObject
{
    Q_OBJECT
public:
    void start(nbtvParam params, int sampRate);
    void step(int16_t* samps, int numSamps, uint8_t* outFrame);
    void stop();
    bool displayUpdate() { return this->updateDisplay; }
    void resetUpdated() { this->updateDisplay = false; }
    bool isSynced() { return this->synced; }

private:
    // Parameters
    nbtvParam mode;
    uint8_t* frame = nullptr;
    int imgSize = 0;
    int sampRate = 0;

    // Receiver
    int imgIndex = 0;
    int pixel = 0;
    int line = 0;
    int ssamps = 0;
    int phase = 0;
    double pixVal = 0;
    float diff = 0.0;
    float agc = 1.0;
    float agcAvg = 0.0;
    int agcMax = 0;
    int agcFrames = 0;
    int agcDelay = 3; // Number of frames to sample
    int syncCount = 0;
    int noSyncCount = 0;

    // States
    bool updateDisplay = false;
    bool running = false;
    bool synced = false;
    bool onSyncPulse = false;
};