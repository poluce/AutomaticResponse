#ifndef SIDEWIDGET_H
#define SIDEWIDGET_H

#include <QListWidget>
#include <QToolButton>
#include <QWidget>
class SideWidget : public QWidget {
    Q_OBJECT
public:
    explicit SideWidget(QWidget* parent = nullptr);
    void addNavButton(QString text, QToolButton* button);

signals:
    void homePageClicked();
    void configPageClicked();
    void databasePageClicked();
private slots:
    void onNavigationToggled(bool checked);

private:
    void initUI();
    void initLayout();
    void initConnect();

private:
    QToolButton* navigation;
    QListWidget* navWidget;
    // 首页
    QToolButton* homePage;
    QToolButton* configPage;
    QToolButton* databasePage;
};

#endif // SIDEWIDGET_H
