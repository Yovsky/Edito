#include "editor.h"
#include "ui_editor.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QTabWidget>
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
    ui->editorTabs->removeTab(0); //Removing the default "Tab 1".

    posStatus = new QLabel("Line 0, Col 0, Pos 0"); //Position label.
    sizeStatus = new QLabel("Size 0, Lines 0"); //Size label.

    ui->statusbar->addPermanentWidget(posStatus, 2);
    ui->statusbar->addPermanentWidget(sizeStatus, 1); //Assign the blocks with their sizes.

    connect(ui->editorTabs, &QTabWidget::currentChanged, this, [this](int index) { //Connecting when swiching tabs.
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

void Editor::FileEdited(bool modified)
{
    QWidget *current = ui->editorTabs->currentWidget();
    CodeEditor *editor = qobject_cast<CodeEditor*>(current);

    if(editor || ui->editorTabs->count() != 0) //Safety.
    {
        int currentTab = ui->editorTabs->currentIndex(); //Setting variables.
        QIcon icon(":/icons/saved.png");
        QIcon nicon(":/icons/notsaved.png");
        QString fileName = tabBaseNames.value(editor, "Unknown");

        if(modified)
        {
            ui->editorTabs->setTabIcon(currentTab, nicon); //Changing the icon (Red).
            ui->editorTabs->setTabText(currentTab, fileName + "*"); //Adding "*" to the name.
        }
        else
        {
            ui->editorTabs->setTabIcon(currentTab, icon); //Changing the icon (Green).
            ui->editorTabs->setTabText(currentTab, fileName); //Returning to default name.
        }
    }
}

void Editor::OpenFile(const QString &FilePath)
{
    QFile file(FilePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) //File opening.
        QMessageBox::critical(this, "Error", "Failed to open file " + FilePath); //Error handling.

    QTextStream in(&file); //Text setup.
    in.setEncoding(QStringConverter::Utf8);//Setting the encoding.
    QString content = in.readAll();
    CodeEditor *editor = new CodeEditor();

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar); //Connecting signals for Status.
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited); //Connecting signal for Unsaved indicator.

    editor->setPlainText(content); //Passing the file content to the text editor.

    QIcon icon(":/icons/saved"); //Setting the icon.

    tabBaseNames.insert(editor, QFileInfo(file).fileName()); //Registering the tab name for later use.
    filePaths.insert(editor,FilePath); //Registering the file path for later use.

    ui->editorTabs->addTab(editor, icon,QFileInfo(file).fileName()); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(editor);

    UpdateStatusBar(); //Status update.
}

void Editor::NewFile()
{
    CodeEditor *editor = new CodeEditor(); //Handle the CreateNew from external windows.

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar); //Connecting signals for Status.
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited); //Connecting signal for Unsaved indicator.

    QIcon icon(":/icons/saved.png");

    tabBaseNames.insert(editor, "Untitled"); //Registering the tab name for later use.

    ui->editorTabs->addTab(editor, icon,"Untitled"); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(editor);

    UpdateStatusBar(); //Status update.
}

void Editor::CloseTab(int index)
{
    ui->editorTabs->removeTab(index); //Close the tab.
}

void Editor::SaveAs(CodeEditor* editor)
{
    QString Deflocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (Deflocation.isEmpty()) Deflocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As..."), Deflocation, tr("Text Files (*.txt)")); //Obtaining new file path.

    filePaths.insert(editor, filePath); //Registering file path for later use.
    tabBaseNames.insert(editor, QFileInfo(filePath).fileName());

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) //File opening.
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << editor->toPlainText();
        file.close();
        FileEdited(true);
    }
    else
        QMessageBox::critical(this, "Error", "Failed to save as " + filePath);
}

void Editor::Save(CodeEditor* editor)
{
    if(filePaths.value(editor).isEmpty()) //If the file does not exist (no path).
        SaveAs(editor);
    else
    {
        QFile file(filePaths.value(editor)); //Pass the file path.
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream.setEncoding(QStringConverter::Utf8); //Encoding setting.
            stream << editor->toPlainText(); //Writing text.
            file.close();
            FileEdited(true);
        }
        else
            QMessageBox::critical(this, "Error", "Failed to save file."); //Error handling.
    }
}

void Editor::on_actionOpen_triggered()
{
    QString DefLocation =  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); //Get the Documents folder.
    if(DefLocation.isEmpty()) DefLocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation); //Fallback to Home if not existing.
    QString FileOpened = QFileDialog::getOpenFileName(this, tr("Open File"), DefLocation, tr("Text Files (*.txt)")); //File path.

    if(!FileOpened.isEmpty()) //Safety.
    {
        qDebug() << "opended: " << FileOpened;
        this->OpenFile(FileOpened);
    }
    else QMessageBox::critical(this, "Error", "Failed to open from a file."); //Handle opening errors.
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
    CodeEditor *editor = currentEditor();
    if (editor) //safety.
        SaveAs(editor); //Pass to SaveAs.
}


void Editor::on_editorTabs_tabCloseRequested(int index)
{
    CloseTab(index); //Send the signal to the function.
}

CodeEditor* Editor::currentEditor() const
{
    return qobject_cast<CodeEditor*>(ui->editorTabs->currentWidget());
}

Editor::~Editor()
{
    delete posStatus;
    delete sizeStatus;
    delete ui;
}
