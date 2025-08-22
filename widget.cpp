#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include <QFile>
#include <QHeaderView>
#include <QJsonDocument>
#include <QKeyEvent>
#include <QRegularExpressionValidator>

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->widget_response->hide();
    ui->pushButton_response->setCheckable(true);
    ui->pushButton_link->setCheckable(true);
    initUI();
    initData();
    initMenu();
    initLayout();
    initConnect();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUI()
{
    // tcp服务
    tcpserver = new QTcpServer(this);

    // 设置响应表格
    responseWidget = new QTableWidget;
    responseWidget->setColumnCount(6);
    responseWidget->setRowCount(100);
    responseWidget->setHorizontalHeaderLabels({ "是否开启", "匹配指令", "应答模板", "备注", "延迟时间", "延时应答" });
    responseWidget->horizontalHeader()->setStretchLastSection(true);
    responseWidget->verticalHeader()->setVisible(false);
    responseWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    responseWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    responseWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 自动应答组件
    commandMatching = new QLineEdit;
    responseTemplate = new QLineEdit;
    remark = new QLineEdit;
    responseDelay = new QLineEdit;
    delayTime = new QSpinBox;
    delayTime->setSuffix("/ms");
    delayTime->setRange(100, 200000);
    delayTime->setDisplayIntegerBase(100);
    delayTimer = new QTimer(this);
    setButton = new QPushButton("设置");

    commandMatchingGroupBox = new QGroupBox("指令匹配");
    responseTemplateGroupBox = new QGroupBox("响应模板");
    remarkGroupBox = new QGroupBox("备注");
    responseDelayGroupBox = new QGroupBox("延迟应答");

    // 16进制输入
    for (auto it : { commandMatching, responseTemplate, responseDelay }) {
        QRegularExpressionValidator* hexValidator = new QRegularExpressionValidator(
            QRegularExpression("^[0-9A-Fa-f ]*$"), it);
        it->setValidator(hexValidator);
        // 安装事件过滤器
        it->installEventFilter(this);
    }
    isDeleteKey = false;
}

void Widget::initData()
{
    QFile file("config.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    file.close();

    if (!doc.isObject()) {
        qDebug() << "JSON 格式错误";
        return;
    }

    QJsonObject obj = doc.object();

    responseWidget->clearContents(); // 清空旧内容（保留行列数）
    int rowCount = obj.size();
    int columnCount = responseWidget->columnCount();

    // 如果需要，根据 JSON 调整表格行数
    if (responseWidget->rowCount() < rowCount)
        responseWidget->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        QString key = QString::number(i);
        if (!obj.contains(key)) {
            continue;
        }

        QJsonArray steps = obj[key].toArray();
        for (int j = 0; j < steps.size() && j < columnCount; ++j) {
            QString text = steps[j].toString();
            if (j == 0) {
                QCheckBox* tmp = new QCheckBox;
                tmp->setChecked(text == "1" ? true : false);
                responseWidget->setCellWidget(i, j, tmp);
            } else {
                QTableWidgetItem* item = new QTableWidgetItem(text);
                responseWidget->setItem(i, j, item);
            }
        }
    }
}

void Widget::initLayout()
{
    // 备注
    QHBoxLayout* layout = new QHBoxLayout(remarkGroupBox);
    layout->addWidget(remark);
    layout->addWidget(setButton);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setStretchFactor(remark, 1);

    // 指令匹配
    QVBoxLayout* commandMatchingLayout = new QVBoxLayout(commandMatchingGroupBox);
    commandMatchingLayout->addWidget(commandMatching);
    commandMatchingLayout->setContentsMargins(0, 0, 0, 0);
    commandMatchingLayout->setSpacing(0);

    // 响应模板
    QVBoxLayout* responseTemplateLayout = new QVBoxLayout(responseTemplateGroupBox);
    responseTemplateLayout->addWidget(responseTemplate);
    responseTemplateLayout->setContentsMargins(0, 0, 0, 0);
    responseTemplateLayout->setSpacing(0);

    // 延迟应答
    QHBoxLayout* delayLayout = new QHBoxLayout(responseDelayGroupBox);
    delayLayout->addWidget(new QLabel("时间:"));
    delayLayout->addWidget(delayTime);
    delayLayout->addWidget(new QLabel("模板:"));
    delayLayout->addWidget(responseDelay);

    // 设置布局
    QVBoxLayout* mainLayout = new QVBoxLayout(ui->widget_response);
    mainLayout->addWidget(responseWidget, 8);
    mainLayout->addWidget(commandMatchingGroupBox, 1);
    mainLayout->addWidget(responseTemplateGroupBox, 1);
    mainLayout->addWidget(responseDelayGroupBox, 1);
    mainLayout->addWidget(remarkGroupBox, 1);
}

