/*
 * Edito - A modern, cross-platform text editor
 * Copyright (C) 2025 Yovsky <Yovsky@proton.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "gotodialog.h"
#include "ui_gotodialog.h"
#include <QRegularExpressionValidator>
#include <QMessageBox>
gotodialog::gotodialog(CodeEditor *editor, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::gotodialog)
    , editor(editor)
    , lengthChar(0)
    , lengthLine(0)
    , line_M(true)
{
    ui->setupUi(this);
    ui->Line_m->setChecked(true);
    ui->Ok->setDefault(true);

    connect(editor, &CodeEditor::selectionChanged, this, [this]() {
        toggleLineOffs();
    });

    lengthLine = editor->document()->lineCount();
    lengthChar = editor->document()->characterCount();

    toggleLineOffs();
}

void gotodialog::toggleLineOffs()
{
    ui->Go_to->clear();

    if (line_M)
    {
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("\\d{0," + QString::number(QString::number(lengthLine).length()) + "}"), this);
        ui->Go_to->setValidator(validator);
        ui->Max->setText(QString::number(lengthLine + 1));
        ui->Current->setText(QString::number(editor->textCursor().blockNumber() + 1));
    }
    else
    {
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("\\d{0," + QString::number(QString::number(lengthChar).length()) + "}"), this);
        ui->Go_to->setValidator(validator);
        ui->Max->setText(QString::number(lengthChar + 1));
        ui->Current->setText(QString::number(editor->textCursor().position() + 1));
    }
}

gotodialog::~gotodialog()
{
    delete ui;
}

void gotodialog::on_Cancel_clicked()
{
    this->close();
}

void gotodialog::on_Line_m_toggled(bool checked)
{
    line_M = checked;
    toggleLineOffs();
}

void gotodialog::on_Off_m_toggled(bool checked)
{
    line_M = !checked;
    toggleLineOffs();
}

void gotodialog::executeGoTo()
{
    QTextCursor cursor = editor->textCursor();
    int target = ui->Go_to->text().toInt() - 1;
    if(line_M && target <= lengthLine)
    {
        cursor.setPosition(0);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, target);
    }
    else if(!line_M && target + 1 <= lengthChar)
        cursor.setPosition(target + 1);
    else
        cursor.setPosition(lengthChar - 1);
    editor->setTextCursor(cursor);
}

void gotodialog::on_Ok_clicked()
{
    if(ui->Go_to->text() != "")
    {
        executeGoTo();
        this->close();
    }
    else
        QMessageBox::warning(this, "Warning", "Target line empty.");
}

