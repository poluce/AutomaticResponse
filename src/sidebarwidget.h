#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include "sidebar.h"
#include "sidebaroptionsbutton.h"
#include <QWidget>

class SidebarWidget : public Sidebar {
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget* parent, int initialWidth);

    void initUI();
    void initConnect();

signals:
    void homePageClicked();
    void configPageClicked();
    void databasePageClicked();

private:
    SidebarOptionsButton* homePage;       // 第一页按钮
    SidebarOptionsButton* configPage;     // 第二页按钮
    SidebarOptionsButton* databasePage;   // 第三页按钮
    QSpacerItem* verticalSpacer;          // 垂直弹簧
    SidebarOptionsButton* setPage;        // 设置按钮
    SidebarOptionsButton* navigationPage; // 展开按钮

    QSpacerItem* placeholder;
};

#endif // SIDEBARWIDGET_H
