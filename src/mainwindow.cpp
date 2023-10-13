#include <QMainWindow>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "log.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioDialog(new AudioDialog(this))
{
    audioDialog->close();
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

    Log::info("Initialized main window");
}

void MainWindow::showAudioDialog() { audioDialog->exec(); }

void MainWindow::exit() { std::exit(0); }