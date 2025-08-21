#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::Preferences)
{
    ui->setupUI(this);
}

Preferences::~Preferences()
{
    delete ui;
}
