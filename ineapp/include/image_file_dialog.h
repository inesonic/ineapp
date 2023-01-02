/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImageFileDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_FILE_DIALOG_H
#define IMAGE_FILE_DIALOG_H

#include <QString>

#include <eqt_file_dialog.h>

#include "app_common.h"

/**
 * This class extends the EQt::FileDialog class to support loading and saving standard image formats.
 */
class APP_PUBLIC_API ImageFileDialog:public EQt::FileDialog {
    Q_OBJECT

    public:
        /**
         * Enumeration of supported modes.
         */
        enum class DialogMode {
            /**
             * Value that indicates we want to load an image.
             */
            LOAD_IMAGE,

            /**
             * Value that indicates we want to import an image.
             */
            IMPORT_IMAGE,

            /**
             * Value that indicates we want to save an image.
             */
            SAVE_IMAGE,

            /**
             * Value that indicates we want to export an image.
             */
            EXPORT_IMAGE
        };

        /**
         * Constructor
         *
         * \param[in] dialogMode The desired dialog mode.
         *
         * \param[in] parent     The pointer to the parent object.
         */
        ImageFileDialog(DialogMode dialogMode = DialogMode::LOAD_IMAGE, QWidget* parent = Q_NULLPTR);

        ~ImageFileDialog() override;

        /**
         * Method you can use to configure the dialog for file open, save, or export operations.
         *
         * \param[in] dialogMode The desired dialog mode.
         */
        void configureMode(DialogMode dialogMode);

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
