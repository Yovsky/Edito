#include "editor.h"
#include "ui_editor.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextDocument>

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
{
    ui->setupUi(this);
    QLabel *posStatus = new QLabel(); //Position label.
    ui->statusbar->addPermanentWidget(posStatus, 2);
    QLabel *sizeStatus = new QLabel(); //Size label.
    ui->statusbar->addPermanentWidget(sizeStatus, 1);
    connect(ui->TextOut, &QTextEdit::cursorPositionChanged, this, [=]()
    {
        QTextCursor cursor = ui->TextOut->textCursor();
        posStatus->setText("Line " + QString::number(cursor.blockNumber() + 1) //Line position.
                         + ", Col " + QString::number(cursor.positionInBlock() + 1) //Column position.
                         + ", Pos " + QString::number(cursor.position() + 1)); //Character position.

        int chCount = ui->TextOut->toPlainText().length();
        int lnCount = ui->TextOut->document()->blockCount();
        sizeStatus->setText("Size " + QString::number(chCount) //Character count.
                          + ", Lines " + QString::number(lnCount)); //Line count.
    });
}

void Editor::OpenFile(const QString &FilePath)
{
    QFile file(FilePath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) //Setting the encoding.
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
