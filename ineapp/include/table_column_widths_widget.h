/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableColumnWidthsWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_COLUMN_WIDTHS_WIDGET_H
#define TABLE_COLUMN_WIDTHS_WIDGET_H

#include <QtGlobal>
#include <QWidget>
#include <QString>
#include <QTableView>

#include <ld_root_import.h>

#include "app_common.h"

/**
 * Class that provides a widget you can use to set table column widths.
 */
class APP_PUBLIC_API TableColumnWidthsWidget:public QTableView {
    Q_OBJECT

    public:
        /**
         * Enumeration of supported units.
         */
        enum Unit {
            /**
             * Indicates an invalid unit.
             */
            INVALID = 0,

            /**
             * Indicates values should be presented in mm.
             */
            MILLIMETER = 1,

            /**
             * Indicates values should be presented in cm.
             */
            CENTIMETER = 2,

            /**
             * Indicates values should be presented in points.
             */
            POINT = 3,

            /**
             * Indicates values should be presented in picas.
             */
            PICA = 4,

            /**
             * Indicates values should be presented in inches.
             */
            INCH = 5,
        };

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this model.
         */
        TableColumnWidthsWidget(QWidget* parent = Q_NULLPTR);

        ~TableColumnWidthsWidget() override;

        /**
         * Method you can use to determine the currently displayed units.
         *
         * \return Returns the currently selected width units.
         */
        Unit displayedUnits() const;

        /**
         * Method you can use to obtain the current total number of columns.
         *
         * \return Returns the current number of table columns.
         */
        unsigned numberTableColumns() const;

        /**
         * Method you can use to determine if this column is proportionally sized or fixed size.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \return Returns true if this column is proportional size.  Returns false if this column is fixed size.
         */
        bool isProportionalColumn(unsigned tableColumnIndex) const;

        /**
         * Method you can use to determine if this column is fixed size or proportionally sized.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \return Returns true if this column is fixed size.  Returns false if this column is proportionally sized.
         */
        bool isFixedColumn(unsigned tableColumnIndex) const;

        /**
         * Method you can use to obtain the column width, either as a fraction or as a size, in points.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \return Returns the column width, either as a fraction or in points.
         */
        float columnWidth(unsigned tableColumnIndex) const;

    signals:
        /**
         * Signal that is emitted when a column value is changed.
         *
         * \param[out] columnIndex     The zero based table column index.
         *
         * \param[out] nowProportional Value that holds true if the table column is now proportionally sized.  The
         *                             value will hold false if the table is now fixed size.
         *
         * \param[out] newWidth        The new width value for the table in points or fractional remaining space.
         */
        void columnWidthChanged(unsigned columnIndex, bool nowProportional, float newWidth);

    public slots:
        /**
         * Slot you can trigger to set the currently selected units.
         *
         * \param[in] newUnits The new units to use for fixed width columns.
         */
        void setDisplayedUnits(Unit newUnits);

        /**
         * Slot you can trigger to set the total number of columns.
         *
         * \param[in] newNumberColumns the new number of table columns.
         */
        void setNumberTableColumns(unsigned newNumberColumns);

        /**
         * Slot you can trigger to specify that a column should be proportionally sized or fixed size.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \param[in] nowProportional If true, the table column will be proportionally sized. If false, the table
         *                            column is now fixed size.
         */
        void setProportionalColumn(unsigned tableColumnIndex, bool nowProportional = true);

        /**
         * Slot you can trigger to specify that a column should be fixed size or proportionally sized.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \param[in] nowFixed         If true, the table column will be fixed size.   If false, the table column is
         *                             now proportionally sized.
         */
        void setFixedColumn(unsigned tableColumnIndex, bool nowFixed = true);

        /**
         * Slot you can trigger to set the table column width.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \param[in] newWidth         The new table column width value.
         */
        void setColumnWidth(unsigned tableColumnIndex, float newWidth);

        /**
         * Slot you can trigger to set the table column width.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         *
         * \param[in] nowProportional  If true, the new width will be viewed as a proportional size.  If false, the new
         *                             width will be viewed as fixed size.
         *
         * \param[in] newWidth         The new table column width value.
         */
        void setColumnWidth(unsigned tableColumnIndex, bool nowProportional, float newWidth);

        /**
         * Slot you can trigger to set the table column width to an invalid value.
         *
         * \param[in] tableColumnIndex The zero based table column index.
         */
        void setColumnWidthInvalid(unsigned tableColumnIndex);
};

#endif
