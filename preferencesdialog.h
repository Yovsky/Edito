#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(bool isStatVisible, QWidget *parent = nullptr);
    ~PreferencesDialog();

private slots:
    void on_buttonBox_rejected();

    void on_PrefList_currentRowChanged(int currentRow);

    void on_Show_clicked();

    void on_Hide_clicked();

    void on_buttonBox_accepted();

private:
    Ui::PreferencesDialog *ui;


signals:
    void toggleStatusBarReq(bool visible);
};

#endif // PREFERENCESDIALOG_H
