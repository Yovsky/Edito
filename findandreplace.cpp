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
    m_selections.clear();
    QString target = ui->Find->text();
    QFlags<QTextDocument::FindFlag> flagList;
    if (ui->MatchCase->isChecked()) flagList.setFlag(QTextDocument::FindCaseSensitively);
    if (ui->MatchWholeWord->isChecked()) flagList.setFlag(QTextDocument::FindWholeWords);

    bool hasNext = true;
    int last = 0;
    while(hasNext)
    {
        QTextCursor cursor = m_editor->document()->find(target, last, flagList);
        if(cursor.isNull())
        {
            hasNext = false;
            break;
        }
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(Qt::blue));
        selection.cursor = cursor;
        m_selections.append(selection);
        last = cursor.selectionEnd();
    }

    if (m_selections.size() == 0)
        ui->label->setText("No results");
    else
    {
        m_index = 0;
        ui->label->setText("1 of " + QString::number(m_selections.size()));
        m_editor->setTextCursor(m_selections[0].cursor);
        m_editor->setExtraSelections(m_selections);
    }
}

void FindAndReplace::ChangeSelection()
{
    ui->label->setText(QString::number(m_index + 1) + " of " + QString::number(m_selections.size()));
    m_editor->setTextCursor(m_selections[m_index].cursor);
    m_editor->setExtraSelections(m_selections);
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

void FindAndReplace::on_Up_clicked()
{
    m_index--;
    if (m_index < 0)
        m_index = m_selections.size() - 1;
    ChangeSelection();
}

void FindAndReplace::on_Down_clicked()
{
    m_index++;
    if (m_index >= m_selections.size())
        m_index = 0;
    ChangeSelection();
}
