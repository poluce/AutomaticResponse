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

struct InputRule {
    QString commandMatching;
    QString responseTemplate;
    int delayTime;
    QString responseDelay;
    QString remark;

    // 友元打印函数
    friend QDebug operator<<(QDebug dbg, const InputRule& rule)
    {
        QDebugStateSaver saver(dbg);
        dbg.nospace() << "ReplyRuleInfo("
                      << "  匹配指令: " << rule.commandMatching
                      << "  应答模板: " << rule.responseTemplate
                      << "  延迟时间: " << rule.delayTime
                      << "  延迟应答指令: " << rule.responseDelay
                      << "  备注: " << rule.remark
                      << ")";
        return dbg;
    }
};

#endif // COMM_H
