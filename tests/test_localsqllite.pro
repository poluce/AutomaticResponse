QT += testlib core sql widgets
CONFIG += testcase c++17 console
TEMPLATE = app

win32 {
    SQLDRIVER_SOURCE = $$[QT_INSTALL_PLUGINS]/sqldrivers/qsqlite.dll
    SQLDRIVER_DIR = $$OUT_PWD/release/sqldrivers
    SQLDRIVER_TARGET = $$SQLDRIVER_DIR/qsqlite.dll

    sqlDriverDeploy.target = $$SQLDRIVER_TARGET
    sqlDriverDeploy.commands = if not exist "$$shell_path($$SQLDRIVER_DIR)" mkdir "$$shell_path($$SQLDRIVER_DIR)" && copy /Y "$$shell_path($$SQLDRIVER_SOURCE)" "$$shell_path($$SQLDRIVER_TARGET)" > nul
    QMAKE_EXTRA_TARGETS += sqlDriverDeploy
    PRE_TARGETDEPS += $$SQLDRIVER_TARGET
}

SOURCES += \
    ../src/localsqllite.cpp \
    test_localsqllite.cpp

HEADERS += \
    ../src/comm.h \
    ../src/localsqllite.h \
    test_localsqllite.h
