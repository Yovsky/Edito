#ifndef EDITOR_H
#define EDITOR_H

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
    ~Editor();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_As_triggered();

private:
    Ui::Editor *ui;
    QLabel *posStatus;
    QLabel *sizeStatus;
};

#endif // EDITOR_H
