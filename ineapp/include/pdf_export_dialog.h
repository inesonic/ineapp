/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PdfExportDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PDF_EXPORT_DIALOG_H
#define PDF_EXPORT_DIALOG_H

#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QRegularExpression>
#include <QList>

#include <eqt_programmatic_dialog.h>

#include "app_common.h"

/**
 * This class provides a programmatic dialog you can use to export PDF content.
 */
class APP_PUBLIC_API PdfExportDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Enumeration used to indicate the user's desired PDF version.  Values align with
         * QPagedPaintDevice::PdfVersion.
         */
        enum class PdfVersion {
            /**
             * Indicates PDF version 1.4
             */
            VERSION_1_4 = 0,

            /**
             * Indicates PDF version A1b
             */
            VERSION_A1B = 1,

            /**
             * Indicates PDF version 1.6
             */
            VERSION_1_6 = 2,
        };

        /**
         * Enumeration used to indicate the user's desired print range.
         */
        enum class PrintRange {
            /**
             * Indicates an invalid print range.
             */
            INVALID,

            /**
             * Indicates the entire document should be printed.
             */
            ENTIRE_DOCUMENT,

            /**
             * Indicates the user wants to print a single page.
             */
            SINGLE_PAGE,

            /**
             * Indicates a user selected range of pages should be printed.
             */
            PAGE_RANGE,

            /**
             * Indicates only the page containing the cursor should be printed.
             */
            CURRENT_PAGE
        };

        /**
         * Type used to indicate a list of page numbers.
         */
        typedef QList<unsigned> PageList;

        /**
         * Value representing an invalid page number.
         */
        static constexpr unsigned invalidPageNumber = 0;

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent object.
         */
        PdfExportDialog(QWidget* parent = Q_NULLPTR);

        ~PdfExportDialog() override;

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
         * Method you can use to determine the user's selected PDF export file.
         *
         * \return Returns the filepath the the location where the PDF file is to be stored.
         */
        QString selectedFile() const;

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
         * Method you can use to obtain the desired PDF version.
         *
         * \return Returns the user's desired PDF version.
         */
        PdfVersion pdfVersion() const;

        /**
         * Method you can use to set the desired PDF version.
         *
         * \param[in] newPdfVersion The newly selected PDF version.
         */
        void setPdfVersion(PdfVersion newPdfVersion);

        /**
         * Method you can use to obtain the current number of pages.
         *
         * \return Returns the current number of pages.
         */
        unsigned numberPages() const;

        /**
         * Method you can use to set the total number of pages.  The value bounds the page ranges the user is allowed
         * to enter.  This method will also reset the dialog to select the entire print range.
         *
         * \param[in] newNumberPages The new number of pages.
         */
        void setNumberPages(unsigned newNumberPages);

        /**
         * Method you can use to obtain the current page number.
         *
         * \return Returns the current page number as presented to the user.  A value of zero indicates that no current
         *         page number is being reported.
         */
        unsigned currentPageNumber() const;

        /**
         * method you can use to set the current page number.  Value is provided for informational purposes only.
         *
         * \param[in] newCurrentPage The new current page number.  A value of zero indicates that no current page
         *                           number should be displayed.
         */
        void setCurrentPageNumber(unsigned newCurrentPage);

        /**
         * Method you can use to obtain a list of pages to be printed.
         *
         * \return Returns a list of pages to be printed.  An empty list will be returned if the page values are
         *         invalid.
         */
        PageList pageList() const;

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

        /**
         * Slot that is triggered when the print entire range radio button is clicked.
         */
        void printEntireProgramRadioButtonClicked();

        /**
         * Slot that is triggered when the print current page radio button is clicked.
         */
        void printCurrentPageRadioButtonClicked();

        /**
         * Slot that is triggered when the print range page radio button is clicked.
         */
        void printRangeRadioButtonClicked();

        /**
         * Slot that is triggered when the page range is changed by the user.
         *
         * \param[in] newText The new value of the print range.
         */
        void printRangeChanged(const QString& newText);

    private:
        /**
         * Regular expression used to validate the print range entry.
         */
        static const QRegularExpression printRangesRegularExpression;

        /**
         * Regular expression used to parse a single print range.
         */
        static const QRegularExpression printRangeRegularExpression;

        /**
         * Method that calculates page ranges from a provided string.
         *
         * \param[in]     rangeString The string to calculate page ranges from.
         *
         * \param[in,out] ok          An optional pointer to a boolean value that will hold false on error.  You must
         *                            initialize the boolean value to true prior to calling this method.
         *
         * \return Returns the calculated page range.
         */
        PageList calculatePrintRange(const QString& rangeString, bool* ok = Q_NULLPTR) const;

        /**
         * Method that updates the export push button state on value changes.
         */
        void updateExportPushButton();

        /**
         * Method that is called from the constructors to configure this dialog.
         */
        void configureWidget();

        /**
         * Value holding the current number of pages.
         */
        unsigned currentNumberPages;

        /**
         * Value holding the current page number.
         */
        unsigned displayedCurrentPageNumber;
};

#endif
