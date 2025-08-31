#include "gotodialog.h"
#include "ui_gotodialog.h"

gotodialog::gotodialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::gotodialog)
{
    ui->setupUi(this);
}

gotodialog::~gotodialog()
{
    delete ui;
}

void gotodialog::on_Cancel_clicked()
{
    this->close();
}

