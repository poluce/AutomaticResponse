#ifndef LOCALSQLLITE_H
#define LOCALSQLLITE_H

#include "comm.h"
#include <QObject>
#include <QSqlDatabase>

class LocalSqlLite : public QObject {
    Q_OBJECT
public:
    enum class DataName {
        DSC,
        ARC,
        TG,
    };

public:
    static LocalSqlLite* instance();

    // 读数据
    QVector<AutoReplyRule> readDSCAutoReplyData();
    QVector<AutoReplyRule> readTGAutoReplyData();
    QVector<AutoReplyRule> readARCAutoReplyData();
    // 写数据
    void writeAutoReplyData(const DataName& dataName, const QVector<AutoReplyRule>& data);

private:
    explicit LocalSqlLite(QObject* parent = nullptr);
    LocalSqlLite(const LocalSqlLite&) = delete;            // 禁止拷贝构造
    LocalSqlLite& operator=(const LocalSqlLite&) = delete; // 禁止赋值
    void initDatabase();
    void createMissingTables(const QStringList& tableList);
    void setDatabase();
    QSqlDatabase db();
    void checkRequiredTables();
    QStringList getMissingTables(const QStringList& requiredTables, const QSqlDatabase& db);

    // 迁移函数从json文件到数据库
    void loadData();

signals:
};

#endif // LOCALSQLLITE_H
