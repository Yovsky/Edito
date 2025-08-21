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
    void UpdateStatusBar();
    void NewFile();
    void CloseTab(int index);
    void FileEdited(bool edited);
    bool SaveAs(CodeEditor* editor);
    bool Save(CodeEditor* editor);
    CodeEditor* currentEditor() const;
    ~Editor();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_As_triggered();

    void on_editorTabs_tabCloseRequested(int index);

    void on_actionSave_triggered();

private:
    Ui::Editor *ui;
    QLabel *posStatus;
    QLabel *sizeStatus;
    QHash<CodeEditor*, QString> tabBaseNames;
    QHash<CodeEditor*, QString> filePaths;
};

#endif // EDITOR_H
