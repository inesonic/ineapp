/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixSizeDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_SIZE_DIALOG_H
#define MATRIX_SIZE_DIALOG_H

#include <QDialog>

#include "app_common.h"

class QWidget;
class QSpinBox;

namespace Ld {
    class BlockFormat;
    class PageFormat;
};

/**
 * Dialog used to obtain initial matrix size in rows/columns.
 */
class APP_PUBLIC_API MatrixSizeDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        MatrixSizeDialog(QWidget* parent = Q_NULLPTR);

        ~MatrixSizeDialog() override;

        /**
         * Method you can use to determine the currently selected number of rows.
         *
         * \return Returns the currently selected number of rows.
         */
        unsigned numberRows() const;

        /**
         * Method you can use to determine the currently selected number of columns.
         *
         * \return Returns the currently selected number of columns.
         */
        unsigned numberColumns() const;

    public slots:
        /**
         * Slot you can trigger to specify the initial number of rows.
         *
         * \param[in] newNumberRows The newly selected number of rows.
         */
        void setNumberRows(unsigned newNumberRows);

        /**
         * Slot you can trigger to specify the initial number of columns.
         *
         * \param[in] newNumberColumns The newly selected number of columns.
         */
        void setNumberColumns(unsigned newNumberColumns);

    private:
        /**
         * The rows spin box.
         */
        QSpinBox* rowsSpinBox;

        /**
         * The columns spin box.
         */
        QSpinBox* columnsSpinBox;
};

#endif