void Widget::initConnect()
{
    // 连接新连接信号
    connect(tcpserver, &QTcpServer::newConnection, this, &Widget::onNewConnection);
    connect(setButton, &QPushButton::clicked, this, &Widget::on_setButtonClicked);
    connect(this, &Widget::represponseWidgetTextChanged, this, &Widget::onRepresponseWidgetTextChanged);

    // 自动大写
    connect(commandMatching, &QLineEdit::editingFinished, this, &Widget::onEditingFinished);
    connect(responseTemplate, &QLineEdit::editingFinished, this, &Widget::onEditingFinished);
    connect(responseDelay, &QLineEdit::editingFinished, this, &Widget::onEditingFinished);
    connect(commandMatching, &QLineEdit::textEdited, this, [&](const QString& str) {
        QString raw = str.toUpper();
        commandMatching->setText(raw);
    });
    connect(responseTemplate, &QLineEdit::textEdited, this, [&](const QString& str) {
        QString raw = str.toUpper();
        responseTemplate->setText(raw);
    });
    connect(commandMatching, &QLineEdit::textEdited, this, [&](const QString& str) {
        QString raw = str.toUpper();
        commandMatching->setText(raw);
    });

    connect(responseWidget, &QTableWidget::cellDoubleClicked, this, [=](int row, int column) {
        Q_UNUSED(column)
        commandMatching->setText(responseWidget->item(row, 1) ? responseWidget->item(row, 1)->text() : "");
        responseTemplate->setText(responseWidget->item(row, 2) ? responseWidget->item(row, 2)->text() : "");
        remark->setText(responseWidget->item(row, 3) ? responseWidget->item(row, 3)->text() : "");
        delayTime->setValue(responseWidget->item(row, 4) ? responseWidget->item(row, 4)->text().toInt() : 0);
        responseDelay->setText(responseWidget->item(row, 5) ? responseWidget->item(row, 5)->text() : "");
    });
}

void Widget::initMenu()
{
    // 添加菜单项
    QAction* addAction = new QAction("增加");
    QAction* deleteAction = new QAction("删除");
    QAction* separator = new QAction();
    separator->setSeparator(true); // 添加分隔符
    QAction* clearAction = new QAction("清空");

    responseWidget->addAction(addAction);
    responseWidget->addAction(deleteAction);
    responseWidget->addAction(separator);
    responseWidget->addAction(clearAction);

    // 将菜单项连接到对应的槽函数
    connect(addAction, &QAction::triggered, this, &Widget::addActionTriggered);
    connect(deleteAction, &QAction::triggered, this, &Widget::deleteActionTriggered);
    connect(clearAction, &QAction::triggered, this, &Widget::clearActionTriggered); // 删除功能槽
}

void Widget::on_pushButton_response_clicked(bool checked)
{
    if (checked)
        ui->widget_response->show();
    else
        ui->widget_response->hide();
}

void Widget::on_pushButton_link_clicked(bool checked)
{
    if (checked) {
        ui->pushButton_link->setText("关闭服务");
        tcpserver->listen(QHostAddress::Any, ui->lineEdit_port->text().toInt());
    } else {
        ui->pushButton_link->setText("开启服务");
    }
}

