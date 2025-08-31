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

#include "edito.h"
#include "editor.h"
#include "ui_edito.h"
#include "preferencesdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QStandardPaths>
#include <QMessageBox>
#include <QIcon>
#include <QSettings>

Edito::Edito(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Edito)
{
    ui->setupUi(this);
}

Edito::~Edito()
{
    delete ui;
}

void Edito::on_Open_clicked()
{
    QString DefLocation =  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); //Get the Documents folder.
    if(DefLocation.isEmpty()) DefLocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation); //Fallback to Home if not existing.
    QString FileOpened = QFileDialog::getOpenFileName(this, tr("Open File"), DefLocation, tr("Text Files (*.txt)")); //File path.

    if(!FileOpened.isEmpty())
    {
        this->close();
        qDebug() << "opended: " << FileOpened;
        Editor *editor = new Editor();
        editor->setAttribute(Qt::WA_DeleteOnClose);
        editor->show();
        editor->raise();
        editor->activateWindow();
        editor->OpenFile(FileOpened);
    }
    else QMessageBox::critical(this, "Error", "Failed to open from a file.");
}


void Edito::on_createNew_clicked()
{
    this->close();
    qDebug() << "Opened new";
    Editor *editor = new Editor();
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    editor->raise();
    editor->activateWindow();
    editor->NewFile();
}


void Edito::on_Options_clicked()
{
    QSettings settings("Yovsky", "Edito");
    bool isVisible = settings.value("ToggleStatBar", true).toBool(); //Load visibility settings.
    PreferencesDialog dialog(&settings, isVisible, this);

    dialog.setVisible(isVisible); //Set according to loaded settings.

    connect(&dialog, &PreferencesDialog::toggleStatusBarReq, this, [](bool visible) { //Connect for setting the visibility.
        QSettings settings("Yovsky", "Edito");
        settings.setValue("ToggleStatBar", visible);
    });

    dialog.exec();
}


void Edito::on_EmptyFile_clicked()
{
    this->close();
    Editor *editor = new Editor();
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    editor->raise();
    editor->activateWindow();
}

