/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationWithNoChildren class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_WITH_NO_CHILDREN_H
#define PRESENTATION_WITH_NO_CHILDREN_H

#include <QObject>
#include <QSizeF>

#include <ld_element.h>
#include <ld_visual_with_no_children.h>

#include "app_common.h"
#include "placement_negotiator.h"
#include "presentation.h"

class QGraphicsItem;

/**
 * This is a pure virtual base class used to present language elements.  The class extends the
 * Ld::VisualWithNoChildren class to bring visuals into the Qt QObject and signal/slot.  The class also provides basic
 * support for placement within the document view of the program.
 */
class APP_PUBLIC_API PresentationWithNoChildren:public Presentation, public virtual Ld::VisualWithNoChildren {
    Q_OBJECT

    public:
        PresentationWithNoChildren();

        ~PresentationWithNoChildren() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::FIXED.
         */
        Ld::Element::ChildPlacement childPlacement() const override;
};

#endif
