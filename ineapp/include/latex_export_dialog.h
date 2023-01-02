/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LaTeXExportDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LATEX_EXPORT_DIALOG_H
#define LATEX_EXPORT_DIALOG_H

#include <QString>
#include <QFileDialog>
#include <QStringList>

#include <eqt_programmatic_dialog.h>

#include <ld_latex_code_generator.h>

#include "app_common.h"

/**
 * This class provides a programmatic dialog you can use to export LaTeX content.
 */
class APP_PUBLIC_API LaTeXExportDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Type used to indicate the LaTeX image export mode.
         */
        typedef Ld::LaTeXCodeGenerator::ImageMode ImageMode;

        /**
         * Type used to indicate how Unicode should be handled.
         */
        typedef Ld::LaTeXCodeGenerator::UnicodeMode UnicodeMode;

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent object.
         */
        LaTeXExportDialog(QWidget* parent = Q_NULLPTR);

        ~LaTeXExportDialog() override;

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void loadSettings(const QString& groupName) final;

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void saveSettings(const QString& groupName) final;

        /**
         * Method you can use to obtain the current user selected directory name.
         *
         * \return Returns the current user selected directory.
         */
        QString selectedDirectory() const;

        /**
         * Method you can use to determine the user's starting directory name.
         *
         * \return Returns the path to the user's selected directory.
         */
        QString directory() const;

        /**
         * Method you can use to set the user's starting directory.
         *
         * \param[in] newDirectory The new user's selected directory.
         */
        void setDirectory(const QString& newDirectory);

        /**
         * Method you can use to obtain the user's selected image export mode.
         *
         * \return Returns the selected image export mode.
         */
        ImageMode imageMode() const;

        /**
         * Method you can use to set the currently displayed image export mode.
         *
         * \param[in] newImageMode The new image export mode to present to the user.
         */
        void setImageMode(ImageMode newImageMode);

        /**
         * Method you can use to obtain the user's selected output format style.
         *
         * \return Returns true if the user has indicated that he/she wants output as a single file.  Returns false if
         *         the user wants the output broken up into multiple files.
         */
        bool singleFile() const;

        /**
         * Method you can use to obtain the user's selected output format style.
         *
         * \return Returns true if the user has indicated that he/she wants output broken up into multiple files.
         *         Returns true if the user wants the output presented in a single file.
         */
        bool multipleFiles() const;

        /**
         * Method you can use to set the user's selected output format style.
         *
         * \param[in] nowSingleFile If true, the selection will indicate output as a single file.  If false, the
         *                          selection will indicate output as multiple files.
         */
        void setSingleFile(bool nowSingleFile = true);

        /**
         * Method you can use to set the user's selected output format style.
         *
         * \param[in] nowMultipleFiles If true, the selection will indicate output in multiple files.  If false, the
         *                             output will be placed in a single file.
         */
        void setMultipleFiles(bool nowMultipleFiles = true);

        /**
         * Method you can use to obtain the user's desired Unicode translation mode.
         *
         * \return Returns the user's Unicode translation mode.
         */
        UnicodeMode unicodeTranslationMode() const;

        /**
         * Method you can use to set the selected Unicode translation mode.
         *
         * \param[in] newUnicodeTranslationMode The newly selected Unicode translation mode.
         */
        void setUnicodeTranslationMode(UnicodeMode newUnicodeTranslationMode);

        /**
         * Method you can use to determine if Imports should be included in the generated LaTeX.
         *
         * \return Returns true if Imports should be included.  Returns false if Imports should not be included.
         */
        bool includeImports() const;

        /**
         * Method you can use to specify whether Imports should be included or excluded.
         *
         * \param[in] nowIncludeImports If true, Imports will be marked as included.  If false, Imports will not be
         *                              marked as included.
         */
        void setIncludeImports(bool nowIncludeImports);

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

        /**
         * Method that is called when the user closes the dialog. This method's behavior is platform specific.  On
         * Windows and Linux, the method will call the base class function.  On MacOS, this method will either accept
         * or reject the close event and set the result code to QDialog::Accepted if the window can be closed.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void closeEvent(QCloseEvent* event) final;

    private slots:
        /**
         * Slot that is triggered when one or more files is selected.
         *
         * \param[in] fileList The list of selected files.
         */
        void filesSelected(const QStringList& fileList);

        /**
         * Slot that is triggered when the user enters a new filename in the file name line editor.
         *
         * \param[in] fileName The newly entered filename.
         */
        void fileNameEdited(const QString& fileName);

    private:
        /**
         * Method that is called from the constructors to configure this dialog.
         */
        void configureWidget();
};

#endif
