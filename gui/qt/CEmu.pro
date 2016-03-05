lessThan(QT_MAJOR_VERSION, 5) : error("You need at least Qt 5 to build CEmu!")

# CEmu version
if (0) { # GitHub release/deployment build. Has to correspond to the git tag.
    DEFINES += CEMU_VERSION=\\\"1.0\\\"
} else { # Development build. Used in the about screen
    GIT_VERSION = $$system(git describe --abbrev=7 --dirty --always --tags)
    DEFINES += CEMU_VERSION=\\\"0.4dev_$$GIT_VERSION\\\"
}

# Code beautifying
DISTFILES += ../../.astylerc

QT += core gui quick widgets quickwidgets network

TARGET = CEmu
TEMPLATE = app

# Localization
TRANSLATIONS += i18n/fr_FR.ts i18n/es_ES.ts

CONFIG += c++11

# Core options
DEFINES += DEBUG_SUPPORT

# GCC/clang flags
if (!win32-msvc*) {
    GLOBAL_FLAGS    += -g3 -W -Wall -Wno-unused-parameter -Werror=shadow -Werror=write-strings -Werror=redundant-decls -Werror=format -Werror=format-security -Werror=declaration-after-statement -Werror=implicit-function-declaration -Werror=date-time -Werror=missing-prototypes -Werror=return-type -Werror=pointer-arith -Winit-self
    GLOBAL_FLAGS    += -ffunction-sections -fdata-sections -fno-strict-overflow
    QMAKE_CFLAGS    += -std=gnu11
    QMAKE_CXXFLAGS  += -fno-exceptions
    CONFIG(release, debug|release): GLOBAL_FLAGS += -O3 -flto
} else {
    # TODO: add equivalent flags
    # Example for -Werror=shadow: /weC4456 /weC4457 /weC4458 /weC4459
    #     Source: https://connect.microsoft.com/VisualStudio/feedback/details/1355600/
    QMAKE_CXXFLAGS  += /Wall
}

if (macx|linux) {
    GLOBAL_FLAGS    += -fPIE -Wstack-protector -fstack-protector-strong --param=ssp-buffer-size=1
    CONFIG(debug, debug|release): GLOBAL_FLAGS += -fsanitize=address,bounds -fsanitize-undefined-trap-on-error -O0
}

macx:  QMAKE_LFLAGS += -Wl,-dead_strip
linux: QMAKE_LFLAGS += -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,--gc-sections -pie

QMAKE_CFLAGS    += $$GLOBAL_FLAGS
QMAKE_CXXFLAGS  += $$GLOBAL_FLAGS
QMAKE_LFLAGS    += $$GLOBAL_FLAGS

ios {
    DEFINES += IS_IOS_BUILD
    QMAKE_INFO_PLIST = Info.plist
}

macx: ICON = resources/icons/icon.icns

SOURCES +=  utils.cpp \
    main.cpp\
    mainwindow.cpp \
    romselection.cpp \
    qtframebuffer.cpp \
    lcdwidget.cpp \
    emuthread.cpp \
    qtkeypadbridge.cpp \
    qmlbridge.cpp \
    keymap.cpp \
    qhexedit/chunks.cpp \
    qhexedit/commands.cpp \
    qhexedit/qhexedit.cpp \
    ../../core/asic.c \
    ../../core/cpu.c \
    ../../core/keypad.c \
    ../../core/lcd.c \
    ../../core/registers.c \
    ../../core/apb.c \
    ../../core/interrupt.c \
    ../../core/flash.c \
    ../../core/misc.c \
    ../../core/schedule.c \
    ../../core/timers.c \
    ../../core/usb.c \
    ../../core/sha256.c \
    ../../core/realclock.c \
    ../../core/backlight.c \
    ../../core/cert.c \
    ../../core/control.c \
    ../../core/mem.c \
    ../../core/link.c \
    ../../core/vat.c \
    ../../core/debug/disasm.cpp \
    ../../core/debug/debug.c \
    ../../core/emu.c \
    capture/gif.cpp \
    datawidget.cpp \
    lcdpopout.cpp

linux|macx|ios: SOURCES += ../../core/os/os-linux.c
win32: SOURCES += ../../core/os/os-win32.c

HEADERS  +=  utils.h \
    mainwindow.h \
    romselection.h \
    qtframebuffer.h \
    lcdwidget.h \
    emuthread.h \
    qtkeypadbridge.h \
    qmlbridge.h \
    keymap.h \
    qhexedit/chunks.h \
    qhexedit/commands.h \
    qhexedit/qhexedit.h \
    ../../core/asic.h \
    ../../core/cpu.h \
    ../../core/defines.h \
    ../../core/keypad.h \
    ../../core/lcd.h \
    ../../core/registers.h \
    ../../core/tidevices.h \
    ../../core/apb.h \
    ../../core/interrupt.h \
    ../../core/emu.h \
    ../../core/flash.h \
    ../../core/misc.h \
    ../../core/schedule.h \
    ../../core/timers.h \
    ../../core/usb.h \
    ../../core/sha256.h \
    ../../core/realclock.h \
    ../../core/backlight.h \
    ../../core/cert.h \
    ../../core/control.h \
    ../../core/mem.h \
    ../../core/link.h \
    ../../core/vat.h \
    ../../core/debug/debug.h \
    ../../core/debug/disasm.h \
    ../../core/os/os.h \
    capture/gif.h \
    capture/giflib.h \
    datawidget.h \
    lcdpopout.h

FORMS    += mainwindow.ui \
    romselection.ui \
    lcdpopout.ui

RESOURCES += \
    resources.qrc

RC_ICONS += resources\icons\icon.ico
