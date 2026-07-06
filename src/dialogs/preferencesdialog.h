#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

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

#include <QDialog>
#include <QSettings>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QSettings *settings, bool isStatVisible, QWidget *parent = nullptr);
    void checkcorrectAS();
    void checkcorrectSE();
    ~PreferencesDialog();

private slots:
    void on_buttonBox_rejected();

    void on_PrefList_currentRowChanged(int currentRow);

    void on_Show_clicked();

    void on_Hide_clicked();

    void on_buttonBox_accepted();

    void on_google_toggled(bool checked);

    void on_dDGo_toggled(bool checked);

    void on_bing_toggled(bool checked);

    void on_yahoo_toggled(bool checked);

    void on_brave_toggled(bool checked);

    void on_stackOF_toggled(bool checked);

    void on_gitHub_toggled(bool checked);

    void on_deMo_toggled(bool checked);

    void on_devDocs_toggled(bool checked);

    void on_sExchange_toggled(bool checked);

    void on_sourceGr_toggled(bool checked);

    void on_grep_toggled(bool checked);

    void on_searchCode_toggled(bool checked);

    void on_On_toggled(bool checked);

    void on_Off_toggled(bool checked);

    void on_SetSave_clicked();

    void on_SetRCF_clicked();

private:
    Ui::PreferencesDialog *ui;
    QSettings *m_settings;


signals:
    void toggleStatusBarReq(bool visible);
    void AutoSaveChanged();
};

#endif // PREFERENCESDIALOG_H
