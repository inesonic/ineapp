/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation::Format class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_FORMAT_H
#define TABLE_FRAME_PRESENTATION_FORMAT_H

#include <QSharedPointer>
#include <QList>
#include <QColor>

#include <ld_table_column_width.h>
#include <ld_table_frame_format.h>

#include "scene_units.h"
#include "table_frame_presentation.h"

namespace Ld {
    class TableFrameFormat;
}

/**
 * Class that wraps a Ld::TableFrameFormat class, providing table dimension data, in scene units.
 */
class TableFramePresentation::Format:public SceneUnits {
    public:
        /**
         * Typedef mapping to the Ld::TableFrameFormat::CellPosition class.  You can use this class to track the cell
         * row/column position as a singular value.
         */
        typedef Ld::TableFrameFormat::CellPosition CellPosition;

        /**
         * Typedef you can use to track the reported justification.
         */
        typedef Ld::TableFrameFormat::Justification Justification;

        /**
         * Constructor
         *
         * \param[in] format        The format instance we want to track and store parameters for.
         *
         * \param[in] numberColumns The total number of columns in the table.
         *
         * \param[in] numberRows    The total number of rows in the table.
         */
        Format(QSharedPointer<Ld::TableFrameFormat> format, unsigned numberColumns, unsigned numberRows);

        ~Format();

        /**
         * Method you can use to obtain the number of columns.
         *
         * \return Returns the current number of columns.
         */
        unsigned numberColumns() const;

        /**
         * Method you can use to obtain the number of rows.
         *
         * \return Returns the current number of rows.
         */
        unsigned numberRows() const;

        /**
         * Method you can use to obtain the format class this instance is marshalling.
         */
        QSharedPointer<const Ld::TableFrameFormat> format() const;

        /**
         * Method you can use to obtain the current left side block indentation, in scene units,
         *
         * \return Returns the left-side indentation, in scene units.
         */
        float leftIndentation() const;

        /**
         * Method you can use to obtain the current right side block indentation, in scene units.
         *
         * \return Returns the right-side indentation, in scene units.
         */
        float rightIndentation() const;

        /**
         * Method you can use to obtain the current top side block spacing, in scene units.
         *
         * \return Returns the top-side spacing, in scene units.
         */
        float topSpacing() const;

        /**
         * Method you can use to obtain the current bottom side block spacing, in scene units.
         *
         * \return Returns the bottom-side spacing, in scene units.
         */
        float bottomSpacing() const;

        /**
         * Method you can use to determine the current justification mode.
         *
         * \return Returns the current justification mode.
         */
        Justification justificationMode() const;

        /**
         * Method you can use to obtain the column width setting for a specific column.  Fixed width values will be
         * converted to scene units.
         *
         * \param[in] columnIndex The column index of issue.
         */
        Ld::TableColumnWidth columnWidth(unsigned columnIndex) const;

        /**
         * Method you can use to obtain the column line specification for a given line.  There will always be one
         * more column line specification than the number of columns.
         *
         * \param[in] lineIndex The zero based line index.  Line 0 represents the left border line.  The last line
         *                      represents the right border line.
         *
         * \return Returns the column line specification.
         */
        Ld::TableLineSettings columnLineSetting(unsigned lineIndex) const;

        /**
         * Method you can use to obtain the row line specification for a given line.  There will always be one
         * more row line specification than the number of rows.
         *
         * \param[in] lineIndex The zero based line index.  Line 0 represents the left border line.  The last line
         *                      represents the right border line.
         *
         * \return Returns the row line specification.
         */
        Ld::TableLineSettings rowLineSetting(unsigned lineIndex) const;

        /**
         * Method you can use to obtain the color to apply to a specific cell.
         *
         * \param[in] rowIndex    The zero based index of the row of the cell.
         *
         * \param[in] columnIndex The zero based column index of the cell.
         *
         * \return Returns the color to apply to the specific cell.
         */
        QColor blendedColor(unsigned rowIndex, unsigned columnIndex) const;

        /**
         * Method you can use to obtain the left cell gutter in scene units.
         *
         * \return returns the current left cell gutter value, in scene units.
         */
        float leftGutter() const;

        /**
         * Method you can use to obtain the right cell gutter in scene units.
         *
         * \return returns the current right cell gutter value, in scene units.
         */
        float rightGutter() const;

        /**
         * Method you can use to obtain the top cell gutter in scene units.
         *
         * \return returns the current top cell gutter value, in scene units.
         */
        float topGutter() const;

        /**
         * Method you can use to obtain the bottom cell gutter in scene units.
         *
         * \return returns the current bottom cell gutter value, in scene units.
         */
        float bottomGutter() const;

    private:
        /**
         * The number of rows in the table.
         */
        unsigned currentNumberRows;

        /**
         * The number of columns in the table.
         */
        unsigned currentNumberColumns;

        /**
         * The current left indentation value, in scene units.
         */
        float currentLeftIndentation;

        /**
         * The current right indentation value, in scene units.
         */
        float currentRightIndentation;

        /**
         * The current top spacing value, in scene units.
         */
        float currentTopSpacing;

        /**
         * The current bottom spacing value, in scene units.
         */
        float currentBottomSpacing;

        /**
         * Shared pointer to the underlying table format.
         */
        QSharedPointer<const Ld::TableFrameFormat> currentFormat;

        /**
         * List of adjusted column width settings, by column.
         */
        QList<Ld::TableColumnWidth> currentColumnWidths;

        /**
         * List indicating a mapping of every cell color by cell index.
         */
        QList<QColor> currentCellColors;

        /**
         * The current left gutter, in scene units.
         */
        float currentLeftGutter;

        /**
         * The current right gutter, in scene units.
         */
        float currentRightGutter;

        /**
         * The current top gutter, in scene units.
         */
        float currentTopGutter;

        /**
         * The current buttom gutter, in scene units.
         */
        float currentBottomGutter;
};


#endif