void Widget::onNewConnection()
{
    // 获取新的客户端连接
    QTcpSocket* clientSocket = tcpserver->nextPendingConnection();

    if (clientSocket) {
        // 添加到客户端列表
        clientList.append(clientSocket);

        // 连接客户端信号
        connect(clientSocket, &QTcpSocket::disconnected, this, &Widget::onClientDisconnected);
        connect(clientSocket, &QTcpSocket::readyRead, this, &Widget::onClientDataReady);

        QString clientInfo = QString("%1:%2")
                                 .arg(clientSocket->peerAddress().toString())
                                 .arg(clientSocket->peerPort());

        qDebug() << "新客户端连接：" << clientInfo;
    }
}

void Widget::onClientDisconnected()
{
}

void Widget::onClientDataReady()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket->state() != QTcpSocket::ConnectedState) {
        return;
    }

    QByteArray data = clientSocket->readAll();

    QString ip = clientSocket->peerAddress().toString();
    if (ip.startsWith("::ffff:"))
        ip = ip.mid(7);

    QDateTime date = QDateTime::currentDateTime();
    ui->textEdit_comm->append(date.toString("hh:mm:ss:zzz"));
    QString msg = QString("%1:%2->:").arg(ip).arg(clientSocket->peerPort()) + data.toHex(' ').toUpper() + "\n";

    ui->textEdit_comm->append(msg);

    // 自动应答是否开启判断
    if (!ui->checkBox->isChecked()) {
        return;
    }

    // 检查匹配模板
    for (int i = 0; i < responseWidget->rowCount(); i++) {
        QTableWidgetItem* item = responseWidget->item(i, 1); // 获取匹配指令列
        // 指令匹配
        if (!item || item->text().trimmed() != data.toHex(' ').toUpper()) {
            continue;
        }

        // 移除空格并转换为16进制二进制数据
        QString hexString = responseWidget->item(i, 2)->text().remove(' '); // 获取应答模板列
        QByteArray hexData = QByteArray::fromHex(hexString.toUtf8());
        clientSocket->write(hexData);

        // 判断是否有延时应答内容
        if (responseWidget->item(i, 5)->text().remove(' ').length() > 0 && ui->checkBox_delay->isChecked()) {
            QTimer::singleShot(responseWidget->item(i, 4)->text().toInt(), this, [=]() {
                if (qobject_cast<QCheckBox*>(responseWidget->cellWidget(i, 0))->isChecked())
                    onResponseToDelay(clientSocket, responseWidget->item(i, 5)->text());
            });
            qDebug() << "responseWidget->item(i, 5)->text()" << responseWidget->item(i, 5)->text();
        }

        QDateTime date = QDateTime::currentDateTime();
        ui->textEdit_comm->append(date.toString("hh:mm:ss:zzz"));
        ui->textEdit_comm->append(" 自动应答：" + responseWidget->item(i, 2)->text().toUpper() + "\n");
        qDebug() << "自动应答：" << hexString;
        break;
    }
}

void Widget::on_setButtonClicked(bool checked)
{
    Q_UNUSED(checked)
    int row = responseWidget->currentRow();
    QCheckBox* tmp = new QCheckBox;
    tmp->setChecked(true);
    responseWidget->setItem(row, 0, new QTableWidgetItem);
    responseWidget->setCellWidget(row, 0, tmp);
    responseWidget->setItem(row, 1, new QTableWidgetItem(commandMatching->text()));
    responseWidget->setItem(row, 2, new QTableWidgetItem(responseTemplate->text()));
    responseWidget->setItem(row, 3, new QTableWidgetItem(remark->text()));
    responseWidget->setItem(row, 4, new QTableWidgetItem(QString::number(delayTime->value())));
    responseWidget->setItem(row, 5, new QTableWidgetItem(responseDelay->text()));
    emit represponseWidgetTextChanged();
}

