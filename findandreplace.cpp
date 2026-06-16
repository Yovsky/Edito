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

    QList<QTextEdit::ExtraSelection> selections;
    bool hasNext = true;
    int found = 0;
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
        selections.append(selection);
        last = cursor.selectionEnd();
        found++;
    }

    if (found == 0)
        ui->label->setText("No results");
    else
        ui->label->setText("Found: " + QString::number(found));
    m_editor->setExtraSelections(selections);
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

