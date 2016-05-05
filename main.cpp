#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(APP_PRODUCT);
    a.setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.setWindowTitle("Episodiac");
    w.setWindowIcon(QIcon("icon.ico"));
    w.show();

    return a.exec();
}
