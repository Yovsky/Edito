#ifndef FINDANDREPLACE_H
#define FINDANDREPLACE_H

#include "codeeditor.h"
#include <QWidget>
#include <QTextEdit>
#include <QDebug>

namespace Ui {
class FindAndReplace;
}

class FindAndReplace : public QWidget
{
    Q_OBJECT

public:
    explicit FindAndReplace(CodeEditor *editor, QWidget *parent = nullptr);
    ~FindAndReplace();
    void Find();

private slots:
    void on_Find_textChanged(const QString &arg1);

    void on_UseRegularExp_clicked();

    void on_MatchWholeWord_clicked();

    void on_MatchCase_clicked();

private:
    Ui::FindAndReplace *ui;
    CodeEditor *m_editor = nullptr;
};

#endif // FINDANDREPLACE_H
