#include <QMainWindow>
#include <QMessageBox>
#include <sndfile.hh>
#include <portaudio.h>
#include <thread>
#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "util/log.h"
#include "nbtv/transmit.h"
#include "nbtv/receive.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioDialog(new AudioDialog(this))
    , videoDialog(new VideoDialog(this))
    , radioDialog(new RadioDialog(this))
{
    audioDialog->close();
    videoDialog->close();
    radioDialog->close();
    ui->setupUi(this);
    updateStatusBar();
    ui->statusBar->addWidget(&statusBarLabel, 1);

    // Disable the sync button, as it is actually just a redneck display, not input
    ui->syncButton->setDisabled(true);
    QPalette palette = ui->syncButton->palette();
    palette.setColor(QPalette::ButtonText, QColor(Qt::white));
    ui->syncButton->setPalette(palette);

    // TEMPORARY: Set up the display labels with placeholder data
    //QPixmap pixmap("/home/cam/Dev/C++/NBTV-TXCVR/test-images/test.png");
    //ui->txGfx->setPixmap(pixmap);
    //ui->rxGfx->setPixmap(pixmap);

    // Signal handling
    connect(ui->fileQuit, &QAction::triggered, this, &MainWindow::exit);
    connect(ui->setupAudio, &QAction::triggered, this, &MainWindow::showAudioDialog);
    connect(ui->setupVideo, &QAction::triggered, this, &MainWindow::showVideoDialog);
    connect(ui->setupRadio, &QAction::triggered, this, &MainWindow::showRadioDialog);
    connect(ui->helpAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->txButton, &QPushButton::pressed, this, &MainWindow::toggleTx);
    connect(ui->rxStartButton, &QPushButton::pressed, this, &MainWindow::toggleRx);

    Log::info("Initialized main window");
}

void MainWindow::mainWorker()
{
    // Most of this setup outside of the main loop is temporary garbage
    // CLEAN IT UP, CAM.
    Log::info("Starting main worker thread");
    int numSamps = mode.lines*mode.pixels*(sampleRate/(mode.lines*mode.pixels*mode.framerate));
    rxFrameBuf = new uint8_t[mode.lines*mode.pixels];
    int16_t* samps = new int16_t[numSamps]; // One frame
    txImage = QImage(txFrameBuf, mode.pixels, mode.lines, QImage::Format_Grayscale8);
    rxImage = QImage(rxFrameBuf, mode.pixels, mode.lines, QImage::Format_Grayscale8);
    rxPixmap = QPixmap::fromImage(rxImage);
    txPixmap = QPixmap::fromImage(txImage);

    int image_size = std::filesystem::file_size("aaa.bin");
    std::ifstream imageFile;
    imageFile.open("aaa.bin");
    if (!imageFile.is_open()) {
        std::cout << "Error: invalid file" << std::endl;
        return;
    }
    txFrameBuf = new uint8_t[image_size];
    imageFile.read((char*)txFrameBuf, image_size);   
    imageFile.close();
    int txBufPtr = 0;

    while(workerRunning) {
        switch (state)
        {
        case State::TX:
            // Shift the image buffer down one frame
            txBufPtr += mode.lines*mode.pixels;
            if(txBufPtr >= image_size)
                txBufPtr = 0;

            // TODO: Add webcam or otherwise video input here
            // Input will clear the frame buffer (remove old frame and RX image)
            txImage = QImage(txFrameBuf+txBufPtr, mode.pixels, mode.lines, QImage::Format_Grayscale8);
            txPixmap.convertFromImage(txImage);
            QMetaObject::invokeMethod(ui->txGfx, "setPixmap", Qt::QueuedConnection, Q_ARG(QPixmap, txPixmap));

            // Generate output samples and write them to the audio stream
            transmitter.step(txFrameBuf+txBufPtr, numSamps, samps);
            err = Pa_WriteStream(audioDialog->stream, samps, numSamps);
            if(err)
                Log::error("Failed to write samples to output stream!");
            break;
        
        case State::RX:
            
            // Read input samples and process them to video
            for(int i = 0; i < numSamps; i++)
                samps[i] = 0;
            err = Pa_ReadStream(audioDialog->stream, samps, FRAMES_PER_BUF);
            if(err)
                Log::error("Failed to read samples from input stream!");
            receiver.step(samps, FRAMES_PER_BUF, rxFrameBuf);

            // Update the GUI
            if(receiver.displayUpdate()) {
                rxImage = QImage(rxFrameBuf, mode.pixels, mode.lines, QImage::Format_Grayscale8);
                rxPixmap.convertFromImage(rxImage);
                QMetaObject::invokeMethod(ui->rxGfx, "setPixmap", Qt::QueuedConnection, Q_ARG(QPixmap, rxPixmap));
                receiver.resetUpdated();

                // Sync 'button'
                if(receiver.isSynced()) {
                    ui->syncButton->setText("SYNCED");
                    QMetaObject::invokeMethod(ui->syncButton, "setStyleSheet", Qt::QueuedConnection, Q_ARG(QString, "background-color: green"));
                }
                else {
                    ui->syncButton->setText("NO SYNC");
                    QMetaObject::invokeMethod(ui->syncButton, "setStyleSheet", Qt::QueuedConnection, Q_ARG(QString, "background-color: red"));
                }
            }
            break;

        default:
            break;
        }
    }
}

