/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RemoveListFormatUpdater class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef REMOVE_LIST_FORMAT_UPDATER_H
#define REMOVE_LIST_FORMAT_UPDATER_H

#include <QStringList>

#include <ld_element_structures.h>
#include <ld_aggregations_by_capability.h>

#include "update_format_command.h"
#include "list_format_updater.h"

namespace Ld {
    class Element;
    class ParagraphFormat;
    class ListParagraphFormatBase;
}

/**
 * List format updater used to remove list formatting over a range of elements.
 */
class RemoveListFormatUpdater:public ListFormatUpdater {
    public:
        RemoveListFormatUpdater();

        ~RemoveListFormatUpdater() override;

    protected:
        /**
         * Method that is called to reset this class instance prior to processing each group.
         */
        void reset() override;

        /**
         * Method that returns a list of capabilities to use to identify elements to be updated.  This implementation
         * returns the list:
         *
         *     * Ld::ListAdditionalParagraphFormat::formatName
         *     * Ld::ListParagraphFormatBase::formatName
         *     * Ld::OrderedListParagraphFormat::formatName
         *     * Ld::UnorderedListParagraphFormat::formatName
         *
         * \return Returns a string list holding the capabilities to use to identify element groups.
         */
        QStringList capabilities() const override;

        /**
         * Method that is called to scan backwards to locate a set of default formats.
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
         * The last paragraph format used before each range.
         */
        QSharedPointer<Ld::ParagraphFormat> lastParagraphFormat;

        /**
         * The last list format used over a given range.  The format will be applied to the first paragraph after the
         * end of the range if it is currently a continuation of the list.
         */
        QSharedPointer<Ld::ListParagraphFormatBase> lastListFormat;
};

#endif
