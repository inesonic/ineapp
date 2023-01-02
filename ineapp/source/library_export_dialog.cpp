/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LibraryExportDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QString>

#if (defined(Q_OS_WIN))

    #include <QVariant>
    #include <QFileInfo>
    #include <QProcess>
    #include <QVBoxLayout>
    #include <QGroupBox>
    #include <QComboBox>
    #include <QPushButton>
    #include <QDialogButtonBox>

    #include <eqt_programmatic_dialog.h>
    #include <eqt_file_selector_widget.h>

    #include "runtime_library_file_dialog.h"

#elif (!defined(Q_OS_LINUX) && !defined(Q_OS_DARWIN))

    #error Unknown platform

#endif

#include <eqt_file_dialog.h>

#include "application.h"
#include "library_export_dialog.h"

/***********************************************************************************************************************
 * Windows version:
 */

#if (defined(Q_OS_WIN))
    LibraryExportDialog::LibraryExportDialog(QWidget* parent):EQt::ProgrammaticDialog("LibraryExportDialog", parent) {
        configureWidget();
        runBuilders();
    }


    LibraryExportDialog::~LibraryExportDialog() {}


    QStringList LibraryExportDialog::selectedFiles() const {
        EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
        return fileSelectorWidget->selectedFiles();
    }


    QString LibraryExportDialog::windowsImportLibrary() const {
        QString    result;

        QComboBox* importLibraryComboBox = widget<QComboBox>("import_library_combo_box");
        QString    importLibrary = importLibraryComboBox->currentText();
        if (importLibrary != tr("<Inesonic>")) {
            QFileInfo importLibraryFileInformation(importLibrary);
            if (importLibraryFileInformation.exists() && importLibraryFileInformation.isFile()) {
                result = importLibrary;
            }
        }

        return result;
    }


    void LibraryExportDialog::loadSettings(const QString& groupName) {
        QSettings* settings = Application::settings();

        settings->beginGroup(groupName);

        EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
        fileSelectorWidget->loadSettings(settings);

        QComboBox* importLibraryComboBox = widget<QComboBox>("import_library_combo_box");
        bool     ok;
        unsigned numberKnownImportLibraries = settings->value("number_import_libraries").toUInt(&ok);
        if (ok) {
            for (unsigned i=0 ; i<numberKnownImportLibraries ; ++i) {
                QString importLibraryPath = settings->value(QString("import_library_%1").arg(i + 1)).toString();

                if (!importLibraryPath.isEmpty()) {
                    QFileInfo importLibraryFileInformation(importLibraryPath);
                    if (importLibraryFileInformation.exists() && importLibraryFileInformation.isFile()) {
                        importLibraryComboBox->addItem(importLibraryPath, QVariant());
                    }
                }
            }
        }

        QString importLibrary = settings->value("import_library").toString();
        if (importLibrary.isEmpty()) {
            importLibraryComboBox->setCurrentText(QString("<Inesonic>"));
        } else {
            QFileInfo importLibraryInformation(importLibrary);
            if (!importLibraryInformation.exists() || !importLibraryInformation.isFile()) {
                importLibraryComboBox->setCurrentText(importLibrary);
            } else {
                importLibraryComboBox->setCurrentText(QString("<Inesonic>"));
            }
        }

        settings->endGroup();
    }


    void LibraryExportDialog::saveSettings(const QString& groupName) {
        QSettings* settings = Application::settings();

        settings->beginGroup(groupName);

        EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
        fileSelectorWidget->saveSettings(settings);

        QComboBox* importLibraryComboBox = widget<QComboBox>("import_library_combo_box");
        unsigned   numberImportLibraries = static_cast<unsigned>(importLibraryComboBox->count());

        unsigned numberCustomImportLibraries = 0;
        for (unsigned i=0 ; i<numberImportLibraries ; ++i) {
            QString importLibraryPath = importLibraryComboBox->itemText(i);
            if (importLibraryPath != tr("<Inesonic>")) {
                QFileInfo importLibraryFileInformation(importLibraryPath);
                if (importLibraryFileInformation.exists() && importLibraryFileInformation.isFile()) {
                    bool automaticPath = importLibraryComboBox->itemData(i).toBool();
                    if (!automaticPath) {
                        ++numberCustomImportLibraries;
                        settings->setValue(
                            QString("import_library_%1").arg(numberCustomImportLibraries),
                            importLibraryPath
                        );
                    }
                }
            }
        }

        settings->setValue("number_import_libraries", numberCustomImportLibraries);

        QString importLibrary = importLibraryComboBox->currentText();
        if (importLibrary == tr("<Inesonic>")) {
            settings->setValue("import_library", QString());
        } else {
            QFileInfo importLibraryFileInformation(importLibrary);
            if (importLibraryFileInformation.exists() && importLibraryFileInformation.isFile()) {
                settings->setValue("import_library", importLibrary);
            } else {
                settings->setValue("import_library", QString());
            }
        }

        settings->endGroup();
    }


    void LibraryExportDialog::populate() {
        locateImportLibraries();
    }


    void LibraryExportDialog::bind() {
        EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
        connect(
            fileSelectorWidget,
            &EQt::FileSelectorWidget::filesSelected,
            this,
            &LibraryExportDialog::filesSelected
        );
        connect(
            fileSelectorWidget,
            &EQt::FileSelectorWidget::fileNameTextChanged,
            this,
            &LibraryExportDialog::fileNameEdited
        );

        QComboBox* importLibraryComboBox = widget<QComboBox>("import_library_combo_box");
        connect(
            importLibraryComboBox,
            static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged),
            this,
            &LibraryExportDialog::runtimeChanged
        );

        QPushButton* importLibraryAddPushButton = widget<QPushButton>("import_library_add_push_button");
        connect(importLibraryAddPushButton, &QPushButton::clicked, this, &LibraryExportDialog::addClicked);

        QPushButton* exportPushButton = widget<QPushButton>("export_push_button");
        connect(exportPushButton, &QPushButton::clicked, this, &LibraryExportDialog::accept);

        QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");
        connect(cancelPushButton, &QPushButton::clicked, this, &LibraryExportDialog::reject);
    }


    void LibraryExportDialog::addClicked() {
        RuntimeLibraryFileDialog runtimeLibraryFileDialog(this);
        runtimeLibraryFileDialog.loadSettings();

        int result = runtimeLibraryFileDialog.exec();
        if (result == RuntimeLibraryFileDialog::DialogCode::Accepted) {
            runtimeLibraryFileDialog.saveSettings();

            QString selectedFile = runtimeLibraryFileDialog.selectedFiles().first();

            QComboBox* importLibraryComboBox = widget<QComboBox>("import_library_combo_box");
            importLibraryComboBox->addItem(selectedFile, QVariant(false));
        }
    }


    void LibraryExportDialog::filesSelected(const QStringList& fileList) {
        if (fileList.isEmpty()) {
            fileNameEdited(QString());
        } else {
            fileNameEdited(fileList.at(0));
        }
    }


    void LibraryExportDialog::fileNameEdited(const QString& fileName) {
        checkIfValid(fileName, windowsImportLibrary());
    }


    void LibraryExportDialog::runtimeChanged(const QString& newRuntime) {
        QStringList selectedFiles = LibraryExportDialog::selectedFiles();
        QString     selectedFile = selectedFiles.isEmpty() ? QString() : selectedFiles.first();

        checkIfValid(selectedFile, newRuntime);
    }


    void LibraryExportDialog::configureWidget() {
        #if (defined(Q_OS_WIN))

            setWindowTitle(tr("Export DLL"));

        #elif (defined(Q_OS_LINUX))

            setWindowTitle(tr("Export Shared Library"));

        #elif (defined(Q_OS_DARWIN))

            setWindowTitle(tr("Export Dynamic Library"));

        #else

            #error Unknown platform

        #endif

        setWindowTitle(tr("Export Dynamic Library"));
        setSizeGripEnabled(true);

        QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
        setLayout(mainLayout);

        EQt::FileSelectorWidget* fileSelector = new EQt::FileSelectorWidget(
            EQt::FileSelectorWidget::AnyFile,
            EQt::FileSelectorWidget::AcceptSave
        );
        fileSelector->setLineEditorAutoUpdateEnabled();
        mainLayout->addWidget(fileSelector, 1);
        registerWidget(fileSelector, "file_selector_widget");

        QGroupBox* importLibraryGroupBox = new QGroupBox(tr("C Runtime Import Library"));
        mainLayout->addWidget(importLibraryGroupBox);
        registerWidget(importLibraryGroupBox, "import_library_group_box");

        QHBoxLayout* importGroupBoxLayout = newHBoxLayout("import_group_box_horizontal_layout");
        importLibraryGroupBox->setLayout(importGroupBoxLayout);

        QComboBox* importLibraryComboBox = new QComboBox;
        importLibraryComboBox->setEditable(true);
        importLibraryComboBox->setDuplicatesEnabled(false);
        importLibraryComboBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAlphabetically);
        importLibraryComboBox->addItem("<Inesonic>");
        importGroupBoxLayout->addWidget(importLibraryComboBox, 1);
        registerWidget(importLibraryComboBox, "import_library_combo_box");

        QPushButton* importLibraryAddPushButton = new QPushButton(tr("Add"));
        importGroupBoxLayout->addWidget(importLibraryAddPushButton);
        registerWidget(importLibraryAddPushButton, "import_library_add_push_button");

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        mainLayout->addWidget(buttonBox);
        registerWidget(buttonBox, "button_box");

        QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
        exportButton->setText(tr("Export"));
        registerWidget(exportButton, "export_push_button");

        registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

        exportButton->setEnabled(false);
    }


    void LibraryExportDialog::locateImportLibraries() {
        QComboBox* importLibraryComboBox = widget<QComboBox>("import_library_combo_box");

        QString applicationPath = Application::applicationDirPath();
        QString vswhereExecutable = applicationPath + "/vswhere.exe";

        if (!QFileInfo(vswhereExecutable).exists()) {
            vswhereExecutable = "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe";
        }

        QStringList visualStudioInstances;
        if (QFileInfo(vswhereExecutable).exists()) {
            QProcess vswhereProcess;
            vswhereProcess.start(vswhereExecutable, QStringList() << "-all" << "-property" << "installationPath");
            if (vswhereProcess.waitForStarted()  &&
                vswhereProcess.waitForFinished() &&
                vswhereProcess.exitCode() == 0      ) {
                QString vswhereOutput = QString::fromLocal8Bit(vswhereProcess.readAllStandardOutput());
                visualStudioInstances = vswhereOutput.split(QString('\n'));
            }
        }

        for (  QStringList::const_iterator visualStudioInstanceIterator    = visualStudioInstances.constBegin(),
                                           visualStudioInstanceEndIterator = visualStudioInstances.constEnd()
             ; visualStudioInstanceIterator != visualStudioInstanceEndIterator
             ; ++visualStudioInstanceIterator
            ) {
            QString visualStudioInstance = visualStudioInstanceIterator->trimmed();
            if (!visualStudioInstance.isEmpty()) {
                QStringList importLibraries = locateFiles(
                    visualStudioInstance,
                    QStringList() << "msvcrt.lib" << "msvcrtd.lib"
                );

                for (  QStringList::const_iterator importLibraryIterator    = importLibraries.constBegin(),
                                                   importLibraryEndIterator = importLibraries.constEnd()
                     ; importLibraryIterator != importLibraryEndIterator
                     ; ++importLibraryIterator
                    ) {
                    const QString& importLibrary = *importLibraryIterator;
                    importLibraryComboBox->addItem(importLibrary, QVariant(true));
                }
            }
        }
    }


    void LibraryExportDialog::checkIfValid(const QString& exportFilename, const QString& runtimeLibrary) {
        bool         enableExportButton = false;
        QPushButton* exportPushButton   = widget<QPushButton>("export_push_button");

        if (!exportFilename.isEmpty()) {
            EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
            QDir                     currentDirectory(fileSelectorWidget->directory());

            if (currentDirectory.exists() && currentDirectory.isReadable()) {
                QFileInfo exportFileInformation(exportFilename);
                enableExportButton = !exportFileInformation.exists() || exportFileInformation.isWritable();
            }
        }

        if (enableExportButton) {
            if (!runtimeLibrary.isEmpty() && runtimeLibrary != tr("<Inesonic>")) {
                QFileInfo runtimeLibraryInformation(runtimeLibrary);
                enableExportButton = (
                       runtimeLibraryInformation.exists()
                    && runtimeLibraryInformation.isReadable()
                    && runtimeLibraryInformation.completeSuffix().toLower() == tr("lib")
                );
            }
        }

        exportPushButton->setEnabled(enableExportButton);
    }


    QStringList LibraryExportDialog::locateFiles(const QString& directory, const QStringList& fileList) {
        QStringList   result;
        QDir          dir(directory);
        QFileInfoList directoryEntries = dir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
        for (  QFileInfoList::const_iterator directoryIterator    = directoryEntries.constBegin(),
                                             directoryEndIterator = directoryEntries.constEnd()
             ; directoryIterator != directoryEndIterator
             ; ++directoryIterator
            ) {
            QString subdirectory = directoryIterator->canonicalFilePath();
            result.append(locateFiles(subdirectory, fileList));
        }

        if (directory.contains("/VC/", Qt::CaseSensitivity::CaseInsensitive) &&
            directory.contains("/x64", Qt::CaseSensitivity::CaseInsensitive)    ) {
            QFileInfoList fileEntries = dir.entryInfoList(fileList, QDir::Filter::Files | QDir::Filter::NoDotAndDotDot);
            for (  QFileInfoList::const_iterator filenameIterator = fileEntries.constBegin(),
                                                 filenameEndIterator = fileEntries.constEnd()
                 ; filenameIterator != filenameEndIterator
                 ; ++filenameIterator
                ) {
                const QFileInfo& fileInformation = *filenameIterator;
                if (fileInformation.isReadable()) {
                    result.append(fileInformation.canonicalFilePath());
                }
            }
        }

        return result;
    }

