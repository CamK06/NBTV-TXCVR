#include <QWidget>
#include <QMessageBox>

#include "mainwindow.h"
#include "videodialog.h"
#include "./ui_videodialog.h"

VideoDialog::VideoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VideoDialog)
{
    ui->setupUi(this);
    setModal(false);
    setMaximumSize(size());
    setMinimumSize(size());
    show();
}