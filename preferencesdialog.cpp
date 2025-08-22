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

void PreferencesDialog::on_PrefList_currentRowChanged(int currentRow)
{
    // Your implementation here
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void PreferencesDialog::on_buttonBox_rejected()
{
    this->close(); //Close the window. (to be changed later)
}

void PreferencesDialog::on_buttonBox_accepted()
{
    this->close(); //Close the window.
}

void PreferencesDialog::on_Show_clicked()
{
    emit toggleStatusBarReq(true);
}

void PreferencesDialog::on_radioButton_2_clicked()
{
    emit toggleStatusBarReq(false);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

