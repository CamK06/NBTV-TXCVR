#include <iostream>
#include <fstream>
#include <sndfile.hh>
#include <cmath>
#include <filesystem>

#define LINES 50
#define PIXELS 32
#define FRAME_RATE 3
#define SAMPS_PER_PIXEL 48000/(FRAME_RATE*(LINES*PIXELS))

int main(int argc, char *argv[])
{   
    // Read the image file
    int image_size = std::filesystem::file_size("ricc.bin");
    std::ifstream imageFile;
    imageFile.open("ricc.bin");
    if (!imageFile.is_open()) {
        std::cout << "Error: invalid file" << std::endl;
        return 1;
    }

    // Create a buffer for the image
    uint8_t *image = new uint8_t[image_size];
    imageFile.read((char*)image, image_size);   
    imageFile.close();

    // Create a buffer for the output samples
    int samps = 48000*60;
    int16_t *buffer = new int16_t[samps];
    int s = 0;
    int j = 0;
    int linePx = 0;
    for (int i = 0; i < samps; i++) {
        buffer[i] = image[j]*((32767)/384);
        //if((uint8_t)((i*FRAME_RATE*LINES*PIXELS)/48000) % PIXELS == 0)
        if(linePx == PIXELS-1) { // End of line
            //buffer[i-2] = -((.75*32767));
            buffer[i-1] = -((.75*32767));
            buffer[i] = -(.75*32767);
            //linePx = 0;
        }

        //buffer[i] = sin(i/((48000/tone)/(M_PI*2)))*32767;

        // Increment pixels
        if(s == SAMPS_PER_PIXEL) {
            s = 0;
            j++;
            linePx++;
            if(linePx == PIXELS)
                linePx = 0;
        }
        if(j > image_size) {
            s = 0;
            j = 0;
            linePx = 0;
        }
        s++;
    }

    SndfileHandle sndFile;
    sndFile = SndfileHandle("test.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 48000);
    if (sndFile.error())
    {
        std::cout << "Error: invalid file" << std::endl;
        return 1;
    }

    sndFile.write(buffer, samps);

    
}