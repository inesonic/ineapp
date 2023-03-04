##-*-makefile-*-########################################################################################################
# Copyright 2016 Inesonic, LLC
# All Rights Reserved
########################################################################################################################

########################################################################################################################
# Basic build characteristics
#

TEMPLATE = app
QT += core testlib gui widgets svg network
CONFIG += testcase c++14

HEADERS = application_wrapper.h \
          test_presentation_with_fixed_children.h \
          test_presentation_with_positional_children.h \
          test_presentation_with_floating_children.h \
          test_placement_line_data.h \
          test_presentation_area.h \
          test_placement_line_start.h \
          test_placement_line_start_list.h \
          test_presentation_locator.h \
          test_root_child_location.h \
          test_root_presentation.h \
          test_command_container.h \

#test_element_database.h \

SOURCES = test_ineapp.cpp \
          application_wrapper.cpp \
          test_presentation_with_fixed_children.cpp \
          test_presentation_with_positional_children.cpp \
          test_presentation_with_floating_children.cpp \
          test_placement_line_data.cpp \
          test_presentation_area.cpp \
          test_placement_line_start.cpp \
          test_placement_line_start_list.cpp \
          test_presentation_locator.cpp \
          test_root_child_location.cpp \
          test_root_presentation.cpp \
          test_command_container.cpp \

#test_element_database.cpp \

########################################################################################################################
# ineapp library:
#

INEAPP_BASE = $${OUT_PWD}/../ineapp/
INCLUDEPATH = $${PWD}/../ineapp/include/ $${PWD}/../ineapp/customer_include/

unix {
    CONFIG(debug, debug|release) {
        LIBS += -L$${INEAPP_BASE}/build/debug/ -lineapp
        PRE_TARGETDEPS += $${INEAPP_BASE}/build/debug/libineapp.so
    } else {
        LIBS += -L$${INEAPP_BASE}/build/release/ -lineapp
        PRE_TARGETDEPS += $${INEAPP_BASE}/build/release/libineapp.so
    }
}

win32 {
    CONFIG(debug, debug|release) {
        LIBS += $${INEAPP_BASE}/build/Debug/ineapp.lib
        PRE_TARGETDEPS += $${INEAPP_BASE}/build/Debug/ineapp.lib
    } else {
        LIBS += $${INEAPP_BASE}/build/Release/ineapp.lib
        PRE_TARGETDEPS += $${INEAPP_BASE}/build/Release/ineapp.lib
    }
}

########################################################################################################################
# Libraries
#

defined(SETTINGS_PRI, var) {
    include($${SETTINGS_PRI})
}

INCLUDEPATH += $${INELD_INCLUDE}
INCLUDEPATH += $${INEEQT_INCLUDE}
INCLUDEPATH += $${INECONTAINER_INCLUDE}
INCLUDEPATH += $${INEQCONTAINER_INCLUDE}
INCLUDEPATH += $${INECBE_INCLUDE}
INCLUDEPATH += $${INEM_INCLUDE}
INCLUDEPATH += $${INEUTIL_INCLUDE}
INCLUDEPATH += $${INEUD_INCLUDE}
INCLUDEPATH += $${INEWH_INCLUDE}
INCLUDEPATH += $${INECRYPTO_INCLUDE}
INCLUDEPATH += $${BOOST_INCLUDE}

LIBS += -L$${INELD_LIBDIR} -lineld
LIBS += -L$${INEEQT_LIBDIR} -lineeqt
LIBS += -L$${INECONTAINER_LIBDIR} -linecontainer
LIBS += -L$${INEQCONTAINER_LIBDIR} -lineqcontainer
LIBS += -L$${INECBE_LIBDIR} -linecbe
LIBS += -L$${INEM_LIBDIR} -linem
LIBS += -L$${INEUTIL_LIBDIR} -lineutil
LIBS += -L$${INEUD_LIBDIR} -lineud
LIBS += -L$${INEWH_LIBDIR} -linewh
LIBS += -L$${INECRYPTO_LIBDIR} -linecrypto

defined(LLVM_PRI, var) {
    include($${LLVM_PRI})
}

defined(INEMAT_PRI, var) {
    include($${INEMAT_PRI})
}

########################################################################################################################
# Operating System
#

unix {
#  Note that some of these libraries may not be part of the OS.  See if we need a distinct PRI file for any of them.
    unix:!macx {
        LIBS += -lrt -ldl
    } else {
        LIBS += -lncurses
    }

    LIBS += -lpthread
    LIBS += -lz
    LIBS += -lm
}

win32 {
    exists("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0") {
        WINDOWS_KIT_BASE="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0"
    } else {
        exists("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0") {
            WINDOWS_KIT_BASE="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0"
        } else {
            error("Unknown/Missing Windows kit.")
        }
    }

    contains(QMAKE_TARGET.arch, x86_64) {
        WINDOWS_KIT_DIRECTORY="$${WINDOWS_KIT_BASE}/um/x64"
    } else {
        WINDOWS_KIT_DIRECTORY="$${WINDOWS_KIT_BASE}/um/x86"
    }

    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Version.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Psapi.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/shell32.lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Ole32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Uuid.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Kernel32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/User32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Gdi32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/WinSpool.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/OleAut32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/User32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/ComDlg32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/AdvAPI32.Lib"

    !contains(DEFINES, CBE_EXTERNAL_LINKER) {
        contains(QMAKE_TARGET.arch, x86_64) {
            LIBS += "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/DIA SDK/lib/amd64/diaguids.lib"
        } else {
            LIBS += "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/DIA SDK/lib/diaguids.lib"
        }
    }
}

########################################################################################################################
# Locate build intermediate and output products
#

TARGET = test_ineapp

CONFIG(debug, debug|release) {
    unix:DESTDIR = build/debug
    win32:DESTDIR = build/Debug
} else {
    unix:DESTDIR = build/release
    win32:DESTDIR = build/Release
}

OBJECTS_DIR = $${DESTDIR}/objects
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui
