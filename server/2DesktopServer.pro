TEMPLATE = app
TARGET = 2DesktopServer

QT += core gui widgets websockets

win32:RC_ICONS += ./icon.ico

SOURCES += main.cpp \
    install.cpp \
    installWIN.cpp

win32:SOURCES +=

RESOURCES += res.qrc

win32-msvc2013:LIBS += -lUser32 -lAdvapi32 -lShell32 -lole32

HEADERS += \
    install.h

