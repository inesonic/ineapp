/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RuntimeLibraryFileDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RUNTIME_LIBRARY_FILE_DIALOG_H
#define RUNTIME_LIBRARY_FILE_DIALOG_H

#include <QString>

#include <eqt_file_dialog.h>

#include "app_common.h"

/**
 * This class extends the EQt::FileDialog class to support selecting runtime libraries.
 */
class APP_PUBLIC_API RuntimeLibraryFileDialog:public EQt::FileDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent object.
         */
        RuntimeLibraryFileDialog(QWidget* parent = Q_NULLPTR);

        ~RuntimeLibraryFileDialog() override;

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
