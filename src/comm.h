#ifndef COMM_H
#define COMM_H

#include <QDebugStateSaver>
#include <QString>
class QDebug;

// 自动应答规则结构体
struct AutoReplyRule {
    int id; // 规则ID
    bool isEnabled;
    QString matchCommand;
    QString responseTemplate;
    QString remarks;
    int delayedTime;
    bool isDelayEnabled;
    QString timeoutResponse;

    // 友元打印函数
    friend QDebug operator<<(QDebug dbg, const AutoReplyRule& rule)
    {
        QDebugStateSaver saver(dbg); // 保存状态
        dbg.nospace() << "AutoReplyRule("
                      << "id=" << rule.id
                      << ", isEnabled=" << rule.isEnabled
                      << ", matchCommand=" << rule.matchCommand
                      << ", responseTemplate=" << rule.responseTemplate
                      << ", remarks=" << rule.remarks
                      << ", delayedTime=" << rule.delayedTime
                      << ", isDelayEnabled=" << rule.isDelayEnabled
                      << ", timeoutResponse=" << rule.timeoutResponse
                      << ")";
        return dbg;
    }
};

#endif // COMM_H
