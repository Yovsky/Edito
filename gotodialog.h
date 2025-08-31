#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include <QDialog>

namespace Ui {
class gotodialog;
}

class gotodialog : public QDialog
{
    Q_OBJECT

public:
    explicit gotodialog(const int &lineCount, const int &charCount,QWidget *parent = nullptr);
    void toggleLineOffs(bool used);
    ~gotodialog();

private slots:
    void on_Cancel_clicked();

    void on_Off_m_toggled(bool checked);

    void on_Line_m_toggled(bool checked);

private:
    Ui::gotodialog *ui;
    int lengthChar;
    int lengthLine;
};

#endif // GOTODIALOG_H
