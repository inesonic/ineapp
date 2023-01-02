/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandBaseWithUndo class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_BASE_WITH_UNDO_H
#define COMMAND_BASE_WITH_UNDO_H

#include <QString>
#include <QSharedPointer>

#include <ld_element_structures.h>
#include <ld_element_position.h>

#include "app_common.h"
#include "cursor.h"
#include "command_base.h"

namespace Ld {
    class ElementCursor;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Class derived from \ref Command that provides a collection of useful methods, including support for simplified undo
 * and redo.
 *
 * The supplied undo/redo functions capture the state of entire paragraphs and allow those paragraphs to be restored.
 */
class APP_PUBLIC_API CommandBaseWithUndo:public CommandBase {
    public:
        CommandBaseWithUndo();

        /**
         * Constructor
         *
         * \param[in] newCursor The cursor who's position that should be updated in response to this command.
         */
        CommandBaseWithUndo(CursorPointer newCursor);

        ~CommandBaseWithUndo() override;

    protected:
        /**
         * Method you can use to capture and store one or more paragraphs worth of information.
         *
         * \param[in] element                 An element used to locate the paragraph of interest.  This method will
         *                                    walk upwards from this element until a paragraph is located.  The
         *                                    reference paragraph index is based on that paragraph's parent.
         *
         * \param[in] startingParagraphOffset A signed offset relative to the provided element.  A positive offset will
         *                                    start at a later paragraph.  A negative offset will start at an earlier
         *                                    paragraph.   Offsets that exceed the available number of paragraphs will
         *                                    be adjusted.
         *
         * \param[in] endingParagraphOffset   A signed offset relative to the provided element.  A positive offset will
         *                                    end at a later paragraph.  A negative offset will end at an earlier
         *                                    paragraph.   Offsets that exceed the available number of paragraphs will
         *                                    be adjusted.  This method will assert if the ending paragraph lands
         *                                    before the starting paragraph.
         */
        void saveParagraphs(
            Ld::ElementPointer element,
            long               startingParagraphOffset = 0,
            long               endingParagraphOffset = 0
        );

        /**
         * Method you can use to capture and store one or more paragraphs worth of information.
         *
         * \param[in] cursor The cursor used to identify the saved range.
         *
         * \return Returns true on success, returns false if the cursor's selection does not allow us to save the
         *         state.
         */
        bool saveParagraphs(const Cursor& cursor);

        /**
         * Method you can use to get the location of the first saved paragraph.
         *
         * \return Returns the location of the first saved paragraph.
         */
        const Ld::ElementPosition& firstSavedParagraph() const;

        /**
         * Method you can use to get the number of saved paragraphs.
         *
         *
         * \return Returns the number of saved paragraphs.
         */
        unsigned long numberSavedParagraphs() const;

        /**
         * Method you can use to mark the paragraph(s) that have been changed by this method.
         *
         * \param[in] element                 An element used to locate the paragraph of interest.  This method will
         *                                    walk upwards from this element until a paragraph is located.  The
         *                                    reference paragraph index is based on that paragraph's parent.
         *
         * \param[in] startingParagraphOffset A signed offset relative to the provided element.  A positive offset will
         *                                    start at a later paragraph.  A negative offset will start at an earlier
         *                                    paragraph.   Offsets that exceed the available number of paragraphs will
         *                                    be adjusted.
         *
         * \param[in] endingParagraphOffset   A signed offset relative to the provided element.  A positive offset will
         *                                    end at a later paragraph.  A negative offset will end at an earlier
         *                                    paragraph.   Offsets that exceed the available number of paragraphs will
         *                                    be adjusted.  This method will assert if the ending paragraph lands
         *                                    before the starting paragraph.
         */
        void markChangedParagraphs(
            Ld::ElementPointer element,
            long               startingParagraphOffset = 0,
            long               endingParagraphOffset = 0
        );

        /**
         * Method you can use to mark the paragraph(s) that have been changed by this method.
         *
         * \param[in] cursor1 The cursor representing one end of the changed range.
         *
         * \param[in] cursor2 The cursor representing the other end of the changed range.
         */
        void markChangedParagraphs(const Ld::ElementCursor& cursor1, const Ld::ElementCursor& cursor2);

        /**
         * Method you can use to explicitly indicate that no paragraphs were changed.  Note that this is different from
         * the CommandBaseWithUndo::clearModifiedParagraphInformation in that calling removeChangedParagraphs will
         * still return true if this method is called.
         *
         * \param[in] element An element indicating a valid cursor position.
         */
        void markNoChangedParagraphs(Ld::ElementPointer element);

        /**
         * Method you can use to get the location of the first changed paragraph.
         *
         * \return Returns the location of the first changed paragraph.
         */
        const Ld::ElementPosition& firstChangedParagraph() const;

        /**
         * Method you can use to get the number of paragraphs marked as changed.
         *
         * \return Returns the number of paragraphs marked as changed.
         */
        unsigned long numberChangedParagraphs() const;

        /**
         * Method you can use to remove all the paragraphs that were marked as changed.  After calling this method, the
         * changed paragraph information will be cleared.
         *
         * \param[in] cursorStateCollection The cursor state collection to be updated.
         *
         * \return This method will return true on success, false if there are no saved paragraphs or the saved
         *         information is invalid.
         */
        bool removeChangedParagraphs(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method you can use to restore the removed paragraphs.  Paragraphs will be inserted where they were saved and
         * newly added paragraphs will be pushed down.
         *
         * Note that this method will also clear the saved paragraph information.
         *
         * \param[in] cursorStateCollection The cursor state collection to be updated.
         *
         * \return This method will return true on success, false if there are no saved paragraphs or the saved
         *         information is invalid.
         */
        bool restoreSavedParagraphs(Ld::CursorStateCollection* cursorStateCollection = nullptr);

        /**
         * Method you can use to clear the saved paragraph content.  Note that this method does not modify the document
         * view, only the data saved in this class instance.
         */
        void clearSavedParagraphsInformation();

        /**
         * Method you can use to clear the changed paragraph information.  Note that this method does not modify the
         * document view, only the data saved in this class instance.
         */
        void clearModifiedParagraphsInformation();

    private:
        /**
         * Helper method used to capture and store one or more paragraphs worth of information.
         *
         * \param[in] parentElement The paragraph's parent element.
         *
         * \param[in] startingIndex The starting index under the parent.
         *
         *
         * \param[in] endingIndex   The ending index under the parent.
         */
        void saveParagraphsHelper(
            Ld::ElementPointer parentElement,
            unsigned long      startingIndex,
            unsigned long      endingIndex
        );

        /**
         * Method that locates a reference paragraph.
         *
         * \param[in] element The element to use as a basis for the search.
         */
        static Ld::ElementCursor findReferenceParagraph(Ld::ElementPointer element);

        /**
         * Cursor that points to the original location of the starting saved paragraph.
         */
        Ld::ElementPosition startingParagraphCursor;

        /**
         * A list of previously saved paragraph elements.
         */
        Ld::ElementPointerList savedParagraphs;

        /**
         * Cursor to the first changed paragraph.
         */
        Ld::ElementPosition startingChangedParagraph;

        /**
         * The count of the number of changed paragraphs.
         */
        unsigned long countChangedParagraphs;
};

#endif
