# Plik utworzony przez menad?era projektów kdevelopa
# ------------------------------------------- 
# Podkatalog wzgl?dem g?ównego katalogu projektu: ./src
# Cel to aplikacja ../bin/qtlen

INSTALLS += icons \
            translations \
            target 
target.path = /usr/bin 
translations.files += ../share/qtlen/translations/*.qm 
translations.path = /usr/share/qtlen/translations 
icons.files += ../share/qtlen/icons/* 
icons.path = /usr/share/qtlen/icons 
TRANSLATIONS += $$LANG_PATH/qtlen_pl.ts \
                $$LANG_PATH/qtlen_en.ts 
HEADERS += qtlen.h \
           tlen.h \
           message.h \
           utils.h \
           auth.h \
           settingsdialog.h \
           chat.h \
           message_manager.h \
           roster_box.h \
           roster_manager.h \
           presence_manager.h \
           pubdir_manager.h \
           trayicon.h \
           editdlg.h \
           history_manager.h \
           sound_manager.h \
           hub_manager.h 
SOURCES += qtlen.cpp \
           main.cpp \
           tlen.cpp \
           message.cpp \
           utils.cpp \
           auth.cpp \
           settingsdialog.cpp \
           chat.cpp \
           message_manager.cpp \
           roster_box.cpp \
           roster_manager.cpp \
           presence_manager.cpp \
           pubdir_manager.cpp \
           trayicon.cpp \
           editdlg.cpp \
           history_manager.cpp \
           sound_manager.cpp \
           hub_manager.cpp 
TARGET = ../bin/qtlen 
CONFIG += debug \
          warn_on \
          qt \
          thread \
          x11 
TEMPLATE = app 
LANG_PATH = ../share/qtlen/translations
DEFAULTCODEC = UTF-8
unix{
  !mac{
    SOURCES += trayicon_x11.cpp
  }
}
win32{
  {
  SOURCES += trayicon_win.cpp
  }
}
mac{
  {
  SOURCES += trayicon_mac.cpp
  }
}
