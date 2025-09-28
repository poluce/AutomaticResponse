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

#define DB_NAME "replydata.db"

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
    readDSCAutoReplyData();
}

void LocalSqlLite::initDatabase()
{
    // 判断文件是否已存在
    bool existed = QFile::exists(DB_NAME);
    // 添加一个数据库连接，类型指定为 SQLite
    QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE");
    db1.setDatabaseName(DB_NAME); // 对于 QSQLITE 驱动程序，如果指定的数据库名称不存在，则会创建该文件

    if (!existed) {
        qDebug() << "数据库文件不存在，重新创建";
        setDatabase();
    }

    if (!db().open()) {
        qDebug() << "打开数据库失败:" << db().lastError().text();
        return;
    }

    // 检查是否有需要的数据表
    checkRequiredTables();
}

void LocalSqlLite::createMissingTables(const QStringList& tableList)
{
    QString sql = R"(CREATE TABLE IF NOT EXISTS %1 (
        id INTEGER PRIMARY KEY AUTOINCREMENT,   /* 自增主键 */
        is_enabled INTEGER NOT NULL DEFAULT 1,    /*用 INTEGER 存储布尔值, 1 代表 true, 0 代表 false*/
        match_command TEXT NOT NULL UNIQUE,     /* 保证唯一 */
        response_template TEXT,
        remarks TEXT,
        delayed_time INTEGER,
        is_delay_enabled INTEGER NOT NULL DEFAULT 1,
        timeout_response TEXT,
        created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
    );)";

    db().open();
    QSqlQuery query;
    for (auto& it : tableList) {
        QString querySql = sql.arg(it);
        query.exec(querySql);
        if (query.lastError().isValid()) {
            qDebug() << "创建" << it << "表失败:" << query.lastError().text();
        } else {
            qDebug() << "创建" << it << "表成功！";
        }
    }
}

void LocalSqlLite::setDatabase()
{
    QStringList list = { "auto_reply_rules_dsc", "auto_reply_rules_arc", "auto_reply_rules_tg" };

    // 创建表
    createMissingTables(list);
}

QSqlDatabase LocalSqlLite::db()
{
    return QSqlDatabase::database();
}

// 用来从指定的congfig文件
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
    db().open();
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

QVector<AutoReplyRule> LocalSqlLite::readDSCAutoReplyData()
{
    QVector<AutoReplyRule> results;
    QString sql = "SELECT id,is_enabled, match_command, response_template, remarks, delayed_time,is_delay_enabled, timeout_response FROM auto_reply_rules_dsc";
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

QVector<AutoReplyRule> LocalSqlLite::readTGAutoReplyData()
{
    QVector<AutoReplyRule> results;
    QString sql = "SELECT id,is_enabled, match_command, response_template, remarks, delayed_time,is_delay_enabled, timeout_response FROM auto_reply_rules_tg";
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

QVector<AutoReplyRule> LocalSqlLite::readARCAutoReplyData()
{
    QVector<AutoReplyRule> results;
    QString sql = "SELECT id,is_enabled, match_command, response_template, remarks, delayed_time,is_delay_enabled, timeout_response FROM auto_reply_rules_arc";
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

void LocalSqlLite::writeAutoReplyData(const DataName& dataName, const QVector<AutoReplyRule>& data)
{
    QString sqlTemplate = R"(INSERT INTO %1 (is_enabled, match_command, response_template, remarks,
            delayed_time, is_delay_enabled, timeout_response)
            VALUES (:is_enabled, :match_command, :response_template, :remarks,
                :delayed_time, :is_delay_enabled, :timeout_response)
        ON CONFLICT(match_command)
        DO UPDATE SET
        is_enabled = excluded.is_enabled,
        response_template = excluded.response_template,
        remarks = excluded.remarks,
        delayed_time = excluded.delayed_time,
        is_delay_enabled = excluded.is_delay_enabled,
        timeout_response = excluded.timeout_response
    )";

    QString tableName;
    if (dataName == DataName::DSC) {
        tableName = "auto_reply_rules_dsc";
    } else if (dataName == DataName::ARC) {
        tableName = "auto_reply_rules_arc";
    } else if (dataName == DataName::TG) {
        tableName = "auto_reply_rules_tg";
    } else {
        qWarning() << "writeAutoReplyData: 未知的 DataName 类型。";
        return;
    }

    // 得到最终的 SQL 语句
    QString finalSql = sqlTemplate.arg(tableName);

    if (!db().transaction()) {
        qWarning() << "开启事务失败：" << db().lastError().text();
        return;
    }

    QSqlQuery query;

    QString deleteSql = QString("DELETE FROM %1").arg(tableName);
    if (!query.exec(deleteSql)) {
        qDebug() << "清空旧数据失败：" << query.lastError().text();
        db().rollback();
        return;
    }

    // 循环外部准备语句
    if (!query.prepare(finalSql)) {
        qDebug() << "SQL 语句准备失败：" << query.lastError().text();
        return;
    }

    for (auto& it : data) {
        query.bindValue(":is_enabled", it.isEnabled ? 1 : 0);
        query.bindValue(":match_command", it.matchCommand);
        query.bindValue(":response_template", it.responseTemplate);
        query.bindValue(":remarks", it.remarks);
        query.bindValue(":delayed_time", it.delayedTime);
        query.bindValue(":is_delay_enabled", it.isDelayEnabled ? 1 : 0);
        query.bindValue(":timeout_response", it.timeoutResponse);
        if (!query.exec()) {
            qDebug() << "更新数据失败：" << query.lastError().text() << "执行语句：" << query.lastQuery();
            qDebug() << it;
        }
    }

    if (db().commit()) {
        qDebug() << "自动回复数据写入成功并提交。";
    } else {
        qDebug() << "数据提交失败，已回滚：" << db().lastError().text();
    }
}

QStringList LocalSqlLite::getMissingTables(const QStringList& requiredTables, const QSqlDatabase& db)
{
    // 构建查询，检查多个表
    QString placeholders = QString("?,").repeated(requiredTables.size());
    placeholders.chop(1); // 移除最后一个逗号

    QString sql = QString("SELECT name FROM sqlite_master WHERE type='table' AND name IN (%1)")
                      .arg(placeholders);

    QSqlQuery query(db);
    query.prepare(sql);

    // 绑定所有表名
    for (const QString& tableName : requiredTables) {
        query.addBindValue(tableName);
    }

    // 获取存在的表
    QStringList existingTables;
    if (query.exec()) {
        while (query.next()) {
            existingTables << query.value(0).toString();
        }
    } else {
        qDebug() << "查询失败:" << query.lastError().text();
        return requiredTables; // 如果查询失败，假设所有表都不存在
    }

    // 找出缺失的表
    QStringList missingTables;
    for (const QString& required : requiredTables) {
        if (!existingTables.contains(required)) {
            missingTables << required;
        }
    }

    return missingTables;
}

void LocalSqlLite::checkRequiredTables()
{
    QStringList requiredTables = {
        "auto_reply_rules_dsc",
        "auto_reply_rules_arc",
        "auto_reply_rules_tg"
    };

    QStringList missingTables = getMissingTables(requiredTables, db());

    if (missingTables.isEmpty()) {
        qDebug() << "所有必需的表都存在";
    } else {
        qDebug() << "缺失的表:" << missingTables;
        // 创建缺失的表
        createMissingTables(missingTables);
    }
}
