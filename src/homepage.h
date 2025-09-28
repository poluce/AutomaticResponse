#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "inputrulewidget.h"
#include "localsqllite.h"
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
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
    ~HomePage();
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
    void onClearSendButtonClicked();
    void oncCearReceiveButtonClicked();
    void onInputRuleButtonCliecked(const InputRule& rule);

    void deleteActionTriggered();
    void clearActionTriggered();

private:
    QTableWidget* creatCustomTabWidget();
    void sendResponseMessage(QTcpSocket* clientSocket, QString data, const QString& selectItemName);
    QWidget* findTabByName(QTabWidget* tabWidget, const QString& tabName);
    void fillTableData(QTableWidget* tabWidget, const QVector<AutoReplyRule>& data);
    QString clientAddressInfo(QTcpSocket* clientSocket);
    QVector<AutoReplyRule> getTableData(QString name);
    QVector<AutoReplyRule> readTableData(QTableWidget* tabWidget);

    void initMenu();

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
    QCheckBox* scheduledReportBox;
    QPushButton* scheduledReportButton;
    QTimer* scheduledReportTimer;

    QGroupBox* editWidgetGroup;
    QStackedWidget* editWidget;   // 右侧编辑的层叠窗口
    QWidget* ruleWidget;          // 一个子窗口
    InputRuleWidget* inputWidget; // 规则输入
    QTabWidget* rulesTabWidget;
    QTableWidget* responseTableDSC;
    QTableWidget* responseTableARC;
    QTableWidget* responseTableTG;

    // 客户端的显示
    QGroupBox* clienInfoGroup;
    QListWidget* clienInfoWidget;
    QMap<QTcpSocket*, QListWidgetItem*> clienInfoMap;

    // 发送接收
    QGroupBox* receiveGroup;
    QTextEdit* receiveTextEdit;

    QGroupBox* sendGroup;
    QTextEdit* sendTextEdit;
    QPushButton* sendButton;
    QPushButton* clearSendButton;
    QPushButton* clearReceiveButton;
};

#endif // HOMEPAGE_H
