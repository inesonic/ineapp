/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UpdateFormatCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UPDATE_FORMAT_COMMAND_H
#define UPDATE_FORMAT_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QMap>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element_cursor.h>
#include <ld_element_position.h>

#include "app_common.h"
#include "command_base.h"

namespace Ld {
    class CursorStateCollection;
    class Format;
}

class Cursor;
class CommandContainer;

/**
 * Command that can be issued to update the page format for one or more pages.  Note that we have a distinct command
 * for page formats because of how pages are managed within the root element.
 */
class APP_PUBLIC_API UpdateFormatCommand:public CommandBase {
    Q_DECLARE_TR_FUNCTIONS(UpdateFormatCommand)

    public:
        /**
         * Class that provides a mapping of formats by element.  The class is a thin wrapper on QMap that provides a
         * few additional functions.
         */
        class APP_PUBLIC_API FormatsByElement:public QMap<Ld::ElementPointer, Ld::FormatPointer> {
            public:
                FormatsByElement();

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied to this instance.
                 */
                FormatsByElement(const FormatsByElement& other);

                /**
                 * Move constructor
                 *
                 * \param[in] other The instance to be copied to this instance.
                 */
                FormatsByElement(FormatsByElement&& other);

                ~FormatsByElement();

                /**
                 * Function that merges entries from another map into this map, replacing any existing entries.
                 *
                 * \param[in] other The formats by element to merge into this instance.
                 */
                void add(const FormatsByElement& other);

                /**
                 * Assignment operator.
                 *
                 * \param[in] other THe instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                FormatsByElement& operator=(const FormatsByElement& other);

                /**
                 * Assignment operator (move semantics).
                 *
                 * \param[in] other THe instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                FormatsByElement& operator=(FormatsByElement&& other);
        };

        UpdateFormatCommand();

        /**
         * Constructor
         *
         * \param[in] formatsByElement A map you can use to represent the formats that should be assigned to specific
         *                             elements.
         *
         * \param[in] allowMerge       If true, elements with identical formats will be merged.   If false, elements
         *                             will not be merged even if the formats are identical.
         */
        UpdateFormatCommand(const FormatsByElement& formatsByElement, bool allowMerge = true);

        ~UpdateFormatCommand() override;

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        CommandType commandType() const final;

        /**
         * Method you can use to set the map of elements whose formats should be updated.
         *
         * \param[in] newFormatsByElement A map you can use to represent the formats that should be assigned to
         *                                specific elements.
         */
        void setFormatsByElement(const FormatsByElement& newFormatsByElement);

        /**
         * Method you can use to obtain a map of formats keyed by element.
         *
         * \return Returns a map of formats that should be assigned to specific elements.
         */
        FormatsByElement formatsByElement() const;

        /**
         * Method you can use to indicate that element merges should be performed, if possible.
         *
         * \param[in] nowMergeAllowed If true, merges will be allowed.  If false, merges will not be allowed.
         */
        void setMergeAllowed(bool nowMergeAllowed = true);

        /**
         * Method you can use to indicate that element merges should not be performed even if the formats would
         * otherwise allow the elements to merge.
         *
         * \param[in] nowMergeNotAllowed If true, merges will be blocked.  If false, merges will be allowed.
         */
        void setMergeNotAllowed(bool nowMergeNotAllowed = true);

        /**
         * Method you can use to determine if merges are allowed.
         *
         * \return Returns true if merges are allowed.  Returns false if merges are not allowed.
         */
        bool mergesAllowed() const;

        /**
         * Method you can use to determine if merges are not allowed.
         *
         * \return Returns true if merges are not allowed.  Returns false if merges are allowed.
         */
        bool mergesNotAllowed() const;

        /**
         * Method that executes the command.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool execute(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that performs an undo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undo(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        QString description() const final;

    private:
        /**
         * Map used internally to track formats by their position.
         */
        typedef QMap<Ld::ElementPosition, Ld::FormatPointer> FormatsByPosition;

        /**
         * Method that converts formats by element to formats by position.
         *
         * \param[in] formatsByElement A map of formats, by element, to be modified.
         *
         * \return Returns a list of formats based on the position of the element.
         */
        static FormatsByPosition toFormatsByPosition(const FormatsByElement& formatsByElement);

        /**
         * Method that converts formats by position to formats by element.
         *
         * \param[in] formatsByPosition A map of formats, by position, to be modified.
         *
         * \return Returns a list of formats based on the position of the element.
         */
        static FormatsByElement toFormatsByElement(const FormatsByPosition& formatsByPosition);

        /**
         * Method that is called to update a collection of formats based on a map of formats by position
         * information.
         *
         * \param[in]     formatsByPosition     A map of formats to be updated based on element positions.
         *
         * \param[in]     mergesAllowed         If true, merges will be performed, if possible.  If false, merges will
         *                                      never be performed.
         *
         * \param[in]     cursor                A cursor that can be used to tell this method how to handle edge cases.
         *                                      The method will use the supplied cursor to split/merge elements.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns an updated list of formats by position that can be used later to undo this operation.
         */
        static FormatsByPosition updateFormats(
            const FormatsByPosition&   formatsByPosition,
            bool                       mergesAllowed,
            const Cursor&              cursor,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * The current map of page formats by page index.
         */
        FormatsByPosition currentFormats;

        /**
         * Flag that indicates if merges are allowed.
         */
        bool currentMergesAllowed;

        /**
         * A map of the page formats that existed before we replaced them.  The map only contains pages that were
         * actually modified.
         */
        FormatsByPosition deletedFormats;
};

#endif
