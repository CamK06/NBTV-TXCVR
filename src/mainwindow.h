#pragma once

#include <QMainWindow>
#include "audiodialog.h"
#include "./ui_audiodialog.h"

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

    AudioDialog *audioDialog;
    Ui::MainWindow *ui;
};