#include "findandreplace.h"
#include "ui_findandreplace.h"

FindAndReplace::FindAndReplace(CodeEditor *editor, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindAndReplace)
    , m_editor(editor)
{
    ui->setupUi(this);
}

FindAndReplace::~FindAndReplace()
{
    delete ui;
}

void FindAndReplace::on_Find_textChanged(const QString &arg1)
{
    QString target = ui->Find->text();
    QFlags<QTextDocument::FindFlag> flagList;
    if (ui->MatchCase->isChecked()) flagList.setFlag(QTextDocument::FindCaseSensitively);
    QTextCursor cursor = m_editor->document()->find(target, 0, flagList);
    m_editor->setTextCursor(cursor);
}

