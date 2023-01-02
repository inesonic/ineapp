/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ObjectExportDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef OBJECT_EXPORT_DIALOG_H
#define OBJECT_EXPORT_DIALOG_H

#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QRegularExpression>
#include <QList>

#include <eqt_file_dialog.h>

#include "app_common.h"

/**
 * This class extends the EQt::FileDialog class to support saving object files.
 */
class APP_PUBLIC_API ObjectExportDialog:public EQt::FileDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent object.
         */
        ObjectExportDialog(QWidget* parent = Q_NULLPTR);

        ~ObjectExportDialog() override;

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
