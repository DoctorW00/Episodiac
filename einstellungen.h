#ifndef EINSTELLUNGEN_H
#define EINSTELLUNGEN_H

#include <QDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class Einstellungen;
}

class Einstellungen : public QDialog
{
    Q_OBJECT

    public:
        explicit Einstellungen(QWidget *parent = 0);
        ~Einstellungen();

    private slots:
        void on_pushButton_2_clicked();
        void on_pushButton_clicked();
        void on_pushButton_3_clicked();
        void on_pushButton_4_clicked();
        // search api url open
        void on_pushButton_5_clicked();
        void on_pushButton_6_clicked();
        void on_pushButton_7_clicked();
        void on_pushButton_8_clicked();

    private:
        Ui::Einstellungen *ui;
};

#endif // EINSTELLUNGEN_H
