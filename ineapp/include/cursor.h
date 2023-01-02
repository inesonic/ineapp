/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Cursor class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CURSOR_H
#define CURSOR_H

#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QRectF>
#include <QPointF>
#include <QList>
#include <QPen>
#include <QBrush>
#include <QPainter>

#include <ld_element_structures.h>
#include <ld_element_cursor.h>
#include <ld_location.h>
#include <ld_cursor.h>

#include "app_common.h"
#include "edit_helpers.h"
#include "selection_data.h"

class QGraphicsView;

class RootPresentation;

/**
 * This class provides support for the application cursor.  The class extends the Ld::Cursor class to provide support
 * for a visual representation of the cursor.
 *
 * This class provides the heuristics required to manage selections.  The heuristics, as coded, will impose a limited
 * number of reasonable scenarios for items included in a selection:
 *
 *     * Selections will start and end on text so we can cut/paste through text and paragraphs.
 *     * Or, a selection will point to text within a single operator such as a function, variable, or literal.
 *     * Or, a selection will point to a flat list of elements under a single parent.
 *
 * These constraints allow us to greatly simplify the logic around selections.
 */
class APP_PUBLIC_API Cursor:public QObject, public Ld::Cursor, private EditHelpers {
    Q_OBJECT

    public:
        /**
         * Enumeration indicating the type of cursor to be displayed.
         */
        enum class Type : std::uint8_t {
            /**
             * Invalid indicates that the cursor points to nothing.
             */
            INVALID = 0,

            /**
             * Indicates a traditional text cursor.
             */
            TEXT = 1,

            /**
             * Indicates that the cursor points to a single element.
             */
            ELEMENT = 2,

            /**
             * Indicates that the cursor points to an empty position that can take an element.
             */
            EMPTY_POSITION = 3,

            /**
             * Indicates that the cursor points to a span or range of elements.
             */
            SELECTION = 4
        };

        /**
         * Enumeration of selection types.
         */
        typedef SelectionData::SelectionType SelectionType;

        /**
         * Enumeration of results from a paste operation.
         */
        enum class PasteResult {
            /**
             * Indicates the paste operation was successful.
             */
            SUCCESS,

            /**
             * Indicates that the supplied paste cursor was invalid.
             */
            INVALID_PASTE_CURSOR,

            /**
             * Indicates that the parent does not allow children.
             */
            PARENT_DOES_NOT_ALLOW_CHILDREN,

            /**
             * Indicates the paste operation failed because of a capabilities mismatch.
             */
            CAPABILITY_MISMATCH,

            /**
             * Indicates that disallowed text is being pasted into an element.
             */
            TEXT_VIOLATES_RULES_FOR_ELEMENT,

            /**
             * Indicates that we're attempting to paste a top level element into a position that is not top level.
             */
            REQUIRES_LOCATION_UNDER_ROOT,

            /**
             * Indicates that an element could't be split.
             */
            SPLIT_NOT_ALLOWED,

            /**
             * Indicates there are too many elements to paste.
             */
            TOO_MANY_ELEMENTS,

            /**
             * Indicates that the paste locations are already occupied.
             */
            LOCATION_OCCUPIED
        };

        /**
         * The pen used to draw the cursor.
         */
        static const QPen cursorPen;

        /**
         * The brush used to draw selection and element cursors.
         */
        static const QBrush cursorBrush;

        /**
         * The composition mode used when drawing element and selection cursors.
         */
        static const QPainter::CompositionMode compositionMode;

        Cursor();

        /**
         * Constructor
         *
         * \param[in] newRootPresentation The root presentation that is intended traversed by this class.
         *
         * \param[in] parent              An optional pointer to the parent object.
         */
        Cursor(RootPresentation* newRootPresentation, QObject* parent = Q_NULLPTR);

        /**
         * Copy constructor
         *
         * \param[in] other The instance to assign to this instance.
         */
        Cursor(const Cursor& other);

        ~Cursor() override;

        /**
         * Method you can use to obtain an element cursor representing the other end of a selected area.
         *
         * \return Returns the calculated Ld::ElementCursor instance representing the other end of the selected area.
         *         If there is no selection, this method will return an invalid element cursor.
         */
        const Ld::ElementCursor& selectionCursor() const;

