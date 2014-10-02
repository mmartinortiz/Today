#-------------------------------------------------
#
# Project created by QtCreator 2014-01-29T09:16:09
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Today
TEMPLATE = app

# --
# Directivas para la compilación del corrector ortográfico.
# Para MAC se usa NsSpellChecker, para los demás Enchant
# Obtenido del código fuente de FocusWritter
# --
macx {
        INCLUDEPATH += src/nsspellchecker /Library/Frameworks/libzip.framework/Headers
        LIBS += -framework libzip -framework AppKit

        HEADERS += src/nsspellchecker/dictionary.h \
                src/nsspellchecker/dictionary_data.h \
                src/nsspellchecker/dictionary_manager.h

        OBJECTIVE_SOURCES += src/nsspellchecker/dictionary.mm \
                src/nsspellchecker/dictionary_data.mm \
                src/nsspellchecker/dictionary_manager.mm

        lessThan(QT_MAJOR_VERSION, 5) {
                HEADERS += src/rtf/clipboard_mac.h
                SOURCES += src/rtf/clipboard_mac.cpp
        }
} else:win32 {
        INCLUDEPATH += enchant libzip src/enchant
        LIBS += ./enchant/libenchant.dll ./libzip/libzip0.dll -lOle32

        HEADERS += src/enchant/dictionary.h \
                src/enchant/dictionary_data.h \
                src/enchant/dictionary_manager.h

        SOURCES += src/enchant/dictionary.cpp \
                src/enchant/dictionary_data.cpp \
                src/enchant/dictionary_manager.cpp

        lessThan(QT_MAJOR_VERSION, 5) {
                HEADERS += src/rtf/clipboard_windows.h
                SOURCES += src/rtf/clipboard_windows.cpp
        }
} else {
        INCLUDEPATH += src/enchant

        CONFIG += link_pkgconfig
        PKGCONFIG += enchant libzip

        HEADERS += src/enchant/dictionary.h \
                src/enchant/dictionary_data.h \
                src/enchant/dictionary_manager.h

        SOURCES += src/enchant/dictionary.cpp \
                src/enchant/dictionary_data.cpp \
                src/enchant/dictionary_manager.cpp
}

SOURCES += \
    src/about.cpp \
    src/main.cpp \
    src/preferences.cpp \
    src/smart_quotes.cpp \
    src/today.cpp \
    src/spell_checker.cpp \
    src/contentprovider.cpp \
    src/utility.cpp

HEADERS  += \
    src/about.h \
    src/preferences.h \
    src/spell_checker.h \
    src/today.h \
    src/smart_quotes.h \
    src/contentprovider.h \
    src/utility.h

FORMS    += \
    src/about.ui \
    src/preferences.ui \
    src/todaywindow.ui

OBJECTIVE_SOURCES += \
    src/nsspellchecker/dictionary_data.mm \
    src/nsspellchecker/dictionary_manager.mm \
    src/nsspellchecker/dictionary.mm

RESOURCES += \
    resources/icons/icons.qrc

TRANSLATIONS = translations/today_es.ts

OTHER_FILES += \
    README.md
