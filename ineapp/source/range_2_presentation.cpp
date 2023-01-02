/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Range2Presentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <eqt_graphics_math_group.h>

#include <ld_format_structures.h>
#include <ld_range_2_element.h>
#include <ld_range_2_visual.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "range_2_presentation.h"

Range2Presentation::Range2Presentation() {}


Range2Presentation::~Range2Presentation() {}


Ld::Visual* Range2Presentation::creator(const QString&) {
    return new Range2Presentation();
}


QString Range2Presentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr(",%1, ").arg(QChar(0x2026));

}
