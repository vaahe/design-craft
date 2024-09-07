QT += core gui widgets

CONFIG += c++17

# Uncomment to disable deprecated APIs
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    src/main.cpp \
    src/windows/mainwindow.cpp

HEADERS += \
    src/windows/mainwindow.h

FORMS += \
    src/windows/mainwindow.ui

# Deployment settings
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
