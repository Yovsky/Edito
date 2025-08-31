#ifndef GOTODIALOG_H
#define GOTODIALOG_H

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

#include "codeeditor.h"
#include <QDialog>

namespace Ui {
class gotodialog;
}

class gotodialog : public QDialog
{
    Q_OBJECT

public:
    explicit gotodialog(CodeEditor *editor,QWidget *parent = nullptr);
    void toggleLineOffs();
    void executeGoTo();
    ~gotodialog();

private slots:
    void on_Cancel_clicked();

    void on_Off_m_toggled(bool checked);

    void on_Line_m_toggled(bool checked);

    void on_Ok_clicked();

private:
    Ui::gotodialog *ui;
    CodeEditor *editor;
    int lengthChar;
    int lengthLine;
    bool line_M;
};

#endif // GOTODIALOG_H
