#include "edito.h"
#include "editor.h"
#include "ui_edito.h"
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QStandardPaths>
#include <QMessageBox>
#include <QIcon>

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

