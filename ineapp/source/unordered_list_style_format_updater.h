/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UnorderedListStyleFormatUpdater class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UNORDERED_LIST_STYLE_FORMAT_UPDATER_H
#define UNORDERED_LIST_STYLE_FORMAT_UPDATER_H

#include <QString>
#include <QStringList>

#include <ld_element_structures.h>
#include <ld_aggregations_by_capability.h>
#include <ld_unordered_list_paragraph_format.h>

#include "update_format_command.h"
#include "list_format_updater.h"

namespace Ld {
    class Element;
    class ParagraphFormat;
    class ListParagraphFormatBase;
}

/**
 * List format updater used either convert paragraphs to unordered lists or update the bullet style used by an
 * unordered list.
 */
class UnorderedListStyleFormatUpdater:public ListFormatUpdater {
    public:
        /**
         * Constructor
         *
         * \param[in] newBulletStyle A string holding the new bullet to be used.
         */
        UnorderedListStyleFormatUpdater(const QString& newBulletStyle = QString());

        ~UnorderedListStyleFormatUpdater() override;

        /**
         * Method you can use to set the desired bullet style.
         *
         * \param[in] newBulletStyle the new bullet style to be applied.
         */
        void setBulletStyle(const QString& newBulletStyle);

        /**
         * Method you can use to obtain the currently selected bullet style.
         *
         * \return Returns the currently selected bullet style.
         */
        QString bulletStyle() const;

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
         * The current bullet style.
         */
        QString currentBulletStyle;
};

#endif
