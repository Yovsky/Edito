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
    void ChangeSelection();

private slots:
    void on_Find_textChanged(const QString &arg1);

    void on_UseRegularExp_clicked();

    void on_MatchWholeWord_clicked();

    void on_MatchCase_clicked();

    void on_Up_clicked();

    void on_Down_clicked();

private:
    Ui::FindAndReplace *ui;
    CodeEditor *m_editor = nullptr;
    QList<QTextEdit::ExtraSelection> m_selections;
    int m_index;
};

#endif // FINDANDREPLACE_H
