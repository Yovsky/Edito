#include "editor.h"
#include "preferencesdialog.h"
#include "qtextobject.h"
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
#include <QDialog>
#include <QRegularExpression>
#include <QSettings>
#include <QClipboard>
#include <QAction>
#include <QDesktopServices>

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
    , posStatus(nullptr)
    , sizeStatus(nullptr)
    , zoomLevel(0)
    , statBarVisibility(true)
    , isReadOnly(false)
    , wordWrap(false)
    , m_settings(nullptr)
{
    ui->setupUi(this);
    ui->editorTabs->removeTab(0); //Removing the default "Tab 1".

    m_settings = new QSettings("Yovsky", "Edito");
    if (!m_settings->contains("SE"))
    {
        m_settings->setValue("SE", "https://www.google.com/search?q=");
        m_settings->setValue("SEToggled", 1);
        m_settings->sync();
    }

    LoadSettings(); //Load saved settings.

    statusBarApperance(statBarVisibility); //Show/Hide status bar on beggining.
    posStatus = new QLabel("Line 0, Col 0, Pos 0"); //Position label.
    sizeStatus = new QLabel("Size 0, Lines 0"); //Size label.
    zoomStatus = new QLabel("100%"); //Zoom label.

    ui->statusbar->addPermanentWidget(zoomStatus,1);
    ui->statusbar->addPermanentWidget(posStatus, 6); //Assign the blocks with their sizes.
    ui->statusbar->addPermanentWidget(sizeStatus,3);


    connect(ui->editorTabs, &QTabWidget::currentChanged, this, [this](int index) { //Connecting when swiching tabs.
        Q_UNUSED(index);
        this->UpdateStatusBar();
    });

    UpdateStatusBar();
}

void Editor::SaveSettings()
{
    m_settings->setValue("Zoom", zoomLevel);
    m_settings->setValue("ToggleStatBar", statBarVisibility);
    m_settings->setValue("ToggleReadOnly", isReadOnly);
    m_settings->setValue("Word Wrap", wordWrap);
    qDebug() << "SAVING - ReadOnly:" << isReadOnly;
    qDebug() << "Settings file:" << m_settings->fileName();
    m_settings->sync();
    qDebug() << "Settings status:" << m_settings->status();
}

void Editor::LoadSettings()
{
    zoomLevel = m_settings->value("Zoom", 0).toInt();
    statBarVisibility = m_settings->value("ToggleStatBar", true).toBool();
    isReadOnly = m_settings->value("ToggleReadOnly", false).toBool();
    applyReadOnly(isReadOnly);
    wordWrap = m_settings->value("Word Wrap", false).toBool();
    toggleWordWrap(wordWrap);

    qDebug() << "Loaded ReadOnly:" << isReadOnly;

    qDebug() << "LOADING - Zoom level:" << zoomLevel;
    qDebug() << "Settings file:" << m_settings->fileName();
    RestoreZoom(zoomLevel);
}

void Editor::selectionTrack(bool hasSelection)
{
    qDebug() << "selection track called";
    ui->actionUPPERCASE->setEnabled(hasSelection);
    ui->actionLowercase->setEnabled(hasSelection);
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
        QString text = editor->toPlainText();
        QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        int chCount = text.length();
        int lnCount = editor->document()->blockCount();
        int wCount = words.size();
        sizeStatus->setText("Size " + QString::number(chCount) //Character count.
                            + ", Words " + QString::number(wCount) //Word count.
                            + ", Lines " + QString::number(lnCount)); //Line count.
    }
    if (zoomStatus)
    {
        zoomStatus->setText(QString::number(100 + zoomLevel * 10) + "%");
    }
}

void Editor::FileEdited(bool modified)
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(sender());

    if (!editor) return;

    resetTabState(editor, modified);
}

