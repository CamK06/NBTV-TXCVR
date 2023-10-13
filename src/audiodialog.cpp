#include <QWidget>
#include <QMessageBox>

#include "mainwindow.h"
#include "audiodialog.h"
#include "./ui_audiodialog.h"

AudioDialog::AudioDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AudioDialog)
{
    ui->setupUi(this);
    setModal(false);
    setMaximumSize(size());
    setMinimumSize(size());
    show();
}