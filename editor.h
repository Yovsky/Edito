#ifndef EDITOR_H
#define EDITOR_H

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

#include "codeeditor.h"
#include "encdetector.h"
#include <QMenu>
#include <QActionGroup>
#include <QMainWindow>
#include <QLabel>
#include <QSettings>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    void OpenFile(const QString &FilePath);
    void SaveSettings();
    void LoadSettings();
    void UpdateStatusBar();
    void CreateEncMenu();
    void onEncodingActionSelected(QAction *selectedAction);
    void UpdateUndoRedo();
    void NewFile();
    void CloseTab(int index);
    void FileEdited(bool edited);
    void resetTabState(CodeEditor *editor, bool edited);
    bool SaveAs(CodeEditor* editor);
    bool Save(CodeEditor* editor);
    CodeEditor* currentEditor() const;
    void RestoreZoom(int zoom);
    void zoomIn();
    void zoomOut();
    void statusBarApperance(bool Visibility);
    void applyReadOnly(bool isRO);
    void toggleWordWrap(bool Wrap);
    void copySelection();
    void selectionTrack(bool hasSelection);
    void encodingTrack();
    void UndoApperance(bool ava);
    void RedoApperance(bool ava);
    void saveCurrentTabs();
    QString saveTempFiles(CodeEditor *editor);
    void restoreTabs(const QStringList &tabsData);
    void restoreSeccionFile(const QString &tempPath, const QString &originalPath, const bool &isModified, const QString &tabName, const QString &encoding);
    void cleanupTempFiles();
    QStringConverter::Encoding textToEnc(const QString &encname);
    ~Editor();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_As_triggered();

    void on_editorTabs_tabCloseRequested(int index);

    void on_actionSave_triggered();

    void on_actionPreferences_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionToggle_Read_Only_toggled(bool arg1);

    void on_actionAlways_On_Top_toggled(bool arg1);

    void on_actionWord_Wrap_toggled(bool arg1);

    void on_actionCopy_triggered();

    void on_actionSelect_All_triggered();

    void on_actionDate_and_Time_Short_triggered();

    void on_actionDate_and_Time_Long_triggered();

    void on_actionDuplicate_Line_triggered();

    void on_actionDelete_line_triggered();

    void on_actionDelete_triggered();

    void on_actionDelete_Word_triggered();

    void on_actionDelete_to_End_of_Line_triggered();

    void on_actionUPPERCASE_triggered();

    void on_actionLowercase_triggered();

    void on_actionPaste_triggered();

    void on_actionCut_triggered();

    void on_actionSearch_on_Web_triggered();

    void on_actionCopy_Current_File_Path_triggered();

    void on_actionCopy_Current_Filename_triggered();

    void on_actionCopy_Current_Dir_triggered();

    void on_actionClose_triggered();

    void on_actionClose_All_triggered();

    void on_actionSave_All_triggered();

    void on_actionExit_triggered();

    void on_actionFull_Screen_toggled(bool arg1);

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionHome_triggered();

    void on_actionRestore_Default_Zoom_triggered();

    void on_actionGo_To_triggered();

    void on_actionToggle_Status_Bar_toggled(bool arg1);

    void on_actionToggle_Tool_Bar_toggled(bool arg1);

private:
    Ui::Editor *ui;
    QLabel *posStatus;
    QLabel *sizeStatus;
    QLabel *encStatus;
    QLabel *zoomStatus;
    QHash<CodeEditor*, QString> tabBaseNames;
    QHash<CodeEditor*, QString> filePaths;
    int zoomLevel;
    bool statBarVisibility;
    bool isReadOnly;
    bool wordWrap;
    QSettings *m_settings;
    int openedTabs;
    QHash<CodeEditor*, bool> isSaved;
    QVector<QString> seccionTabs;
    QHash<CodeEditor*, QString> currentEncodings;
    QHash<CodeEditor*, bool> hasBOM;
    QMenu *encMenu;
    QActionGroup *encActionGrp;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void toggleStatusBarReq(bool Visibility);
    void selectionChanged();
};

#endif // EDITOR_H