void Editor::resetTabState(CodeEditor *editor,bool modified)
{
    if (!editor) return; //Safety.

    int tabIndex = ui->editorTabs->indexOf(editor);
    if (tabIndex == -1) return; //Safety.

    if(editor || ui->editorTabs->count() != 0) //Safety.
    {
        QIcon icon(":/icons/saved.png"); //Setting variables.
        QIcon nicon(":/icons/notsaved.png");
        QString fileName = tabBaseNames.value(editor, "Unknown");

        if(modified)
        {
            ui->editorTabs->setTabIcon(tabIndex, nicon); //Changing the icon (Red).
            ui->editorTabs->setTabText(tabIndex, fileName + "*"); //Adding "*" to the name.
        }
        else
        {
            ui->editorTabs->setTabIcon(tabIndex, icon); //Changing the icon (Green).
            ui->editorTabs->setTabText(tabIndex, fileName); //Returning to default name.
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

    editor->editorActions(ui->actionCut, ui->actionCopy, ui->actionPaste, ui->actionSelect_All, ui->actionUPPERCASE, ui->actionLowercase, ui->actionSearch_on_Web); //Pass actions for context menu.

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar); //Connecting signals for Status.
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited); //Connecting signal for Unsaved indicator.
    connect(editor, &CodeEditor::zoomInRequested, this, &Editor::zoomIn); //Connecting signals for zoom feature.
    connect(editor, &CodeEditor::zoomOutRequested, this, &Editor::zoomOut);
    connect(editor, &CodeEditor::cutRequested, this, &Editor::on_actionCut_triggered); //Connecting signal for shortcuts.
    connect(editor, &CodeEditor::copyRequested, this, &Editor::copySelection);
    connect(editor, &CodeEditor::pasteRequested, this, &Editor::on_actionPaste_triggered);
    connect(editor, &CodeEditor::selectAllRequested, this, &Editor::on_actionSelect_All_triggered);
    connect(editor, &CodeEditor::selectionStateChanged, this, &Editor::selectionTrack); //Connecting signal for selection track.

    editor->setPlainText(content); //Passing the file content to the text editor.
    editor->setReadOnly(isReadOnly); //Read-Only state.
    editor->setLineWrapMode(wordWrap? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap); //Word Wrap state.

    QIcon icon(":/icons/saved"); //Setting the icon.

    tabBaseNames.insert(editor, QFileInfo(file).fileName()); //Registering the tab name for later use.
    filePaths.insert(editor,FilePath); //Registering the file path for later use.

    ui->editorTabs->addTab(editor, icon,QFileInfo(file).fileName()); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(editor);

    editor->setZoomLevel(zoomLevel);
    RestoreZoom(zoomLevel); //Set zoom.

    resetTabState(editor, false);
    editor->document()->setModified(false);

    UpdateStatusBar(); //Status update.
}

void Editor::NewFile()
{
    CodeEditor *editor = new CodeEditor(); //Handle the CreateNew from external windows.

    editor->editorActions(ui->actionCut, ui->actionCopy, ui->actionPaste, ui->actionSelect_All, ui->actionUPPERCASE, ui->actionLowercase, ui->actionSearch_on_Web); //Pass actions for context menu.

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar); //Connecting signals for Status.
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited); //Connecting signal for Unsaved indicator.
    connect(editor, &CodeEditor::zoomInRequested, this, &Editor::zoomIn); //Connecting signals for zoom feature.
    connect(editor, &CodeEditor::zoomOutRequested, this, &Editor::zoomOut);
    connect(editor, &CodeEditor::cutRequested, this, &Editor::on_actionCut_triggered); //Connecting signal for shortcuts.
    connect(editor, &CodeEditor::copyRequested, this, &Editor::copySelection);
    connect(editor, &CodeEditor::pasteRequested, this, &Editor::on_actionPaste_triggered);
    connect(editor, &CodeEditor::selectAllRequested, this, &Editor::on_actionSelect_All_triggered);
    connect(editor, &CodeEditor::selectionStateChanged, this, &Editor::selectionTrack); //Connecting signal for selection track.

    QIcon icon(":/icons/saved.png");

    tabBaseNames.insert(editor, "Untitled"); //Registering the tab name for later use.

    ui->editorTabs->addTab(editor, icon,"Untitled"); //Set tab parameters.
    ui->editorTabs->setCurrentWidget(editor);

    editor->setZoomLevel(zoomLevel); //Set Zoom.
    editor->setReadOnly(isReadOnly); //Read-Only state.
    editor->setLineWrapMode(wordWrap? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);

    RestoreZoom(zoomLevel); //Set zoom.

    editor->document()->setModified(false);
    resetTabState(editor, false);

    UpdateStatusBar(); //Status update.
}

