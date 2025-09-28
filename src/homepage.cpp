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
    initMenu();
}

HomePage::~HomePage()
{

    // 添加关闭程序 保存数据
    localSqlLite->writeAutoReplyData(LocalSqlLite::DataName::DSC, getTableData("DSC"));
    localSqlLite->writeAutoReplyData(LocalSqlLite::DataName::ARC, getTableData("ARC"));
    localSqlLite->writeAutoReplyData(LocalSqlLite::DataName::TG, getTableData("TG"));
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

    configBox = new QComboBox;
    configBox->addItem("DSC");
    configBox->addItem("ARC");
    configBox->addItem("TG");

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

    responseTableDSC = creatCustomTabWidget();
    responseTableARC = creatCustomTabWidget();
    responseTableTG = creatCustomTabWidget();

    ruleWidget = new QWidget;
    rulesTabWidget = new QTabWidget;
    rulesTabWidget->addTab(responseTableDSC, "DSC");
    rulesTabWidget->addTab(responseTableARC, "ARC");
    rulesTabWidget->addTab(responseTableTG, "TG");
    inputWidget = new InputRuleWidget;

    editWidget = new QStackedWidget;
    // editWidget->addWidget(rulesTabWidget);

    scheduledReportBox = new QCheckBox("定时上发");
    scheduledReportButton = new QPushButton("上发编辑");
    scheduledReportTimer = new QTimer;

    // 客户端显示
    clienInfoGroup = new QGroupBox("链接客户端");
    clienInfoWidget = new QListWidget;
    clienInfoWidget->setObjectName("HomePage_clienInfoWidget");
    clienInfoWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    clienInfoGroup->setMaximumWidth(200);
    int width = clienInfoGroup->sizeHint().width();
    clienInfoGroup->resize(width, 5 * width);

    // 接收
    receiveGroup = new QGroupBox("消息接收区");
    receiveTextEdit = new QTextEdit;
    receiveTextEdit->setObjectName("HomePage_receiveTextEdit");
    QFont font("Microsoft YaHei", 10); // 字体名 + 字号
    font.setBold(true);                // 粗体
    receiveTextEdit->setFont(font);
    receiveTextEdit->setFocusPolicy(Qt::NoFocus); // 通过静止获取焦点来禁止用户输入

    // 发送
    sendGroup = new QGroupBox("消息发送区");
    sendGroup->setMaximumHeight(220);
    sendTextEdit = new QTextEdit;
    sendTextEdit->setObjectName("HomePage_sendTextEdit");
    sendButton = new QPushButton("发送");
    clearSendButton = new QPushButton("⮏ 清空");
    clearReceiveButton = new QPushButton("⮍ 清空");
    QFont font1 = clearSendButton->font();
    font1.setPointSize(10);
    clearSendButton->setFont(font1);
    clearReceiveButton->setFont(font1);
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
    severLayout->addWidget(configBox);

    QGridLayout* functionLayout = new QGridLayout(functionConfigGroup);
    functionLayout->addWidget(responseFunction, 0, 0, 1, 1);
    functionLayout->addWidget(delayFunction, 0, 1, 1, 1);
    functionLayout->addWidget(responseEditButton, 1, 0, 1, 2);
    functionLayout->addWidget(scheduledReportBox, 2, 0, 1, 1);
    functionLayout->addWidget(scheduledReportButton, 2, 1, 1, 1);

    // 左侧客户端显示
    QGridLayout* clientLayout = new QGridLayout(clienInfoGroup);
    clientLayout->addWidget(clienInfoWidget);
    clientLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(severConfigGroup);
    leftLayout->addWidget(functionConfigGroup);
    leftLayout->addWidget(clienInfoGroup, 1);
    leftLayout->addStretch();

    // 右侧中
    //[0]
    QVBoxLayout* receiveLayout = new QVBoxLayout(receiveGroup);
    receiveLayout->addWidget(receiveTextEdit);
    receiveLayout->setSpacing(0);
    receiveLayout->setContentsMargins(4, 4, 4, 4);

    //[1]
    QVBoxLayout* sendButtonLayout = new QVBoxLayout;
    sendButtonLayout->addWidget(clearReceiveButton);
    sendButtonLayout->addWidget(clearSendButton);
    sendButtonLayout->addSpacerItem(new QSpacerItem(0, 20));
    sendButtonLayout->addWidget(sendButton);
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
    QVBoxLayout* ruleLayout = new QVBoxLayout(ruleWidget);
    ruleLayout->addWidget(rulesTabWidget, 1);
    ruleLayout->addWidget(inputWidget);

    // TODO:添加规则编辑窗口
    ruleLayout->setSpacing(0);
    ruleLayout->setContentsMargins(0, 0, 0, 0);

    editWidget->addWidget(ruleWidget);

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
    connect(clearReceiveButton, &QPushButton::clicked, this, &HomePage::oncCearReceiveButtonClicked);
    connect(clearSendButton, &QPushButton::clicked, this, &HomePage::onClearSendButtonClicked);
    connect(inputWidget, &InputRuleWidget::InputRuleButtonCliecked, this, &HomePage::onInputRuleButtonCliecked);
}

