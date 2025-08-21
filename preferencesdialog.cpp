#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

void PreferencesDialog::on_buttonBox_rejected()
{
    this->close(); //Close the window.
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}


