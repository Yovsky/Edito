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
    QTextCursor cursor = m_editor->document()->find(target);
    m_editor->setTextCursor(cursor);
}

