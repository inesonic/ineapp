/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationWithFloatingChildren class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_WITH_FLOATING_CHILDREN_H
#define PRESENTATION_WITH_FLOATING_CHILDREN_H

#include <QObject>
#include <QPointF>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_location.h>
#include <ld_visual_with_floating_children.h>

#include "app_common.h"
#include "presentation.h"

/**
 * This is a pure virtual base class used to present language elements.  The class extends the
 * Ld::VisualWithFloatingChildren class to bring visuals into the Qt QObject and signal/slot framework and to include
 * additional methods used to support positioning and rendering of components.
 */
class APP_PUBLIC_API PresentationWithFloatingChildren:public Presentation,
                                                      public virtual Ld::VisualWithFloatingChildren {
    Q_OBJECT

    public:
        PresentationWithFloatingChildren();

        ~PresentationWithFloatingChildren() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::FLOATING.
         */
        Ld::Element::ChildPlacement childPlacement() const final;

    signals:
        /**
         * Signal that is emitted when a child is removed by the associated
         * Ld::ElementWithFloatingChildren class.  This signal is emitted by both the
         * Ld::ElementWithFloatingChildren::removeChild(Ld::ElementPointer) method.
         *
         * \param[in] location          The location where the child existed.
         *
         * \param[in] childPresentation The child presentation being removed.
         */
        void removingChildPresentation(const QPointF& location, Presentation* childPresentation);

        /**
         * Signal that is emitted when a child is inserted at a specified location by the
         * Ld::ElementWithFloatingChildren class.  This slot is triggered by the
         * Ld::ElementWithFloatingChildren::insertChild method.
         *
         * \param[in] childPresentation The child that was just added.
         *
         * \param[in] location          The desired location for the child element.
         */
        void childPresentationNowInserted(Presentation* childPresentation, const QPointF& location);

        /**
         * Signal that is emitted when a child is moved by the Ld::ElementWithFloatingChildren class.  This slot is
         * triggered by the Ld::ElementWithFloatingChildren::relocateChild method.
         *
         * \param[in] childPresentation The child to be relocated.
         *
         * \param[in] oldLocation       The old location for the child.
         *
         * \param[in] newLocation       The new location for the child.
         */
        void childPresentationNowRelocated(
            Presentation*  childPresentation,
            const QPointF& oldLocation,
            const QPointF& newLocation
        );

    protected slots:
        /**
         * Slot that is triggered when a child is removed by the associated
         * Ld::ElementWithFloatingChildren class.  This slot is triggered by both the
         * Ld::ElementWithFloatingChildren::removeChild(Ld::ElementPointer) method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] location          The location where the child used to exist.
         *
         * \param[in] childPresentation The child element being removed.
         */
        virtual void processRemovingChildPresentation(const QPointF& location, Presentation* childPresentation);

        /**
         * Slot that is triggered when a child is inserted at a specified location by the
         * Ld::ElementWithFloatingChildren class.  This slot is triggered by the
         * Ld::ElementWithFloatingChildren::insertChild method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childPresentation The child that was just added.
         *
         * \param[in] location          The desired location for the child element.
         */
        virtual void processChildPresentationInserted(Presentation* childPresentation, const QPointF& location);

        /**
         * Slot that is triggered when a child is moved by the Ld::ElementWithFloatingChildren class.  This slot is
         * triggered by the Ld::ElementWithFloatingChildren::relocateChild method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childPresentation The child to be relocated.
         *
         * \param[in] oldLocation       The old location for the child.
         *
         * \param[in] newLocation       The new location for the child.
         */
        virtual void processChildPresentationRelocated(
            Presentation*  childPresentation,
            const QPointF& oldLocation,
            const QPointF& newLocation
        );

    private:
        /**
         * Pure virtual method that is called just before a child is removed by the associated
         * Ld::ElementWithFloatingChildren class.  This method is triggered by both the
         * Ld::ElementWithFloatingChildren::removeChild(Ld::ElementPointer) method.
         *
         * \param[in] location     The current location for the child that is being removed.
         *
         * \param[in] childElement The child element being removed.
         */
        void aboutToRemoveChild(const Ld::Location& location, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when a child is inserted before a specified location by the
         * Ld::ElementWithFloatingChildren class.  This method is triggered by the
         * Ld::ElementWithFloatingChildren::insertChild method.
         *
         * \param[in] childElement The child that was just added.
         *
         * \param[in] location     The desired location for the child element.
         */
        void childInserted(Ld::ElementPointer childElement, const Ld::Location& location) final;

        /**
         * Method that is called when a child is moved by the Ld::ElementWithFloatingChildren class.  This method is
         * triggered by the Ld::ElementWithFloatingChildren::relocateChild method.
         *
         * \param[in] childElement The child to be relocated.
         *
         * \param[in] oldLocation  The old location for the child.
         *
         * \param[in] newLocation  The new location for the child.
         */
        void childRelocated(
            Ld::ElementPointer  childElement,
            const Ld::Location& oldLocation,
            const Ld::Location& newLocation
        ) final;
};

#endif