void HomePage::initData()
{
}

void HomePage::initResponseTable()
{
    const QVector<AutoReplyRule>& dscData = localSqlLite->readDSCAutoReplyData();
    const QVector<AutoReplyRule>& arcData = localSqlLite->readARCAutoReplyData();
    const QVector<AutoReplyRule>& tgData = localSqlLite->readTGAutoReplyData();

    fillTableData(responseTableDSC, dscData);
    fillTableData(responseTableARC, arcData);
    fillTableData(responseTableTG, tgData);
}

void HomePage::appendReceiveText(QString title, QString str)
{
    QDateTime date = QDateTime::currentDateTime();
    QString receiveText = QString("时间：%1 \n%2:[%3 ] \n").arg(date.toString("hh:mm:ss:zzz"), title, str);
    receiveTextEdit->append(receiveText);
    receiveTextEdit->moveCursor(QTextCursor::End);
    receiveTextEdit->ensureCursorVisible();
}

QTableWidget* HomePage::creatCustomTabWidget()
{
    QTableWidget* table = new QTableWidget();
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({ "开启状态", "匹配指令", "应答", "备注", "延迟时间", "延时应答" });
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    table->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeMode::ResizeToContents);
    return table;
}

void HomePage::sendResponseMessage(QTcpSocket* clientSocket, QString data, const QString& selectItemName)
{
    QTableWidget* tableWidget = qobject_cast<QTableWidget*>(findTabByName(rulesTabWidget, selectItemName));

    if (!tableWidget) {
        qDebug() << "没有查找到" << selectItemName << "相关的数据窗口";
        return;
    }
    // 检查匹配模板
    for (int i = 0; i < tableWidget->rowCount(); i++) {

        // 前置应答勾选检查
        QCheckBox* cb = qobject_cast<QCheckBox*>(tableWidget->cellWidget(i, 0));
        if (!cb->isChecked()) {
            return;
        }

        // 匹配检查
        QTableWidgetItem* item = tableWidget->item(i, 1); // 获取匹配指令列
        if (item->text().trimmed() != data) {
            continue;
        }
        // 发送应答模板
        QString hexString = tableWidget->item(i, 2)->text().remove(' '); // 移除空格转换为16进制数据
        QByteArray hexData = QByteArray::fromHex(hexString.toUtf8());
        clientSocket->write(hexData);

        QString resStr = QString("%1::%2")
                             .arg(tableWidget->item(i, 3)->text(), tableWidget->item(i, 2)->text());
        appendReceiveText("自动应答", resStr);

        // 主页延时应答勾选检查
        if (!delayFunction->isChecked()) {
            return;
        }
        // 是否勾选延时应答
        QCheckBox* cb2 = qobject_cast<QCheckBox*>(tableWidget->cellWidget(i, 4));
        if (!cb2->isChecked()) {
            return;
        }

        // 判断是否有延时应答内容
        if (!(tableWidget->item(i, 5)->text().remove(' ').length() > 0)) {
            return;
        }

        // 设置定时器，延时发送
        QTimer::singleShot(cb2->text().toInt(), this, [=]() {
            onResponseToDelay(clientSocket, tableWidget->item(i, 3)->text(), tableWidget->item(i, 5)->text());
        });

        qDebug() << "自动应答：" << hexString;
        break;
    }
}

