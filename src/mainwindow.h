#pragma once

#include <QMainWindow>
#include "dialogs/audiodialog.h"
#include "dialogs/videodialog.h"
#include "dialogs/radiodialog.h"
#include "./ui_audiodialog.h"
#include "./ui_videodialog.h"
#include "./ui_radiodialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    void exit();
    void showAudioDialog();
    void showVideoDialog();
    void showRadioDialog();
    void showAboutDialog();

    AudioDialog *audioDialog;
    VideoDialog *videoDialog;
    RadioDialog *radioDialog;
    Ui::MainWindow *ui;
};