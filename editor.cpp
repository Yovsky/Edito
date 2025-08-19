#include "editor.h"
#include "ui_editor.h"
#include <QFile>
#include <QTextStream>

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
{
    ui->setupUi(this);
}

void Editor::OpenFile(const QString &FilePath)
{
    QFile file(FilePath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream content(&file);
        content.setEncoding(QStringConverter::Utf8);
        ui->TextOut->setPlainText(content.readAll());
    }
}

Editor::~Editor()
{
    delete ui;
}
