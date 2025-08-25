#ifndef EDITOR_H
#define EDITOR_H

#include "codeeditor.h"
#include <QMainWindow>
#include <QLabel>

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

signals:
    void toggleStatusBarReq(bool Visibility);
};

#endif // EDITOR_H
