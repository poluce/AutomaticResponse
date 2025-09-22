#include "homepage.h"
#include "comm.h"

#include <QDateTime>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QTcpSocket>
#include <QTimer>
#include <QVBoxLayout>

HomePage::HomePage(LocalSqlLite* database, QWidget* parent)
    : QWidget { parent }, localSqlLite { database }
{
    initUI();
    initLayout();
    initConnect();
    initResponseTable();
}

void HomePage::initUI()
{
    tcpServer = new QTcpServer(this);

    // 服务器信息
    severConfigGroup = new QGroupBox("服务端配置");
    severIPLabel = new QLabel("本地主机地址");
    severIPEdit = new QLineEdit;
    severPortLabel = new QLabel("本地主机端口");
    severPortSpinBox = new QSpinBox;
    severPortSpinBox->setRange(1024, 65535);
    severPortSpinBox->setButtonSymbols(QSpinBox::ButtonSymbols::NoButtons);
    severPortSpinBox->setValue(8088);
    applyButton = new QPushButton("开启服务");
    applyButton->setCheckable(true);

    // 功能配置
    functionConfigGroup = new QGroupBox("功能配置");
    responseFunction = new QCheckBox("自动应答");
    responseFunction->setChecked(true);
    delayFunction = new QCheckBox("延迟应答");
    delayFunction->setChecked(true);
    responseEditButton = new QPushButton("应答编辑");
    responseEditButton->setCheckable(true);

    // 配置编辑窗口
    editWidgetGroup = new QGroupBox("信息配置");
    editWidgetGroup->hide();
    responseTable = new QTableWidget;
    editWidget = new QStackedWidget;
    editWidget->addWidget(responseTable);

    responseTable->setColumnCount(6);
    responseTable->setHorizontalHeaderLabels({ "开启状态", "匹配指令", "应答", "备注", "延迟时间", "延时应答" });
    responseTable->horizontalHeader()->setStretchLastSection(true);
    responseTable->verticalHeader()->setVisible(false);
    responseTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    responseTable->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    responseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    responseTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
    responseTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeMode::ResizeToContents);

    // 接收
    receiveGroup = new QGroupBox("消息接收区");
    receiveTextEdit = new QTextEdit;
    receiveTextEdit->setObjectName("HomePage_receiveTextEdit");
    QFont font("Microsoft YaHei", 10); // 字体名 + 字号
    font.setBold(true);                // 粗体
    receiveTextEdit->setFont(font);

    // 发送
    sendGroup = new QGroupBox("消息发送区");
    sendGroup->setMaximumHeight(220);
    sendTextEdit = new QTextEdit;
    sendTextEdit->setObjectName("HomePage_sendTextEdit");
    sendButton = new QPushButton("发送");
    clearButton = new QPushButton("清空");
}

void HomePage::initLayout()
{
    // 左侧
    QVBoxLayout* severLayout = new QVBoxLayout(severConfigGroup);
    severLayout->addWidget(severIPLabel);
    severLayout->addWidget(severIPEdit);
    severLayout->addWidget(severPortLabel);
    severLayout->addWidget(severPortSpinBox);
    severLayout->addWidget(applyButton);

    QGridLayout* functionLayout = new QGridLayout(functionConfigGroup);
    functionLayout->addWidget(responseFunction, 0, 0, 1, 1);
    functionLayout->addWidget(delayFunction, 0, 1, 1, 1);
    functionLayout->addWidget(responseEditButton, 1, 0, 1, 2);

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(severConfigGroup);
    leftLayout->addWidget(functionConfigGroup);
    leftLayout->addStretch();

    // 右侧中
    //[0]
    QVBoxLayout* receiveLayout = new QVBoxLayout(receiveGroup);
    receiveLayout->addWidget(receiveTextEdit);
    receiveLayout->setSpacing(0);
    receiveLayout->setContentsMargins(4, 4, 4, 4);

    //[1]
    QVBoxLayout* sendButtonLayout = new QVBoxLayout;
    sendButtonLayout->addWidget(sendButton);
    sendButtonLayout->addWidget(clearButton);
    sendButtonLayout->addStretch();
    sendButtonLayout->setSpacing(0);
    sendButtonLayout->setContentsMargins(0, 0, 0, 0);
    //[2]
    QHBoxLayout* sendLayout = new QHBoxLayout(sendGroup);
    sendLayout->addWidget(sendTextEdit);
    sendLayout->addLayout(sendButtonLayout);
    sendLayout->setSpacing(0);
    sendLayout->setContentsMargins(4, 4, 4, 4);
    //[3]
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(receiveGroup);
    splitter->addWidget(sendGroup);

    // 右侧应答窗口
    //[0]
    QVBoxLayout* eidtLayout = new QVBoxLayout(editWidgetGroup);
    eidtLayout->addWidget(editWidget);
    eidtLayout->setSpacing(0);
    eidtLayout->setContentsMargins(4, 4, 4, 4);

    QSplitter* hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(splitter);
    hSplitter->addWidget(editWidgetGroup);

    // 总布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout, 0);
    mainLayout->addWidget(hSplitter, 1);
}

void HomePage::initConnect()
{
    connect(applyButton, &QPushButton::clicked, this, &HomePage::onApplyButtonClicked);
    connect(responseEditButton, &QPushButton::clicked, this, &HomePage::onResponseEditButtonClicked);
    connect(tcpServer, &QTcpServer::newConnection, this, &HomePage::onNewConnection);
}

void HomePage::initData()
{
}

