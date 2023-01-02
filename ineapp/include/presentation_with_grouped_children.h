/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationWithGroupedChildren class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_WITH_GROUPED_CHILDREN_H
#define PRESENTATION_WITH_GROUPED_CHILDREN_H

#include <QObject>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_grouped_children.h>

#include "app_common.h"
#include "presentation_with_positional_children.h"

/**
 * This is a pure virtual base class used to present language elements.  The class extends the
 * Ld::VisualWithGroupedChildren class to bring visuals into the Qt QObject and signal/slot framework and
 * to include additional methods used to support positioning and rendering of components.
 */
class APP_PUBLIC_API PresentationWithGroupedChildren:public         PresentationWithPositionalChildren,
                                                     public virtual Ld::VisualWithGroupedChildren {
    Q_OBJECT

    public:
        PresentationWithGroupedChildren();

        ~PresentationWithGroupedChildren() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::GROUPED.
         */
        Ld::Element::ChildPlacement childPlacement() const override;

    signals:
        /**
         * Slot that is triggered just before a child is removed by the associated Ld::ElementWithGroupedChildren
         * class.  This slot is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::removeChild(ElementPointer, CursorStateCollection*)
         *     * Ld::ElementWithPositionalChildren::removeChild(unsigned long, CursorStateCollection*)
         *     * Ld::ElementWithGroupedChildren:removeChildFromGroup
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.
         *
         * \param[in] childPresentation The child presentation being removed.
         */
        void removingChildFromGroup(
            unsigned long childIndex,
            unsigned      groupIndex,
            unsigned long childIndexInGroup,
            Presentation* childPresentation
        );

        /**
         * Slot that is triggered just before all the children are removed from a group by the associated
         * Ld::ElementWithGroupedChildren class.  This slot is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::removeChild(ElementPointer, CursorStateCollection*)
         *     * Ld::ElementWithPositionalChildren::removeChild(unsigned long, CursorStateCollection*)
         *     * Ld::ElementWithGroupedChildren:removeChildFromGroup
         *
         * \param[in] startingChildIndex The zero based index of the first child to remove.
         *
         * \param[in] numberChildren     The number of children in the group.
         *
         * \param[in] groupIndex         The zero based group index of the group containing the child.
         */
        void removingChildrenFromGroup(
            unsigned long startingChildIndex,
            unsigned long numberChildren,
            unsigned      groupIndex
        );

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::insertBefore
         *     * Ld::ElementWithGroupedChildren::insertIntoGroupBefore
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.
         *
         * \param[in] childPresentation The child presentation being placed.
         */
        void childWasInsertedIntoGroupBefore(
            unsigned long childIndex,
            unsigned      groupIndex,
            unsigned long childIndexInGroup,
            Presentation* childPresentation
        );

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::insertAfter
         *     * Ld::ElementWithGroupedChildren::insertIntoGroupAfter
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.  An invalid
         *                              index indicates that the child was inserted as the first element.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.  An invalid
         *                              index indicates that the child was inserted into an empty group.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void childWasInsertedIntoGroupAfter(
            unsigned long childIndex,
            unsigned      groupIndex,
            unsigned long childIndexInGroup,
            Presentation* childPresentation
        );

        /**
         * Slot that is triggered when one or more groups are removed.  This method is triggered by the
         * Ld::ElementWithGroupedChildren::removeGroups.
         *
         * \param[in] startingGroupIndex The starting group index of the range being removed.
         *
         * \param[in] numberGroups       The number of groups being removed.
         */
        void groupsWereRemoved(unsigned startingGroupIndex, unsigned numberGroups);

        /**
         * Slot that is triggered when a new group is inserted.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithGroupedChildren::insertGroupBefore
         *     * Ld::ElementWithGroupedChildren::insertGroupAfter
         *
         * \param[in] groupIndex   The group index of the first newly inserted group.
         *
         * \param[in] numberGroups The number of newly added groups.
         */
        void groupsWereInserted(unsigned groupIndex, unsigned numberGroups);

    protected slots:
        /**
         * Slot that is triggered just before a child is removed by the associated Ld::ElementWithGroupedChildren
         * class.  This slot is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::removeChild(ElementPointer, CursorStateCollection*)
         *     * Ld::ElementWithPositionalChildren::removeChild(unsigned long, CursorStateCollection*)
         *     * Ld::ElementWithGroupedChildren:removeChildFromGroup
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.
         *
         * \param[in] childPresentation The child presentation being removed.
         */
        virtual void processAboutToRemoveChildFromGroup(
            unsigned long childIndex,
            unsigned      groupIndex,
            unsigned long childIndexInGroup,
            Presentation* childPresentation
        );

        /**
         * Slot that is triggered just before all the children are removed from a group by the associated
         * Ld::ElementWithGroupedChildren class.  This slot is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::removeChild(ElementPointer, CursorStateCollection*)
         *     * Ld::ElementWithPositionalChildren::removeChild(unsigned long, CursorStateCollection*)
         *     * Ld::ElementWithGroupedChildren:removeChildFromGroup
         *
         * \param[in] startingChildIndex The zero based index of the first child to remove.
         *
         * \param[in] numberChildren     The number of children in the group.
         *
         * \param[in] groupIndex         The zero based group index of the group containing the child.
         */
        virtual void processAboutToRemoveChildrenFromGroup(
            unsigned long startingChildIndex,
            unsigned long numberChildren,
            unsigned      groupIndex
        );

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::insertBefore
         *     * Ld::ElementWithGroupedChildren::insertIntoGroupBefore
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildInsertedIntoGroupBefore(
            unsigned long childIndex,
            unsigned      groupIndex,
            unsigned long childIndexInGroup,
            Presentation* childPresentation
        );

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::insertAfter
         *     * Ld::ElementWithGroupedChildren::insertIntoGroupAfter
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.  An invalid
         *                              index indicates that the child was inserted as the first element.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.  An invalid
         *                              index indicates that the child was inserted into an empty group.
         *
         * \param[in] childElement      The new child element being placed.
         */
        virtual void processChildInsertedIntoGroupAfter(
            unsigned long childIndex,
            unsigned      groupIndex,
            unsigned long childIndexInGroup,
            Presentation* childElement
        );

        /**
         * Slot that is triggered when one or more groups are removed.  This method is triggered by the
         * Ld::ElementWithGroupedChildren::removeGroups.
         *
         * \param[in] startingGroupIndex The starting group index of the range being removed.
         *
         * \param[in] numberGroups       The number of groups being removed.
         */
        virtual void processGroupsRemoved(unsigned startingGroupIndex, unsigned numberGroups);

        /**
         * Slot that is triggered when a new group is inserted.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithGroupedChildren::insertGroupBefore
         *     * Ld::ElementWithGroupedChildren::insertGroupAfter
         *
         * \param[in] groupIndex   The group index of the first newly inserted group.
         *
         * \param[in] numberGroups The number of newly added groups.
         */
        virtual void processGroupsInserted(unsigned groupIndex, unsigned numberGroups);

    private:
        /**
         * Overwritten method that is called just before a child is removed by the associated
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::removeChild(ElementPointer, CursorStateCollection*)
         *     * Ld::ElementWithPositionalChildren::removeChild(unsigned long, CursorStateCollection*)
         *     * Ld::ElementWithGroupedChildren:removeChildFromGroup
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.
         *
         * \param[in] childElement      The child element being removed.
         */
        void aboutToRemoveChildFromGroup(
            unsigned long      childIndex,
            unsigned           groupIndex,
            unsigned long      childIndexInGroup,
            Ld::ElementPointer childElement
        ) override;

        /**
         * Overwritten method that is called just before all the children are removed from a group by the associated
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::removeChild(ElementPointer, CursorStateCollection*)
         *     * Ld::ElementWithPositionalChildren::removeChild(unsigned long, CursorStateCollection*)
         *     * Ld::ElementWithGroupedChildren:removeChildFromGroup
         *
         * \param[in] startingChildIndex The zero based index of the first child to remove.
         *
         * \param[in] numberChildren     The number of children in the group.
         *
         * \param[in] groupIndex         The zero based group index of the group containing the child.
         */
        void aboutToRemoveChildrenFromGroup(
            unsigned long startingChildIndex,
            unsigned long numberChildren,
            unsigned      groupIndex
        ) override;

        /**
         * Overwritten method that is called when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::insertBefore
         *     * Ld::ElementWithGroupedChildren::insertIntoGroupBefore
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.
         *
         * \param[in] childElement      The new child element being placed.
         */
        void childInsertedIntoGroupBefore(
            unsigned long      childIndex,
            unsigned           groupIndex,
            unsigned long      childIndexInGroup,
            Ld::ElementPointer childElement
        ) override;

        /**
         * Overwritten method that is called when a child is inserted after a specified location by the
         * Ld::ElementWithGroupedChildren class.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithPositionalChildren::insertAfter
         *     * Ld::ElementWithGroupedChildren::insertIntoGroupAfter
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.  An invalid
         *                              index indicates that the child was inserted as the first element.
         *
         * \param[in] groupIndex        The zero based group index of the group containing the child.
         *
         * \param[in] childIndexInGroup The index of the child relative to the start of the group.  An invalid
         *                              index indicates that the child was inserted into an empty group.
         *
         * \param[in] childElement      The new child element being placed.
         */
        void childInsertedIntoGroupAfter(
            unsigned long      childIndex,
            unsigned           groupIndex,
            unsigned long      childIndexInGroup,
            Ld::ElementPointer childElement
        ) override;

        /**
         * Overwritten method that is called when one or more groups are removed.  This method is triggered by the
         * Ld::ElementWithGroupedChildren::removeGroups.
         *
         * \param[in] startingGroupIndex The starting group index of the range being removed.
         *
         * \param[in] numberGroups       The number of groups being removed.
         */
        void groupsRemoved(unsigned startingGroupIndex, unsigned numberGroups) override;

        /**
         * Overwritten method that is called when a new group is inserted.  This method is triggered by the methods:
         *
         *     * Ld::ElementWithGroupedChildren::insertGroupBefore
         *     * Ld::ElementWithGroupedChildren::insertGroupAfter
         *
         * \param[in] groupIndex   The group index of the first newly inserted group.
         *
         * \param[in] numberGroups The number of newly added groups.
         */
        void groupsInserted(unsigned groupIndex, unsigned numberGroups) override;
};

#endif
