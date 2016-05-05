#include "info.h"
#include "ui_info.h"

Info::Info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);

    ui->plainTextEdit->setStyleSheet("background: transparent;");
    ui->plainTextEdit_2->setStyleSheet("background: transparent;");
    ui->pushButton->setIcon(QIcon(":/bilder/icons/close.png"));
    ui->plainTextEdit_2->appendPlainText("Version: " APP_VERSION);
    QString build = QString("%1%2")
    .arg(QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).toString("ddMMyyyy"))
    .arg(QString("%1%2%3%4").arg(__TIME__[0])
    .arg(__TIME__[1])
    .arg(__TIME__[3])
    .arg(__TIME__[4]));
    ui->plainTextEdit_2->appendPlainText("Build: " + build);
}

Info::~Info()
{
    delete ui;
}

void Info::on_pushButton_clicked()
{
    this->close();
}
