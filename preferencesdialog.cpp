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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QListWidget>

PreferencesDialog::PreferencesDialog(QSettings *settings, bool isStatVisible, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
    , m_settings(settings)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/icons/settings.png"));

    ui->PrefList->setCurrentRow(0);

    ui->On->setChecked(m_settings->value("Auto Save", true).toBool());
    ui->Off->setChecked(!m_settings->value("Auto Save", true).toBool());

    if (isStatVisible) ui->Show->setChecked(true); //Check the Show button on Visible.
    else if (!isStatVisible) ui->Hide->setChecked(true); //Check the Hide button on Hidden.
    checkcorrectSE();

    connect(ui->PrefList, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex); //Change widgets on each item.
}

void PreferencesDialog::on_PrefList_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void PreferencesDialog::on_buttonBox_rejected()
{
    this->close(); //Close the window. (to be changed later)
}

void PreferencesDialog::on_buttonBox_accepted()
{
    this->close(); //Close the window.
}

void PreferencesDialog::on_Show_clicked()
{
    emit toggleStatusBarReq(true);
}

void PreferencesDialog::on_Hide_clicked()
{
    emit toggleStatusBarReq(false);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::checkcorrectSE()
{
    int SEN = m_settings->value("SEToggled", 1).toInt();
    if (SEN == 1)
        ui->google->setChecked(true);
    else if (SEN == 2)
        ui->dDGo->setChecked(true);
    else if (SEN == 3)
        ui->bing->setChecked(true);
    else if (SEN == 4)
        ui->yahoo->setChecked(true);
    else if (SEN == 5)
        ui->brave->setChecked(true);
    else if (SEN == 6)
        ui->stackOF->setChecked(true);
    else if (SEN == 7)
        ui->gitHub->setChecked(true);
    else if (SEN == 8)
        ui->deMo->setChecked(true);
    else if (SEN == 9)
        ui->devDocs->setChecked(true);
    else if (SEN == 10)
        ui->sExchange->setChecked(true);
    else if (SEN == 11)
        ui->sourceGr->setChecked(true);
    else if (SEN == 12)
        ui->grep->setChecked(true);
    else
        ui->searchCode->setChecked(true);
}

void PreferencesDialog::on_google_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 1);
        m_settings->setValue("SE", "https://www.google.com/search?q=");
    }
}


void PreferencesDialog::on_dDGo_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 2);
        m_settings->setValue("SE", "https://duckduckgo.com/?q=");
    }
}


void PreferencesDialog::on_bing_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 3);
        m_settings->setValue("SE", "https://www.bing.com/search?q=");
    }
}


void PreferencesDialog::on_yahoo_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 4);
        m_settings->setValue("SE", "https://search.yahoo.com/search?p=");
    }
}


void PreferencesDialog::on_brave_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 5);
        m_settings->setValue("SE", "https://search.brave.com/search?q=");
    }
}


void PreferencesDialog::on_stackOF_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 6);
        m_settings->setValue("SE", "https://stackoverflow.com/search?q=");
    }
}


void PreferencesDialog::on_gitHub_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 7);
        m_settings->setValue("SE", "https://github.com/search?q=");
    }
}


void PreferencesDialog::on_deMo_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 8);
        m_settings->setValue("SE", "https://developer.mozilla.org/search?q=");
    }
}


void PreferencesDialog::on_devDocs_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 9);
        m_settings->setValue("SE", "https://devdocs.io/#q=");
    }
}


void PreferencesDialog::on_sExchange_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 10);
        m_settings->setValue("SE", "https://stackexchange.com/search?q=");
    }
}


void PreferencesDialog::on_sourceGr_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 11);
        m_settings->setValue("SE", "https://sourcegraph.com/search?q=");
    }
}


void PreferencesDialog::on_grep_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 12);
        m_settings->setValue("SE", "https://grep.app/search?q=");
    }
}


void PreferencesDialog::on_searchCode_toggled(bool checked)
{
    if (checked)
    {
        m_settings->setValue("SEToggled", 13);
        m_settings->setValue("SE", "https://searchcode.com/?q=");
    }
}

void PreferencesDialog::on_On_toggled(bool checked)
{
    m_settings->setValue("Auto Save", true);
    emit AutoSaveChanged();
}

void PreferencesDialog::on_Off_toggled(bool checked)
{
    m_settings->setValue("Auto Save", false);
    emit AutoSaveChanged();
}

