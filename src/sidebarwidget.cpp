#include "sidebarwidget.h"
#include "customstyle.h"
SidebarWidget::SidebarWidget(QWidget* parent, int initialWidth)
    : Sidebar { parent, initialWidth }
{
    initUI();
    initConnect();
}

void SidebarWidget::initUI()
{
    homePage = new SidebarOptionsButton(this, 0);
    configPage = new SidebarOptionsButton(this, 1);
    databasePage = new SidebarOptionsButton(this, 2);
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    setPage = new SidebarOptionsButton(this, 3);
    navigationPage = new SidebarOptionsButton(this);

    // 设置选中提示线颜色
    homePage->setPromptLineColor(QColor(0, 103, 192));
    configPage->setPromptLineColor(QColor(0, 103, 192));
    databasePage->setPromptLineColor(QColor(0, 103, 192));
    setPage->setPromptLineColor(QColor(0, 103, 192));

    // 设置按钮图标
    homePage->setIcon(CustomStyle::getPixmapPath("首页"));
    configPage->setIcon(CustomStyle::getPixmapPath("配置"));
    databasePage->setIcon(CustomStyle::getPixmapPath("数据库"));
    setPage->setIcon(CustomStyle::getPixmapPath("设置"));
    navigationPage->setIcon(CustomStyle::getPixmapPath("导航"));

    // 设置右侧文本
    homePage->setText("首页");
    configPage->setText("配置");
    databasePage->setText("数据库");
    setPage->setText("设置");

    // 添加到sidebar中（Sidebar默认的垂直布局,注意添加顺序:从上到下）
    addWidget(navigationPage);
    addWidget(homePage);
    addWidget(configPage);
    addWidget(databasePage);
    addItem(verticalSpacer);
    addWidget(setPage);

    homePage->setChecked(true);                     // 默认第一个按钮被选中
    navigationPage->setFixedSize(40, 36);           // 展开按钮固定大小
    navigationPage->setDrawPromptLineEnable(false); // 展开按钮不绘制选中提示线条
    navigationPage->setCheckable(false);
}

void SidebarWidget::initConnect()
{
    connect(navigationPage, &QRadioButton::clicked, this, [&]() { autoExpand(); });

    connect(homePage, &QToolButton::clicked, this, &SidebarWidget::homePageClicked);
    connect(configPage, &QToolButton::clicked, this, &SidebarWidget::configPageClicked);
    connect(databasePage, &QToolButton::clicked, this, &SidebarWidget::databasePageClicked);
}
