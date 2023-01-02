/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationWithFixedChildren class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_WITH_FIXED_CHILDREN_H
#define PRESENTATION_WITH_FIXED_CHILDREN_H

#include <QObject>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_fixed_children.h>

#include "app_common.h"
#include "presentation.h"

/**
 * This is a pure virtual base class used to present language elements.  The class extends the
 * Ld::VisualWithFixedChildren class to bring visuals into the Qt QObject and signal/slot framework and to
 * include additional methods used to support positioning and rendering of components.
 */
class APP_PUBLIC_API PresentationWithFixedChildren:public Presentation, public virtual Ld::VisualWithFixedChildren {
    Q_OBJECT

    public:
        PresentationWithFixedChildren();

        ~PresentationWithFixedChildren() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::FIXED.
         */
        Ld::Element::ChildPlacement childPlacement() const override;

    signals:
        /**
         * Signal that is emitted when a child element is removed.  You can use this signal to process notification
         * that a presentation tied to this presentation should be destroyed.
         *
         * This signal is emitted by the methods:
         *
         *     * \ref ElementWithFixedChildren::removeChild(Element*)
         *     * \ref ElementWithFixedChildren::removeChild(unsigned long)
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child presentation that is being removed.  Note that elements with no
         *                              associated visual will trigger this signal with this value set to null.
         */
        void childPresentationRemoved(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Signal that is emitted when a child element is added.  You can use this signal to receive notification when
         * the presentation needs to be updated to add a new child presentation.
         *
         * This signal is emitted by the Ld::ElementWithFixedChildren::setChild method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child element being placed.  This value may be null if a child is
         *                              added with no presentation set.
         */
        void childPresentationSet(unsigned long childIndex, Presentation* childPresentation);

    protected slots:
        /**
         * Slot that is triggered when a child element is removed.  You can overload this slot if you wish to receive
         * notification that a child presentation should be removed.  This slot is ultimately triggered by the methods:
         *
         *     * ElementWithFixedChildren::removeChild(Element*)
         *     * ElementWithFixedChildren::removeChild(unsigned long)
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread
         * boundaries.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child element being removed.
         */
        virtual void processPresentationRemoved(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Slot that is triggered when a child element is added.  You can overload this slot if you wish to receive
         * notification that a child presentation has been added.  This slot is ultimately triggered by the
         * Ld::ElementWithFixedChildren::setChild method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child element being placed.
         */
        virtual void processChildPresentationSet(unsigned long childIndex, Presentation* childPresentation);

    private:
        /**
         * Method that is called just before a child is removed by the associated.  This method is triggered by the
         * methods:
         *
         *     * \ref ElementWithFixedChildren::removeChild(Element*)
         *     * \ref ElementWithFixedChildren::removeChild(unsigned long)
         *
         * The default implementation will disconnect the presentation from the child and then emit the
         * \ref PresentationWithFixedChildren::childPresentationRemoved method.
         *
         * \param[in] childIndex   The zero based index of the removed child.
         *
         * \param[in] childElement The child element being removed.
         */
        void aboutToRemoveChild(unsigned long childIndex, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when a child is added.
         * Ld::ElementWithFixedChildren class.  This method is triggered by the
         * Ld::ElementWithFixedChildren::setChild method.
         *
         * \param[in] childIndex   The zero based index where the desired child should be placed.
         *
         * \param[in] childElement The new child element being placed.
         */
        void childSet(unsigned long childIndex, Ld::ElementPointer childElement) final;
};

#endif
