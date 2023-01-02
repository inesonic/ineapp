/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PdfExportDialog class.
***********************************************************************************************************************/

#include <QFileDialog>
#include <QWidget>
#include <QSettings>
#include <QStringList>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QFontMetrics>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QPalette>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionValidator>

#include <eqt_programmatic_dialog.h>
#include <eqt_file_selector_widget.h>

#include "application.h"
#include "pdf_export_dialog.h"

const QRegularExpression PdfExportDialog::printRangesRegularExpression(
    "^([1-9][0-9]*)(-([1-9][0-9]*))?(,([1-9][0-9]*)(-([1-9][0-9]*))?)*$"
);

const QRegularExpression PdfExportDialog::printRangeRegularExpression("^([1-9][0-9]*)(-([1-9][0-9]*))?$");

PdfExportDialog::PdfExportDialog(QWidget* parent):EQt::ProgrammaticDialog("PdfExportDialog", parent) {
    configureWidget();
    runBuilders();
}


PdfExportDialog::~PdfExportDialog() {}


void PdfExportDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->loadSettings(settings);

    QString pdfVersion = settings->value("pdf_version", "A1b").toString();
    if (pdfVersion == "1.4") {
        setPdfVersion(PdfVersion::VERSION_1_4);
    } else if (pdfVersion == "A1b") {
        setPdfVersion(PdfVersion::VERSION_A1B);
    } else if (pdfVersion == "1.6") {
        setPdfVersion(PdfVersion::VERSION_1_6);
    } else {
        setPdfVersion(PdfVersion::VERSION_A1B);
    }

    settings->endGroup();
}


void PdfExportDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->saveSettings(settings);

    QString pdfVersion;
    switch (PdfExportDialog::pdfVersion()) {
        case PdfVersion::VERSION_1_4: { pdfVersion = "1.4"; break; }
        case PdfVersion::VERSION_A1B: { pdfVersion = "A1b"; break; }
        case PdfVersion::VERSION_1_6: { pdfVersion = "1.6"; break; }
        default: {
            Q_ASSERT(false);
            break;
        }
    }

    settings->setValue("pdf_version", pdfVersion);

    settings->endGroup();
}


QString PdfExportDialog::selectedFile() const {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    QStringList              selectedFiles      = fileSelectorWidget->selectedFiles();

    return selectedFiles.isEmpty() ? QString() : selectedFiles.at(0);
}


QString PdfExportDialog::directory() const {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    return fileSelectorWidget->directory().absolutePath();
}


void PdfExportDialog::setDirectory(const QString& directory) {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->setDirectory(directory);
}


PdfExportDialog::PdfVersion PdfExportDialog::pdfVersion() const {
    PdfVersion result;

    QRadioButton* pdfVersion14RadioButton  = widget<QRadioButton>("pdf_version_1_4_radio_button");
    QRadioButton* pdfVersionA1bRadioButton = widget<QRadioButton>("pdf_version_a1b_radio_button");
    QRadioButton* pdfVersion16RadioButton  = widget<QRadioButton>("pdf_version_16_radio_button");

    if (pdfVersion14RadioButton->isChecked()) {
        result = PdfVersion::VERSION_1_4;
    } else if (pdfVersionA1bRadioButton->isChecked()) {
        result = PdfVersion::VERSION_A1B;
    } else if (pdfVersion16RadioButton->isChecked()) {
        result = PdfVersion::VERSION_1_6;
    } else {
        result = PdfVersion::VERSION_A1B;
    }

    return result;
}


void PdfExportDialog::setPdfVersion(PdfExportDialog::PdfVersion newPdfVersion) {
    QRadioButton* pdfVersion14RadioButton  = widget<QRadioButton>("pdf_version_1_4_radio_button");
    QRadioButton* pdfVersionA1bRadioButton = widget<QRadioButton>("pdf_version_a1b_radio_button");
    QRadioButton* pdfVersion16RadioButton  = widget<QRadioButton>("pdf_version_16_radio_button");

    pdfVersion14RadioButton->setChecked(newPdfVersion == PdfVersion::VERSION_1_4);
    pdfVersionA1bRadioButton->setChecked(newPdfVersion == PdfVersion::VERSION_A1B);
    pdfVersion16RadioButton->setChecked(newPdfVersion == PdfVersion::VERSION_1_6);
}


unsigned PdfExportDialog::numberPages() const {
    return currentNumberPages;
}


