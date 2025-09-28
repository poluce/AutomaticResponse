#ifndef INPUTRULEWIDGET_H
#define INPUTRULEWIDGET_H

#include "comm.h"
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QWidget>

class InputRuleWidget : public QWidget {
    Q_OBJECT
public:
    explicit InputRuleWidget(QWidget* parent = nullptr);
    void initUI();
    void initLayout();
    void initConnect();

signals:
    void InputRuleButtonCliecked(const InputRule& rule);
private slots:
    void onSetButtonClicked();
    void onEditingFinished();

private:
    QGroupBox* commandMatchingGroupBox; // 匹配
    QLineEdit* commandMatching;
    QGroupBox* responseTemplateGroupBox; // 模板
    QLineEdit* responseTemplate;
    QGroupBox* remarkGroupBox; // 备注
    QLineEdit* remark;
    QGroupBox* responseDelayGroupBox; // 延迟应答
    QLineEdit* responseDelay;
    QSpinBox* delayTime;
    QPushButton* setRuleButton;
    QTimer* delayTimer;
};

#endif // INPUTRULEWIDGET_H
