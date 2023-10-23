#pragma once

#include <QMainWindow>
#include "dialogs/audiodialog.h"
#include "dialogs/videodialog.h"
#include "dialogs/radiodialog.h"
#include "./ui_audiodialog.h"
#include "./ui_videodialog.h"
#include "./ui_radiodialog.h"
#include "nbtv/transmit.h"
#include "nbtv/receive.h"

#include <thread>
#include <portaudio.h>

#define FRAMES_PER_BUF 256

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum State {
    IDLE,
    TX,
    RX
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    void toggleTx();
    void toggleRx();
    void exit();
    void mainWorker();
    void showAudioDialog();
    void showVideoDialog();
    void showRadioDialog();
    void showAboutDialog();
    void updateStatusBar();

    // UI Stuff
    AudioDialog *audioDialog;
    VideoDialog *videoDialog;
    RadioDialog *radioDialog;
    Ui::MainWindow *ui;
    QLabel statusBarLabel;
    
    // TV Functionality
    NBTVTransmit transmitter;
    NBTVReceive receiver;
    State state = IDLE;
    nbtvParam mode = nbtvModes[Mode::KCN];
    std::thread worker;
    bool workerRunning = false;

    // Audio stuff
    PaStreamParameters outputParameters;
    PaStreamParameters inputParameters;
    PaDeviceInfo *inputDeviceInfo;
    PaDeviceInfo *outputDeviceInfo;
    PaStream *stream = nullptr;
    PaError err;
    int sampleRate = 48000;
};