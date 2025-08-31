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
    explicit gotodialog(QWidget *parent = nullptr);
    ~gotodialog();

private slots:
    void on_Cancel_clicked();

private:
    Ui::gotodialog *ui;
};

#endif // GOTODIALOG_H
