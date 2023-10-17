#pragma once

#include <QDialog>
#include <QWidget>

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
    Ui::AudioDialog *ui;

private:
    void Ok();
    void Cancel();

    int inputDevice = 0;
    int outputDevice = 0;
};