void PdfExportDialog::setNumberPages(unsigned newNumberPages) {
    currentNumberPages = newNumberPages;

    QRadioButton* printEntireProgramRadioButton = widget<QRadioButton>("print_entire_program_radio_button");
    QRadioButton* printCurrentPageRadioButton   = widget<QRadioButton>("print_current_page_radio_button");
    QRadioButton* printRangeRadioButton         = widget<QRadioButton>("print_range_radio_button");
    QLineEdit*    printRangeLineEdit            = widget<QLineEdit>("print_range_line_edit");
    QLabel*       printRangeLabel               = widget<QLabel>("print_range_label");

    QGroupBox* printRangeGroupBox = widget<QGroupBox>("print_range_group_box");

    if (newNumberPages > 1) {
        printRangeGroupBox->setEnabled(true);

        printEntireProgramRadioButton->setChecked(true);
        printCurrentPageRadioButton->setChecked(false);
        printRangeRadioButton->setChecked(false);

        printRangeLineEdit->clear();

        printEntireProgramRadioButton->setEnabled(true);
        printCurrentPageRadioButton->setEnabled(true);
        printRangeRadioButton->setEnabled(true);
        printRangeLineEdit->setEnabled(false);
        printRangeLabel->setEnabled(false);
    } else {
        printRangeGroupBox->setEnabled(false);

        printEntireProgramRadioButton->setChecked(true);
        printCurrentPageRadioButton->setChecked(false);
        printRangeRadioButton->setChecked(false);

        printRangeLineEdit->clear();

        printEntireProgramRadioButton->setEnabled(false);
        printCurrentPageRadioButton->setEnabled(false);
        printRangeRadioButton->setEnabled(false);
        printRangeLineEdit->setEnabled(false);
        printRangeLabel->setEnabled(false);
    }
}


unsigned PdfExportDialog::currentPageNumber() const {
    return displayedCurrentPageNumber;
}


void PdfExportDialog::setCurrentPageNumber(unsigned int newCurrentPage) {
    displayedCurrentPageNumber = newCurrentPage;

    QRadioButton* printCurrentPageRadioButton = widget<QRadioButton>("print_current_page_radio_button");

    QString radioButtonText;
    QString toolTipText;
    QString whatsThisText;

    if (newCurrentPage == invalidPageNumber) {
        radioButtonText = tr("Current page");
        toolTipText     = tr("Export just the current page");
        whatsThisText   = tr("Selecting this option will just the current page to be exported to the PDF file.");
    } else {
        radioButtonText = tr("Current page (page %1)").arg(newCurrentPage);
        toolTipText     = tr("Export just the current page (%1)").arg(newCurrentPage);
        whatsThisText   = tr(
            "Selecting this option will just the current page, %1, to be exported to the PDF file."
        ).arg(newCurrentPage);
    }

    printCurrentPageRadioButton->setText(radioButtonText);
    printCurrentPageRadioButton->setToolTip(toolTipText);
    printCurrentPageRadioButton->setWhatsThis(whatsThisText);
}


PdfExportDialog::PageList PdfExportDialog::pageList() const {
    PageList result;

    QRadioButton* printEntireProgramRadioButton = widget<QRadioButton>("print_entire_program_radio_button");
    QRadioButton* printCurrentPageRadioButton   = widget<QRadioButton>("print_current_page_radio_button");
    QRadioButton* printRangeRadioButton         = widget<QRadioButton>("print_range_radio_button");

    if (printEntireProgramRadioButton->isChecked()) {
        for (unsigned pageNumber=1 ; pageNumber<=currentNumberPages ; ++pageNumber) {
            result.append(pageNumber);
        }
    } else if (printCurrentPageRadioButton->isChecked()) {
        result.append(displayedCurrentPageNumber);
    } else if (printRangeRadioButton->isChecked()) {
        QLineEdit* printRangeLineEdit = widget<QLineEdit>("print_range_line_edit");
        QString    printRanges        = printRangeLineEdit->text();

        result = calculatePrintRange(printRanges);
    }

    return result;
}


void PdfExportDialog::populate() {
    EQt::ProgrammaticDialog::populate();
}


