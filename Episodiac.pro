QT       += core
QT       += gui
QT       += network
QT       += xml

DEFINES  += QT_NO_SSL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Episodiac
TEMPLATE = app

SOURCES     += main.cpp\
               mainwindow.cpp \
               einstellungen.cpp \
               info.cpp

HEADERS     += mainwindow.h \
               einstellungen.h \
               info.h

FORMS       += mainwindow.ui \
               einstellungen.ui \
               info.ui

RESOURCES   += \
               episodiacres.qrc

TRANSLATIONS = languages/lang_en.ts \
               languages/lang_de.ts

win32 {
    INCLUDEPATH += "C:\Qt\Qt5.5.1\5.5\msvc2013\lib"
    INCLUDEPATH += "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include"
    LIBS += -L"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib" -ladvapi32 -luser32
    RC_ICONS = icon.ico
}

macx {
    ICON = episodiac.icns
}

VERSION = 1.4.0.0
win32 {
    QMAKE_TARGET_COMPANY = "GrafSauger"
    QMAKE_TARGET_PRODUCT = "Episodiac  - TV Show Rename Tool"
    QMAKE_TARGET_DESCRIPTION = "Episodiac TV - Show Rename Tool"
    QMAKE_TARGET_COPYRIGHT = "2016 by GrafSauger"

    DEFINES += APP_VERSION=\\\"$$VERSION\\\"
    DEFINES += APP_PRODUCT=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\"
}
