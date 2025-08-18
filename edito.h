#ifndef EDITO_H
#define EDITO_H

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

private:
    Ui::Edito *ui;
};
#endif // EDITO_H
