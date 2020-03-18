QT       += core gui webengine webenginewidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp \
    autorisationrequest.cpp \
    decoder.cpp \
    directory.cpp \
    internetconnection.cpp \
    listview.cpp \
    main.cpp \
    mainwindow.cpp \
    messagesend.cpp \
    messageview.cpp \
    mime_message.cpp \
    mimemessage.cpp \
    parser.cpp \
    treelistview.cpp

HEADERS += \
    account.h \
    autorisationrequest.h \
    decoder.h \
    directory.h \
    internetconnection.h \
    listview.h \
    mainwindow.h \
    messagesend.h \
    messageview.h \
    mime_message.h \
    mimemessage.h \
    parser.h \
    treelistview.h

FORMS += \
    autorisationrequest.ui \
    mainwindow.ui \
    messagesend.ui \
    messageview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += C:/OpenSSL-Win32/include/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -LC:/OpenSSL-Win32/lib/VC/ -llibeay32MD
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/OpenSSL-Win32/lib/VC/ -llibeay32MDd
else:unix: LIBS += -LC:/OpenSSL-Win32/lib/VC/ -llibeay32MD

INCLUDEPATH += C:/OpenSSL-Win32/lib/VC
DEPENDPATH += C:/OpenSSL-Win32/lib/VC

win32:CONFIG(release, debug|release): LIBS += -LC:/OpenSSL-Win32/lib/VC/ -lssleay32MD
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/OpenSSL-Win32/lib/VC/ -lssleay32MDd
else:unix: LIBS += -LC:/OpenSSL-Win32/lib/VC/ -lssleay32MD

INCLUDEPATH += C:/OpenSSL-Win32/lib/VC
DEPENDPATH += C:/OpenSSL-Win32/lib/VC
