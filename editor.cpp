#include "editor.h"
#include "ui_editor.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
{
    ui->setupUi(this);
    ui->editorTabs->removeTab(0);
    QLabel *posStatus = new QLabel(); //Position label.
    ui->statusbar->addPermanentWidget(posStatus, 2);
    QLabel *sizeStatus = new QLabel(); //Size label.
    ui->statusbar->addPermanentWidget(sizeStatus, 1);
    connect(ui->TextOut, &QPlainTextEdit::cursorPositionChanged, this, [=]()
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

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) //Setting the encoding.
        return;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    CodeEditor *editor = new CodeEditor();
    editor->setPlainText(content); //Passing the file content to the text editor.

    QIcon icon(":/icons/saved"); //Setting the icon.
    ui->editorTabs->addTab(editor, icon,QFileInfo(file).fileName());
    ui->editorTabs->setCurrentWidget(editor);
}

void Editor::NewFile()
{
    CodeEditor *content = new CodeEditor(); //Handle the CreateNew from external windows.

    QIcon icon(":/icons/saved.png");

    ui->editorTabs->addTab(content, icon,"Untitled"); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(content);
}

Editor::~Editor()
{
    delete ui;
}

void Editor::on_actionOpen_triggered()
{
    QString DefLocation =  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); //Get the Documents folder.
    if(DefLocation.isEmpty()) DefLocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation); //Fallback to Home if not existing.
    QString FileOpened = QFileDialog::getOpenFileName(this, tr("Open File"), DefLocation, tr("Text Files (*.txt)")); //File path.

    if(!FileOpened.isEmpty())
    {
        qDebug() << "opended: " << FileOpened;
        this->OpenFile(FileOpened);
    }
    else QMessageBox::critical(this, "Error", "Failed to open from a file.");
}


void Editor::on_actionNew_triggered()
{
    CodeEditor *content = new CodeEditor(); //Handle CreateNew from menus bar.

    QIcon icon(":/icons/saved.png");

    ui->editorTabs->addTab(content, icon, "Untitled"); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(content);
}

