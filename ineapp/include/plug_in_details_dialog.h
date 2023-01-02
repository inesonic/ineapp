/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlugInDetailsDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLUG_IN_DETAILS_DIALOG_H
#define PLUG_IN_DETAILS_DIALOG_H

#include <eqt_programmatic_dialog.h>

#include <ld_plug_in_information.h>

#include "app_common.h"

class QWidget;
class QTableWidget;

/**
 * Dialog used to display detailed information about a plug-in.
 */
class APP_PUBLIC_API PlugInDetailsDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlugInDetailsDialog(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] newPlugInInformation The new plug-in information to be displayed.
         *
         * \param[in] parent               Pointer to the parent object.
         */
        PlugInDetailsDialog(const Ld::PlugInInformation& newPlugInInformation, QWidget* parent = Q_NULLPTR);

        ~PlugInDetailsDialog() override;

        /**
         * Method you can use to obtain the current plug in information.
         *
         * \return Returns the currently displayed plug-in information.
         */
        Ld::PlugInInformation plugInInformation() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    public slots:
        /**
         * Slot you can trigger to update the current plug-in information.
         *
         * \param[in] newPlugInInformation The new plug-in information to be displayed.
         */
        void setPlugInInformation(const Ld::PlugInInformation& newPlugInInformation);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * The plug in information to be displayed.
         */
        Ld::PlugInInformation currentPlugInInformation;
};

#endif
