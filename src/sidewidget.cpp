#include "sidewidget.h"
#include "customstyle.h"

#include <QDebug>
#include <QVBoxLayout>

SideWidget::SideWidget(QWidget* parent)
    : QWidget { parent }
{
    setFixedWidth(40);
    setAttribute(Qt::WA_StyledBackground, true); // 让样式表负责绘制背景

    initUI();
    initLayout();
    initConnect();
}

void SideWidget::addNavButton(QString text, QToolButton* button)
{
    button->setText(text);
    button->setIcon(CustomStyle::geticon(text));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setProperty("fullText", text); // 保存完整文字
    QListWidgetItem* item = new QListWidgetItem();
    navWidget->addItem(item); // 先将 item 添加到列表中
    navWidget->setItemWidget(item, button);
    navWidget->setStyleSheet(
        "QListWidget {"
        "    border: none; /* 移除 QListWidget 整体的边框 */"
        "    background: transparent; /* 背景透明，可选 */"
        "}"
        "QListWidget::item {"
        "    border: none; /* 移除所有项的边框 */"
        "    padding: 0px; /* 移除内边距 */"
        "}"
        "QListWidget::item:selected {"
        "    border: none; /* 移除选中时的边框 */"
        "    background: transparent; /* 移除选中时的背景，可选 */"
        "}");
}

void SideWidget::onNavigationToggled(bool checked)
{
    if (checked) {
        setFixedWidth(200);
        // navWidget->setFixedWidth(200);
    } else {
        setFixedWidth(40);
    }
}

void SideWidget::initUI()
{
    navigation = new QToolButton;
    navigation->setCheckable(true);
    navigation->setText("导航");
    navigation->setIcon(CustomStyle::geticon("导航"));
    navWidget = new QListWidget;

    homePage = new QToolButton;
    addNavButton("首页", homePage);
    // CustomStyle::addToolButtonIcon(homePage, "首页", 20);

    configPage = new QToolButton;
    addNavButton("编辑", configPage);
    // CustomStyle::addToolButtonIcon(configPage, "配置", 20);

    databasePage = new QToolButton;
    addNavButton("数据库", databasePage);
    // CustomStyle::addToolButtonIcon(databasePage, "数据库", 20);
}
void SideWidget::initLayout()
{
    QHBoxLayout* navLayout = new QHBoxLayout;
    navLayout->addWidget(navigation);
    navLayout->addStretch();
    navLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(navLayout);
    mainLayout->addWidget(navWidget);
}

void SideWidget::initConnect()
{
    connect(homePage, &QToolButton::clicked, this, &SideWidget::homePageClicked);
    connect(configPage, &QToolButton::clicked, this, &SideWidget::configPageClicked);
    connect(databasePage, &QToolButton::clicked, this, &SideWidget::databasePageClicked);
    connect(navigation, &QToolButton::clicked, this, &SideWidget::onNavigationToggled);
}
