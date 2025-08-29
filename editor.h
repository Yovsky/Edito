#ifndef EDITOR_H
#define EDITOR_H

#include "codeeditor.h"
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
    void wheelEvent(QWheelEvent *event);
    void statusBarApperance(bool Visibility);
    void applyReadOnly(bool isRO);
    void toggleWordWrap(bool Wrap);
    void copySelection();
    void selectionTrack(bool hasSelection);
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

private:
    Ui::Editor *ui;
    QLabel *posStatus;
    QLabel *sizeStatus;
    QLabel *zoomStatus;
    QHash<CodeEditor*, QString> tabBaseNames;
    QHash<CodeEditor*, QString> filePaths;
    int zoomLevel;
    bool statBarVisibility;
    bool isReadOnly;
    bool wordWrap;
    QSettings *m_settings;

signals:
    void toggleStatusBarReq(bool Visibility);
};

#endif // EDITOR_H
