#include <QWidget>
#include <QMessageBox>
#include <portaudio.h>

#include "mainwindow.h"
#include "audiodialog.h"
#include "./ui_audiodialog.h"
#include "util/log.h"

AudioDialog::AudioDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AudioDialog)
{
    ui->setupUi(this);
    setModal(false);
    setMaximumSize(size());
    setMinimumSize(size());
    show();
    
    // Initialize PortAudio
    PaError error = Pa_Initialize();
    if(error != paNoError) {
        Log::error("Portaudio failed to initialize!");
        return;
    }
    Log::info("Initialized PortAudio");

    // Enumerate devices
    int numDevices = Pa_GetDeviceCount();
    if(numDevices < 0) {
        Log::error("PortAudio failed to enumerate devices!");
        return;
    }

    // Populate the input and output device dropdowns
    for(int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if(deviceInfo->maxInputChannels > 0) {
            ui->inputDevice->addItem(deviceInfo->name);
        }
        if(deviceInfo->maxOutputChannels > 0) {
            ui->outputDevice->addItem(deviceInfo->name);
        }
    }

    // Set the default devices
    inputDevice = Pa_GetDefaultInputDevice();
    outputDevice = Pa_GetDefaultOutputDevice();
    ui->inputDevice->setCurrentIndex(inputDevice-1);
    ui->outputDevice->setCurrentIndex(outputDevice-1);

    // Signal handling
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AudioDialog::Ok);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AudioDialog::Cancel);
}

void AudioDialog::Ok() {
    inputDevice = ui->inputDevice->currentIndex()+1;
    outputDevice = ui->outputDevice->currentIndex()+1;
}

void AudioDialog::Cancel() {
    ui->inputDevice->setCurrentIndex(inputDevice-1);
    ui->outputDevice->setCurrentIndex(outputDevice-1);
}

int AudioDialog::getInputDeviceIndex() { return this->inputDevice; }
int AudioDialog::getOutputDeviceIndex() { return this->outputDevice; }