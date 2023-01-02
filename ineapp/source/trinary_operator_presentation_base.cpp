/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref TrinaryOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QList>
#include <QBitArray>
#include <QFontMetricsF>
#include <QGraphicsItem>
#include <QFont>
#include <QPen>
#include <QBrush>

#include <util_algorithm.h>

#include <eqt_graphics_math_group.h>

#include <limits>
#include <algorithm>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_operator_format.h>

#include "application.h"
#include "presentation.h"
#include "presentation_with_fixed_children.h"
#include "scene_units.h"
#include "trinary_operator_presentation_base.h"

TrinaryOperatorPresentationBase::TrinaryOperatorPresentationBase() {}


TrinaryOperatorPresentationBase::~TrinaryOperatorPresentationBase() {}
