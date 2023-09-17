QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../include/mainframe_global.h \
    ../include/mainframeinterface.h \
    ../include/moduleinterface.h \
    ../include/ribbonframewindow.h \
    mainframedefine.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


CONFIG(release, debug|release): {
    LIBS += -L$$PWD/../bin/release/ -lRibbonFrame -lScintillaQt
    DESTDIR = $$PWD/../bin/release
}
else:CONFIG(debug, debug|release): {
    LIBS += -L$$PWD/../bin/debug/ -lRibbonFrame -lScintillaQt
    DESTDIR = $$PWD/../bin/debug
}

INCLUDEPATH += $$PWD/../include \
    $$PWD/../scintilla/include \
    $$PWD/../scintilla/qt/ScintillaEditBase


RESOURCES += \
    resource.qrc

RC_FILE += \
    res/res.rc
