/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_VIEW_PROXY_H
#define MATRIX_VIEW_PROXY_H

#include <QObject>

#include <ld_element_structures.h>

#include "page_list.h"
#include "update_format_command.h"
#include "view_widget.h"
#include "math_view_proxy_base.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class InsertElementCommand;

/**
 * View proxy used to manage functions related to matrices.
 */
class MatrixViewProxy:public MathViewProxyBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        MatrixViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~MatrixViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

    private slots:
        /**
         * Slot that is triggered when the matrix operator button is clicked.
         */
        void matrixOperatorButtonClicked();

        /**
         * Slot that is triggered when the matrix combine left-to-right button is clicked.
         */
        void matrixCombineLeftToRightButtonClicked();

        /**
         * Slot that is triggered when the matrix combine top-to-bottom button is clicked.
         */
        void matrixCombineTopToBottomButtonClicked();

        /**
         * Slot that is triggered when the Hadamard product operator button is clicked.
         */
        void hadamardProductOperatorButtonClicked();

        /**
         * Slot that is triggered when the Kronecker product operator button is clicked.
         */
        void kroneckerProductOperatorButtonClicked();

        /**
         * Slot that is triggered when the matrix transpose operator button is clicked.
         */
        void matrixTransposeOperatorButtonClicked();

        /**
         * Slot that is triggered when the matrix conjugate transpose operator button is clicked.
         */
        void matrixConjugateTransposeOperatorButtonClicked();
};

#endif
