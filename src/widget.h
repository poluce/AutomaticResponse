#ifndef WIDGET_H
#define WIDGET_H

#include <QGroupBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();
    void initUI();
    void initData();
    void initLayout();
    void initConnect();
    void initMenu();

    int focusFirstEmptyRow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void represponseWidgetTextChanged();
    void responseToDelay(QString str);
private slots:

    void on_pushButton_response_clicked(bool checked);

    void on_pushButton_link_clicked(bool checked);
    void onNewConnection();
    void onClientDisconnected();
    void onClientDataReady();
    void on_setButtonClicked(bool checked);
    void onRepresponseWidgetTextChanged();
    void onEditingFinished();
    void onResponseToDelay(QTcpSocket* socket, QString str);

    void on_pushButton_response_clicked();
    void addActionTriggered();
    void deleteActionTriggered();
    void clearActionTriggered();

    void on_pushButton_clear_clicked();

    void on_pushButton_send_clicked();

private:
    Ui::Widget* ui;
    QTableWidget* responseWidget;
    QTcpServer* tcpserver;
    QList<QTcpSocket*> clientList;
    // 自动应答
    QGroupBox* commandMatchingGroupBox; // 匹配
    QLineEdit* commandMatching;
    QGroupBox* responseTemplateGroupBox; // 模板
    QLineEdit* responseTemplate;
    QGroupBox* remarkGroupBox; // 备注
    QLineEdit* remark;
    QGroupBox* responseDelayGroupBox; // 延迟应答
    QLineEdit* responseDelay;
    QSpinBox* delayTime;
    bool isDeleteKey;
    QTimer* delayTimer;

    QPushButton* setButton;
    QMenu* menu; // 右键菜单
};
#endif // WIDGET_H
