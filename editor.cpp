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
    , posStatus(nullptr)
    , sizeStatus(nullptr)
{
    ui->setupUi(this);
    ui->editorTabs->removeTab(0);

    posStatus = new QLabel("Line 0, Col 0, Pos 0"); //Position label.
    sizeStatus = new QLabel("Size 0, Lines 0"); //Size label.

    ui->statusbar->addPermanentWidget(posStatus, 2);
    ui->statusbar->addPermanentWidget(sizeStatus, 1); //Assign the blocks with their sizes.

    connect(ui->editorTabs, &QTabWidget::currentChanged, this, [this](int index) {
        Q_UNUSED(index);
        this->UpdateStatusBar();
    });

    UpdateStatusBar();
}

void Editor::UpdateStatusBar()
{
    QWidget *currentWidget = ui->editorTabs->currentWidget();
    CodeEditor *editor = qobject_cast<CodeEditor*>(currentWidget);

    if (!editor || ui->editorTabs->count() == 0) //Safety.
    {
        posStatus->setText("No editor");
        sizeStatus->clear();
        return;
    }

    QTextCursor cursor = editor->textCursor();
    if (posStatus) //For position.
    {
        posStatus->setText("Line " + QString::number(cursor.blockNumber() + 1) //Line position.
                           + ", Col " + QString::number(cursor.positionInBlock() + 1) //Column position.
                           + ", Pos " + QString::number(cursor.position() + 1)); //Character position.
    }

    if (sizeStatus) //For size.
    {
        int chCount = editor->toPlainText().length();
        int lnCount = editor->document()->blockCount();
        sizeStatus->setText("Size " + QString::number(chCount) //Character count.
                            + ", Lines " + QString::number(lnCount)); //Line count.
    }
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

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);

    editor->setPlainText(content); //Passing the file content to the text editor.

    QIcon icon(":/icons/saved"); //Setting the icon.
    ui->editorTabs->addTab(editor, icon,QFileInfo(file).fileName());
    ui->editorTabs->setCurrentWidget(editor);

    UpdateStatusBar(); //Status update.
}

void Editor::NewFile()
{
    CodeEditor *editor = new CodeEditor(); //Handle the CreateNew from external windows.

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);

    QIcon icon(":/icons/saved.png");

    ui->editorTabs->addTab(editor, icon,"Untitled"); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(editor);

    UpdateStatusBar(); //Status update.
}

Editor::~Editor()
{
    delete posStatus;
    delete sizeStatus;
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

    connect(content, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar);
    connect(content, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar); //Connecting the signals for Status.

    QIcon icon(":/icons/saved.png");

    ui->editorTabs->addTab(content, icon, "Untitled"); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(content);
    UpdateStatusBar();
}


void Editor::on_actionSave_As_triggered()
{

}