void Editor::CloseTab(int index)
{
    if (index < 0 || index > ui->editorTabs->count()) return; //Safety.

    CodeEditor *editor = qobject_cast<CodeEditor*>(ui->editorTabs->widget(index));
    if (!editor) return; //Safety.

    if (!editor->document()->isModified()) //File is saved.
        ui->editorTabs->removeTab(index); //Close the tab.
    else //File is not saved.
    {
        QString fileName = tabBaseNames.value(editor, "Unkown"); //Get the file name.

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Unsaved Changes", //Message title.
                                      QString("Save changes to \"%1\" before closing?").arg(fileName), //Message.
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel); //Message buttons.

        if (reply == QMessageBox::Save)
        {
            if(!Save(editor)) return; //Pass to save and handle failure.
        }
        else if (reply == QMessageBox::Cancel)
        {
            return; //User canceled
        }

        //cleaning.
        ui->editorTabs->removeTab(index); //Close the tab.
        tabBaseNames.remove(editor); //Remove tab data.
        filePaths.remove(editor);
        delete editor;
    }
}

bool Editor::SaveAs(CodeEditor* editor)
{
    QString Deflocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (Deflocation.isEmpty()) Deflocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As..."), Deflocation, tr("Text Files (*.txt)")); //Obtaining new file path.

    if (filePath.isEmpty())
        return false; //File not saved.

    filePaths.insert(editor, filePath); //Registering file path for later use.
    tabBaseNames.insert(editor, QFileInfo(filePath).fileName());

    int tabIndex = ui->editorTabs->indexOf(editor);
    ui->editorTabs->setTabText(tabIndex, QFileInfo(filePath).fileName()); //Changes tab title to new file name.

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) //File opening.
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << editor->toPlainText();
        file.close();

        editor->document()->setModified(false); //Set file as saved.
        resetTabState(editor, false);
        return true; //File saved.
    }
    return false; //Save failed.
}

bool Editor::Save(CodeEditor* editor)
{
    if(filePaths.value(editor).isEmpty()) //If the file does not exist (no path).
    {
        if(!SaveAs(editor))
        {
            QMessageBox::critical(this, "Error", "Failed to save file."); //Error handling.
            return false; //SaveAs failed.
        }
        return true; //SaveAs passed.
    }
    else
    {
        QFile file(filePaths.value(editor)); //Pass the file path.
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream.setEncoding(QStringConverter::Utf8); //Encoding setting.
            stream << editor->toPlainText(); //Writing text.
            file.close();

            editor->document()->setModified(false); //Set file as saved.
            resetTabState(editor, false);
            return true; //Save sucessful.
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to save file."); //Error handling.
            return false; //Save failed.
        }
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
        OpenFile(FileOpened);
    }
    else QMessageBox::critical(this, "Error", "Failed to open from a file."); //Handle opening errors.
}


void Editor::on_actionNew_triggered()
{
    NewFile();
}


void Editor::on_actionSave_As_triggered()
{
    CodeEditor *editor = currentEditor();
    if (editor) //Safety.
        SaveAs(editor); //Pass to SaveAs.
}

void Editor::on_actionSave_triggered()
{
    CodeEditor *editor = currentEditor();
    if(editor) //Safety.
        Save(editor); //Pass to Save.
}

void Editor::on_editorTabs_tabCloseRequested(int index)
{
    CloseTab(index); //Send the signal to the function.
}

CodeEditor* Editor::currentEditor() const
{
    return qobject_cast<CodeEditor*>(ui->editorTabs->currentWidget());
}

void Editor::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(m_settings, statBarVisibility, this);
    connect(&dialog, &PreferencesDialog::toggleStatusBarReq, this, &Editor::statusBarApperance); //Signal to toggle the status bar.
    dialog.exec(); //Open preferences.
}

void Editor::RestoreZoom(int zoom)
{
    for (int i = 0; i < ui->editorTabs->count(); i++)
    {
        CodeEditor *editor = qobject_cast<CodeEditor*>(ui->editorTabs->widget(i));
        if (editor) //Safety.
            editor->setZoomLevel(zoom);
    }
}

