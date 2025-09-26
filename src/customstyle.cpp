#include "customstyle.h"
#include <QDebug>
#include <QFile>

CustomStyle& CustomStyle::instance()
{
    static CustomStyle instance(nullptr); // 局部静态变量，确保唯一实例
    return instance;
}

void CustomStyle::addToolButtonIcon(QToolButton* button, const QString& iconName, int size)
{
    button->setText(CustomStyle::instance().getIconChar(iconName));
    button->setFont(CustomStyle::instance().getIconFont(size));
    QFont font = CustomStyle::instance().getIconFont(size);
    font.setStyleStrategy(QFont::PreferAntialias); // 开启抗锯齿
    button->setFont(font);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 设置为上图下文
    button->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
}

QFont CustomStyle::getIconFont(int size) const
{
    return QFont(iconFontFamily, size);
}

QString CustomStyle::getPixmapPath(const QString& iconname)
{

    if (iconname == "导航") {
        return ":/icon_png/daohang.png";
    } else if (iconname == "配置") {
        return ":/icon_png/bianji.png";
    } else if (iconname == "数据库") {
        return ":/icon_png/shujuku.png";
    } else if (iconname == "首页") {
        return ":/icon_png/shouye.png";
    } else if (iconname == "设置") {
        return ":/icon_png/shezhi.png";
    }

    return "";
}

void CustomStyle::loadStyleSheet(QWidget* widget, const QString& fileNamePath, const QString& otherstyle)
{
    if (!widget) {
        return;
    }

    widget->setStyleSheet(QString()); // 清除部件的当前样式

    QFile file(fileNamePath);
    if (file.open(QFile::ReadOnly)) {
        QString qss = file.readAll();
        qss += otherstyle;
        qDebug() << "加载样式表成功：" << fileNamePath;
        widget->setStyleSheet(qss); // 设置部件样式
        file.close();
        return;
    }
}

QString CustomStyle::getStyleSheet(const QString& fileNamePath)
{
    QString qss = "";
    QFile file(fileNamePath);
    if (file.open(QFile::ReadOnly)) {
        qss = file.readAll();
        file.close();
    }
    return qss;
}
// TODO:链接数据库 读取配置图标的文件
QIcon CustomStyle::geticon(const QString& iconname)
{
    if (iconname == "导航") {
        return QIcon(":/icon_png/daohang.png");
    } else if (iconname == "编辑") {
        return QIcon(":/icon_png/bianji.png");
    } else if (iconname == "数据库") {
        return QIcon(":/icon_png/shujuku.png");
    } else if (iconname == "首页") {
        return QIcon(":/icon_png/shouye.png");
    }
    return QIcon();
}

bool CustomStyle::initFont(const QString& fontPath)
{
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (fontFamilies.empty()) {
        qDebug() << "图标字体加载失败";
        return false;
    }
    iconFontFamily = fontFamilies.at(0);
    return true;
}

QChar CustomStyle::getIconChar(const QString& str) const
{
    return charMap.contains(str) ? charMap.value(str) : QChar();
}

void CustomStyle::initFontMap()
{
    charMap.insert("首页", QChar(0xe609));
    charMap.insert("配置", QChar(0xe68f));
    charMap.insert("数据库", QChar(0xe690));
    charMap.insert("iconfont-delete", QChar(0xe903));
    charMap.insert("iconfont-clear", QChar(0xe904));
    charMap.insert("iconfont-link", QChar(0xe905));
    charMap.insert("iconfont-unlink", QChar(0xe906));
    charMap.insert("iconfont-response", QChar(0xe907));
    charMap.insert("iconfont-noreponse", QChar(0xe908));
}

CustomStyle::CustomStyle(QObject* parent)
    : QObject { parent }
{
    initFont();
    initFontMap();
}
