/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TupleInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TUPLE_INSPECTOR_WIDGET_H
#define TUPLE_INSPECTOR_WIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QToolButton>
#include <QPoint>

#include <ld_calculated_value.h>

#include "app_common.h"
#include "inspector_widget.h"

namespace Ld {
    class CalculatedValue;
};

/**
 * Inspector widget for tuples.
 */
class APP_PUBLIC_API TupleInspectorWidget:public InspectorWidget {
    Q_OBJECT

    public:
        /**
         * Factory function
         *
         * \param[in] parent The parent widget.
         */
        static InspectorWidget* creator(QWidget* parent);

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent widget.
         */
        TupleInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~TupleInspectorWidget() override;

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

    private slots:
        /**
         * Slot that is triggered when the selection changes.
         *
         * \param[in] selected   The new list of selected items.
         *
         * \param[in] deselected The new list of deselected items.
         */
        void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        /**
         * Slot that is triggered when an item is double clicked.
         *
         * \param[in] index The index of the item that was double clicked.
         */
        void itemDoubleClicked(const QModelIndex& index);

        /**
         * Slot that is triggered when the custom context menu is requested.
         *
         * \param[in] position The desired position for the custom context menu.
         */
        void customContextMenuRequested(const QPoint& position);

        /**
         * Slot that is triggered when the user requests that an item be moved up.
         */
        void moveItemsUp();

        /**
         * Slot that is triggered when the user requests that an item be moved down.
         */
        void moveItemsDown();

        /**
         * Slot that is triggered when the user wants to inspect or modify the current item.
         */
        void inspectModifyItem();

        /**
         * Slot that is triggered when the user wants to remove item(s) from the tuple.
         */
        void deleteItems();

        /**
         * Slot that is triggered when the user requests a new boolean value be inserted.
         */
        void insertBoolean();

        /**
         * Slot that is triggered when the user requests a new integer value be inserted.
         */
        void insertInteger();

        /**
         * Slot that is triggered when the user requests a new real value be inserted.
         */
        void insertReal();

        /**
         * Slot that is triggered when the user requests a new complex value be inserted.
         */
        void insertComplex();

        /**
         * Slot that is triggered when the user requests a new boolean matrix be inserted.
         */
        void insertMatrixBoolean();

        /**
         * Slot that is triggered when the user requests a new integer matrix be inserted.
         */
        void insertMatrixInteger();

        /**
         * Slot that is triggered when the user requests a new real matrix be inserted.
         */
        void insertMatrixReal();

        /**
         * Slot that is triggered when the user requests a new complex matrix be inserted.
         */
        void insertMatrixComplex();

        /**
         * Slot that is triggered when the user requests a new set be inserted.
         */
        void insertSet();

        /**
         * Slot that is triggered when the user requests a new tuple value be inserted.
         */
        void insertTuple();

    private:
        /**
         * Method that modified or inserts a new value.
         *
         * \param[in] currentValue The initial value.
         *
         * \param[in] currentRow   The row containing the item being modified.
         */
        void modify(const Model::Variant& currentValue, unsigned long currentRow);

        /**
         * Method that inserts a new value above the current location.
         *
         * \param[in] initialValue The initial value to modify for insertion.
         */
        void insert(const Model::Variant& initialValue);

        /**
         * The list view used to display the tuple.
         */
        QTableView* currentTableView;

        /**
         * The move up button.
         */
        QPushButton* moveUpButton;

        /**
         * The move up button.
         */
        QPushButton* moveDownButton;

        /**
         * The list entry delete button.
         */
        QPushButton* deleteButton;

        /**
         * The list add push button.
         */
        QPushButton* insertButton;
};

#endif
