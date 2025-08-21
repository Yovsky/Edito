#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QListWidget>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    connect(ui->PrefList, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}


void PreferencesDialog::on_buttonBox_rejected()
{
    this->close(); //Close the window.
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}





