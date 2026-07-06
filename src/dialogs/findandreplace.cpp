#include "findandreplace.h"
#include "ui_findandreplace.h"

FindAndReplace::FindAndReplace(CodeEditor *editor, QString selected, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindAndReplace)
    , m_editor(editor)
{
    ui->setupUi(this);

    // destroy the object on closure
    setAttribute(Qt::WA_DeleteOnClose);

    ui->Down->setDisabled(true);
    ui->Up->setDisabled(true);

    // add the selected text by the user to find box
    ui->Find->setText(selected);
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
        QTextCursor cursor;
        if (ui->UseRegularExp->isChecked())
        {
            QRegularExpression regex = QRegularExpression(target);
            if(!regex.isValid())
            {
                ui->label->setText(regex.errorString());
                return;
            }
            cursor = m_editor->document()->find(regex, last, flagList);
        }
        else
            cursor = m_editor->document()->find(target, last, flagList);
        if(cursor.isNull())
        {
            hasNext = false;
            break;
        }
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(Qt::blue));
        selection.cursor = cursor;
        m_selections.append(selection);
        if(cursor.selectionStart() == cursor.selectionEnd())
            last = cursor.selectionStart() + 1;
        else
            last = cursor.selectionEnd();
    }

    if (m_selections.size() == 0)
    {
        ui->label->setText("No results");
        ui->Down->setDisabled(true);
        ui->Up->setDisabled(true);
        m_editor->textCursor().clearSelection();
    }
    else
    {
        m_index = 0;
        ui->label->setText("1 of " + QString::number(m_selections.size()));
        m_editor->setTextCursor(m_selections[0].cursor);
        ui->Down->setDisabled(false);
        ui->Up->setDisabled(false);
    }
    m_editor->SetFindAndReplaceSelections(m_selections);
}

void FindAndReplace::Replace()
{
    QTextCursor cursor = m_editor->textCursor();
    QString text = GetReplaceText();
    cursor.insertText(text);
    Find();
}

QString FindAndReplace::GetReplaceText()
{
    QString replace = ui->Replace->text();
    QString original = m_editor->textCursor().selectedText();
    if (original == original.toUpper())
        return replace.toUpper();
    else if (original == original.toLower())
        return replace.toLower();
    else if (original.at(0).isUpper())
    {
        QString res = replace.toLower();
        res[0] = replace.at(0).toUpper();
        return res;
    }
    return replace;
}

void FindAndReplace::ChangeSelection()
{
    ui->label->setText(QString::number(m_index + 1) + " of " + QString::number(m_selections.size()));
    m_editor->setTextCursor(m_selections[m_index].cursor);
    m_editor->SetFindAndReplaceSelections(m_selections);
}

FindAndReplace::~FindAndReplace()
{
    // remove all selections after window is closed
    m_editor->SetFindAndReplaceSelections({});

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

void FindAndReplace::on_ReplaceButton_clicked()
{
    if (m_selections.isEmpty())
    {
        QMessageBox::warning(this, "Error", "No matchings found, cannot replace.");
        return;
    }
    Replace();
}


void FindAndReplace::on_ReplaceAllButton_clicked()
{
    if (m_selections.isEmpty())
    {
        QMessageBox::warning(this, "Error", "No matchings found, cannot replace.");
        return;
    }
    while (m_selections.size() > 0)
    {
        Replace();
    }
}

