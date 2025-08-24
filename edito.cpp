#include "edito.h"
#include "editor.h"
#include "ui_edito.h"
#include "preferencesdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QStandardPaths>
#include <QMessageBox>
#include <QIcon>
#include <QSettings>

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

void Edito::on_Open_clicked()
{
    QString DefLocation =  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); //Get the Documents folder.
    if(DefLocation.isEmpty()) DefLocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation); //Fallback to Home if not existing.
    QString FileOpened = QFileDialog::getOpenFileName(this, tr("Open File"), DefLocation, tr("Text Files (*.txt)")); //File path.

    if(!FileOpened.isEmpty())
    {
        this->close();
        qDebug() << "opended: " << FileOpened;
        Editor *editor = new Editor();
        editor->setAttribute(Qt::WA_DeleteOnClose);
        editor->show();
        editor->raise();
        editor->activateWindow();
        editor->OpenFile(FileOpened);
    }
    else QMessageBox::critical(this, "Error", "Failed to open from a file.");
}


void Edito::on_createNew_clicked()
{
    this->close();
    qDebug() << "Opened new";
    Editor *editor = new Editor();
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    editor->raise();
    editor->activateWindow();
    editor->NewFile();
}


void Edito::on_Options_clicked()
{
    QSettings settings("Yovsky", "Edito");
    bool isVisible = settings.value("ToggleStatBar", true).toBool(); //Load visibility settings.
    PreferencesDialog dialog(isVisible ,this);

    dialog.setVisible(isVisible); //Set according to loaded settings.

    connect(&dialog, &PreferencesDialog::toggleStatusBarReq, this, [](bool visible) { //Connect for setting the visibility.
        QSettings settings("Yovsky", "Edito");
        settings.setValue("ToggleStatBar", visible);
    });

    dialog.exec();
}


void Edito::on_EmptyFile_clicked()
{
    this->close();
    Editor *editor = new Editor();
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    editor->raise();
    editor->activateWindow();
}

