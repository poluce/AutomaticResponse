#include "inputrulewidget.h"

#include <QLabel>
#include <QVBoxLayout>

InputRuleWidget::InputRuleWidget(QWidget* parent)
    : QWidget { parent }
{
    initUI();
    initLayout();
    initConnect();
}

void InputRuleWidget::initUI()
{
    commandMatching = new QLineEdit;
    responseTemplate = new QLineEdit;
    remark = new QLineEdit;
    responseDelay = new QLineEdit;
    delayTime = new QSpinBox;
    delayTime->setSuffix("/ms");
    delayTime->setRange(100, 200000);
    delayTimer = new QTimer(this);

    commandMatchingGroupBox = new QGroupBox("指令匹配");
    responseTemplateGroupBox = new QGroupBox("响应模板");
    remarkGroupBox = new QGroupBox("备注");
    responseDelayGroupBox = new QGroupBox("延迟应答");
    setRuleButton = new QPushButton("设置");
    remark = new QLineEdit();

    // 16进制输入
    for (auto it : { commandMatching, responseTemplate, responseDelay }) {
        QRegularExpressionValidator* hexValidator = new QRegularExpressionValidator(
            QRegularExpression("^[0-9A-Fa-f ]*$"), it);
        it->setValidator(hexValidator);
    }
}

void InputRuleWidget::initLayout()
{
    // 备注
    QHBoxLayout* layout = new QHBoxLayout(remarkGroupBox);
    layout->addWidget(remark);
    layout->addWidget(setRuleButton);
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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(commandMatchingGroupBox, 1);
    mainLayout->addWidget(responseTemplateGroupBox, 1);
    mainLayout->addWidget(responseDelayGroupBox, 1);
    mainLayout->addWidget(remarkGroupBox, 1);
}

void InputRuleWidget::initConnect()
{
    connect(setRuleButton, &QPushButton::clicked, this, &InputRuleWidget::onSetButtonClicked);
    connect(commandMatching, &QLineEdit::editingFinished, this, &InputRuleWidget::onEditingFinished);
    connect(responseTemplate, &QLineEdit::editingFinished, this, &InputRuleWidget::onEditingFinished);
    connect(responseDelay, &QLineEdit::editingFinished, this, &InputRuleWidget::onEditingFinished);
}

void InputRuleWidget::onSetButtonClicked()
{
    InputRule rule;

    rule.commandMatching = commandMatching->text();
    rule.responseTemplate = responseTemplate->text();
    rule.delayTime = delayTime->value();
    rule.responseDelay = responseDelay->text();
    rule.remark = remark->text();
    emit InputRuleButtonCliecked(rule);
    qDebug() << rule;
}

void InputRuleWidget::onEditingFinished()
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
