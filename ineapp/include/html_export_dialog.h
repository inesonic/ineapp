/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HtmlExportDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HTML_EXPORT_DIALOG_H
#define HTML_EXPORT_DIALOG_H

#include <QString>
#include <QFileDialog>
#include <QStringList>

#include <eqt_programmatic_dialog.h>

#include <ld_html_code_generator.h>

#include "app_common.h"

/**
 * This class provides a programmatic dialog you can use to export HTML content.
 */
class APP_PUBLIC_API HtmlExportDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Type used to indicate the HTML math export mode.
         */
        typedef Ld::HtmlCodeGenerator::MathMode MathMode;

        /**
         * Type used to indicate the HTML style to be generated.
         */
        typedef Ld::HtmlCodeGenerator::HtmlStyle HtmlStyle;

        /**
         * Type used to indicate how images should be handled.
         */
        typedef Ld::HtmlCodeGenerator::ImageHandlingMode ImageHandlingMode;

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent object.
         */
        HtmlExportDialog(QWidget* parent = Q_NULLPTR);

        ~HtmlExportDialog() override;

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
         * Method you can use to obtain the user's selected math mode.
         *
         * \return Returns the selected math mode.
         */
        MathMode mathMode() const;

        /**
         * Method you can use to set the currently displayed math mode.
         *
         * \param[in] newMathMode The new math mode to present to the user.
         */
        void setMathMode(MathMode newMathMode);

        /**
         * Method you can use to determine the user's selected HTML style.
         *
         * \return Returns the selected HTML style.
         */
        HtmlStyle htmlStyle() const;

        /**
         * Method you can use to set the selected HTML style.
         *
         * \param[in] newHtmlStyle The newly selected HTML style.
         */
        void setHtmlStyle(HtmlStyle newHtmlStyle);

        /**
         * Method you can use to determine the currently selected image handling mode.
         *
         * \return Returns the currently selected image handling mode.
         */
        ImageHandlingMode imageHandlingMode() const;

        /**
         * Method you can use to set the currently selected image handling mode.
         *
         * \param[in] newImageHandlingMode The newly selected image handling mode.
         */
        void setImageHandlingMode(ImageHandlingMode newImageHandlingMode);

        /**
         * Method you can use to determine if imports should be included in the generated HTML.
         *
         * \return Returns true if imports should be included.  Returns false if imports should not be included.
         */
        bool includeImports() const;

        /**
         * Method you can use to specify whether imports should be included or excluded.
         *
         * \param[in] nowIncludeImports If true, imports will be marked as included.  If false, imports will not be
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
