QT += testlib core sql widgets network
CONFIG += testcase c++17 console
TEMPLATE = app

win32 {
    SQLDRIVER_SOURCE = /e/Qt/Qt5.14.2/5.14.2/mingw73_32/plugins/sqldrivers/qsqlite.dll
    SQLDRIVER_DIR = $$OUT_PWD/release/sqldrivers
    SQLDRIVER_TARGET = $$SQLDRIVER_DIR/qsqlite.dll

    sqlDriverDeploy.target = $$SQLDRIVER_TARGET
    sqlDriverDeploy.commands = mkdir -p "$$SQLDRIVER_DIR" && cp "$$SQLDRIVER_SOURCE" "$$SQLDRIVER_TARGET"
    QMAKE_EXTRA_TARGETS += sqlDriverDeploy
    PRE_TARGETDEPS += $$SQLDRIVER_TARGET
}

SOURCES += \
    ../src/homepage.cpp \
    ../src/inputrulewidget.cpp \
    ../src/localsqllite.cpp \
    test_homepage.cpp

HEADERS += \
    ../src/comm.h \
    ../src/homepage.h \
    ../src/inputrulewidget.h \
    ../src/localsqllite.h
