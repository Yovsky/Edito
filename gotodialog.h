#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include "codeeditor.h"
#include <QDialog>

namespace Ui {
class gotodialog;
}

class gotodialog : public QDialog
{
    Q_OBJECT

public:
    explicit gotodialog(CodeEditor *editor,QWidget *parent = nullptr);
    void toggleLineOffs();
    void executeGoTo();
    ~gotodialog();

private slots:
    void on_Cancel_clicked();

    void on_Off_m_toggled(bool checked);

    void on_Line_m_toggled(bool checked);

    void on_Ok_clicked();

private:
    Ui::gotodialog *ui;
    CodeEditor *editor;
    int lengthChar;
    int lengthLine;
    bool line_M;
};

#endif // GOTODIALOG_H
