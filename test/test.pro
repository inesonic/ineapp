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
# Libraries
#

include("$${SOURCE_ROOT}/libraries/inecontainer/inecontainer.pri")
include("$${SOURCE_ROOT}/libraries/ineqcontainer/ineqcontainer.pri")
include("$${SOURCE_ROOT}/libraries/inecbe/inecbe.pri")
include("$${SOURCE_ROOT}/libraries/inem/inem.pri")
include("$${SOURCE_ROOT}/libraries/inemkl/inemkl.pri")
include("$${SOURCE_ROOT}/libraries/inemat/inemat.pri")
include("$${SOURCE_ROOT}/libraries/ineud/ineud.pri")
include("$${SOURCE_ROOT}/libraries/inewh/inewh.pri")
include("$${SOURCE_ROOT}/libraries/ineld/ineld.pri")
include("$${SOURCE_ROOT}/libraries/ineapp/ineapp.pri")
include("$${SOURCE_ROOT}/libraries/ineeqt/ineeqt.pri")
include("$${SOURCE_ROOT}/libraries/inecrypto/inecrypto.pri")
include("$${SOURCE_ROOT}/libraries/ineutil/ineutil.pri")

include("$${SOURCE_ROOT}/third_party/llvm.pri")
include("$${SOURCE_ROOT}/third_party/boost.pri")
include("$${SOURCE_ROOT}/third_party/mkl.pri")
include("$${SOURCE_ROOT}/third_party/ipp.pri")
include("$${SOURCE_ROOT}/third_party/operating_system.pri")

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