#else

    LibraryExportDialog::LibraryExportDialog(QWidget* parent):EQt::FileDialog(parent) {
        setOptions(QFileDialog::DontUseNativeDialog);

        #if (defined(Q_OS_WIN))

            setWindowTitle(tr("Export DLL"));

        #elif (defined(Q_OS_LINUX))

            setWindowTitle(tr("Export Shared Library"));

        #elif (defined(Q_OS_DARWIN))

            setWindowTitle(tr("Export Dynamic Library"));

        #else

            #error Unknown platform

        #endif

        setAcceptMode(AcceptMode::AcceptSave);
        setFileMode(FileMode::AnyFile);
        setLabelText(DialogLabel::Accept, tr("Export"));
    }


    LibraryExportDialog::~LibraryExportDialog() {}


    QString LibraryExportDialog::settingsGroupName() const {
        return "LibraryExportDialog";
    }


    QStringList LibraryExportDialog::filterList() const {
        QStringList filters;

        #if (defined(Q_OS_WIN))

            filters << tr("DLL Files (*.dll)") << tr("Any files (*.*)");

        #elif (defined(Q_OS_LINUX))

            filters << tr("Shared Object Files (*.so)") << tr("Any files (*)");

        #elif (defined(Q_OS_DARWIN))

            filters << tr("Dynamic Library Files (*.dylib)") << tr("Any files (*)");

        #else

            #error Unknown platform

        #endif

        return filters;
    }

#endif