void Editor::zoomIn()
{
    zoomLevel++; //Track zoom level to save.
    RestoreZoom(zoomLevel);
    SaveSettings();
    UpdateStatusBar();
}

void Editor::zoomOut()
{
    zoomLevel--; //Track zoom level to save.
    if (zoomLevel < -9) zoomLevel = -9; //Nothing below 10%.
    RestoreZoom(zoomLevel);
    SaveSettings();
    UpdateStatusBar();
}

void Editor::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();  // This updates ALL editors and saves
        } else {
            zoomOut(); // This updates ALL editors and saves
        }
        event->accept();
    } else {
        QMainWindow::wheelEvent(event);
    }
    UpdateStatusBar();
}

void Editor::on_actionZoom_In_triggered()
{
    zoomIn(); //Call zoom in.
}

void Editor::on_actionZoom_Out_triggered()
{
    zoomOut(); //Call zoom out.
}

void Editor::statusBarApperance(bool Visibility)
{
    ui->statusbar->setVisible(Visibility);
    statBarVisibility = Visibility;
}

void Editor::on_actionToggle_Read_Only_toggled(bool arg1)
{
    applyReadOnly(arg1);
    isReadOnly = arg1;
    SaveSettings();
}

void Editor::applyReadOnly(bool isRO)
{
    for (int i = 0; i < ui->editorTabs->count(); i++)
    {
        QWidget *currentWidget = ui->editorTabs->widget(i);
        CodeEditor *editor = qobject_cast<CodeEditor*>(currentWidget);
        editor->setReadOnly(isRO);
    }
    ui->actionToggle_Read_Only->setChecked(isRO);
}
void Editor::on_actionAlways_On_Top_toggled(bool arg1)
{
    setWindowFlag(Qt::WindowStaysOnTopHint, arg1); //Always On Top feature.
    show();
}

void Editor::toggleWordWrap(bool Wrap)
{
    for (int i = 0; i < ui->editorTabs->count(); i++)
    {
        CodeEditor *editor = qobject_cast<CodeEditor*>(ui->editorTabs->widget(i));
        if (Wrap) editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        else editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
    ui->actionWord_Wrap->setChecked(Wrap);
}

void Editor::on_actionWord_Wrap_toggled(bool arg1)
{
    toggleWordWrap(arg1);
    wordWrap = arg1;
    SaveSettings();
}

void Editor::copySelection()
{
    CodeEditor *editor = currentEditor();
    if(editor)
    {
        QTextCursor cursor = editor->textCursor();
        if (!cursor.hasSelection())
        {
            cursor.select(QTextCursor::LineUnderCursor);
        }
        QString text = cursor.selectedText();
        QApplication::clipboard()->setText(text);
    }
}

Editor::~Editor()
{
    SaveSettings(); //Save current configuration.

    delete posStatus;
    delete sizeStatus;
    delete ui;
}
void Editor::on_actionCopy_triggered()
{
    copySelection();
}

void Editor::on_actionSelect_All_triggered()
{
    CodeEditor *editor = currentEditor();
    editor->selectAll();
}

void Editor::on_actionDate_and_Time_Short_triggered()
{
    CodeEditor *editor = currentEditor();
    if (editor) //Safety.
    {
        QDateTime current = QDateTime::currentDateTime();
        QString dateTime = current.toString("hh:mm d/M/yyyy");
        editor->insertPlainText(dateTime);
    }
}

void Editor::on_actionDate_and_Time_Long_triggered()
{
    CodeEditor *editor = currentEditor();
    if (editor) //Safety.
    {
        QDateTime current = QDateTime::currentDateTime();
        QString dateTime = current.toString("hh:mm AP dddd, MMMM dd, yyyy");
        editor->insertPlainText(dateTime);
    }
}

void Editor::on_actionDuplicate_Line_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        editor->setTextCursor(cursor);
    }
}

void Editor::on_actionDelete_line_triggered()
{
    CodeEditor *editor = currentEditor();
    if (editor)
    {
        QTextCursor cursor = editor->textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        editor->setTextCursor(cursor);
    }
}

void Editor::on_actionDelete_triggered()
{
    if (CodeEditor *editor = currentEditor())
        editor->textCursor().deleteChar();
}

