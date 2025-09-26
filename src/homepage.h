#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "localsqllite.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
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
    QTableWidget* creatCustomTabWidget();

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
    QComboBox* configBox;

    QGroupBox* editWidgetGroup;
    QStackedWidget* editWidget;
    QTabWidget* rulesTabWidget;
    QTableWidget* responseTableDSC;
    QTableWidget* responseTableARC;
    QTableWidget* responseTableTG;

    // 发送接收
    QGroupBox* receiveGroup;
    QTextEdit* receiveTextEdit;

    QGroupBox* sendGroup;
    QTextEdit* sendTextEdit;
    QPushButton* sendButton;
    QPushButton* clearButton;
};

#endif // HOMEPAGE_H