        /**
         * Method you can use to determine the cursor type.
         *
         * \return Returns the current cursor type.
         */
        Type cursorType() const;

        /**
         * Method you can use to determine the current selection type.
         *
         * \return Returns the current selection type.
         */
        SelectionType selectionType() const;

        /**
         * Method you can use to obtain the starting or left cursor over a selected range.  This cursor will always be
         * valid.
         *
         * \return Returns the top or left-most cursor over a selected range.
         */
        const Ld::ElementCursor& startCursor() const;

        /**
         * Method you can use to obtain the ending or right cursor over a selected range.  This cursor will always be
         * valid.
         *
         * \return Returns the bottom or right-most cursor over a selected range.
         */
        const Ld::ElementCursor& endCursor() const;

        /**
         * Method you can use to move the cursor forward by an element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true the cursor was moved.  Returns false if the cursor was already at the end of the
         *         document view.
         */
        bool moveForwardByElement(bool selection = false);

        /**
         * Method you can use to move the cursor backward by an element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the beginning
         *         of the document view.
         */
        bool moveBackwardByElement(bool selection = false);

        /**
         * Method you can use to move the cursor forward by a single region in an element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true the cursor was moved.  Returns false if the cursor was already at the end of the
         *         document view.
         */
        bool moveForwardByRegion(bool selection = false);

        /**
         * Method you can use to move the cursor backward by a single region in an element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the beginning
         *         of the document view.
         */
        bool moveBackwardByRegion(bool selection = false);

        /**
         * Method you can use to move the cursor forward by whitespace, then by regions, then by elements.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the end of the
         *         document view.
         */
        bool moveForwardByWhitespace(bool selection = false);

        /**
         * Method you can use to move backward by whitespace, then by region, then by element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the beginning
         *         of the document view.
         */
        bool moveBackwardByWhitespace(bool selection = false);

        /**
         * Method you can use to move forward by character, then by region, then by element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the end of the
         *         document view.
         */
        bool moveForwardByCharacter(bool selection = false);

        /**
         * Method you can use to move backward by character, then by region, then by element.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the beginning
         *         of the document view.
         */
        bool moveBackwardByCharacter(bool selection = false);

        /**
         * Method you can call to move the cursor to the start of the current document.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the start of
         *         the document.
         */
        bool moveToStartOfDocumentView(bool selection = false);

        /**
         * Method you can call to move the cursor to the end of the current document.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the start of
         *         the document.
         */
        bool moveToEndOfDocumentView(bool selection = false);

        /**
         * Method you can call to move the cursor to the end of the current line.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the end of the
         *         current line.
         */
        bool moveToEndOfLine(bool selection = false);

        /**
         * Method you can call to move the cursor to the beginning of the current line.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the beginning
         *         of the current line.
         */
        bool moveToStartOfLine(bool selection = false);

        /**
         * Method you can use to move down a line.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the end of the
         *         document view.
         */
        bool moveForwardByLine(bool selection = false);

        /**
         * Method you can use to move up a line.
         *
         * \param[in] selection If true, then this cursor movement should be considered part of a selection
         *                      process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the cursor was already at the begining of
         *         the document view.
         */
        bool moveBackwardByLine(bool selection = false);

        /**
         * Method you can use to move to a specific location.
         *
         * \param[in] newLocation The new location.
         *
         * \param[in] selection   If true, then this cursor movement should be considered part of a selection
         *                        process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the a cursor location can not be resolved
         *         from the provided location.
         */
        bool moveToPosition(const QPointF& newLocation, bool selection = false);

        /**
         * Method you can use to move near a specific location.
         *
         * \param[in] areaUpperLeft  A value indicating the upper left corner of the search area.
         *
         * \param[in] areaLowerRight A value indicating the lower right corner of the search area.
         *
         * \param[in] newLocation    The new optimal location in the scene, in scene units.
         *
         * \param[in] selection      If true, then this cursor movement should be considered part of a selection
         *                           process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the a cursor location can not be resolved
         *         from the provided location.
         */
        bool moveNearPosition(
            const QPointF& areaUpperLeft,
            const QPointF& areaLowerRight,
            const QPointF& newLocation,
            bool           selection = false
        );

