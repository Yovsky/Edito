#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

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

private:
    Ui::PreferencesDialog *ui;
    QSettings *m_settings;


signals:
    void toggleStatusBarReq(bool visible);
};

#endif // PREFERENCESDIALOG_H
