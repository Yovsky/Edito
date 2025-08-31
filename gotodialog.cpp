#include "gotodialog.h"
#include "ui_gotodialog.h"
#include <QRegularExpressionValidator>

gotodialog::gotodialog(const int &lineCount, const int &charCount, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::gotodialog)
    , lengthChar(0)
    , lengthLine(0)
{
    ui->setupUi(this);
    lengthLine = lineCount;
    lengthChar = charCount;

    ui->Line_m->setChecked(true);
    toggleLineOffs(true);
}

void gotodialog::toggleLineOffs(bool used)
{
    ui->Go_to->clear();
    if (used)
    {
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("\\d{0," + QString::number(QString::number(lengthLine).length()) + "}"), this);
        ui->Go_to->setValidator(validator);
        ui->Max->setText(QString::number(lengthLine));
    }
    else
    {
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("\\d{0," + QString::number(QString::number(lengthChar).length()) + "}"), this);
        ui->Go_to->setValidator(validator);
        ui->Max->setText(QString::number(lengthChar));
    }
}

gotodialog::~gotodialog()
{
    delete ui;
}

void gotodialog::on_Cancel_clicked()
{
    this->close();
}


void gotodialog::on_Off_m_toggled(bool checked)
{
    toggleLineOffs(!checked);
}


void gotodialog::on_Line_m_toggled(bool checked)
{
    toggleLineOffs(checked);
}

