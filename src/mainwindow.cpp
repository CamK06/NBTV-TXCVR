#include <QMainWindow>
#include <QMessageBox>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "util/log.h"

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

    Log::info("Initialized main window");
}

void MainWindow::showAudioDialog() { audioDialog->exec(); }
void MainWindow::showVideoDialog() { videoDialog->exec(); }
void MainWindow::showRadioDialog() { radioDialog->exec(); }
void MainWindow::showAboutDialog() { QMessageBox::about(this, "About NBTV Transceiver", "NBTV Transceiver\n\nWritten by Cam K. VE3KCN\nLicensed under the BSD 2-Clause license"); }

void MainWindow::exit() { std::exit(0); }