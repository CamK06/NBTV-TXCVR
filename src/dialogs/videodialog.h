#pragma once

#include <QDialog>
#include <QWidget>

#include "./ui_videodialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VideoDialog; }
QT_END_NAMESPACE

class VideoDialog : public QDialog
{
    Q_OBJECT

public:
    VideoDialog(QWidget *parent = nullptr);
    Ui::VideoDialog *ui;

private:
    
};