/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_H
#define TABLE_FRAME_PRESENTATION_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QList>
#include <QBitArray>

#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_table_frame_element.h>
#include <ld_table_frame_visual.h>

#include "app_common.h"
#include "scene_units.h"
#include "presentation_with_grouped_children.h"

namespace EQt {
    class GraphicsItemGroup;
}

class PlacementNegotiator;
class PlacementTracker;
class Presentation;
class PresentationArea;
class TableActiveAreaGroup;
class TableFrameGraphicsItemGroup;

/**
 * This class presents a table within the document view.
 */
class APP_PUBLIC_API TableFramePresentation:public         PresentationWithGroupedChildren,
                                            public virtual Ld::TableFrameVisual {
    Q_OBJECT

    public:
        TableFramePresentation();

        ~TableFramePresentation() override;

        /**
         * Static method that creates an instance of a \ref TableFramePresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::GROUPED.
         */
        Ld::Element::ChildPlacement childPlacement() const final;

        /**
         * Method that is called by a child to the parent to request that the parent reposition the contents of the
         * child.
         *
         * \param[in] childPresentation The presentation that needs to be repositioned.
         */
        void requestRepositioning(Presentation* childPresentation) final;

        /**
         * Method that is called by a parent to a child to tell the child to start the placement operation,
         * recalculating sizes and positions.  This method will repeatedly call the parent's instance of
         * \ref PlacementNegotiator::requestArea followed by the parent's instance of one of
         * \ref PlacementNegotiator::allocateArea or \ref PlacementNegotiator::areaInsufficient until the data
         * managed by the child is processed and placed.
         *
         * \param[in] placementTracker       A pointer to the placement tracker that can be used to track and update
         *                                   status during a time-consuming placement operation.
         *
         * \param[in] parent                 Pointer to the parent presentation negotiator.  Provided so that the child
         *                                   does not have to do the bit of additional work to identify its parent.
         *
         * \param[in] childIdentifier        An identifier the parent uses to quickly identify the child.
         *
         * \param[in] nextSibling            A pointer to the next sibling under the parent.  This child can use this to
         *                                   determine if the next sibling's space requirements should be taken into
         *                                   account when allocating room for this child.
         *
         * \param[in] honorLeadingWhitespace If true, the child should honor, and not remove, whitespace at the left
         *                                   side of the string.
         *
         * \param[in] minimumTopSpacing      Spacing to apply above the first line of the placement area.  The value
         *                                   is primarily intended to be used to manage paragraph spacing.  A negative
         *                                   value indicates that there should be no imposed top spacing.
         *
         * \param[in] lineSpacing            Additional line spacing that should be optionally included by this child.
         *                                   Children can adjust the height they report for their presentation areas.
         *                                   A value of 1.0 indicates single spacing.  A value of 1.5 indicates 1 1/2
         *                                   line spacing.  A value of 2.0 indicates double spacing.
         *
         * \param[in] relativeScale          A hint value used to indicate if this child should try to scale smaller
         *                                   than usual. This value is intended for cases where an equation element
         *                                   needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                   indicates normal scale.  The value is intended as a hint, children are not
         *                                   required to honor this parameter.
         */
        void recalculatePlacement(
            PlacementTracker*    placementTracker,
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            Presentation*        nextSibling,
            bool                 honorLeadingWhitespace,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) final;

        /**
         * Method that is called by a parent to a child to tell the child to start the placement operation using
         * existing data.  This method will call the parent \ref PlacementNegotiator::allocateArea instance on each
         * already analyzed placement area.
         *
         * \param[in] parent                  Pointer to the parent presentation negotiator.  Provided so that the child
         *                                    does not have to do the bit of additional work to identify its parent.
         *
         * \param[in] childIdentifier         An identifier the parent uses to quickly identify the child.
         *
         * \param[in] firstPresentationAreaId The zero based ID of the first presentation area to place.
         *
         * \param[in] minimumTopSpacing       Spacing to apply above the first line of the placement area.  The value
         *                                    is primarily intended to be used to manage paragraph spacing.  A negative
         *                                    value indicates that there should be no imposed top spacing.
         *
         * \param[in] lineSpacing             Additional line spacing that should be optionally included by this child.
         *                                    Children can adjust the height they report for their presentation areas.
         *                                    A value of 1.0 indicates single spacing.  A value of 1.5 indicates 1 1/2
         *                                    line spacing.  A value of 2.0 indicates double spacing.
         *
         * \param[in] relativeScale           A hint value used to indicate if this child should try to scale smaller
         *                                    than usual. This value is intended for cases where an equation element
         *                                    needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                    indicates normal scale.  The value is intended as a hint, children are not
         *                                    required to honor this parameter.
         */
        void redoPlacement(
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            unsigned long        firstPresentationAreaId = 0,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) final;

        /**
         * Method called by the parent to determine the spacing that should be applied between this placement negotiator
         * and the next placement negotiator.
         *
         * \return Returns the spacing to apply between this placement negotiator and the next placement negotiator.
         *         The spacing should be the maximum of this value and any top spacing requirement for the next
         *         placement negotiator.
         */
        float bottomSpacingSceneUnits() final;

        /**
         * Method called by a child to request the next available area on the current line.  Note that this method does
         * not actually modify the parent.
         *
         * \param[in]  childIdentifier An identifier this child must provide to the parent to help it quickly identify
         *                             the child.
         *
         * \param[out] spaceQualifier  A qualifier indicating the nature of the provided space.
         *
         * \return Returns the width and height of the next available area on the current line.  Values represent the
         *         maximum available space in each dimension.
         */
        QSizeF requestArea(unsigned long childIdentifier, SpaceQualifier* spaceQualifier = Q_NULLPTR) final;

        /**
         * Method called by a child to a parent to allocate the next available area.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] size               The required space.  This value must be less than or equal to the space
         *                               required by PlacementNegotiator:requestArea.
         *
         * \param[in] ascent             Ascent used for text to perform vertical alignment.  Non text elements should
         *                               set this value to 0.
         *
         * \param[in] canStretch         If true, the parent can ask the child to horizontally stretch this presentation
         *                               area.  This parameter is used to justify text.
         */
        void allocateArea(
            unsigned long childIdentifier,
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0,
            bool          canStretch = false
        ) final;

        /**
         * Method called by a child to indicate that the provided space by PlacementNegotiator::requestArea is
         * insufficient.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] size               The last reported available space.  This value must be equal to the space
         *                               reported by PlacementNegotiator:requestArea.
         */
        void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) final;

        /**
         * Method called by a parent to apply a stretch factor to a child.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] stretchFactor      The stretch factor to be applied to this child's presentation area.  A value of
         *                               0 indicates normal unstretched width.  A value of 0.1 indicates that the area
         *                               should be stretched to 110% of it's normal width.  A value of 1.0 indicates
         *                               that the area should be twice as wide as normal.
         *
         * \return Returns the new stretched width.
         */
        void applyStretch(unsigned long presentationAreaId, float stretchFactor) final;

        /**
         * Method called by a parent to obtain a specified child graphics item, by presentation area ID.
         *
         * \param[in] presentationAreaId A zero based presentation area ID indicating which graphics region is of
         *                               interest.
         *
         * \return Returns a pointer to a QGraphicsItem used to represent this visual.  A null pointer is returned if
         *         the presentationAreaId is invalid.  Note that the \ref PlacementNegotiator or \ref Presentation
         *         instance always maintains full ownership of the graphics items and is responsible for managing their
         *         life cycle.
         */
        QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const final;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.  Note that this method must call the
         * \ref PlacementNegotiator::resetPlacement method of every child.
         */
        void resetPlacement() final;

        /**
         * Method that is called when this placement negotiator needs to remove itself from the scene.  You should
         * overload this method in derived classes with code that removes and optionally destroys placement areas from
         * the parent.
         */
        void removeFromScene() final;

        /**
         * Method that is called by the cursor logic to determine if this element should be highlighted during
         * selection.
         *
         * \return Returns true if the entire element can be highlighted.  Returns false if the element should not be
         *         highlighted by the cursor.
         */
        bool cursorCanHighlight() const final;

    signals:
        /**
         * Signal that is emitted when one or more rows are added to the associated Ld::TableFrameElement.
         *
         * \param[out] rowNumber     The zero based number of the first newly inserted row.
         *
         * \param[out] numberRows    The number of newly inserted rows.
         *
         * \param[out] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void rowsWereInserted(unsigned rowNumber, unsigned numberRows, bool formatUpdated);

        /**
         * Signal that is emitted when one or more columns are added to the associated Ld::TableFrameElement.
         *
         * \param[out] columnNumber  The zero based number of the first newly inserted columns.
         *
         * \param[out] numberColumns The number of newly inserted columns.
         *
         * \param[out] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void columnsWereInserted(unsigned columnNumber, unsigned numberColumns, bool formatUpdated);

        /**
         * Signal that is emitted when a row is about to be removed from the associated Ld::TableFrameElement.
         *
         * \param[out] rowNumber           The zero based number of row we are about to remove.
         *
         * \param[out] formatWillBeUpdated If true, the format will also be updated.  If false, the format will be
         *                                 left unchanged.
         */
        void removingRow(unsigned rowNumber, bool formatWillBeUpdated);

        /**
         * Signal that is emitted when a row has been removed from the associated Ld::TableFrameElement
         *
         * \param[out] rowNumber     The zero based number of the row we just removed.
         *
         * \param[out] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void rowWasRemoved(unsigned rowNumber, bool formatUpdated);

        /**
         * Signal that is emitted when a column is about to be removed from the associated
         * Ld::TableFrameElement.
         *
         * \param[out] columnNumber        The zero based number of column we are about to remove.
         *
         * \param[out] formatWillBeUpdated If true, the format will also be updated.  If false, the format will be
         *                                 left unchanged.
         */
        void removingColumn(unsigned columnNumber, bool formatWillBeUpdated);

        /**
         * Signal that is emitted when a column has been removed from the associated Ld::TableFrameElement
         *
         * \param[out] columnNumber  The zero based number of the column we just removed.
         *
         * \param[out] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void columnWasRemoved(unsigned columnNumber, bool formatUpdated);

        /**
         * Signal that is emitted when we are about to merge cells in the associated Ld::TableFrameElement.
         *
         * \param[out] rowIndex                The zero based row index of the top-most cell we are merging.
         *
         * \param[out] columnIndex             The zero based column index of the left-most cell we are merging.
         *
         * \param[out] numberCellsToMergeRight The number of additional cells to the right of the cell that should
         *                                     be merged into the specified cell.
         *
         * \param[out] numberCellsToMergeDown  The number of additional cells below the cell that should be merged
         *                                     into the specified cell.
         *
         * \param[out] formatWillBeUpdated     If true, the format will also be updated.  If false, the format will
         *                                     be left unchanged.
         */
        void mergingCells(
            unsigned rowIndex,
            unsigned columnIndex,
            unsigned numberCellsToMergeRight,
            unsigned numberCellsToMergeDown,
            bool     formatWillBeUpdated
        );

        /**
         * Signal that is emitted after cells are merged in the associated Ld::TableFrameElement.
         *
         * \param[out] rowIndex      The zero based row index of the top-most cell we merged.
         *
         * \param[out] columnIndex   The zero based column index of the left-most cell we merged.
         *
         * \param[out] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void cellsWereMerged(unsigned rowIndex, unsigned columnIndex, bool formatUpdated);

        /**
         * Signal that is emitted when we are about to unmerge cells in the Ld::TableFrameElement class.
         *
         * \param[out] rowIndex    The zero based row index of the left-most merged cell.
         *
         * \param[out] columnIndex The zero based column index of the left-most merged cell.
         */
        void unmergingCells(unsigned rowIndex, unsigned columnIndex);

        /**
         * Signal that is emitted when we have completed an unmerge operation.
         *
         * \param[out] rowIndex    The zero based row index of the left-most merged cell.
         *
         * \param[out] columnIndex The zero based column index of the left-most merged cell.
         */
        void cellsWereUnmerged(unsigned rowIndex, unsigned columnIndex);

    protected slots:
        /**
         * Slot that is triggered when one or more rows are added to the associated Ld::TableFrameElement.
         *
         * \param[in] rowNumber     The zero based number of the first newly inserted row.
         *
         * \param[in] numberRows    The number of newly inserted rows.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        virtual void processRowsInserted(unsigned rowNumber, unsigned numberRows, bool formatUpdated);

        /**
         * Slot that is triggered when one or more columns are added to the associated Ld::TableFrameElement.
         *
         * \param[in] columnNumber  The zero based number of the first newly inserted columns.
         *
         * \param[in] numberColumns The number of newly inserted columns.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        virtual void processColumnsInserted(unsigned columnNumber, unsigned numberColumns, bool formatUpdated);

        /**
         * Slot that is triggered when a row is about to be removed from the associated Ld::TableFrameElement.
         *
         * \param[in] rowNumber           The zero based number of row we are about to remove.
         *
         * \param[in] formatWillBeUpdated If true, the format will also be updated.  If false, the format will be
         *                                left unchanged.
         */
        virtual void processAboutToRemoveRow(unsigned rowNumber, bool formatWillBeUpdated);

        /**
         * Slot that is triggered when a row has been removed from the associated Ld::TableFrameElement
         *
         * \param[in] rowNumber     The zero based number of the row we just removed.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        virtual void processRowRemoved(unsigned rowNumber, bool formatUpdated);

        /**
         * Slot that is triggered when a column is about to be removed from the associated
         * Ld::TableFrameElement.
         *
         * \param[in] columnNumber        The zero based number of column we are about to remove.
         *
         * \param[in] formatWillBeUpdated If true, the format will also be updated.  If false, the format will be
         *                                left unchanged.
         */
        virtual void processAboutToRemoveColumn(unsigned columnNumber, bool formatWillBeUpdated);

        /**
         * Slot that is triggered when a column has been removed from the associated Ld::TableFrameElement
         *
         * \param[in] columnNumber  The zero based number of the column we just removed.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        virtual void processColumnRemoved(unsigned columnNumber, bool formatUpdated);

        /**
         * Slot that is triggered when we are about to merge cells in the associated Ld::TableFrameElement.
         *
         * \param[in] rowIndex                The zero based row index of the top-most cell we are merging.
         *
         * \param[in] columnIndex             The zero based column index of the left-most cell we are merging.
         *
         * \param[in] numberCellsToMergeRight The number of additional cells to the right of the cell that should
         *                                    be merged into the specified cell.
         *
         * \param[in] numberCellsToMergeDown  The number of additional cells below the cell that should be merged
         *                                    into the specified cell.
         *
         * \param[in] formatWillBeUpdated     If true, the format will also be updated.  If false, the format will
         *                                    be left unchanged.
         */
        virtual void processAboutToMergeCells(
            unsigned rowIndex,
            unsigned columnIndex,
            unsigned numberCellsToMergeRight,
            unsigned numberCellsToMergeDown,
            bool     formatWillBeUpdated
        );

        /**
         * Slot that is triggered after cells are merged in the associated Ld::TableFrameElement.
         *
         * \param[in] rowIndex      The zero based row index of the top-most cell we merged.
         *
         * \param[in] columnIndex   The zero based column index of the left-most cell we merged.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        virtual void processCellsMerged(unsigned rowIndex, unsigned columnIndex, bool formatUpdated);

        /**
         * Slot that is triggered when we are about to unmerge cells in the Ld::TableFrameElement class.
         *
         * \param[out] rowIndex    The zero based row index of the left-most merged cell.
         *
         * \param[out] columnIndex The zero based column index of the left-most merged cell.
         */
        virtual void processAboutToUnmergeCells(unsigned rowIndex, unsigned columnIndex);

        /**
         * Slot that is triggered when we have completed an unmerge operation.
         *
         * \param[out] rowIndex    The zero based row index of the left-most merged cell.
         *
         * \param[out] columnIndex The zero based column index of the left-most merged cell.
         */
        virtual void processCellsUnmerged(unsigned rowIndex, unsigned columnIndex);

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
        virtual void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation) final;

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildPresentationInsertedBefore(
            unsigned long childIndex,
            Presentation* childPresentation
        ) final;

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildPresentationInsertedAfter(
            unsigned long childIndex,
            Presentation* childPresentation
        ) final;

        /**
         * Slot that is triggered when this object is newly coupled to an element.  You can use this slot to trigger
         * updates to the presentation based on the element contents and type.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] element The element that this visual is now tied to.
         */
        void tiedToElement(Ld::ElementPointer element) final;

        /**
         * Slot that is triggered when a Ld::Element reports that its format has been changed.  You can use this slot
         * to perform updates to the visual representation of the element.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default version of this slot will trigger the presentation to be reposition and redrawn.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void processDiagnostic(Ld::DiagnosticPointer diagnostic) final;

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.
         */
        void clearDiagnosticDisplay() final;

    private:
        /**
         * Private class used to cache scene-unit specific format parameters.
         */
        class Format;

        /**
         * Private class used to track volatile table area data.
         */
        class Area;

        /**
         * Private class used to track the position across rows and within individual cells.
         */
        class Cursor;

        /**
         * Private class used to track row top/bottom locations.
         */
        class RowLocation;

        /**
         * Private class used to represent a rectangular region of a table.
         */
        class Rectangle;

        /**
         * Private class used to track child presentation areas as well as the graphics item groups that contain them.
         */
        class Tracker;

        /**
         * Value indicating the minimum allowed space before the end of page, in points.
         */
        static constexpr unsigned minimumVerticalSpacePoints = 36;

        /**
         * Method that is called when one or more rows are added to the associated Ld::TableFrameElement.
         *
         * \param[in] rowNumber     The zero based number of the first newly inserted row.
         *
         * \param[in] numberRows    The number of newly inserted rows.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void rowsInserted(unsigned rowNumber, unsigned numberRows, bool formatUpdated) final;

        /**
         * Method that is called when one or more columns are added to the associated Ld::TableFrameElement.
         *
         * \param[in] columnNumber  The zero based number of the first newly inserted columns.
         *
         * \param[in] numberColumns The number of newly inserted columns.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void columnsInserted(unsigned columnNumber, unsigned numberColumns, bool formatUpdated) final;

        /**
         * Method that is called when a row is about to be removed from the associated Ld::TableFrameElement.
         *
         * \param[in] rowNumber           The zero based number of row we are about to remove.
         *
         * \param[in] formatWillBeUpdated If true, the format will also be updated.  If false, the format will be
         *                                left unchanged.
         */
        void aboutToRemoveRow(unsigned rowNumber, bool formatWillBeUpdated) final;

        /**
         * Method that is called when a row has been removed from the associated Ld::TableFrameElement
         *
         * \param[in] rowNumber     The zero based number of the row we just removed.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void rowRemoved(unsigned rowNumber, bool formatUpdated) final;

        /**
         * Method that is called when a column is about to be removed from the associated
         * Ld::TableFrameElement.
         *
         * \param[in] columnNumber        The zero based number of column we are about to remove.
         *
         * \param[in] formatWillBeUpdated If true, the format will also be updated.  If false, the format will be
         *                                left unchanged.
         */
        void aboutToRemoveColumn(unsigned columnNumber, bool formatWillBeUpdated) final;

        /**
         * Method that is called when a column has been removed from the associated Ld::TableFrameElement
         *
         * \param[in] columnNumber  The zero based number of the column we just removed.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void columnRemoved(unsigned columnNumber, bool formatUpdated) final;

        /**
         * Method that is called when we are about to merge cells in the associated Ld::TableFrameElement.
         *
         * \param[in] rowIndex                The zero based row index of the top-most cell we are merging.
         *
         * \param[in] columnIndex             The zero based column index of the left-most cell we are merging.
         *
         * \param[in] numberCellsToMergeRight The number of additional cells to the right of the cell that should
         *                                    be merged into the specified cell.
         *
         * \param[in] numberCellsToMergeDown  The number of additional cells below the cell that should be merged
         *                                    into the specified cell.
         *
         * \param[in] formatWillBeUpdated     If true, the format will also be updated.  If false, the format will
         *                                    be left unchanged.
         */
        void aboutToMergeCells(
            unsigned rowIndex,
            unsigned columnIndex,
            unsigned numberCellsToMergeRight,
            unsigned numberCellsToMergeDown,
            bool     formatWillBeUpdated
        ) final;

        /**
         * Method that is called after cells are merged in the associated Ld::TableFrameElement.
         *
         * \param[in] rowIndex      The zero based row index of the top-most cell we merged.
         *
         * \param[in] columnIndex   The zero based column index of the left-most cell we merged.
         *
         * \param[in] formatUpdated If true, the format has been updated.  If false, the format was not changed.
         */
        void cellsMerged(unsigned rowIndex, unsigned columnIndex, bool formatUpdated) final;

        /**
         * Method that is called when we are about to unmerge cells in the Ld::TableFrameElement class.
         *
         * \param[in] rowIndex    The zero based row index of the left-most merged cell.
         *
         * \param[in] columnIndex The zero based column index of the left-most merged cell.
         */
        void aboutToUnmergeCells(unsigned rowIndex, unsigned columnIndex) final;

        /**
         * Method that is called when we have completed an unmerge operation.
         *
         * \param[in] rowIndex    The zero based row index of the left-most merged cell.
         *
         * \param[in] columnIndex The zero based column index of the left-most merged cell.
         */
        void cellsUnmerged(unsigned rowIndex, unsigned columnIndex) final;

        /**
         * Method that is called to trigger repositioning.
         */
        void requestRepositioning();

        /**
         * Method that is called when the underlying format is changed.  This method updates internal data used to
         * render the table lines.
         *
         * \param[in] newFormat The new table frame format instance.
         */
        void updateFormatData(Ld::FormatPointer newFormat);

        /**
         * Method that is called to draw all background fill embellishments for the table.
         *
         * \param[in]     cellDataByPosition Map providing the row/column locations of each cell.
         *
         * \param[in]     cursor             The cursor used to identify row/column positions.
         *
         * \param[in]     format             The presentation format used to get dimensional data.
         *
         * \param[in,out] tracker            The tracker used to draw the embellishments.
         */
        static void drawCellBackgrounds(
            const Ld::TableFrameElement::CellDataByPosition& cellDataByPosition,
            const Cursor&                                    cursor,
            const Format&                                    format,
            Tracker&                                         tracker
        );

        /**
         * Method that is called to draw all row lines for the table.
         *
         * \param[in]     cursor             The cursor used to identify row/column positions.
         *
         * \param[in]     format             The presentation format used to get dimensional data.
         *
         * \param[in,out] tracker            The tracker used to draw the embellishments.
         */
        static void drawRowLines(const Cursor& cursor, const Format& format, Tracker& tracker);

        /**
         * Method that is called to draw all column lines for the table.
         *
         * \param[in]     cursor             The cursor used to identify row/column positions.
         *
         * \param[in]     format             The presentation format used to get dimensional data.
         *
         * \param[in,out] tracker            The tracker used to draw the embellishments.
         */
        static void drawColumnLines(const Cursor& cursor, const Format& format, Tracker& tracker);

        /**
         * Method that updates the tracker with the row line locations.
         *
         * \param[in]     cursor  The cursor used to identify the row positions.
         *
         * \param[in]     format  The presentation format used to get dimensional data.
         *
         * \param[in,out] tracker The tracker to be updated.
         */
        static void updateTrackerRowLocations(const Cursor& cursor, const Format& format, Tracker& tracker);

        /**
         * The current child presentation.
         */
        Presentation* currentChildPresentation;

        /**
         * Pointer to the class containing the engine used to layout the table.
         */
        Cursor* currentCursor;

        /**
         * Pointer to tracker holding the table layout information and graphics elements.
         */
        Tracker* currentTracker;
};

#endif
