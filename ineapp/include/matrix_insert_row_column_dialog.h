/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixInsertRowColumnDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_INSERT_ROW_COLUMN_DIALOG_H
#define MATRIX_INSERT_ROW_COLUMN_DIALOG_H

#include <QDialog>

#include "app_common.h"

class QWidget;
class QSpinBox;
class QRadioButton;
class QLabel;

namespace Ld {
    class BlockFormat;
    class PageFormat;
};

/**
 * Dialog used to obtain the number of rows or columns to be inserted.
 */
class APP_PUBLIC_API MatrixInsertRowColumnDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Enumeration of row/column orientation values.
         */
        enum class Orientation {
            /**
             * Indicates we're inserting rows.
             */
            ROWS,

            /**
             * Indicates we're inserting columns.
             */
            COLUMNS
        };

        /**
         * Enumeration of insertion locations.
         */
        enum class Location {
            /**
             * Indicates we should insert before the current location.
             */
            BEFORE,

            /**
             * Indicates we should insert after the current location.
             */
            AFTER
        };

        /**
         * Constructor
         *
         * \param[in] orientation The orientation used to configure the dialog.
         *
         * \param[in] parent      Pointer to the parent object.
         */
        MatrixInsertRowColumnDialog(Orientation orientation, QWidget* parent = Q_NULLPTR);

        ~MatrixInsertRowColumnDialog() override;

        /**
         * Method you can use to determine the current orientation.
         *
         * \return Returns the currently selected orientation.
         */
        Orientation orientation() const;

        /**
         * Method you can use to obtain the requested location setting.
         *
         * \return Returns the requested location.
         */
        Location location() const;

        /**
         * Method you can use to obtain the requested count.
         *
         * \return Returns the requested insertion count.
         */
        unsigned count() const;

    public slots:
        /**
         * Slot you can use to set the desired orientation.
         *
         * \param[in] newOrientation The new orientatin.
         */
        void setOrientation(Orientation newOrientation);

        /**
         * Slot you can use to set the desired location.
         *
         * \param[in] newLocation The new location.
         */
        void setLocation(Location newLocation);

        /**
         * Slot you can use to set the desired insertion count.
         *
         * \param[in] newCount The new insertion count.
         */
        void setCount(unsigned newCount);

    private:
        /**
         * Label used to describe the count.
         */
        QLabel* countLabel;

        /**
         * The count spin box.
         */
        QSpinBox* countSpinBox;

        /**
         * The before location radio button.
         */
        QRadioButton* beforeRadioButton;

        /**
         * The after radio button.
         */
        QRadioButton* afterRadioButton;
};

#endif
