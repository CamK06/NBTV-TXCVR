#include <QMainWindow>
#include <QMessageBox>
#include <sndio.h>
#include <portaudio.h>
#include <thread>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "util/log.h"
#include "modes.h"

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
    statusBarLabel.setText("MODE: KCN-NBTV | STATE: IDLE");
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
    Log::info("Starting main worker thread");
    while(workerRunning) {

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
        state = State::TX;
        ui->txButton->setText("Stop");
        ui->txButton->setStyleSheet("background-color: red");
        ui->rxStartButton->setDisabled(true);
        ui->rxStartButton->setText("Start");
        ui->rxStartButton->setStyleSheet("");
    }
    else { // We autostart RX when TX ends, as you likely want it if you're TXing
        state = State::RX;
        ui->txButton->setText("Start");
        ui->txButton->setStyleSheet("");
        ui->rxStartButton->setDisabled(false);
        ui->rxStartButton->setText("Stop");
        ui->rxStartButton->setStyleSheet("background-color: red");
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
        state = State::RX;
        ui->rxStartButton->setText("Stop");
        ui->rxStartButton->setStyleSheet("background-color: red");
    }
    else {
        state = State::IDLE;
        ui->rxStartButton->setText("Start");
        ui->rxStartButton->setStyleSheet("");

        // If we aren't transmitting and aren't receiving, we may as well kill the worker
        workerRunning = false;
        if(worker.joinable()) 
            worker.join();
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    // TODO: Update the mode dynamically using the QString once a getModeName() func is implemented
    switch(state) {
        case State::IDLE:
            statusBarLabel.setText(QString("MODE: KCN | STATE: IDLE"));
            break;
        case State::TX:
            statusBarLabel.setText(QString("MODE: KCN | STATE: TX"));
            break;
        case State::RX:
            statusBarLabel.setText(QString("MODE: KCN | STATE: RX"));
            break;
    }
}

void MainWindow::showAudioDialog() { audioDialog->exec(); }
void MainWindow::showVideoDialog() { videoDialog->exec(); }
void MainWindow::showRadioDialog() { radioDialog->exec(); }
void MainWindow::showAboutDialog() { QMessageBox::about(this, "About NBTV Transceiver", "NBTV Transceiver\n\nWritten by Cam K. VE3KCN\nLicensed under the BSD 2-Clause license"); }
void MainWindow::exit() { std::exit(0); }