void PdfExportDialog::bind() {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    connect(fileSelectorWidget, &EQt::FileSelectorWidget::filesSelected, this, &PdfExportDialog::filesSelected);
    connect(
        fileSelectorWidget,
        &EQt::FileSelectorWidget::fileNameTextChanged,
        this,
        &PdfExportDialog::fileNameEdited
    );

    QRadioButton* printEntireProgramRadioButton = widget<QRadioButton>("print_entire_program_radio_button");
    QRadioButton* printCurrentPageRadioButton   = widget<QRadioButton>("print_current_page_radio_button");
    QRadioButton* printRangeRadioButton         = widget<QRadioButton>("print_range_radio_button");
    QLineEdit*    printRangeLineEdit            = widget<QLineEdit>("print_range_line_edit");

    connect(
        printEntireProgramRadioButton,
        &QRadioButton::clicked,
        this,
        &PdfExportDialog::printEntireProgramRadioButtonClicked
    );

    connect(
        printCurrentPageRadioButton,
        &QRadioButton::clicked,
        this,
        &PdfExportDialog::printCurrentPageRadioButtonClicked
    );

    connect(printRangeRadioButton, &QRadioButton::clicked, this, &PdfExportDialog::printRangeRadioButtonClicked);
    connect(printRangeLineEdit, &QLineEdit::textEdited, this, &PdfExportDialog::printRangeChanged);

    QPushButton* exportPushButton = widget<QPushButton>("export_push_button");
    connect(exportPushButton, &QPushButton::clicked, this, &PdfExportDialog::accept);

    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");
    connect(cancelPushButton, &QPushButton::clicked, this, &PdfExportDialog::reject);
}


void PdfExportDialog::closeEvent(QCloseEvent* event) {
    EQt::ProgrammaticDialog::closeEvent(event);
}


void PdfExportDialog::filesSelected(const QStringList& fileList) {
    if (fileList.isEmpty()) {
        fileNameEdited(QString());
    } else {
        fileNameEdited(fileList.at(0));
    }
}


void PdfExportDialog::fileNameEdited(const QString&) {
    updateExportPushButton();
}


void PdfExportDialog::printEntireProgramRadioButtonClicked() {
    QLineEdit* printRangeLineEdit = widget<QLineEdit>("print_range_line_edit");
    QLabel*    printRangeLabel    = widget<QLabel>("print_range_label");

    printRangeLineEdit->setEnabled(false);
    printRangeLabel->setEnabled(false);

    updateExportPushButton();
}


void PdfExportDialog::printCurrentPageRadioButtonClicked() {
    printEntireProgramRadioButtonClicked();
}


void PdfExportDialog::printRangeRadioButtonClicked() {
    QLineEdit* printRangeLineEdit = widget<QLineEdit>("print_range_line_edit");
    QLabel*    printRangeLabel    = widget<QLabel>("print_range_label");

    printRangeLineEdit->setEnabled(true);
    printRangeLabel->setEnabled(true);

    updateExportPushButton();
}


void PdfExportDialog::printRangeChanged(const QString& newText) {
    bool ok = true;
    calculatePrintRange(newText, &ok);

    QLineEdit* printRangeLineEdit = widget<QLineEdit>("print_range_line_edit");
    QPalette palette;
    QColor   textColor = ok ? Qt::black : Qt::red;
    palette.setColor(QPalette::Active,   QPalette::Text, textColor);
    palette.setColor(QPalette::Inactive, QPalette::Text, textColor);
    printRangeLineEdit->setPalette(palette);

    updateExportPushButton();
}


PdfExportDialog::PageList PdfExportDialog::calculatePrintRange(const QString& rangeString, bool* ok) const {
    bool     success = true;
    PageList result;

    QStringList                 ranges           = rangeString.split(QChar(','));
    QStringList::const_iterator rangeIterator    = ranges.constBegin();
    QStringList::const_iterator rangeEndIterator = ranges.constEnd();
    while (success && rangeIterator != rangeEndIterator) {
        QRegularExpressionMatch match        = printRangeRegularExpression.match(*rangeIterator);
        int                     lastCaptured = match.lastCapturedIndex();
        if (lastCaptured == 1) {
            QString  pageNumberString = match.captured(1);
            unsigned pageNumber       = pageNumberString.toUInt(&success);
            if (success) {
                if (pageNumber > 0 && pageNumber <= currentNumberPages) {
                    result.append(pageNumber);
                } else {
                    success = false;
                }
            }
        } else if (lastCaptured == 3) {
            QString  startingPageNumberString = match.captured(1);
            unsigned startingPageNumber       = startingPageNumberString.toUInt(&success);
            if (success) {
                if (startingPageNumber > 0 && startingPageNumber <= currentNumberPages) {
                    QString  endingPageNumberString = match.captured(3);
                    unsigned endingPageNumber       = endingPageNumberString.toUInt(&success);
                    if (success) {
                        if (endingPageNumber >= startingPageNumber && endingPageNumber <= currentNumberPages) {
                            for (unsigned page=startingPageNumber ; page<=endingPageNumber ; ++page) {
                                result.append(page);
                            }
                        } else {
                            success = false;
                        }
                    }
                } else {
                    success = false;
                }
            }
        } else {
            success = false;
        }

        ++rangeIterator;
    }

    if (!success) {
        result.clear();
    }

    if (ok != Q_NULLPTR) {
        *ok = success;
    }

    return result;
}


