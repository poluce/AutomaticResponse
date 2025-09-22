#include "customstyle.h"
#include "localsqllite.h"
#include "mainwindow.h"
#include "sidewidget.h"
#include "widget.h"

#include <QApplication>
#include <QFontDatabase>
#include <QPushButton>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // Widget w;
    // w.show();

    MainWindow mw;
    mw.show();

    // SideWidget sw;
    // sw.show();

    return a.exec();
}
