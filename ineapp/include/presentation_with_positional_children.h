/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationWithPositionalChildren class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_WITH_POSITIONAL_CHILDREN_H
#define PRESENTATION_WITH_POSITIONAL_CHILDREN_H

#include <QObject>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_positional_children.h>

#include "app_common.h"
#include "presentation.h"

/**
 * This is a pure virtual base class used to present language elements.  The class extends the
 * Ld::VisualWithPositionalChildren class to bring visuals into the Qt QObject and signal/slot framework and
 * to include additional methods used to support positioning and rendering of components.
 */
class APP_PUBLIC_API PresentationWithPositionalChildren:public Presentation,
                                                        public virtual Ld::VisualWithPositionalChildren {
    Q_OBJECT

    public:
        PresentationWithPositionalChildren();

        ~PresentationWithPositionalChildren() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::POSITIONAL.
         */
        Ld::Element::ChildPlacement childPlacement() const override;

    signals:
        /**
         * Signal that is emitted when a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This signal is emitted by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child presentation being removed.
         */
        void removingChildPresentation(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Signal that is emitted when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This signal is emitted by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void childPresentationNowInsertedBefore(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Signal that is emitted when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This signal is emitted by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void childPresentationNowInsertedAfter(unsigned long childIndex, Presentation* childPresentation);

    protected slots:
        /**
         * Slot that is triggered when a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread
         * boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child element being removed.
         */
        virtual void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread
         * boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread
         * boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation);

    private:
        /**
         * Method that is called just before a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This method is triggered by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * \param[in] childIndex   The zero based index of the removed child.
         *
         * \param[in] childElement The child element being removed.
         */
        void aboutToRemoveChild(unsigned long childIndex, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This method is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * \param[in] childIndex   The zero based index where the desired child should be placed.
         *
         * \param[in] childElement The new child element being placed.
         */
        void childInsertedBefore(unsigned long childIndex, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This method is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * \param[in] childIndex   The zero based index where the desired child should be placed.
         *
         * \param[in] childElement The new child element being placed.
         */
        void childInsertedAfter(unsigned long childIndex, Ld::ElementPointer childElement) final;
};

#endif
