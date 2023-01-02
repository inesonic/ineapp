/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableColumnWidthsModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_COLUMN_WIDTHS_MODEL_H
#define TABLE_COLUMN_WIDTHS_MODEL_H

#include <QtGlobal>
#include <QString>
#include <QVariant>
#include <QString>
#include <QList>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <eqt_dimension_line_edit.h>

#include "table_column_widths_widget.h"

/**
 * Class that provides a model for a view-model-delegate model you can use to adjust table column widths.
 */
class TableColumnWidthsModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Type used to represent our base units.
         */
        typedef TableColumnWidthsWidget::Unit Unit;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this model.
         */
        TableColumnWidthsModel(QObject* parent = Q_NULLPTR);

        ~TableColumnWidthsModel() override;

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

        /**
         * Method you can use to determine the currently selected units.
         *
         * \return Returns the currently selected width units.
         */
        Unit units() const;

        /**
         * Method used by the view to determine what operations can be performed on the cell.
         *
         * \param[in] index The index into the model used to locate the cell.
         *
         * \return Returns flags indicating what operations are acceptable.
         */
        Qt::ItemFlags flags(const QModelIndex& index) const final;

        /**
         * Method used by the view to obtain the data to be displayed.
         *
         * \param[in] index The index into the model used to locate the data.
         *
         * \param[in] role  The display role for the data.
         *
         * \return Returns a variant type holding the data to be displayed.
         */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const final;

        /**
         * Method used by the view to update data in the model.
         *
         * \param[in] index The index into the model used to point to the location of the data.
         *
         * \param[in] value The new data for the cell.
         *
         * \param[in] role  The role associated with the newly edited data.
         *
         * \return Returns true if the entry was successfully updated.  Returns false if the cell could not be updated.
         */
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) final;

        /**
         * Method used by the view to obtain headers to be displayed.
         *
         * \param[in] section     The section number for the header.
         *
         * \param[in] orientation The orientation of the headers being displayed.
         *
         * \param[in] role        The role for the displayed data.
         *
         * \return Returns the data for the header.
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

        /**
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable columns.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const final;

        /**
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const final;

    signals:
        /**
         * Signal that is emitted when a column value is changed.
         *
         * \param[out] columnIndex     The zero based table column index that was changed.
         *
         * \param[out] nowProportional Value that holds true if the table column is now proportionally sized.  The
         *                             value will hold false if the table is now fixed size.
         *
         * \param[out] newWidth        The new width value for the table in points or fractional remaining space.
         */
        void columnWidthChanged(unsigned columnIndex, bool nowProportional, float newWidth);

    public slots:
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
         * Slot you can trigger to set the currently selected units.
         *
         * \param[in] newUnits The new units to use for fixed width columns.
         */
        void setUnits(Unit newUnits);

    private:
        /**
         * Value indicating the default column type setting.
         */
        static const bool defaultToProportional;

        /**
         * Value indicating the default column width setting for fixed width columns.
         */
        static const float defaultColumnFixedWidth;

        /**
         * Value indicating the default column width setting for proportional width columns.
         */
        static const float defaultColumnProportionalWidth;

        /**
         * Method that performs any re-adjustment of the proportional columns.
         */
        void adjustProportionalColumns();

        /**
         * The displayed units.
         */
        Unit currentUnits;

        /**
         * List of proportional/fixed table column setting values.
         */
        QList<bool> currentProportionalColumn;

        /**
         * List of table column width values.
         */
        QList<float> currentColumnWidths;
};

#endif
