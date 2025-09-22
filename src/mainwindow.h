#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sidewidget.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    void initUI();
    void initLayout();
    void initConnect();

signals:
private:
    SideWidget* sideWidget;
    QStackedWidget* mainStackedWidget;
};

#endif // MAINWINDOW_H
