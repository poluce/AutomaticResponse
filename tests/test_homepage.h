#ifndef TEST_HOMEPAGE_H
#define TEST_HOMEPAGE_H

#include <QObject>

class HomePageTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void sendResponseMessage_skipsDisabledRowsAndMatchesLaterRule();
    void onInputRuleButtonCliecked_preservesDelayTimeInTableData();
};

#endif // TEST_HOMEPAGE_H