void HomePage::initResponseTable()
{
    QVector<AutoReplyRule> tmp = localSqlLite->readAutoReplyData();
    responseTable->setRowCount(tmp.size());
    for (int i = 0; i < tmp.size(); i++) {
        QCheckBox* enableStatus = new QCheckBox;
        enableStatus->setChecked(tmp.at(i).isEnabled);
        responseTable->setCellWidget(i, 0, enableStatus);

        responseTable->setItem(i, 1, new QTableWidgetItem(tmp.at(i).matchCommand));
        responseTable->setItem(i, 2, new QTableWidgetItem(tmp.at(i).responseTemplate));
        responseTable->setItem(i, 3, new QTableWidgetItem(tmp.at(i).remarks));

        QCheckBox* delayStatus = new QCheckBox(QString::number(tmp.at(i).delayedTime));
        delayStatus->setChecked(tmp.at(i).isDelayEnabled);
        responseTable->setCellWidget(i, 4, delayStatus);

        responseTable->setItem(i, 5, new QTableWidgetItem(tmp.at(i).timeoutResponse));
    }
}

void HomePage::appendReceiveText(QString title, QString str)
{
    QDateTime date = QDateTime::currentDateTime();
    QString receiveText = QString("时间：%1 \n%2:[%3 ] \n").arg(date.toString("hh:mm:ss:zzz"), title, str);
    receiveTextEdit->append(receiveText);
    receiveTextEdit->moveCursor(QTextCursor::End);
    receiveTextEdit->ensureCursorVisible();
}

void HomePage::onApplyButtonClicked(bool checked)
{
    if (checked) {
        int port = severPortSpinBox->value();
        if (tcpServer->listen(QHostAddress::Any, port)) {
            applyButton->setText("关闭服务");
        } else {
            QMessageBox::warning(this, "错误", "服务启动失败，端口可能被占用");
            applyButton->setChecked(false); // 恢复按钮状态
        }
    } else {
        applyButton->setText("开启服务");
        tcpServer->close();
    }
}

void HomePage::onResponseEditButtonClicked(bool checked)
{
    if (checked) {
        editWidgetGroup->show();
        editWidget->setCurrentIndex(0);
    } else {
        editWidgetGroup->hide();
    }
}

void HomePage::onNewConnection()
{
    // 获取新的客户端连接
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();

    if (clientSocket) {
        // 添加到客户端列表
        clientList.append(clientSocket);

        // 连接客户端信号
        connect(clientSocket, &QTcpSocket::disconnected, this, &HomePage::onClientDisconnected);
        connect(clientSocket, &QTcpSocket::readyRead, this, &HomePage::onClientDataReady);

        QString clientInfo = QString("%1:%2")
                                 .arg(clientSocket->peerAddress().toString())
                                 .arg(clientSocket->peerPort());

        qDebug() << "新客户端连接：" << clientInfo;
    }
}

void HomePage::onClientDisconnected()
{
}

void HomePage::onClientDataReady()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket->state() != QTcpSocket::ConnectedState) {
        return;
    }

    QByteArray sourcedata = clientSocket->readAll();
    QString data = QString::fromUtf8(sourcedata.toHex(' ').toUpper());

    QString ip = clientSocket->peerAddress().toString();
    if (ip.startsWith("::ffff:")) {
        ip = ip.mid(7);
    }

    QString msg = ip + ":" + QString::number(clientSocket->peerPort()) + "::" + data;
    appendReceiveText("客户端", msg);

    // 主页面自动应答是否开启判断
    if (!responseFunction->isChecked()) {
        return;
    }

    // 检查匹配模板
    for (int i = 0; i < responseTable->rowCount(); i++) {

        // 前置应答勾选检查
        QCheckBox* cb = qobject_cast<QCheckBox*>(responseTable->cellWidget(i, 0));
        if (!cb->isChecked()) {
            return;
        }

        // 匹配检查
        QTableWidgetItem* item = responseTable->item(i, 1); // 获取匹配指令列
        if (item->text().trimmed() != data) {
            continue;
        }
        // 发送应答模板
        QString hexString = responseTable->item(i, 2)->text().remove(' '); // 移除空格转换为16进制数据
        QByteArray hexData = QByteArray::fromHex(hexString.toUtf8());
        clientSocket->write(hexData);

        QString resStr = QString("%1::%2")
                             .arg(responseTable->item(i, 3)->text(), responseTable->item(i, 2)->text());
        appendReceiveText("自动应答", resStr);

        // 主页延时应答勾选检查
        if (!delayFunction->isChecked()) {
            return;
        }
        // 是否勾选延时应答
        QCheckBox* cb2 = qobject_cast<QCheckBox*>(responseTable->cellWidget(i, 4));
        if (!cb2->isChecked()) {
            return;
        }

        // 判断是否有延时应答内容
        if (!(responseTable->item(i, 5)->text().remove(' ').length() > 0)) {
            return;
        }

        // 设置定时器，延时发送
        QTimer::singleShot(cb2->text().toInt(), this, [=]() {
            onResponseToDelay(clientSocket, responseTable->item(i, 3)->text(), responseTable->item(i, 5)->text());
        });

        qDebug() << "自动应答：" << hexString;
        break;
    }
}

void HomePage::onResponseToDelay(QTcpSocket* socket, QString title, QString str)
{
    QString raw = str;
    QByteArray hexData = QByteArray::fromHex(raw.remove(' ').toUtf8());
    socket->write(hexData);
    socket->flush();

    QString msg = QString("%1 :: %2").arg(title, str);
    appendReceiveText("延时应答", msg);
}
