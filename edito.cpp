#include "edito.h"
#include "ui_edito.h"

Edito::Edito(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Edito)
{
    ui->setupUi(this);
}

Edito::~Edito()
{
    delete ui;
}