void PdfExportDialog::updateExportPushButton() {
    bool enableButton = true;

    QString fileName = selectedFile();
    if (fileName.isEmpty()) {
        enableButton = false;
    } else {
        EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
        QDir    currentDirectory = fileSelectorWidget->directory();

        QFileInfo currentDirectoryInformation(currentDirectory.absolutePath());
        if (!currentDirectoryInformation.isDir() || !currentDirectoryInformation.isWritable()) {
            enableButton = false;
        } else {
            QString targetFile = selectedFile();
            QFileInfo targetFileInformation(targetFile);
            if (targetFileInformation.exists()            &&
                (targetFileInformation.isDir()      ||
                 !targetFileInformation.isWritable()    )    ) {
                enableButton = false;
            }
        }
    }

    if (enableButton) {
        QRadioButton* printRangeRadioButton = widget<QRadioButton>("print_range_radio_button");
        if (printRangeRadioButton->isChecked()) {
            QLineEdit* printRangeLineEdit = widget<QLineEdit>("print_range_line_edit");
            calculatePrintRange(printRangeLineEdit->text(), &enableButton);
        }
    }

    QPushButton* exportPushButton = widget<QPushButton>("export_push_button");
    exportPushButton->setEnabled(enableButton);
}


void PdfExportDialog::configureWidget() {
    setWindowTitle(tr("Export LaTeX"));
    setSizeGripEnabled(true);

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* mainHorizontalLayout = newHBoxLayout("horizontal_layout");
    mainLayout->addLayout(mainHorizontalLayout);

    EQt::FileSelectorWidget* fileSelector = new EQt::FileSelectorWidget(
        EQt::FileSelectorWidget::AnyFile,
        EQt::FileSelectorWidget::AcceptSave
    );
    fileSelector->setLineEditorAutoUpdateEnabled();
    mainHorizontalLayout->addWidget(fileSelector, 1);
    registerWidget(fileSelector, "file_selector_widget");

    QStringList filters;

    #if (defined(Q_OS_WIN))

        filters << tr("PDF Files (*.pdf)") << tr("Any files (*.*)");

    #elif (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        filters << tr("PDF Files (*.pdf)") << tr("Any files (*)");

    #else

        #error Unknown platform

    #endif

    fileSelector->setNameFilters(filters);

    QVBoxLayout* additionalControlsLayout = newVBoxLayout("additional_controls_layout");
    mainHorizontalLayout->addLayout(additionalControlsLayout, 0);

    additionalControlsLayout->addSpacing(30);

    QGroupBox* printRangeGroupBox = new QGroupBox(tr("Print Range"));
    additionalControlsLayout->addWidget(printRangeGroupBox);
    registerWidget(printRangeGroupBox, "print_range_group_box");

    printRangeGroupBox->setToolTip(tr("Determines what pages are exported"));
    printRangeGroupBox->setWhatsThis(
        tr("You can use the options in this group box to exactly what pages are exported to the PDF file")
    );

    QVBoxLayout* printRangeLayout = newVBoxLayout("print_range_layout");
    printRangeGroupBox->setLayout(printRangeLayout);

    QRadioButton* printEntireProgramRadioButton = new QRadioButton(tr("Entire program"));
    printRangeLayout->addWidget(printEntireProgramRadioButton);
    registerWidget(printEntireProgramRadioButton, "print_entire_program_radio_button");

    printEntireProgramRadioButton->setToolTip(tr("Export the entire program"));
    printEntireProgramRadioButton->setWhatsThis(
        tr("Selecting this option will cause all pages of your program to be exported to the PDF file.")
    );

    QRadioButton* printCurrentPageRadioButton = new QRadioButton(tr("???"));
    printRangeLayout->addWidget(printCurrentPageRadioButton);
    registerWidget(printCurrentPageRadioButton, "print_current_page_radio_button");

    printCurrentPageRadioButton->setToolTip(tr("???"));
    printCurrentPageRadioButton->setWhatsThis(tr("???"));

    QHBoxLayout* printRangeRadioButtonLayout = newHBoxLayout("print_range_radio_button_layout");
    printRangeRadioButtonLayout->setContentsMargins(0, 0, 0, 0);
    printRangeLayout->addLayout(printRangeRadioButtonLayout);

    QRadioButton* printRangeRadioButton = new QRadioButton(tr("Range: "));
    printRangeRadioButtonLayout->addWidget(printRangeRadioButton);
    registerWidget(printRangeRadioButton, "print_range_radio_button");

    printRangeRadioButton->setToolTip(tr("Export a range of pages"));
    printRangeRadioButton->setWhatsThis(
        tr("Selecting this option will allow you to export a range of pages.")
    );

    QFontMetrics applicationFontMetrics(Application::font());
    unsigned lineEditorWidth = static_cast<unsigned>(applicationFontMetrics.horizontalAdvance("XXXXXXXXXXXXX"));

    QLineEdit* printRangeLineEdit = new QLineEdit;
    printRangeLineEdit->setFixedWidth(lineEditorWidth);
    printRangeRadioButtonLayout->addWidget(printRangeLineEdit);
    registerWidget(printRangeLineEdit, "print_range_line_edit");

    printRangeLineEdit->setToolTip(tr("Print range, e.g. 1-5,7"));
    printRangeLineEdit->setWhatsThis(tr("You can enter the desired range of pages to print in this field."));

    printRangeRadioButtonLayout->addStretch(0);

    QRegularExpressionValidator* printRangeValidator = new QRegularExpressionValidator(
        printRangesRegularExpression,
        printRangeLineEdit
    );
    printRangeLineEdit->setValidator(printRangeValidator);
    registerObject(printRangeValidator, "print_range_validator");

    QLabel* printRangeLabel = new QLabel(tr("You can enter page numbers or\npage ranges separated by commas"));
    printRangeLayout->addWidget(printRangeLabel);
    registerWidget(printRangeLabel, "print_range_label");

    additionalControlsLayout->addStretch(0);

    QGroupBox* pdfVersionGroupBox = new QGroupBox(tr("PDF Version"));
    additionalControlsLayout->addWidget(pdfVersionGroupBox);
    registerWidget(pdfVersionGroupBox, "pdf_version_group_box");

    pdfVersionGroupBox->setToolTip(tr("Determines the PDF version to export"));
    pdfVersionGroupBox->setWhatsThis(
        tr("You can use the options in this group box to select which version of the PDF standard to export.")
    );

    QVBoxLayout* pdfVersionLayout = newVBoxLayout("pdf_version_layout");
    pdfVersionGroupBox->setLayout(pdfVersionLayout);

    QRadioButton* pdfVersion14RadioButton = new QRadioButton(tr("Adobe PDF Version 1.4"));
    pdfVersionLayout->addWidget(pdfVersion14RadioButton);
    registerWidget(pdfVersion14RadioButton, "pdf_version_1_4_radio_button");

    pdfVersion14RadioButton->setToolTip(tr("Output will conform to Adobe PDF version 1.4"));
    pdfVersion14RadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause the exported PDF file to conform to PDF version 1.4.  Use this version "
           "if you need your PDF file to be compatible with very old older PDF readers (prior to 2005)."
          )
    );

    QRadioButton* pdfVersionA1bRadioButton = new QRadioButton(tr("PDF Standard ISO 19005-1 (A1b)"));
    pdfVersionLayout->addWidget(pdfVersionA1bRadioButton);
    registerWidget(pdfVersionA1bRadioButton, "pdf_version_a1b_radio_button");

    pdfVersionA1bRadioButton->setToolTip(tr("Output will conform to PDF standard ISO 19005-1 (A1b)"));
    pdfVersionA1bRadioButton->setWhatsThis(
        tr(
            "Selecting this option will cause the exported PDF file to conform to ISO standard 19005-1 (A1b).  Use "
            "this version if you want your generated PDF to be suitable for archival purposes and usable across "
            "a wide range of different PDF readers."
          )
    );

    QRadioButton* pdfVersion16RadioButton = new QRadioButton(tr("Adobe PDF version 1.6"));
    pdfVersionLayout->addWidget(pdfVersion16RadioButton);
    registerWidget(pdfVersion16RadioButton, "pdf_version_16_radio_button");

    pdfVersion16RadioButton->setToolTip(tr("Output will conform to Adobe PDF version 1.6"));
    pdfVersion16RadioButton->setWhatsThis(
        tr(
            "Selecting this option will cause the exported PDF file to conform to ISO standard 19005-1 (16).  Use "
            "this version if you want your generated PDF to leverage features specific to Adobe Acrobat 7.0."
          )
    );

    additionalControlsLayout->addStretch(0);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    additionalControlsLayout->addWidget(buttonBox);
    registerWidget(buttonBox, "button_box");

    QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
    exportButton->setText(tr("Export"));
    registerWidget(exportButton, "export_push_button");

    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    exportButton->setEnabled(false);

    setPdfVersion(PdfVersion::VERSION_A1B);
    setCurrentPageNumber(invalidPageNumber);
    setNumberPages(0);
}
