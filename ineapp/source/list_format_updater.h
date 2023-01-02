/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ListFormatUpdater class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LIST_FORMAT_UPDATER_H
#define LIST_FORMAT_UPDATER_H

#include <QStringList>

#include <ld_element_structures.h>
#include <ld_aggregations_by_capability.h>

#include "update_format_command.h"

namespace Ld {
    class Element;
    class ListParagraphFormatBase;
    class FontFormat;
}

/**
 * Pure virtual base class for engines used to perform updates to lists.
 */
class ListFormatUpdater {
    public:
        /**
         * Type used to report formats to be updated.
         */
        typedef UpdateFormatCommand::FormatsByElement FormatsByElement;

        ListFormatUpdater();

        virtual ~ListFormatUpdater();

        /**
         * Method that performs the requested update.  The default implementation:
         *
         *     * Calls the reset method.
         *
         *     * First scans backwards from the top of each element group to identify a default paragraph and list
         *       format to be applied.  This backwards scan is performed for each group by the method
         *       \ref ListFormatUpdater::locateDefaultFormats
         *
         *     * Then updates the first element in each group by calling
         *       \ref ListFormatUpdater::updateFirstElementInGroup.
         *
         *     * The updates subsequent elements in each group by calling
         *       \ref ListFormatUpdater::updateSubsequentElementInGroup.
         *
         *     * Finally, the method applies formats updates to the first element following each group by calling
         *       \ref ListFormatUpdater::updateElementFollowingGroup.
         *
         * \param[in] aggregationsByCapability A structure holding aggregations with every format and element over the
         *                                     current user selection.
         *
         * \return Returns a map or hash or formats, keyed by element, to be updated.
         */
        virtual FormatsByElement update(const Ld::AggregationsByCapability& aggregationsByCapability);

    protected:
        /**
         * Method that is called to reset this class instance prior to processing each group.  The default
         * implementation does nothing.
         */
        virtual void reset();

        /**
         * Method that returns a list of capabilities to use to identify elements to be updated.  The default
         * implementation returns the list:
         *
         *     * Ld::ParagraphFormat::formatName
         *     * Ld::ListAdditionalParagraphFormat::formatName
         *     * Ld::ListParagraphFormatBase::formatName
         *     * Ld::OrderedListParagraphFormat::formatName
         *     * Ld::UnorderedListParagraphFormat::formatName
         *
         * \return Returns a string list holding the capabilities to use to identify element groups.
         */
        virtual QStringList capabilities() const;

        /**
         * Method that is called to scan backwards to locate a set of default formats.
         *
         * \param[in] precedingElement A shared pointer to the element that immediately precedes the group.  A null
         *                             pointer is issued if there is no preceding element.
         */
        virtual void locateDefaultFormats(Ld::ElementPointer precedingElement) = 0;

        /**
         * Method that is called to update the first element in each group.
         *
         * \param[in] element A shared pointer to the first element in the group.
         *
         * \return Returns updates to be applied.
         */
        virtual FormatsByElement updateFirstElementInGroup(Ld::ElementPointer element) = 0;

        /**
         * Method that is called to update an element other than the first element in each group.
         *
         * \param[in] element A shared pointer to the first element in the group.
         *
         * \return Returns updates to be applied.
         */
        virtual FormatsByElement updateSubsequentElementInGroup(Ld::ElementPointer element) = 0;

        /**
         * Method that is called to update the first element following the group.
         *
         * \param[in] element A shared pointer to the element that follows the first element in the group.  A null
         *                    pointer will be issued if there is no element after the end of the group.
         *
         * \return Returns updates to be applied.  The default implementation returns no updates.
         */
        virtual FormatsByElement updateElementFollowingGroup(Ld::ElementPointer element);

        /**
         * Method that copies formatting information from one format instance to another.
         *
         * \param[in] destination The destination format.
         *
         * \param[in] source      The source format.
         */
        void copyFormatData(
            QSharedPointer<Ld::ListParagraphFormatBase> destination,
            QSharedPointer<Ld::ListParagraphFormatBase> source
        );

        /**
         * Method that copies formatting information from one format instance to another.
         *
         * \param[in] destination The destination format.
         *
         * \param[in] source      The source format.
         */
        void copyFormatData(
            QSharedPointer<Ld::ListParagraphFormatBase> destination,
            QSharedPointer<Ld::FontFormat>              source
        );

        /**
         * Method that scans children of an element to identify the first child with a usable font format.
         *
         * \param[in] parent The parent to be scanned.
         *
         * \return Returns a shared pointer to the child's font format.
         */
        QSharedPointer<Ld::FontFormat> firstChildFontFormat(Ld::ElementPointer parent);
};

#endif
