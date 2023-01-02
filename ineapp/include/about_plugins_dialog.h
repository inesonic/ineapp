/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref AboutPlugInsDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ABOUT_PLUGINS_DIALOG_H
#define ABOUT_PLUGINS_DIALOG_H

#include <eqt_programmatic_dialog.h>

#include "app_common.h"

class QWidget;
class QModelIndex;

/**
 * Dialog used to display information about this application.
 */
class APP_PUBLIC_API AboutPlugInsDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        AboutPlugInsDialog(QWidget* parent = Q_NULLPTR);

        ~AboutPlugInsDialog() override;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when a table item is clicked.
         *
         * \param[in] index The model index of the selected item.
         */
        void itemClicked(const QModelIndex& index);

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();
};

#endif
