#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QFontDatabase>
#include <QMap>
#include <QObject>
#include <QString>
#include <QToolButton>

class CustomStyle : public QObject {
    Q_OBJECT
public:
    static CustomStyle& instance();
    static void addToolButtonIcon(QToolButton* button, const QString& iconName, int size = 16);
    static void loadStyleSheet(QWidget* widget, const QString& fileName, const QString& otherstyle = QString());
    static QString getStyleSheet(const QString& fileNamePath);

    static QIcon geticon(const QString& iconname);

    bool initFont(const QString& fontPath = ":/iconfont/autoResponseIconFont.ttf");
    void initFontMap();

    QChar getIconChar(const QString& str) const;
    QFont getIconFont(int size = 16) const;

private:
    CustomStyle();      // 默认构造
    ~CustomStyle() { }; // 析构
    explicit CustomStyle(QObject* parent = nullptr);
    CustomStyle(const CustomStyle&) = delete;            // 禁止拷贝
    CustomStyle& operator=(const CustomStyle&) = delete; // 禁止赋值

private:
    QString iconFontFamily;
    QMap<QString, QChar> charMap;
};

#endif // CUSTOMSTYLE_H
