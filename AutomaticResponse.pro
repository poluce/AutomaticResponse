QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/customstyle.cpp \
    src/homepage.cpp \
    src/localsqllite.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/sidebar.cpp \
    src/sidebaroptionsbutton.cpp \
    src/sidebarwidget.cpp \
    src/widget.cpp


HEADERS += \
    src/comm.h \
    src/customstyle.h \
    src/homepage.h \
    src/localsqllite.h \
    src/mainwindow.h \
    src/sidebar.h \
    src/sidebaroptionsbutton.h \
    src/sidebarwidget.h \
    src/widget.h


FORMS += \
    src/widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/res.qrc
