/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LibraryExportDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LIBRARY_EXPORT_DIALOG_H
#define LIBRARY_EXPORT_DIALOG_H

#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QRegularExpression>
#include <QList>

#if (defined(Q_OS_WIN))

    #include <eqt_programmatic_dialog.h>

#elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

    #include <eqt_file_dialog.h>

#else

    #error Unknown platform

#endif

#include "app_common.h"

#if (defined(Q_OS_WIN))

    /**
     * This class provides a file open dialog used to create dynamic libraries.  This version is specific to Windows
     * and provides the ability to set an optional import library.
     */
    class APP_PUBLIC_API LibraryExportDialog:public EQt::ProgrammaticDialog {
        Q_OBJECT

        public:
            /**
             * Constructor
             *
             * \param[in] parent The pointer to the parent object.
             */
            LibraryExportDialog(QWidget* parent = Q_NULLPTR);

            ~LibraryExportDialog() override;

            /**
             * Method you can use to obtain a list of selected files.
             *
             * \return Returns a list of all the selected files.
             */
            QStringList selectedFiles() const;

            /**
             * Method you can use to obtain the desired MSVCRT import library.
             *
             * \return Returns the path to the desired MSVCRT import library.  An empty string is returned if the user
             *         selects the default import library.
             */
            QString windowsImportLibrary() const;

            /**
             * Loads settings to be used by the file dialog.  If no settings exist, the method will set the current
             * directory to the user's home directory.
             *
             * \param[in] groupName The group name.
             */
            void loadSettings(const QString& groupName) override;

            /**
             * Saves the file dialog settings.
             *
             * \param[in] groupName The group name.
             */
            void saveSettings(const QString& groupName) override;

            /**
             * Method that can be called to populate the dialog's widgets.
             */
            void populate() final;

        protected:
            /**
             * Performs binding of signals to slots.  This method will be executed by the
             * \ref ProgrammaticDialog::runBuilders method.
             *
             * You can overload this method if you want to bind signals to slots prior to the proxy classes.
             */
            void bind() final;

        private slots:
            /**
             * Slot that is triggered when the user clicks the "Add" button.
             */
            void addClicked();

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
            void fileNameEdited(const QString& filenName);

            /**
             * Slot that is triggered when a new runtime is selected.
             *
             * \param[in] newRuntime The newly selected runtime.
             */
            void runtimeChanged(const QString& newRuntime);

        private:
            /**
             * Method that is called to check if the current settings are valid.
             *
             * \param[in] exportFilename The name of the DLL to be exported.
             *
             * \param[in] runtimeLibrary The name of the runtime library to be used.
             */
            void checkIfValid(const QString& exportFilename, const QString& runtimeLibrary);

            /**
             * Method that configures this dialog.
             */
            void configureWidget();

            /**
             * Method that locates all available import libraries on the current system.
             */
            void locateImportLibraries();

            /**
             * Method that scans a directory for one of a number of files.  The scan is case insensitive.
             *
             * \param[in] directory The directory to be scanned.
             *
             * \param[in] fileList  The list of filenames to be located.
             *
             * \return Returns the list of found files.
             */
            static QStringList locateFiles(const QString& directory, const QStringList& fileList);
    };

#else

    /**
     * This class extends the EQt::FileDialog class to support saving libraries.  This version is for use on MacOS and
     * Linux.
     */
    class APP_PUBLIC_API LibraryExportDialog:public EQt::FileDialog {
        Q_OBJECT

        public:
            /**
             * Constructor
             *
             * \param[in] parent The pointer to the parent object.
             */
            LibraryExportDialog(QWidget* parent = Q_NULLPTR);

            ~LibraryExportDialog() override;

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

#endif
