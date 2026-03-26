#include <QtTest>
#include <QSqlDatabase>

#include "test_localsqllite.h"
#include "../src/localsqllite.h"

void LocalSqlLiteTest::initTestCase()
{
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/sqldrivers");
    QVERIFY(QSqlDatabase::drivers().contains("QSQLITE"));
}

void LocalSqlLiteTest::readAutoReplyData_readsDataByDataName()
{
    const QString databasePath = QCoreApplication::applicationDirPath() + "/test_replydata.db";
    QFile::remove(databasePath);
    LocalSqlLite db(nullptr, databasePath);

    const AutoReplyRule dscRule { 0, true, "AA 01", "BB 01", "DSC", 1000, true, "CC 01" };
    const AutoReplyRule arcRule { 0, true, "AA 02", "BB 02", "ARC", 1200, false, "CC 02" };
    const AutoReplyRule tgRule { 0, false, "AA 03", "BB 03", "TG", 1500, true, "CC 03" };

    db.writeAutoReplyData(LocalSqlLite::DataName::DSC, { dscRule });
    db.writeAutoReplyData(LocalSqlLite::DataName::ARC, { arcRule });
    db.writeAutoReplyData(LocalSqlLite::DataName::TG, { tgRule });

    const QVector<AutoReplyRule> dscData = db.readAutoReplyData(LocalSqlLite::DataName::DSC);
    const QVector<AutoReplyRule> arcData = db.readAutoReplyData(LocalSqlLite::DataName::ARC);
    const QVector<AutoReplyRule> tgData = db.readAutoReplyData(LocalSqlLite::DataName::TG);

    QCOMPARE(dscData.size(), 1);
    QCOMPARE(dscData.first().matchCommand, QString("AA 01"));
    QCOMPARE(dscData.first().responseTemplate, QString("BB 01"));

    QCOMPARE(arcData.size(), 1);
    QCOMPARE(arcData.first().matchCommand, QString("AA 02"));
    QCOMPARE(arcData.first().remarks, QString("ARC"));

    QCOMPARE(tgData.size(), 1);
    QCOMPARE(tgData.first().matchCommand, QString("AA 03"));
    QCOMPARE(tgData.first().isEnabled, false);

    QFile::remove(databasePath);
}

QTEST_MAIN(LocalSqlLiteTest)
