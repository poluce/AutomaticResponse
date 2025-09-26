#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sidebarwidget.h"
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
protected:
    void resizeEvent(QResizeEvent* event);

private:
    SidebarWidget* sidebar;
    QStackedWidget* mainStackedWidget;
    QSpacerItem* placeholder;
};

#endif // MAINWINDOW_H
