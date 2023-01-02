/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref OrderedListStyleFormatUpdater class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ORDERED_LIST_STYLE_FORMAT_UPDATER_H
#define ORDERED_LIST_STYLE_FORMAT_UPDATER_H

#include <QStringList>

#include <ld_element_structures.h>
#include <ld_aggregations_by_capability.h>
#include <ld_ordered_list_paragraph_format.h>

#include "update_format_command.h"
#include "list_format_updater.h"

namespace Ld {
    class Element;
    class ParagraphFormat;
    class ListParagraphFormatBase;
}

/**
 * List format updater used either convert paragraphs to numbered lists or update the number style used by numbered
 * lists.
 */
class OrderedListStyleFormatUpdater:public ListFormatUpdater {
    public:
        /**
         * Type used to specify the number style to be applied.
         */
        typedef Ld::OrderedListParagraphFormat::Style NumberStyle;

        /**
         * Constructor
         *
         * \param[in] newNumberStyle The new number style to be applied.
         */
        OrderedListStyleFormatUpdater(NumberStyle newNumberStyle = NumberStyle::INVALID);

        ~OrderedListStyleFormatUpdater() override;

        /**
         * Method you can use to set the desired number style.
         *
         * \param[in] newNumberStyle the new number style to be applied.
         */
        void setNumberStyle(NumberStyle newNumberStyle);

        /**
         * Method you can use to obtain the currently selected number style.
         *
         * \return Returns the currently selected number style.
         */
        NumberStyle numberStyle() const;

    protected:
        /**
         * Method that is called to reset this class instance prior to processing each group.
         */
        void reset() override;

        /**
         * Method that is called to scan backwards to locate a set of default formats.  This version performs no
         * action.
         *
         * \param[in] precedingElement A shared pointer to the element that immediately precedes the group.  A null
         *                             pointer is issued if there is no preceding element.
         */
        void locateDefaultFormats(Ld::ElementPointer precedingElement) override;

        /**
         * Method that is called to update the first element in each group.
         *
         * \param[in] element A shared pointer to the first element in the group.
         *
         * \return Returns updates to be applied.
         */
        FormatsByElement updateFirstElementInGroup(Ld::ElementPointer element) override;

        /**
         * Method that is called to update an element other than the first element in each group.
         *
         * \param[in] element A shared pointer to the first element in the group.
         *
         * \return Returns updates to be applied.
         */
        FormatsByElement updateSubsequentElementInGroup(Ld::ElementPointer element) override;

        /**
         * Method that is called to update the first element following the group.
         *
         * \param[in] element A shared pointer to the element that follows the first element in the group.  A null
         *                    pointer will be issued if there is no element after the end of the group.
         *
         * \return Returns updates to be applied.  This version will update the element following the list by applying
         *         the last list formatting removed in each group.
         */
        FormatsByElement updateElementFollowingGroup(Ld::ElementPointer element) override;

    private:
        /**
         * The last list format used over a given range.  The format will be applied to the first paragraph after the
         * end of the range if it is currently a continuation of the list.
         */
        QSharedPointer<Ld::ListParagraphFormatBase> lastListFormat;

        /**
         * The current number style.
         */
        NumberStyle currentNumberStyle;
};

#endif