        /**
         * Convenience method you can use to move near a specific location.
         *
         * \param[in] areaBoundingRectangle A bounding rectangle defining the search area boundaries.
         *
         * \param[in] newLocation           The new optimal location in the scene, in scene units.
         *
         * \param[in] selection             If true, then this cursor movement should be considered part of a selection
         *                                  process.  If false, this is a normal cursor movement.
         *
         * \return Returns true if the cursor was moved.  Returns false if the a cursor location can not be resolved
         *         from the provided location.
         */
        bool moveNearPosition(
            const QRectF&  areaBoundingRectangle,
            const QPointF& newLocation,
            bool           selection = false
        );

        /**
         * Method you can use to make a copy (or clone) the current selection.
         *
         * \return Returns a list of cloned Ld::Element instances representing the copied contents.
         */
        SelectionDataPointer copySelection() const;

        /**
         * Method you can use to delete the current selection.  Note that this method will also clear the current
         * selection.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.
         *
         * \return Returns true on success, returns false if there is no active selection.
         */
        bool removeSelection(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method you can use to cut the current selection.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.
         *
         * \return Returns a list of cloned Ld::Element instances representing the copied contents.
         */
        SelectionDataPointer cutSelection(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method you can use to paste the current selection at an element cursor position.
         *
         * \param[in]     selectionData         The selection data to be pasted.
         *
         * \param[in]     elementCursor         The element cursor position to receive the selection.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteSelection(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor = Q_NULLPTR
        );

        /**
         * Method you can use to paste the current selection at the current element cursor position.
         *
         * \param[in]     selectionData         The selection data to be pasted.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.
         *
         * \param[in,out] endingCursor          An optional Ld::ElementCursor instance that will point to the location
         *                                      of the last pasted entry.
         *
         * \return Returns the result from the paste operation.
         */
        PasteResult pasteSelection(
            SelectionDataPointer      selectionData,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor = nullptr
        );

        /**
         * Method you can use to obtain a textual representation of the current selection.
         *
         * \param[out] ok        Optional pointer to a boolean value that will be set to true if the current
         *                       selection can be converted to a contigious block of text.
         *
         * \param[out] breakdown Optional map that can be used to identify each component of the selection.  The
         *                       map key is the character index into the map where each component would begin.  The
         *                       value is an element cursor representing a cursor that points to the position.
         *
         * \return Returns the translated text.
         */
        QString selectionAsText(bool* ok = nullptr, QMap<unsigned long, Ld::ElementCursor>* breakdown = nullptr) const;

        /**
         * Method you can call to change the root presentation used by this cursor instance.
         *
         * \param[in] newRootPresentation The root presentation this cursor will perform location translation functions
         *                                for.
         */
        void setRootPresentation(RootPresentation* newRootPresentation);

        /**
         * Method you can use to obtain the root presentation that this cursor performs location translation function
         * on.
         *
         * \return Returns a pointer to the root presentation instance.
         */
        RootPresentation* rootPresentation() const;

        /**
         * Method you can call to obtain a rectangle indicating the suggested position and size of the cursor, in
         * scene coordinates.
         *
         * \return Returns a rectangle indicating the recommended position and size for the cursor rectangle, in scene
         *         coordinates.
         */
        QRectF cursorRectangle();

        /**
         * Method you call to obtain a list of rectangles indicating the areas that should be highlighted based on the
         * current selection.
         *
         * \return Returns a list of QRectF instances indicating the areas in the scene that should be highlighed.  If
         *         there is no selection, this method will return a list containing a single entry matching the value of
         *         the \ref Cursor::cursorRectangle method.  An empty list will be returned if one or more elements
         *         covered by the selection have not been positioned.
         */
        const QList<QRectF>& selectionRectangles() const;

        /**
         * Method you can use to determine the cursor bounds, in scene units.
         *
         * \return Returns the cursor bounds in scene units.  This value represents a bounding rectangle that will
         *         contain all the selection rectangles returned by \ref Cursor::selectionRectangles.  A zero size or
         *         null rectangle will be returned if one or more elements tied to the cursor have not been positioned.
         */
        const QRectF& boundingRectangle() const;

        /**
         * Method you can use to obtain the current cursor position, in scene coordinates.
         *
         * \return Returns the current cursor position in scene coordinates.
         */
        QPointF cursorPosition() const;

        /**
         * Method that can be called to draw the cursor.
         *
         * \param[in] painter        The painter that should be used to draw the cursor.
         *
         * \param[in] sceneRectangle A rectangle defining the portion of the foreground to be drawn, in scene
         *                           coordinates.
         *
         * \param[in] makeVisible    If true, the cursor will be shown.  If false, the cursor will be hidden.
         */
        void drawCursor(QPainter* painter, const QRectF& sceneRectangle, bool makeVisible = true);

        /**
         * Method you can call to force the cursor's screen position to be updated.
         *
         * \param[in] emitSignals If true, this method will emit signals after triggering cursor physical location data
         *                        to be realculated.
         */
        void forceScreenUpdate(bool emitSignals = true);

        /**
         * Method that is called when the cursor is updated.  This version performs the following additional actions:
         *
         *     * Updates the reported cursor type
         *     * Makes adjustments to the selection cursor, if needed.
         *     * Triggers events to perform updates to the user interface.
         *
         * \param[in] movingForward If true, the cursor position has moved forward.  If false, the cursor position
         *                          has moved backward.
         */
        void cursorChanged(bool movingForward) final;

        /**
         * Assignment operator
         *
         * \param[in] other The instance to assign to this instance.
         *
         * \return Returns a reference to this instance.
         */
        Cursor& operator=(const Cursor& other);

        /**
         * Assignment operator
         *
         * \param[in] other The instance to assign to this instance.
         *
         * \return Returns a reference to this instance.
         */
        Cursor& operator=(const Ld::Cursor& other) override;

    signals:
        /**
         * Signal that is emitted when the cursor position has been updated.  You can use this to trigger the cursor
         * position to be redrawn.
         */
        void cursorUpdated();

        /**
         * Signal that is emitted when the element stack contents have changed.
         */
        void elementStackChanged();

    private:
        /**
         * The default DPI value to use for exported images.
         */
        static constexpr float defaultExportDpi = 300;

        /**
         * Method that is called to update the cursor type.
         */
        void updateCursorType();

        /**
         * Method that is called to make adjustments to the selection cursor to manage selection rules.  This
         * method primarily wraps the Ld::Cursor::adjustSelectionCursor method.
         *
         * \param[in] movingForward If true, we're moving the element cursor forward.  If false, we're moving the
         *                          element cursor backwards.
         */
        void performSelectionCursorAdjustments(bool movingForward);

        /**
         * Method that is called to determine if we are starting a selection.
         *
         * \param[in] selection If true, the user is trying to select an area within the document view.  If false,
         *                      the user is attempting to move the cursor.
         */
        void handleSelection(bool selection);

        /**
         * Method that can be overloaded in derived classes to translate a location into a corresponding cursor
         * position.
         *
         * \param[in] location The location to be translated.
         *
         * \return Returns the Ld::ElementCursor corresponding to the specified location.  The default
         *         implementation returns an invalid cursor.
         */
        Ld::ElementCursor cursorAtLocation(const QPointF& location) const;

        /**
         * Method that can be overloaded in derived classes to translate an element cursor into a location.
         *
         * \param[in] cursor The cursor to be translated to a position.
         *
         * \return Returns the location indicated by the cursor.  The default version returns an invalid location.
         */
        Ld::Location locationOfCursor(const Ld::ElementCursor& cursor) const;

        /**
         * Method that is called to update the selection rectangles.
         */
        void updateSelectionRectangles() const;

        /**
         * Method that can be used to advance a selection cursor.
         *
         * \param[in,out] cursor       The cursor tracking the element.
         *
         * \param[in,out] element      The element to be used and then updated.
         */
        static void advanceSelectionCursor(Ld::ElementCursor& cursor, Ld::ElementPointer& element);

        /**
         * Method that provides selection rectangles for a given cursor and associated element, then advances the
         * cursor and element.
         *
         * \param[in,out] cursor       The cursor tracking the element.
         *
         * \param[in,out] element      The element to be used and then updated.
         *
         * \param[in]     endingCursor The ending cursor.
         *
         * \return Returns a list of selection rectangles to be included.
         */
        static QList<QRectF> selectionRectanglesForElement(
            Ld::ElementCursor&       cursor,
            Ld::ElementPointer&      element,
            const Ld::ElementCursor& endingCursor
        );

        /**
         * Method that calculates the selection rectangles for an element.
         *
         * \param[in] element        The element to be used and then updated.
         *
         * \param[in] startingCursor The cursor tracking the element.
         *
         * \param[in] endingCursor   The ending cursor.
         *
         * \return Returns the selection rectangles for an element.
         */
        static QList<QRectF> calculateSelectionRectanglesForElement(
            Ld::ElementPointer       element,
            const Ld::ElementCursor& startingCursor,
            const Ld::ElementCursor& endingCursor
        );

        /**
         * Method that adds a child under an arbitrary parent.
         *
         * \param[in]     referenceDepth    The depth of the most top level tail element.
         *
         * \param[in]     cursor            The current position in the source tree.
         *
         * \param[in]     cursorElement     The current element pointed to by the cursor.
         *
         * \param[in]     clone             The clone to insert.
         *
         * \param[in,out] copiedEndingStack The stack of the tail elements.  The stack must contain at least one
         *                                  element.
         */
        static void addTailElement(
            unsigned                 referenceDepth,
            const Ld::ElementCursor& cursor,
            Ld::ElementPointer       cursorElement,
            Ld::ElementPointer       clone,
            Ld::ElementPointerList&  copiedEndingStack
        );

        /**
         * Method that adds sideband image data for an element to a \ref SelectionData instance.
         *
         * \param[in] clone         The clone to be inserted.
         *
         * \param[in] original      The original element that we've cloned.
         *
         * \param[in] selectionData The selection data instance to be updated.
         *
         * \param[in] isTopLevel    If true, we should capture the image if the image comes from the visual
         */
        static void addSidebandImageData(
            Ld::ElementPointer   clone,
            Ld::ElementPointer   original,
            SelectionDataPointer selectionData,
            bool                 isTopLevel
        );

        /**
         * Method that adds sideband calculated values an element to a \ref SelectionData instance.
         *
         * \param[in] clone         The clone to be inserted.
         *
         * \param[in] original      The original element that we've cloned.
         *
         * \param[in] selectionData The selection data instance to be updated.
         */
        static void addSidebandCalculatedValues(
            Ld::ElementPointer   clone,
            Ld::ElementPointer   original,
            SelectionDataPointer selectionData
        );

        /**
         * Method that performs fix-ups on parent elements.
         *
         * \param[in] elementsToFixUp A list of element pointers to the elements that need post processing.
         */
        static void fixUpElements(const Ld::ElementPointerList& elementsToFixUp);

        /**
         * Method that pastes text into one or more text regions.
         *
         * \param[in]     selectionData         A \ref SelectionData instance holding the data to paste.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteTextAnnotation(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor
        );

        /**
         * Method that pastes text from a single element into a text annotation.
         *
         * \param[in]     selectionData         A \ref SelectionData instance holding the data to paste.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteTextFromElementIntoAnnotation(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor
        );

        /**
         * Method that pastes text from a single element.
         *
         * \param[in]     selectionData         A \ref SelectionData instance holding the data to paste.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteTextFromElement(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor
        );

        /**
         * Method that pastes text from a single element as a distinct element.  This method is a thin wrapper on
         * the Cursor::pasteElementList methods.
         *
         * \param[in]     selectionData         A \ref SelectionData instance holding the data to paste.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteTextAsElement(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor
        );

        /**
         * Method that pastes entire elements at the cursor position.
         *
         * \param[in]     selectionData         A \ref SelectionData instance holding the data to paste.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteElementList(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor
        );

        /**
         * Method that inserts an element list into a text annotation.
         *
         * \param[in]     selectionData         A \ref SelectionData instance holding the data to paste.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult pasteElementListIntoText(
            SelectionDataPointer       selectionData,
            const Ld::ElementCursor&   elementCursor,
            Ld::CursorStateCollection* cursorStateCollection,
            Ld::ElementCursor*         endingCursor
        );

        /**
         * Method that inserts elements into text at the cursor.  This method assumes that the insertion content has
         * been checked against the insertion location and the newly inserted content is valid.
         *
         * \param[in]     elementsToClone       A list of all the elements to be cloned and inserted.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult insertPastedElementsIntoText(
            const Ld::ElementPointerList& elementsToClone,
            const Ld::ElementCursor&      elementCursor,
            Ld::CursorStateCollection*    cursorStateCollection,
            Ld::ElementCursor*            endingCursor
        );

        /**
         * Method that inserts elements at the cursor into an element with fixed positioning.  This method assumes
         * that the insertion content has been checked against the insertion location and the newly inserted content
         * is valid.
         *
         * \param[in]     elementsToClone       A list of all the elements to be cloned and inserted.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in]     insertAfterCursor     If true, insertions need to occur after the cursor.  If false, the
         *                                      insertions need to occur before the cursor.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult insertPastedElementsIntoFixedElement(
            const Ld::ElementPointerList& elementsToClone,
            const Ld::ElementCursor&      elementCursor,
            bool                          insertAfterCursor,
            Ld::CursorStateCollection*    cursorStateCollection,
            Ld::ElementCursor*            endingCursor
        );

        /**
         * Method that inserts elements at the cursor into an element with fixed positioning.  This method assumes
         * that the insertion content has been checked against the insertion location and the newly inserted content
         * is valid.
         *
         * \param[in]     elementsToClone       A list of all the elements to be cloned and inserted.
         *
         * \param[in]     elementCursor         A cursor that will point to the insertion location for the paste
         *                                      operation.  When the paste operation completes, this cursor will point
         *                                      to the start of the paste location.
         *
         * \param[in]     insertAfterCursor     If true, insertions need to occur after the cursor.  If false, the
         *                                      insertions need to occur before the cursor.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to be updated by the paste operation.
         *
         * \param[in,out] endingCursor          A pointer to an optional cursor pointing to the end of the newly pasted
         *                                      region.
         *
         * \return Returns the result from the paste operation.
         */
        static PasteResult insertPastedElementsIntoPositionalElement(
            const Ld::ElementPointerList& elementsToClone,
            const Ld::ElementCursor&      elementCursor,
            bool                          insertAfterCursor,
            Ld::CursorStateCollection*    cursorStateCollection,
            Ld::ElementCursor*            endingCursor
        );

        /**
         * Method that adds a cursor to the cursor state collection, returning an index to the location.
         *
         * \param[in]     cursor                The cursor to be added.
         *
         * \param[in,out] cursorStateCollection The cursor state collection to receive the cursor.  The cursor will
         *                                      not be added if this value is null.
         *
         * \return Returns the index into the cursor state collection where the cursor resides.  A value of
         *         static_cast<unsigned>(-1) is returned if the cursor is not added.
         */
        static unsigned addCursorToCollection(
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method that calls the fixer for an element during a paste operation.
         *
         * \param[in] element               The element to trigger the fixer on.
         *
         * \param[in] cursorStateCollection The cursor state collection to be managed.
         */
        static void pasteFixer(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection);

        /**
         * The cursor type.
         */
        Type currentCursorType;

        /**
         * The last cursor type.
         */
        Type lastCursorType;

        /**
         * The current selection type
         */
        SelectionType currentSelectionType;

        /**
         * The selection cursor we report.
         */
        Ld::ElementCursor reportedSelectionCursor;

        /**
         * Weak pointer to the last element that the cursor pointed to.
         */
        Ld::ElementWeakPointer lastSelectedElement;

        /**
         * The current list of selection rectangles.
         */
        mutable QList<QRectF> currentSelectionRectangles;

        /**
         * Rectangle representing the bounding rectangle for the scene.
         */
        mutable QRectF currentBoundingRectangle;
};

/**
 * Convenience type used to track a cursor pointer.
 */
typedef QSharedPointer<Cursor> CursorPointer;

/**
 * Convenience type used to track a weap pointer to a cursor.
 */
typedef QWeakPointer<Cursor> CursorWeakPointer;

#endif
