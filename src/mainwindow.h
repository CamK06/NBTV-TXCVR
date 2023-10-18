#pragma once

#include <QMainWindow>
#include "dialogs/audiodialog.h"
#include "dialogs/videodialog.h"
#include "dialogs/radiodialog.h"
#include "./ui_audiodialog.h"
#include "./ui_videodialog.h"
#include "./ui_radiodialog.h"
#include "modes.h"

#include <thread>

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

    bool workerRunning = false;
    State state = IDLE;
    Mode mode = Mode::KCN;
    AudioDialog *audioDialog;
    VideoDialog *videoDialog;
    RadioDialog *radioDialog;
    Ui::MainWindow *ui;
    std::thread worker;
    QLabel statusBarLabel;
};