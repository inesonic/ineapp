/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DocumentFileDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DOCUMENT_FILE_DIALOG_H
#define DOCUMENT_FILE_DIALOG_H

#include <QString>

#include <eqt_file_dialog.h>

#include "app_common.h"

/**
 * This class extends the EQt::FileDialog class to support loading and saving Document instances.
 */
class APP_PUBLIC_API DocumentFileDialog:public EQt::FileDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] savingFile If true, the dialog will be configured for file save operations.  If false, the
         *                       dialog will be configured for file load operations.
         *
         * \param[in] parent     The pointer to the parent object.
         */
        DocumentFileDialog(bool savingFile = false, QWidget* parent = Q_NULLPTR);

        ~DocumentFileDialog() override;

        /**
         * Method you can use to configure the dialog for file open operations.
         *
         * \param[in] nowFileOpen If true, the dialog will be configured for file open operations.  If false, the
         *                        dialog will be configured for file save operations.
         */
        void configureForFileOpen(bool nowFileOpen = true);

        /**
         * Method you can use to configure the dialog for file save operations.
         *
         * \param[in] nowFileSave If true, the dialog will be configured for file save operations.  If false, the
         *                        dialog will be configured for file opene operations.
         */
        void configureForFileSave(bool nowFileSave = true);

    protected:
        /**
         * Method that returns the group name to use when loading and saving dialog state information.
         *
         * \return Returns the dialog group name.
         */
        QString settingsGroupName() const final;

        /**
         * Method that returns a QStringList of filters that the user can apply to the dialog.
         *
         * \return Returns a QStringList holding the supported filters.
         */
        QStringList filterList() const final;
};

#endif