void MainWindow::toggleTx()
{
    if(!workerRunning) {
        worker = std::thread(&MainWindow::mainWorker, this);
        workerRunning = true;
    }

    // Toggle the transmitter
    if(state != State::TX) {
        ui->txButton->setText("Stop");
        ui->txButton->setStyleSheet("background-color: red");
        ui->rxStartButton->setDisabled(true);
        if(state == State::RX)
            toggleRx();

        // Start the audio stream
        if(!Pa_IsStreamActive(audioDialog->stream))
            err = Pa_StartStream(audioDialog->stream);
        if(err != paNoError) {
            Log::error("Failed to open audio stream");
            return;
        }

        mode.sampsPerPixel = sampleRate/(mode.lines*mode.pixels*mode.framerate);
        transmitter.start(mode, sampleRate);
        state = State::TX;
    }
    else { // We autostart RX when TX ends, as you likely want it if you're TXing
        state = State::IDLE;
        ui->txButton->setText("Start");
        ui->txButton->setStyleSheet("");
        ui->rxStartButton->setDisabled(false);
        transmitter.stop();
        toggleRx();
    }
    updateStatusBar();
}

void MainWindow::toggleRx()
{
    if(!workerRunning) {
        worker = std::thread(&MainWindow::mainWorker, this);
        workerRunning = true;
    }

    // Toggle the receiver
    if(state != State::RX) {
        ui->rxStartButton->setText("Stop");
        ui->rxStartButton->setStyleSheet("background-color: red");

        // Start the audio stream
        if(!Pa_IsStreamActive(audioDialog->stream))
            err = Pa_StartStream(audioDialog->stream);
        if(err != paNoError) {
            Log::error("Failed to open audio stream");
            return;
        }

        mode.sampsPerPixel = sampleRate/(mode.lines*mode.pixels*mode.framerate);
        receiver.start(mode, sampleRate);
        state = State::RX;
    }
    else {
        state = State::IDLE;
        ui->rxStartButton->setText("Start");
        ui->rxStartButton->setStyleSheet("");
        ui->syncButton->setText("NO SYNC");
        ui->syncButton->setStyleSheet("background-color: red");
        receiver.stop();
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    // TODO: Update the mode dynamically using the QString once a getModeName() func is implemented
    switch(state) {
        case State::IDLE:
            statusBarLabel.setText(QString("MODE: %1 | STATE: IDLE").arg(mode.name));
            break;
        case State::TX:
            statusBarLabel.setText(QString("MODE: %1 | STATE: TX").arg(mode.name));
            break;
        case State::RX:
            statusBarLabel.setText(QString("MODE: %1 | STATE: RX").arg(mode.name));
            break;
    }
}

void MainWindow::showAudioDialog() { audioDialog->exec(); }
void MainWindow::showVideoDialog() { videoDialog->exec(); }
void MainWindow::showRadioDialog() { radioDialog->exec(); }
void MainWindow::showAboutDialog() { QMessageBox::about(this, "About NBTV Transceiver", "NBTV Transceiver\n\nWritten by Cam K. VE3KCN\nLicensed under the BSD 2-Clause license"); }
void MainWindow::exit() { std::exit(0); }
