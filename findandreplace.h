#ifndef FINDANDREPLACE_H
#define FINDANDREPLACE_H

#include "codeeditor.h"
#include <QWidget>

namespace Ui {
class FindAndReplace;
}

class FindAndReplace : public QWidget
{
    Q_OBJECT

public:
    explicit FindAndReplace(CodeEditor *editor, QWidget *parent = nullptr);
    ~FindAndReplace();

private slots:
    void on_Find_textChanged(const QString &arg1);

private:
    Ui::FindAndReplace *ui;
    CodeEditor *m_editor = nullptr;
};

#endif // FINDANDREPLACE_H
