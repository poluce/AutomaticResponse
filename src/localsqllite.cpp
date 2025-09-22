#include "localsqllite.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
LocalSqlLite* LocalSqlLite::instance()
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    static LocalSqlLite* instance = new LocalSqlLite(nullptr);
    return instance;
}

LocalSqlLite::LocalSqlLite(QObject* parent)
    : QObject { parent }
{
    initDatabase();
    // loadData();
    readAutoReplyData();
}

void LocalSqlLite::initDatabase()
{
    // 添加一个数据库连接，类型指定为 SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");
    // 判断文件是否已存在
    QString dbName = "replydata.db";
    bool existed = QFile::exists(dbName);

    db.setDatabaseName(dbName); // 对于 QSQLITE 驱动程序，如果指定的数据库名称不存在，则会创建该文件

    if (!db.open()) {
        qDebug() << "打开数据库失败:" << db.lastError().text();
        return;
    }

    if (existed) {
        qDebug() << "数据库已存在";
    } else {
        qDebug() << "数据库不存在，创建新数据库";
        createTable();
    }
}

void LocalSqlLite::createTable()
{
    QString sql = R"(CREATE TABLE IF NOT EXISTS auto_reply_rules (
        id INTEGER PRIMARY KEY AUTOINCREMENT,       /*SQLite 的自增主键*/
        is_enabled INTEGER NOT NULL DEFAULT 1,    /*用 INTEGER 存储布尔值, 1 代表 true, 0 代表 false*/
        match_command TEXT NOT NULL,              /*字符串统一使用 TEXT*/
        response_template TEXT,
        remarks TEXT,
        delayed_time INTEGER,
        is_delay_enabled INTEGER NOT NULL DEFAULT 1,
        timeout_response TEXT,
        created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
    );)";
    QSqlQuery query(sql);
    if (query.lastError().isValid()) {
        qDebug() << "创建表失败:" << query.lastError().text();
        return;
    } else {
        qDebug() << "创建表成功或已存在！";
    }
}

void LocalSqlLite::loadData()
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
    db.open();
    QSqlQuery query;
    QString insertSql = R"(INSERT INTO auto_reply_rules
        (is_enabled, match_command, response_template, remarks, delayed_time, timeout_response)
        VALUES (:enabled, :cmd, :resp, :remark, :delay, :timeout))";

    for (int i = 0; i < obj.size(); ++i) {
        QString key = QString::number(i);
        if (!obj.contains(key)) {
            continue;
        }

        QJsonArray steps = obj[key].toArray();
        if (steps.size() < 6) {
            qDebug() << "数据格式错误，跳过索引：" << i;
            continue;
        }

        query.prepare(insertSql);
        query.bindValue(":enabled", steps[0].toString() == "1" ? 1 : 0); // is_enabled
        query.bindValue(":cmd", steps[1].toString());                    // match_command
        query.bindValue(":resp", steps[2].toString());                   // response_template
        query.bindValue(":remark", steps[3].toString());                 // remarks
        query.bindValue(":delay", 1000);                                 // delayed_time
        query.bindValue(":timeout", steps[5].toString());                // timeout_response

        // qDebug() << "SQL:" << insertSql;
        qDebug() << "绑定参数:" << query.boundValues();

        if (!query.exec()) {
            qDebug() << "插入数据失败:" << query.lastError().text();
        } else {
            qDebug() << "插入数据成功，索引：" << i;
        }
    }
}

QVector<AutoReplyRule> LocalSqlLite::readAutoReplyData()
{
    QVector<AutoReplyRule> results;
    QString sql = "SELECT id,is_enabled, match_command, response_template, remarks, delayed_time,is_delay_enabled, timeout_response FROM auto_reply_rules";
    QSqlQuery query(sql);
    while (query.next()) {
        AutoReplyRule tmp;
        tmp.id = query.value(0).toInt();
        tmp.isEnabled = query.value(1).toInt() == 1;
        tmp.matchCommand = query.value(2).toString();
        tmp.responseTemplate = query.value(3).toString();
        tmp.remarks = query.value(4).toString();
        tmp.delayedTime = query.value(5).toInt();
        tmp.isDelayEnabled = query.value(6).toInt() == 1;
        tmp.timeoutResponse = query.value(7).toString();
        results.append(tmp);
        qDebug() << tmp;
    }
    return results;
}
