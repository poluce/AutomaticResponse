#include <QtTest>
#include <QSqlDatabase>

#include "../src/homepage.h"
#include "../src/localsqllite.h"

class HomePageTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void sendResponseMessage_skipsDisabledRowsAndMatchesLaterRule();
    void onInputRuleButtonCliecked_preservesDelayTimeInTableData();
};

void HomePageTest::initTestCase()
{
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/sqldrivers");
    QVERIFY(QSqlDatabase::drivers().contains("QSQLITE"));
}

void HomePageTest::sendResponseMessage_skipsDisabledRowsAndMatchesLaterRule()
{
    const QString databasePath = QCoreApplication::applicationDirPath() + "/homepage_test_replydata.db";
    QFile::remove(databasePath);
    LocalSqlLite db(nullptr, databasePath);

    const AutoReplyRule disabledRule { 0, false, "AA 01", "BB 01", "DISABLED", 1000, false, "" };
    const AutoReplyRule matchedRule { 0, true, "AA 01", "CC 02", "MATCHED", 1500, false, "" };
    db.writeAutoReplyData(LocalSqlLite::DataName::DSC, { disabledRule, matchedRule });

    HomePage page(&db);

    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, server.serverPort());
    QVERIFY(client.waitForConnected());

    QVERIFY(server.waitForNewConnection());
    QTcpSocket* peer = server.nextPendingConnection();
    QVERIFY(peer != nullptr);

    page.sendResponseMessage(peer, "AA 01", "DSC");
    QVERIFY(client.waitForReadyRead());
    QCOMPARE(client.readAll().toHex(' ').toUpper(), QByteArray("CC 02"));

    peer->disconnectFromHost();
    client.disconnectFromHost();
    QFile::remove(databasePath);
}

void HomePageTest::onInputRuleButtonCliecked_preservesDelayTimeInTableData()
{
    const QString databasePath = QCoreApplication::applicationDirPath() + "/homepage_test_replydata_2.db";
    QFile::remove(databasePath);
    LocalSqlLite db(nullptr, databasePath);
    HomePage page(&db);

    InputRule rule;
    rule.commandMatching = "AA 10";
    rule.responseTemplate = "BB 10";
    rule.delayTime = 1500;
    rule.responseDelay = "CC 10";
    rule.remark = "remark";

    page.onInputRuleButtonCliecked(rule);

    const QVector<AutoReplyRule> data = page.getTableData("DSC");
    QCOMPARE(data.size(), 1);
    QCOMPARE(data.first().delayedTime, 1500);
    QCOMPARE(data.first().timeoutResponse, QString("CC 10"));

    QFile::remove(databasePath);
}

QTEST_MAIN(HomePageTest)
#include "test_homepage.moc"
