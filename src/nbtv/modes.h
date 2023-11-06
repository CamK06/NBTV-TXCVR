#pragma once

#include <QString>

#define NUM_MODES 4

enum Mode {
    KCNSSB,
    KCNFM,
    TEKADE,
    BAIRD,
    NBTVA
};

struct nbtvParam {
    Mode id;
    QString name; // Display name
    uint lines; // Vertical lines (NOT horizontal, this is pixels in standard modes)
    uint pixels; // Pixels per line
    uint framerate; // Frames per second
    float sampsPerPixel; // Samples per pixel, at 48000, might remove this
};

extern nbtvParam nbtvModes[NUM_MODES];