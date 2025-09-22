#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "localsqllite.h"
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTcpServer>
#include <QTextEdit>
#include <QWidget>

class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(LocalSqlLite* database = nullptr, QWidget* parent = nullptr);
    void initUI();
    void initLayout();
    void initConnect();
    void initData();
    void initResponseTable();
    void appendReceiveText(QString title, QString str);

signals:

private slots:
    void onApplyButtonClicked(bool checked);
    void onResponseEditButtonClicked(bool checked);
    void onNewConnection();
    void onClientDisconnected();
    void onClientDataReady();
    void onResponseToDelay(QTcpSocket* socket, QString title, QString str);

private:
    // 通信配置
    QTcpServer* tcpServer;
    QList<QTcpSocket*> clientList;

    // 数据库的操作
    LocalSqlLite* localSqlLite;

    // 服务器信息
    QGroupBox* severConfigGroup;
    QLabel* severIPLabel;
    QLineEdit* severIPEdit;
    QLabel* severPortLabel;
    QSpinBox* severPortSpinBox;
    QPushButton* applyButton;

    // 功能配置
    QGroupBox* functionConfigGroup;
    QCheckBox* responseFunction;
    QCheckBox* delayFunction;
    QPushButton* responseEditButton;

    QGroupBox* editWidgetGroup;
    QStackedWidget* editWidget;
    QTableWidget* responseTable;

    // 发送接收
    QGroupBox* receiveGroup;
    QTextEdit* receiveTextEdit;

    QGroupBox* sendGroup;
    QTextEdit* sendTextEdit;
    QPushButton* sendButton;
    QPushButton* clearButton;
};

#endif // HOMEPAGE_H
