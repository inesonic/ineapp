/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ToolButtonSizingDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TOOL_BUTTON_SIZING_DIALOG_H
#define TOOL_BUTTON_SIZING_DIALOG_H

#include <QDialog>

#include "app_common.h"

class QWidget;
class QTimer;
class QShowEvent;

/**
 * Small dialog that is used on application launch and screen geometry updates to determine an optimal size for tool
 * buttons.
 */
class APP_PUBLIC_API ToolButtonSizingDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        ToolButtonSizingDialog(QWidget* parent = Q_NULLPTR);

        ~ToolButtonSizingDialog() override;

        /**
         * Method you can call to calculate an optimal size for tool buttons.
         *
         * \return Returns the recommended size for the tool buttons.
         */
        unsigned determineRecommendedButtonSize();

    protected:
        /**
         * Method that is called when the dialog is displayed.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void showEvent(QShowEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the dialog should be closed.
         */
        void closeDialog();

    private:
        /**
         * Timer used to automatically close this dialog when displayed.
         */
        QTimer* closeTimer;

        /**
         * The recommended tool button size, in pixels.
         */
        unsigned recommendedButtonSize;
};

#endif
