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

    sideWidget = new SideWidget;
    sideWidget->setObjectName("MainWindow_sideWidget");
    mainStackedWidget = new QStackedWidget;
    mainStackedWidget->addWidget(new Widget);
    mainStackedWidget->addWidget(new HomePage(LocalSqlLite::instance()));
    mainStackedWidget->addWidget(new QWidget);
}

void MainWindow::initLayout()
{
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget());
    mainLayout->addWidget(sideWidget);
    mainLayout->addWidget(mainStackedWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::initConnect()
{
    connect(sideWidget, &SideWidget::homePageClicked, this, [this]() { mainStackedWidget->setCurrentIndex(0); });
    connect(sideWidget, &SideWidget::configPageClicked, this, [this]() { mainStackedWidget->setCurrentIndex(1); });
    connect(sideWidget, &SideWidget::databasePageClicked, this, [this]() { mainStackedWidget->setCurrentIndex(2); });
}
