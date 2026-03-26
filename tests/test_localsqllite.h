#ifndef TEST_LOCALSQLLITE_H
#define TEST_LOCALSQLLITE_H

#include <QObject>

class LocalSqlLiteTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void readAutoReplyData_readsDataByDataName();
};

#endif // TEST_LOCALSQLLITE_H