void Editor::on_actionDelete_Word_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();

        QString originalTrimmed = cursor.block().text().trimmed();
        int originalPos = cursor.position();

        cursor.select(QTextCursor::WordUnderCursor);
        cursor.removeSelectedText();

        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        while(cursor.selectedText() == " ")
        {
            cursor.removeSelectedText();
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }

        cursor.setPosition(originalPos);
        if ((cursor.block().text().isEmpty() || cursor.block().text().trimmed().isEmpty()) && originalTrimmed.isEmpty())
        {
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.removeSelectedText();
            cursor.deleteChar();
        }
        else
        {
            cursor.clearSelection();
            cursor.movePosition(QTextCursor::Left);
        }

        editor->setTextCursor(cursor);
    }
}

void Editor::on_actionDelete_to_End_of_Line_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();

        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();

        editor->setTextCursor(cursor);
    }
}

void Editor::on_actionUPPERCASE_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();
        if (cursor.hasSelection())
        {
            int selectionStart = cursor.selectionStart(); //Save user selection.
            int selectionEnd = cursor.selectionEnd();

            QString text = cursor.selectedText().toUpper();
            cursor.insertText(text);

            cursor.setPosition(selectionStart); //Restore user selection.
            cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);

            editor->setTextCursor(cursor);
        }
    }
}

void Editor::on_actionLowercase_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();

        int selectionStart = cursor.selectionStart(); //Save user selection.
        int selectionEnd = cursor.selectionEnd();

        QString text = cursor.selectedText().toLower();
        cursor.insertText(text);

        cursor.setPosition(selectionStart); //Restore user selection.
        cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);

        editor->setTextCursor(cursor);
    }
}

void Editor::on_actionPaste_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();
        QString text = QApplication::clipboard()->text();

        cursor.insertText(text);
        editor->setTextCursor(cursor);
    }
}

void Editor::on_actionCut_triggered()
{
    if (CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();

        if(cursor.hasSelection())
        {
            QString text = cursor.selectedText();
            cursor.removeSelectedText();
            QApplication::clipboard()->setText(text);
            editor->setTextCursor(cursor);
        }
        else
        {
            cursor.select(QTextCursor::LineUnderCursor);
            QString text = cursor.selectedText();
            cursor.removeSelectedText();
            QApplication::clipboard()->setText(text);
            editor->setTextCursor(cursor);
        }
    }
}

void Editor::on_actionSearch_on_Web_triggered()
{
    if(CodeEditor *editor = currentEditor())
    {
        QTextCursor cursor = editor->textCursor();
        QString text;
        if (cursor.hasSelection())
            text = cursor.selectedText();
        else
        {
            cursor.select(QTextCursor::WordUnderCursor);
            if (cursor.hasSelection())
                text = cursor.selectedText();
            else
            {
                cursor.select(QTextCursor::LineUnderCursor);
                text = cursor.selectedText();
            }
        }
        QString URL = m_settings->value("SE", "https://www.google.com/search?q=").toString();

        QUrl url(URL + QUrl::toPercentEncoding(text));
        QDesktopServices::openUrl(url);
    }
}

void Editor::on_actionCopy_Current_File_Path_triggered()
{
    CodeEditor *editor = currentEditor();
    QApplication::clipboard()->setText(filePaths.value(editor));
}

void Editor::on_actionCopy_Current_Filename_triggered()
{
    CodeEditor *editor = currentEditor();
    QApplication::clipboard()->setText(QFileInfo(filePaths.value(editor)).fileName());
}


void Editor::on_actionCopy_Current_Dir_triggered()
{
    CodeEditor *editor = currentEditor();
    QApplication::clipboard()->setText(QFileInfo(filePaths.value(editor)).absolutePath());
}


void Editor::on_actionClose_triggered()
{
    CloseTab(ui->editorTabs->currentIndex());
}


void Editor::on_actionClose_All_triggered()
{
    while (ui->editorTabs->count() > 0)
        CloseTab(0);
}


void Editor::on_actionSave_All_triggered()
{
    for (int i = 0; i < ui->editorTabs->count(); i++)
    {
        QWidget *current = ui->editorTabs->widget(i);
        CodeEditor *editor = qobject_cast<CodeEditor*>(current);
        Save(editor);
    }
}

