#include "mainwindow.h"
#include "customstyle.h"
#include "homepage.h"
#include "localsqllite.h"

#include "widget.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow { parent }
{
    initUI();
    initLayout();
    initConnect();
    resize(1200, 800);
}

void MainWindow::initUI()
{
    // 加载字体文件
    CustomStyle::instance();
    // 加载配置文件
    CustomStyle::loadStyleSheet(this, ":/customQss/mainWindow.css");

    setCentralWidget(new QWidget);
    resize(800, 600);
    setWindowTitle("自动应答服务端");

    sidebar = new SidebarWidget(this, 50);
    sidebar->setObjectName("MainWindow_sideWidget");
    sidebar->raise();
    mainStackedWidget = new QStackedWidget;
    mainStackedWidget->addWidget(new HomePage(LocalSqlLite::instance()));
    mainStackedWidget->addWidget(new Widget);
    mainStackedWidget->addWidget(new QWidget);
}

void MainWindow::initLayout()
{
    placeholder = new QSpacerItem(50, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget());
    mainLayout->addItem(placeholder);
    mainLayout->addWidget(mainStackedWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::initConnect()
{
    connect(sidebar, &SidebarWidget::homePageClicked, this, [this]() { mainStackedWidget->setCurrentIndex(0); });
    connect(sidebar, &SidebarWidget::configPageClicked, this, [this]() { mainStackedWidget->setCurrentIndex(1); });
    connect(sidebar, &SidebarWidget::databasePageClicked, this, [this]() { mainStackedWidget->setCurrentIndex(2); });
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    sidebar->resize(sidebar->width(), this->height());
    QMainWindow::resizeEvent(event);
}