QWidget* HomePage::findTabByName(QTabWidget* tabWidget, const QString& tabName)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == tabName) {
            return tabWidget->widget(i); // 返回对应的页面指针
        }
    }
    return nullptr; // 没找到
}

void HomePage::fillTableData(QTableWidget* tabWidget, const QVector<AutoReplyRule>& data)
{
    tabWidget->setRowCount(data.size());
    for (int i = 0; i < data.size(); i++) {
        QCheckBox* enableStatus = new QCheckBox;
        enableStatus->setChecked(data.at(i).isEnabled);
        tabWidget->setCellWidget(i, 0, enableStatus);

        tabWidget->setItem(i, 1, new QTableWidgetItem(data.at(i).matchCommand));
        tabWidget->setItem(i, 2, new QTableWidgetItem(data.at(i).responseTemplate));
        tabWidget->setItem(i, 3, new QTableWidgetItem(data.at(i).remarks));

        QCheckBox* delayStatus = new QCheckBox(QString::number(data.at(i).delayedTime));
        delayStatus->setChecked(data.at(i).isDelayEnabled);
        tabWidget->setCellWidget(i, 4, delayStatus);

        tabWidget->setItem(i, 5, new QTableWidgetItem(data.at(i).timeoutResponse));
    }
}

QString HomePage::clientAddressInfo(QTcpSocket* clientSocket)
{
    QString ip = clientSocket->peerAddress().toString();
    if (ip.startsWith("::ffff:")) {
        ip = ip.mid(7);
    }

    QString msg = ip + "    :   " + QString::number(clientSocket->peerPort());
    return msg;
}

QVector<AutoReplyRule> HomePage::getTableData(QString name)
{
    QVector<AutoReplyRule> tmp;
    QTableWidget* tableWidget = nullptr; // 初始化为空

    if (name == "DSC") {
        tableWidget = responseTableDSC;
    } else if (name == "ARC") {
        tableWidget = responseTableARC;
    } else if (name == "TG") {
        tableWidget = responseTableTG;
    }

    if (!tableWidget) {
        return tmp; // 没匹配到，直接返回空
    }
    return readTableData(tableWidget);
}

QVector<AutoReplyRule> HomePage::readTableData(QTableWidget* tabWidget)
{
    QVector<AutoReplyRule> data;

    if (!tabWidget) {
        return data; // 如果指针为空，返回空向量
    }

    int rowCount = tabWidget->rowCount();
    for (int i = 0; i < rowCount; i++) {
        AutoReplyRule rule;

        // 1. 读取第 0 列：启用状态 (QCheckBox 控件)
        QWidget* enableWidget = tabWidget->cellWidget(i, 0);
        QCheckBox* enableBox = qobject_cast<QCheckBox*>(enableWidget);
        if (enableBox) {
            rule.isEnabled = enableBox->isChecked();
        } else {
            rule.isEnabled = false;
        }

        // 2. 读取第 1, 2, 3, 5 列：文本项 (QTableWidgetItem)
        QTableWidgetItem* item1 = tabWidget->item(i, 1);
        QTableWidgetItem* item2 = tabWidget->item(i, 2);
        QTableWidgetItem* item3 = tabWidget->item(i, 3);
        QTableWidgetItem* item5 = tabWidget->item(i, 5);

        rule.matchCommand = item1 ? item1->text() : QString();
        rule.responseTemplate = item2 ? item2->text() : QString();
        rule.remarks = item3 ? item3->text() : QString();
        rule.timeoutResponse = item5 ? item5->text() : QString();

        // 3. 读取第 4 列：延迟状态 (QCheckBox 控件)
        QWidget* delayWidget = tabWidget->cellWidget(i, 4);
        QCheckBox* delayBox = qobject_cast<QCheckBox*>(delayWidget);

        if (delayBox) {
            rule.isDelayEnabled = delayBox->isChecked();
            bool ok;
            rule.delayedTime = delayBox->text().toInt(&ok);
            if (!ok) {
                rule.delayedTime = 0;
            }
        } else {
            rule.isDelayEnabled = false;
            rule.delayedTime = 0;
        }

        data.append(rule);
    }

    return data;
}

