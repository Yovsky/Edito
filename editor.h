#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    void OpenFile(const QString &FilePath);
    void NewFile();
    ~Editor();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

private:
    Ui::Editor *ui;
};

#endif // EDITOR_H
