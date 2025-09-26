#include "customstyle.h"
#include "localsqllite.h"
#include "mainwindow.h"
#include "widget.h"

#include <QApplication>
#include <QFontDatabase>
#include <QPushButton>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWindow mw;
    mw.show();

    return a.exec();
}