void HomePage::initMenu()
{
    // 添加菜单项
    QAction* deleteAction = new QAction("删除");
    QAction* separator = new QAction();
    separator->setSeparator(true); // 添加分隔符
    QAction* clearAction = new QAction("清空");

    for (QTableWidget* table : { responseTableDSC, responseTableARC, responseTableTG }) {
        table->addAction(deleteAction);
        table->addAction(separator);
        table->addAction(clearAction);
    }

    // 将菜单项连接到对应的槽函数

    connect(deleteAction, &QAction::triggered, this, &HomePage::deleteActionTriggered);
    connect(clearAction, &QAction::triggered, this, &HomePage::clearActionTriggered); // 删除功能槽
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
        // 添加到显示当中
        QString msg = clientAddressInfo(clientSocket);
        QListWidgetItem* item = new QListWidgetItem(msg);
        clienInfoWidget->addItem(item);
        clienInfoWidget->setCurrentRow(clienInfoWidget->count() - 1);
        clienInfoWidget->setFocus();
        clienInfoWidget->update();

        clienInfoMap[clientSocket] = item;

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
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    for (int i = 0; i < clientList.size(); i++) {
        if (clientList.at(i) == clientSocket) {

            QListWidgetItem* itemToDelete = clienInfoMap.value(clientSocket, nullptr);

            if (itemToDelete) {
                int rowToRemove = clienInfoWidget->row(itemToDelete);
                clienInfoWidget->takeItem(rowToRemove);
                // 如果有关联的itemWidget 需要先移除itemWidget
                delete itemToDelete;
            }

            clienInfoMap.remove(clientSocket);
            clientList.removeAt(i);
        }
    }
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

    // 发送应答消息
    sendResponseMessage(clientSocket, data, configBox->currentText());
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

void HomePage::onClearSendButtonClicked()
{
    sendTextEdit->clear();
}

void HomePage::oncCearReceiveButtonClicked()
{
    receiveTextEdit->clear();
}

void HomePage::onInputRuleButtonCliecked(const InputRule& rule)
{
    QTableWidget* tmp = qobject_cast<QTableWidget*>(rulesTabWidget->currentWidget());
    if (!tmp) {
        return; // 确保转换成功
    }

    int row = tmp->rowCount();
    tmp->insertRow(row);
    QCheckBox* enableStatus = new QCheckBox;
    enableStatus->setChecked(true);
    tmp->setCellWidget(row, 0, enableStatus);
    tmp->setItem(row, 1, new QTableWidgetItem(rule.commandMatching));
    tmp->setItem(row, 2, new QTableWidgetItem(rule.responseTemplate));
    tmp->setItem(row, 3, new QTableWidgetItem(rule.remark));

    QCheckBox* delayStatus = new QCheckBox(QString::number(rule.delayTime));
    delayStatus->setChecked(true);
    tmp->setCellWidget(row, 4, delayStatus);
    tmp->setItem(row, 4, new QTableWidgetItem(rule.delayTime));
    tmp->setItem(row, 5, new QTableWidgetItem(rule.responseDelay));
}

void HomePage::deleteActionTriggered()
{
    QTableWidget* tmp = qobject_cast<QTableWidget*>(rulesTabWidget->currentWidget());
    tmp->removeRow(tmp->currentRow());
}

void HomePage::clearActionTriggered()
{
}
