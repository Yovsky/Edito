/*
 * Edito - A modern, cross-platform text editor
 * Copyright (C) 2025 Yovsky <Yovsky@proton.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "editor.h"
#include "edito.h"
#include "gotodialog.h"
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
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDateTime>
#include <QCloseEvent>
#include <QStringDecoder>
#include <QStringEncoder>

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
    , posStatus(nullptr)
    , sizeStatus(nullptr)
    , encStatus(nullptr)
    , zoomLevel(0)
    , statBarVisibility(true)
    , isReadOnly(false)
    , wordWrap(false)
    , m_settings(nullptr)
    , openedTabs(0)
{
    ui->setupUi(this);
    ui->editorTabs->removeTab(0); //Removing the default "Tab 1".
    setAcceptDrops(true);

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
    encStatus = new QLabel("");
    zoomStatus = new QLabel("100%"); //Zoom label.

    ui->statusbar->addPermanentWidget(zoomStatus,2);
    ui->statusbar->addPermanentWidget(posStatus, 18); //Assign the blocks with their sizes.
    ui->statusbar->addPermanentWidget(sizeStatus,9);
    ui->statusbar->addPermanentWidget(encStatus, 3);


    connect(ui->editorTabs, &QTabWidget::currentChanged, this, [this](int index) //Connecting when swiching tabs.
    {
        Q_UNUSED(index);
        CodeEditor *editor = currentEditor();
        bool hasEditor = (editor != nullptr);
        for (QAction *action : encMenu->actions())
        {
            action->setEnabled(hasEditor);
            if (!hasEditor)
            {
                action->setChecked(false);
            }
        }

        if (editor && currentEncodings.contains(editor))
        {
            QString encoding = currentEncodings.value(editor);
            for (QAction *action : encMenu->actions())
            {
                if (action->data().toString() == encoding)
                {
                    action->setChecked(true);
                    break;
                }
            }
        }

        this->UpdateStatusBar();
        this->UpdateUndoRedo();
    });

    CreateEncMenu();
    UpdateStatusBar();

    for (QAction *action : encMenu->actions())
    {
        action->setEnabled(false);
        action->setChecked(false);
    }
}

void Editor::SaveSettings()
{
    m_settings->setValue("Zoom", zoomLevel);
    m_settings->setValue("ToggleStatBar", statBarVisibility);
    m_settings->setValue("ToggleReadOnly", isReadOnly);
    m_settings->setValue("Word Wrap", wordWrap);
    m_settings->setValue("SeccionTabs", seccionTabs);
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
    seccionTabs = m_settings->value("SeccionTabs").toStringList();
    restoreTabs(seccionTabs);
    RestoreZoom(zoomLevel);
    ui->statusbar->setVisible(m_settings->value("StatusBar Apperance", true).toBool());
    ui->actionToggle_Status_Bar->setChecked(m_settings->value("StatusBar Apperance", true).toBool());
    ui->toolBar->setVisible(m_settings->value("ToolBar Apperance", true).toBool());
    ui->actionToggle_Tool_Bar->setChecked(m_settings->value("ToolBar Apperance", true).toBool());
}

void Editor::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void Editor::dropEvent(QDropEvent *event)
{
    const QMimeData *data = event->mimeData();
    if (data->hasUrls())
    {
        QList<QUrl> urls = data->urls();

        for(const QUrl &url : urls)
        {
            QString path = url.toLocalFile();
            if (!path.isEmpty())
                OpenFile(path);
        }
        event->acceptProposedAction();
    }
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
    if (encStatus)
    {
        encStatus->setText(currentEncodings.value(editor, "Unknown"));
    }
}

void Editor::CreateEncMenu()
{
    encMenu = menuBar()->addMenu(tr("&Encoding"));
    menuBar()->insertMenu(ui->menuView->menuAction(), encMenu);

    encActionGrp = new QActionGroup(this);
    encActionGrp->setExclusive(true);
    QStringList supportedEnc
    {
        "UTF-8",
        "UTF-16LE",
        "UTF-16BE",
        "UTF-32LE",
        "UTF-32BE",
        "ISO-8859-1",
        "System"
    };
    for (const QString &enc : supportedEnc)
    {
        QAction *action = encMenu->addAction(enc);

        action->setCheckable(true);
        action->setData(enc);

        connect(action, &QAction::triggered, this, [action, this]() {
            onEncodingActionSelected(action);
        });

        encActionGrp->addAction(action);
    }

    encMenu->actions().first()->setChecked(true);
}

void Editor::onEncodingActionSelected(QAction *selectedAction)
{
    CodeEditor *editor = currentEditor();
    if (!editor)
    {
        for (QAction *action : encMenu->actions())
        {
            action->setChecked(false);
            action->setEnabled(false);
        }
        return;
    }

    for (QAction *action : encMenu->actions())
    {
        action->setEnabled(true);
    }
    currentEncodings.insert(editor, selectedAction->data().toString());
    encStatus->setText(selectedAction->data().toString());
}

void Editor::UpdateUndoRedo()
{
    CodeEditor *editor = currentEditor();
    if (editor)
    {
        UndoApperance(editor->document()->isUndoAvailable());
        RedoApperance(editor->document()->isRedoAvailable());
    }
    else
    {
        ui->actionUndo->setDisabled(true);
        ui->actionRedo->setDisabled(true);
    }
}

void Editor::FileEdited(bool modified)
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(sender());

    if (!editor) return;

    isSaved.insert(editor, !modified);
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

    if(!file.open(QIODevice::ReadOnly)) //File opening.
    {
        QMessageBox::critical(this, "Error", "Failed to open file " + FilePath); //Error handling.
        return;
    }

    //Read content.
    QByteArray data = file.readAll();
    file.close();

    //Detect Encoding.
    encdetector::encodingResult result = encdetector::detectEncoding(data);

    QStringDecoder decoder = QStringDecoder(result.converterEnc);
    QString content = decoder.decode(data);

    if (decoder.hasError())
    {
        QMessageBox::warning(this, "Warning", "Detected encoding may be incorrect, Using fallback encoding.");
        QStringDecoder fallBackDec = QStringDecoder(QStringConverter::Encoding::Utf8);
        content = fallBackDec.decode(data);
        if (fallBackDec.hasError())
        {
            QStringDecoder latin1Dec = QStringDecoder(QStringConverter::Encoding::Latin1);
            content = latin1Dec.decode(data);
        }
    }

    CodeEditor *editor = new CodeEditor();
    isSaved.insert(editor, true);

    editor->editorActions(ui->actionCut, ui->actionCopy, ui->actionPaste, ui->actionSelect_All, ui->actionUPPERCASE, ui->actionLowercase, ui->actionSearch_on_Web); //Pass actions for context menu.

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar); //Connecting signals for Status.
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited); //Connecting signal for Unsaved indicator.
    connect(editor, &QPlainTextEdit::undoAvailable, this, &Editor::UndoApperance);
    connect(editor, &QPlainTextEdit::redoAvailable, this, &Editor::RedoApperance);
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
    currentEncodings.insert(editor, result.encoding);

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

    isSaved.insert(editor, false);
    currentEncodings.insert(editor, "UTF-8");

    for (QAction *action : encMenu->actions())
    {
        action->setEnabled(true);
        if (action->data().toString() == "UTF-8")
        {
            action->setChecked(true);
        }
    }

    editor->editorActions(ui->actionCut, ui->actionCopy, ui->actionPaste, ui->actionSelect_All, ui->actionUPPERCASE, ui->actionLowercase, ui->actionSearch_on_Web); //Pass actions for context menu.
    ui->actionUTF_8->setChecked(true);

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar); //Connecting signals for Status.
    connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
    connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited); //Connecting signal for Unsaved indicator.
    connect(editor, &QPlainTextEdit::undoAvailable, this, &Editor::UndoApperance);
    connect(editor, &QPlainTextEdit::redoAvailable, this, &Editor::RedoApperance);
    connect(editor, &CodeEditor::zoomInRequested, this, &Editor::zoomIn); //Connecting signals for zoom feature.
    connect(editor, &CodeEditor::zoomOutRequested, this, &Editor::zoomOut);
    connect(editor, &CodeEditor::cutRequested, this, &Editor::on_actionCut_triggered); //Connecting signal for shortcuts.
    connect(editor, &CodeEditor::copyRequested, this, &Editor::copySelection);
    connect(editor, &CodeEditor::pasteRequested, this, &Editor::on_actionPaste_triggered);
    connect(editor, &CodeEditor::selectAllRequested, this, &Editor::on_actionSelect_All_triggered);
    connect(editor, &CodeEditor::selectionStateChanged, this, &Editor::selectionTrack); //Connecting signal for selection track.

    QIcon icon(":/icons/saved.png");

    if(openedTabs == 0)
    {
        tabBaseNames.insert(editor, "Untitled"); //Registering the tab name for later use.

        ui->editorTabs->addTab(editor, icon,"Untitled"); //Set tab parameters.
    }
    else
    {
        tabBaseNames.insert(editor, "Untitled-" + QString::number(openedTabs)); //Registering the tab name for later use.

        ui->editorTabs->addTab(editor, icon,"Untitled-" + QString::number(openedTabs)); //Set tab parameters.
    }
    openedTabs++;

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
        currentEncodings.remove(editor);
        isSaved.remove(editor);
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
        QStringConverter::Encoding enc = textToEnc(currentEncodings.value(editor));

        QStringEncoder encoder(enc);
        QByteArray data = encoder.encode(editor->toPlainText());
        if (encoder.hasError())
        {
            QStringEncoder fallBackEnc = QStringEncoder(QStringConverter::Encoding::Utf8);
            data = fallBackEnc.encode(editor->toPlainText());
            if (fallBackEnc.hasError())
                return false;
        }

        file.write(data);
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
            QStringConverter::Encoding enc = textToEnc(currentEncodings.value(editor));

            QStringEncoder encoder(enc);
            QByteArray data = encoder.encode(editor->toPlainText());
            if(encoder.hasError())
            {
                QStringEncoder fallBackEnc = QStringEncoder(QStringConverter::Encoding::Utf8);
                data = fallBackEnc.encode(editor->toPlainText());
                if (fallBackEnc.hasError())
                    return false;
            }

            file.write(data);
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
        if (editor)
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
    saveCurrentTabs();

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

void Editor::on_actionExit_triggered()
{
    close();
}

void Editor::on_actionFull_Screen_toggled(bool arg1)
{
    if (arg1)
        this->setWindowState(Qt::WindowFullScreen);
    else
        this->setWindowState(Qt::WindowNoState);
}

void Editor::UndoApperance(bool ava)
{
    if (ava)
        ui->actionUndo->setEnabled(true);
    else
        ui->actionUndo->setDisabled(true);
}

void Editor::RedoApperance(bool ava)
{
    if (ava)
        ui->actionRedo->setEnabled(true);
    else
        ui->actionRedo->setDisabled(true);
}

void Editor::on_actionUndo_triggered()
{
    CodeEditor *editor = currentEditor();
    if (editor)
        editor->undo();
}

void Editor::on_actionRedo_triggered()
{
    CodeEditor *editor = currentEditor();
    if (editor)
        editor->redo();
}

void Editor::saveCurrentTabs()
{
    seccionTabs.clear();

    for (int i = 0; i < ui->editorTabs->count(); i++)
    {
        CodeEditor *editor = qobject_cast<CodeEditor*>(ui->editorTabs->widget(i));
        if (editor && editor->document()->isModified())
        {
            QString tempPath = saveTempFiles(editor);

            QString seccionData = tempPath + "|";

            if(filePaths.contains(editor) && !filePaths.value(editor).isEmpty())
                seccionData += filePaths.value(editor) + "|";
            else
                seccionData += "|";

            if(editor->document()->isModified())
                seccionData += "1|";
            else
                seccionData += "0|";

            seccionData += tabBaseNames.value(editor, "Unknown") + "|";

            seccionData += currentEncodings.value(editor, "UTF-8");

            seccionTabs.push_back(seccionData);
        }
    }
}

QString Editor::saveTempFiles(CodeEditor *editor)
{
    QString baseName;
    if (filePaths.contains(editor) && !filePaths.value(editor).isEmpty())
    {
        baseName = QFileInfo(filePaths.value(editor)).fileName();
    }
    else
    {
        baseName = tabBaseNames.value(editor, "Untitled");
    }

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                       "/edito_temp_" + QString::number(QDateTime::currentSecsSinceEpoch()) +
                       "_" + tabBaseNames.value(editor);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) //File opening.
    {
        QStringConverter::Encoding enc = textToEnc(currentEncodings.value(editor));
        QStringEncoder encoder = QStringEncoder(enc);

        QByteArray data = encoder.encode(editor->toPlainText());

        if(encoder.hasError())
        {
            QStringEncoder fallBackenc = QStringEncoder(QStringConverter::Encoding::Utf8);
            data = fallBackenc.encode(editor->toPlainText());
        }

        file.write(data);
        file.close();

        return filePath;
    }
    return QString();
}

void Editor::restoreTabs(const QStringList &tabsData)
{
    for (QString seccionData : tabsData)
    {
        QStringList Data = seccionData.split("|");
        if (Data.size() == 5)
        {
            QString tempPath = Data[0];
            QString originalPath = Data[1];
            bool isModified = (Data[2] == "1");
            QString tabName = Data[3];
            QString encoding = Data[4];

            restoreSeccionFile(tempPath, originalPath, isModified, tabName, encoding);
        }
    }
}

void Editor::restoreSeccionFile(const QString &tempPath, const QString &originalPath, const bool &isModified, const QString &tabName, const QString &encoding)
{
    QFile file(tempPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray data = file.readAll();
        QStringDecoder decoder = QStringDecoder(textToEnc(encoding));
        QString content = decoder.decode(data);

        if (decoder.hasError())
        {
            QStringDecoder fallbackDecoder(QStringConverter::Utf8);
            content = fallbackDecoder.decode(data);

            if (fallbackDecoder.hasError())
            {
                QStringDecoder latin1Decoder(QStringConverter::Latin1);
                content = latin1Decoder.decode(data);
                if (latin1Decoder.hasError())
                    QMessageBox::critical(this, "Decoding Error", "Error reading data from last seccion file " + tabName);
            }
        }

        // Create editor with restored content
        CodeEditor *editor = new CodeEditor();
        editor->setPlainText(content);
        editor->document()->setModified(isModified); // Restore modified state

        // Set up editor connections
        editor->editorActions(ui->actionCut, ui->actionCopy, ui->actionPaste,
                              ui->actionSelect_All, ui->actionUPPERCASE,
                              ui->actionLowercase, ui->actionSearch_on_Web);

        connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Editor::UpdateStatusBar);
        connect(editor, &QPlainTextEdit::textChanged, this, &Editor::UpdateStatusBar);
        connect(editor, &QPlainTextEdit::modificationChanged, this, &Editor::FileEdited);
        connect(editor, &QPlainTextEdit::undoAvailable, this, &Editor::UndoApperance);
        connect(editor, &QPlainTextEdit::redoAvailable, this, &Editor::RedoApperance);
        connect(editor, &CodeEditor::zoomInRequested, this, &Editor::zoomIn);
        connect(editor, &CodeEditor::zoomOutRequested, this, &Editor::zoomOut);
        connect(editor, &CodeEditor::cutRequested, this, &Editor::on_actionCut_triggered);
        connect(editor, &CodeEditor::copyRequested, this, &Editor::copySelection);
        connect(editor, &CodeEditor::pasteRequested, this, &Editor::on_actionPaste_triggered);
        connect(editor, &CodeEditor::selectAllRequested, this, &Editor::on_actionSelect_All_triggered);
        connect(editor, &CodeEditor::selectionStateChanged, this, &Editor::selectionTrack);

        // Restore file metadata
        if (!originalPath.isEmpty()) {
            filePaths.insert(editor, originalPath);
            tabBaseNames.insert(editor, tabName);
        } else {
            tabBaseNames.insert(editor, tabName);
        }
        currentEncodings.insert(editor, encoding);

        // Set editor properties
        editor->setReadOnly(isReadOnly);
        editor->setLineWrapMode(wordWrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
        editor->setZoomLevel(zoomLevel);

        // Add to tabs with correct icon and name
        QIcon icon = isModified ? QIcon(":/icons/notsaved.png") : QIcon(":/icons/saved.png");
        QString displayName = isModified ? tabName + "*" : tabName;

        ui->editorTabs->addTab(editor, icon, displayName);

        // Clean up temp file
        QFile::remove(tempPath);
    }
}

void Editor::cleanupTempFiles()
{
    QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    QStringList filePaths = tempDir.entryList(QStringList() << "edito_temp_*", QDir::Files);

    for (const QString &file : filePaths)
    {
        QFileInfo fileInfo(tempDir.filePath(file));
        if(fileInfo.lastModified().daysTo(QDateTime::currentDateTime()) > 1)
            tempDir.remove(file);
    }
}

void Editor::closeEvent(QCloseEvent *event)
{
    SaveSettings();
    saveCurrentTabs();
    event->accept();
}

void Editor::on_actionHome_triggered()
{
    this->close();
    Edito *edito = new Edito();
    edito->setAttribute(Qt::WA_DeleteOnClose);
    edito->show();
    edito->raise();
    edito->activateWindow();
}

void Editor::on_actionRestore_Default_Zoom_triggered()
{
    zoomLevel = 0;
    for (int i = 0; i < ui->editorTabs->count(); i++)
    {
        CodeEditor *editor = qobject_cast<CodeEditor*>(ui->editorTabs->widget(i));
        if(editor)
            editor->setZoomLevel(0);
    }
    UpdateStatusBar();
}

void Editor::on_actionGo_To_triggered()
{
    CodeEditor *editor = currentEditor();

    gotodialog *dialog = new gotodialog(editor, this);
    dialog->setWindowModality(Qt::NonModal);
    dialog->show();
}

QStringConverter::Encoding Editor::textToEnc(const QString &encname)
{

    if (encname == "UTF-8") return QStringConverter::Utf8;
    if (encname == "UTF-16LE") return QStringConverter::Utf16LE;
    if (encname == "UTF-16BE") return QStringConverter::Utf16BE;
    if (encname == "UTF-32LE") return QStringConverter::Utf32LE;
    if (encname == "UTF-32BE") return QStringConverter::Utf32BE;
    if (encname == "ISO-8859-1") return QStringConverter::Latin1;
    if (encname == "System") return QStringConverter::System;

    return QStringConverter::Utf8;
}

void Editor::on_actionToggle_Status_Bar_toggled(bool arg1)
{
    ui->statusbar->setVisible(arg1);
    m_settings->setValue("StatusBar Apperance", arg1);
}

void Editor::on_actionToggle_Tool_Bar_toggled(bool arg1)
{
    ui->toolBar->setVisible(arg1);
    m_settings->setValue("ToolBar Apperance", arg1);
}
