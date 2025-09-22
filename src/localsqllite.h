#ifndef LOCALSQLLITE_H
#define LOCALSQLLITE_H

#include "comm.h"
#include <QObject>
#include <QSqlDatabase>

class LocalSqlLite : public QObject {
    Q_OBJECT

public:
    static LocalSqlLite* instance();

    QVector<AutoReplyRule> readAutoReplyData();

private:
    explicit LocalSqlLite(QObject* parent = nullptr);
    LocalSqlLite(const LocalSqlLite&) = delete;            // 禁止拷贝构造
    LocalSqlLite& operator=(const LocalSqlLite&) = delete; // 禁止赋值
    void initDatabase();
    void createTable();

    // 迁移函数从json文件到数据库
    void loadData();

    // TODO:添加读取数据的函数
    // TODO:添加写入数据的函数

signals:

private:
    QSqlDatabase db;
};

#endif // LOCALSQLLITE_H
