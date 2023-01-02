/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_INSPECTOR_WIDGET_H
#define MATRIX_INSPECTOR_WIDGET_H

#include <QWidget>
#include <QPoint>
#include <QMap>
#include <QList>
#include <QFont>
#include <QComboBox>
#include <QAbstractTableModel>
#include <QAbstractItemDelegate>
#include <QItemSelectionModel>
#include <QTableView>

#include <ld_calculated_value.h>

#include "app_common.h"
#include "inspector_widget.h"

namespace Ld {
    class CalculatedValue;
};

class MatrixInspectorModel;

/**
 * Inspector widget base class for matrix variables.
 */
class APP_PUBLIC_API MatrixInspectorWidget:public InspectorWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent widget.
         */
        MatrixInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~MatrixInspectorWidget() override;

        /**
         * Method you can call to obtain the currently displayed calculated value.
         *
         * \return Returns the currently displayed calculated value.
         */
        Ld::CalculatedValue calculatedValue() const override;

        /**
         * method you can call to determine if the currently displayed calculated value is modified.
         *
         * \return Returns true if the currently displayed calculated value is different from the original calculated
         *         value.  Returns false if the calculated value matches the original calculated value.
         */
        bool calculatedValueChanged() const override;

        /**
         * Method that determines if load/save buttons should be displayed.
         *
         * \return Returns true if the load/save buttons should be included.  Returns false if the load/save buttons
         *         should be excluded.  This version queries the mode.
         */
        bool includeLoadSaveSupport() const override;

    public slots:
        /**
         * Pure virtual slot you should overload to receive the calculated value to be displayed by this widget.
         *
         * \param[in] calculatedValue The calculated value to display in this widget.
         */
        void setCalculatedValue(const Ld::CalculatedValue& calculatedValue) override;

        /**
         * Pure virtual slot you should overload to receive notification that the calculated value should be reset.
         */
        void resetCalculatedValue() override;

    protected:
        /**
         * Method that is called to generate the model behind the table view.
         *
         * \param[in] calculatedValue The calculated value.
         *
         * \return Returns a model suitable for use.
         */
        virtual MatrixInspectorModel* createModel(const Ld::CalculatedValue& calculatedValue) = 0;

        /**
         * Method that is called to generate a custom delegate for the table view.  The default implementation returns
         * a null pointer.
         *
         * \return Returns the item delegate.  Returns a null pointer if the default delegate should be used.
         */
        virtual QAbstractItemDelegate* createDelegate();

        /**
         * Method that is called to generate a custom selection model for the table view.  The default implementation
         * returns a null pointer.
         *
         * \param[in] tableView The table view the selection model is interacting with.
         *
         * \param[in] model     The model holding the table data.
         *
         * \return Returns the custom selection model.  Returns a null pointer if the default selection model should
         *         be used.
         */
        virtual QItemSelectionModel* createSelectionModel(QTableView* tableView, MatrixInspectorModel* model);

        /**
         * Method that calculates a recommended/initial column size.
         *
         * \return Returns the recommended column size for each column.  A value of 0 will cause the horizontal
         *         headers to auto-resize.
         */
        virtual unsigned initialColumnWidth() const = 0;

        /**
         * Method that calculates a recommended/initial row size.
         *
         * \return Returns the recommended column size for each column.  A value of 0 will cause the vertical headers
         *         to auto-resize.  The default implementation returns 1.25 times the current font height.
         */
        virtual unsigned initialRowHeight() const;

        /**
         * Method that is called to load the variable contents from a file.
         *
         * \param[in] filename     The filename of the file holding the contents to be loaded.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        bool loadFromFile(const QString& filename, bool binaryFormat) override;

        /**
         * Method that is called to save the variable contents to a file.
         *
         * \param[in] filename     The filename of the file to save the contents to.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        bool saveToFile(const QString& filename, bool binaryFormat) override;

        /**
         * Method you can use to obtain the current table view font.
         *
         * \return Returns the current table view font.
         */
        QFont tableFont() const;

    private slots:
        /**
         * Slot that is triggered when the user requests a pop-up menu.
         *
         * \param[in] position The position where the popup menu has been requested.
         */
        void customContextMenuRequested(const QPoint& position);

        /**
         * Slot that is triggered when the user requests the columns be resized to the contents.
         */
        void autoAdjustColumns();

        /**
         * Slot that is triggered when the user requests a new row be inserted before the current position.
         */
        void insertRowsBefore();

        /**
         * Slot that is triggered when the user requests a new row be inserted after the current position.
         */
        void insertRowsAfter();

        /**
         * Slot that is triggered when the user requests a new column be inserted before the current position.
         */
        void insertColumnsBefore();

        /**
         * Slot that is triggered when the user requests a new column be inserted after the current position.
         */
        void insertColumnsAfter();

        /**
         * Slot that is triggered when the user requests a row be removeed.
         */
        void removeRow();

        /**
         * Slot that is triggered when the user requests a column be removeed.
         */
        void removeColumn();

    private:
        /**
         * Method that generates an ordered list of selections by row.
         *
         * \param[in] selectionModel The selection model.
         *
         * \return Returns the map of selections.  The keys representing the starting rows.  The value represents the
         *         lengths.
         */
        static QMap<unsigned long, unsigned long> rowRanges(const QItemSelectionModel* selectionModel);

        /**
         * Method that generates an ordered list of selections by column.
         *
         * \param[in] selectionModel The selection model.
         *
         * \return Returns the map of selections.  The keys representing the starting columns.  The value represents
         *         the lengths.
         */
        static QMap<unsigned long, unsigned long> columnRanges(const QItemSelectionModel* selectionModel);

        /**
         * The table view.
         */
        QTableView* currentTableView;
};

#endif
