#ifndef EDITO_H
#define EDITO_H

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

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Edito;
}
QT_END_NAMESPACE

class Edito : public QMainWindow
{
    Q_OBJECT

public:
    Edito(QWidget *parent = nullptr);
    ~Edito();

private slots:
    void on_Open_clicked();

    void on_createNew_clicked();

    void on_Options_clicked();

    void on_EmptyFile_clicked();

private:
    Ui::Edito *ui;
};
#endif // EDITO_H
