QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = edito.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += \
    HUNSPELL_STATIC \
    PROJECT_ROOT=\\\"$$PWD/\\\"

INCLUDEPATH += \
    $$PWD/third-party/hunspell \
    $$PWD/third-party/uchardet

LIBS += -L$$PWD/third-party/uchardet \
        -luchardet

SOURCES += \
    codeeditor.cpp \
    editor.cpp \
    encdetector.cpp \
    findandreplace.cpp \
    gotodialog.cpp \
    main.cpp \
    edito.cpp \
    preferencesdialog.cpp \
    spellchecker.cpp \
    third-party/hunspell/affentry.cxx \
    third-party/hunspell/affixmgr.cxx \
    third-party/hunspell/csutil.cxx \
    third-party/hunspell/filemgr.cxx \
    third-party/hunspell/hashmgr.cxx \
    third-party/hunspell/hunspell.cxx \
    third-party/hunspell/hunzip.cxx \
    third-party/hunspell/phonet.cxx \
    third-party/hunspell/replist.cxx \
    third-party/hunspell/suggestmgr.cxx

HEADERS += \
    codeeditor.h \
    edito.h \
    editor.h \
    encdetector.h \
    findandreplace.h \
    gotodialog.h \
    preferencesdialog.h \
    spellchecker.h \
    third-party/hunspell/affentry.hxx \
    third-party/hunspell/affixmgr.hxx \
    third-party/hunspell/atypes.hxx \
    third-party/hunspell/baseaffix.hxx \
    third-party/hunspell/csutil.hxx \
    third-party/hunspell/filemgr.hxx \
    third-party/hunspell/hashmgr.hxx \
    third-party/hunspell/htypes.hxx \
    third-party/hunspell/hunspell.h \
    third-party/hunspell/hunspell.hxx \
    third-party/hunspell/hunversion.h \
    third-party/hunspell/hunvisapi.h \
    third-party/hunspell/hunzip.hxx \
    third-party/hunspell/langnum.hxx \
    third-party/hunspell/phonet.hxx \
    third-party/hunspell/replist.hxx \
    third-party/hunspell/suggestmgr.hxx \
    third-party/hunspell/utf_info.hxx \
    third-party/hunspell/w_char.hxx

FORMS += \
    edito.ui \
    editor.ui \
    findandreplace.ui \
    gotodialog.ui \
    preferencesdialog.ui

TRANSLATIONS += \
    Edito_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