void Widget::onRepresponseWidgetTextChanged()
{
    QJsonObject obj;
    for (int i = 0; i < focusFirstEmptyRow(); i++) {
        QJsonArray steps;
        for (int j = 0; j < responseWidget->columnCount(); j++) {
            QTableWidgetItem* item = responseWidget->item(i, j);
            if (j == 0) {
                QCheckBox* tmp = qobject_cast<QCheckBox*>(responseWidget->cellWidget(i, j));
                steps.append(tmp ? (tmp->isChecked() ? "1" : "0") : "");
            } else {
                steps.append(item ? item->text() : "");
            }
        }
        obj[QString("%1").arg(i)] = steps;
    }

    QJsonDocument doc(obj); // 转为文档

    QString filePath = "config.json"; //
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // 美化格式
        file.close();
        qDebug() << "写入成功：" << filePath;
    } else {
        qDebug() << "文件写入失败：" << file.errorString();
    }
}

void Widget::onEditingFinished()
{
    QLineEdit* tmp = qobject_cast<QLineEdit*>(sender());
    // 去掉所有空格
    QString raw = tmp->text();
    raw.remove(' ');

    // 每 2 个字符插入一个空格
    QString formatted;
    for (int i = 0; i < raw.length(); ++i) {
        formatted.append(raw[i]);
        if ((i % 2) == 1 && i != raw.length() - 1) // 每两个字符后加空格，但最后不用
            formatted.append(' ');
    }
    tmp->setText(formatted); // 设置格式化后的文本
}

void Widget::onResponseToDelay(QTcpSocket* socket, QString str)
{
    QString raw = str;
    QByteArray hexData = QByteArray::fromHex(raw.remove(' ').toUtf8());
    socket->write(hexData);
    socket->flush();
    QDateTime date = QDateTime::currentDateTime();
    qDebug() << "延时应答：" << str;
    ui->textEdit_comm->append(date.toString("hh:mm:ss:zzz"));
    ui->textEdit_comm->append(" 延时应答：" + str + "\n");
}

void Widget::on_pushButton_response_clicked()
{
}

void Widget::addActionTriggered()
{
    // 在响应表格中添加新行
    focusFirstEmptyRow();
    commandMatching->clear();
    responseTemplate->clear();
    remark->clear();
}

void Widget::deleteActionTriggered()
{
    int row = responseWidget->currentRow(); // 当前选中的行
    if (row >= 0) {
        responseWidget->removeRow(row); // 删除
    }
}

void Widget::clearActionTriggered()
{
}

int Widget::focusFirstEmptyRow()
{
    int rowCount = responseWidget->rowCount();
    int columnCount = responseWidget->columnCount();

    for (int row = 0; row < rowCount; ++row) {
        bool isEmpty = true;

        // 判断整行是否为空
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem* item = responseWidget->item(row, col);
            if (item && !item->text().trimmed().isEmpty()) {
                isEmpty = false;
                break;
            }
        }

        // 如果该行是空的，则设置焦点
        if (isEmpty) {
            responseWidget->setCurrentCell(row, 0); // 设置当前行的第一列为焦点
            responseWidget->setFocus();             // 聚焦控件
            return row;                             // 返回找到的空行索引
        }
    }

    // 如果没有找到空行，则添加一行
    responseWidget->insertRow(rowCount);
    responseWidget->setCurrentCell(rowCount, 0); // 设置当前行的第一列为焦点
    responseWidget->setFocus();                  // 聚焦控件
    return rowCount;                             // 返回新添加的行索引
}

bool Widget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == commandMatching && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Backspace || keyEvent->key() == Qt::Key_Delete) {
            isDeleteKey = true; // 设置删除标志位
        } else {
            isDeleteKey = false;
        }
    }

    return QWidget::eventFilter(watched, event); // 保留默认处理
}

void Widget::on_pushButton_clear_clicked()
{
    ui->textEdit_comm->clear();
}

void Widget::on_pushButton_send_clicked()
{
    QString hexString = ui->textEdit_send->toPlainText().remove(' ');
    QByteArray hexData = QByteArray::fromHex(hexString.toUtf8());
    for (auto it : clientList) {
        qDebug() << "data :::" << hexData;
        it->write(hexData);
    }

    ui->textEdit_comm->append("发送" + hexData.toHex(' '));
}
