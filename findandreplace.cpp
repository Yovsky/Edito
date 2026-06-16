#include "findandreplace.h"
#include "ui_findandreplace.h"

FindAndReplace::FindAndReplace(CodeEditor *editor, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindAndReplace)
    , m_editor(editor)
{
    ui->setupUi(this);
}

void FindAndReplace::Find()
{
    QString target = ui->Find->text();
    QFlags<QTextDocument::FindFlag> flagList;
    if (ui->MatchCase->isChecked()) flagList.setFlag(QTextDocument::FindCaseSensitively);
    if (ui->MatchWholeWord->isChecked()) flagList.setFlag(QTextDocument::FindWholeWords);
    QTextCursor cursor = m_editor->document()->find(target, 0, flagList);
    m_editor->setTextCursor(cursor);
}

FindAndReplace::~FindAndReplace()
{
    delete ui;
}

void FindAndReplace::on_Find_textChanged(const QString &arg1)
{
    Find();
}

void FindAndReplace::on_UseRegularExp_clicked()
{
    Find();
}


void FindAndReplace::on_MatchWholeWord_clicked()
{
    Find();
}


void FindAndReplace::on_MatchCase_clicked()
{
    Find();
}

