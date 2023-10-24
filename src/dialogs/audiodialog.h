#pragma once

#include <QDialog>
#include <QWidget>
#include <portaudio.h>

#include "./ui_audiodialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AudioDialog; }
QT_END_NAMESPACE

class AudioDialog : public QDialog
{
    Q_OBJECT

public:
    AudioDialog(QWidget *parent = nullptr);
    int getInputDeviceIndex();
    int getOutputDeviceIndex();
    int getSampleRate();
    Ui::AudioDialog *ui;
    PaStream *stream = nullptr;

private:
    void reloadStream();
    void Ok();
    void Cancel();

    int inputDevice = 0;
    int outputDevice = 0;
    PaStreamParameters outputParameters;
    PaStreamParameters inputParameters;
    PaDeviceInfo *inputDeviceInfo;
    PaDeviceInfo *outputDeviceInfo;
    int sampleRate = 48000;
};