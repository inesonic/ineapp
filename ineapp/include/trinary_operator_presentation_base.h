/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TrinaryOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TRINARY_OPERATOR_PRESENTATION_BASE_H
#define TRINARY_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QFont>

#include <ld_element.h>
#include <ld_format_structures.h>

#include "app_common.h"
#include "presentation_area_tracker.h"
#include "operator_presentation_base.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsMathGroup;
}

class PlacementNegotiator;
class Presentation;
class PresentationArea;

/**
 * You can use this class as a base class for trinary operator presentations.
 */
class APP_PUBLIC_API TrinaryOperatorPresentationBase:public OperatorPresentationBase {
    Q_OBJECT

    public:
        TrinaryOperatorPresentationBase();

        ~TrinaryOperatorPresentationBase() override;

    protected:

    private:
};

#endif

