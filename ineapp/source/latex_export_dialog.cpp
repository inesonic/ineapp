/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LaTeXExportDialog class.
***********************************************************************************************************************/

#include <QFileDialog>
#include <QWidget>
#include <QSettings>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>

#include <eqt_programmatic_dialog.h>
#include <eqt_file_selector_widget.h>

#include "application.h"
#include "latex_export_dialog.h"

LaTeXExportDialog::LaTeXExportDialog(QWidget* parent):EQt::ProgrammaticDialog("LaTeXExportDialog", parent) {
    configureWidget();
    runBuilders();
}


LaTeXExportDialog::~LaTeXExportDialog() {}


void LaTeXExportDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->loadSettings(settings);

    QString imageMode = settings->value("image_mode", "PREFER_PNG").toString();
    if (imageMode == "NO_IMAGES") {
        setImageMode(ImageMode::NO_IMAGES);
    } else if (imageMode == "ORIGINAL") {
        setImageMode(ImageMode::ORIGINAL);
    } else if (imageMode == "FORCE_PNG") {
        setImageMode(ImageMode::FORCE_PNG);
    } else if (imageMode == "FORCE_JPEG") {
        setImageMode(ImageMode::FORCE_JPEG);
    } else if (imageMode == "PREFER_PNG") {
        setImageMode(ImageMode::PREFER_PNG);
    } else if (imageMode == "PREFER_JPEG") {
        setImageMode(ImageMode::PREFER_JPEG);
    } else {
        setImageMode(ImageMode::PREFER_PNG);
    }

    QString unicodeMode = settings->value("unicode_mode", "CONVERT_TO_LATEX_COMMAND").toString();
    if (unicodeMode == "INSERT_UNICODE") {
        setUnicodeTranslationMode(UnicodeMode::INSERT_UNICODE);
    } else if (unicodeMode == "CONVERT_TO_CARAT_NOTATION") {
        setUnicodeTranslationMode(UnicodeMode::CONVERT_TO_HAT_NOTATION);
    } else if (unicodeMode == "CONVERT_TO_LATEX_COMMAND") {
        setUnicodeTranslationMode(UnicodeMode::CONVERT_TO_LATEX_COMMAND);
    } else {
        setUnicodeTranslationMode(UnicodeMode::CONVERT_TO_LATEX_COMMAND);
    }

    bool singleFileFormat = settings->value("single_file_format", false).toBool();
    setSingleFile(singleFileFormat);

    bool includeExternalImports = settings->value("include_imports", false).toBool();
    setIncludeImports(includeExternalImports);

    settings->endGroup();
}


void LaTeXExportDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->saveSettings(settings);

    QString imageMode;
    switch (LaTeXExportDialog::imageMode()) {
        case ImageMode::NO_IMAGES:   { imageMode = "NO_IMAGES";   break; }
        case ImageMode::ORIGINAL:    { imageMode = "ORIGINAL";    break; }
        case ImageMode::FORCE_PNG:   { imageMode = "FORCE_PNG";   break; }
        case ImageMode::FORCE_JPEG:  { imageMode = "FORCE_JPEG";  break; }
        case ImageMode::PREFER_PNG:  { imageMode = "PREFER_PNG";  break; }
        case ImageMode::PREFER_JPEG: { imageMode = "PREFER_JPEG"; break; }
        default: {
            Q_ASSERT(false);
            break;
        }
    }

    settings->setValue("image_mode", imageMode);

    QString unicodeMode;
    switch (unicodeTranslationMode()) {
        case UnicodeMode::INSERT_UNICODE:           { unicodeMode = "INSERT_UNICODE";            break; }
        case UnicodeMode::CONVERT_TO_HAT_NOTATION:  { unicodeMode = "CONVERT_TO_CARAT_NOTATION"; break; }
        case UnicodeMode::CONVERT_TO_LATEX_COMMAND: { unicodeMode = "CONVERT_TO_LATEX_COMMAND";  break; }
        default: {
            Q_ASSERT(false);
            break;
        }
    }

    settings->setValue("unicode_mode", unicodeMode);

    settings->setValue("single_file_format", singleFile());
    settings->setValue("include_imports", includeImports());

    settings->endGroup();
}


QString LaTeXExportDialog::selectedDirectory() const {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    QStringList              selectedFiles      = fileSelectorWidget->selectedFiles();

    return selectedFiles.isEmpty() ? QString() : selectedFiles.at(0);
}


QString LaTeXExportDialog::directory() const {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    return fileSelectorWidget->directory().absolutePath();
}


void LaTeXExportDialog::setDirectory(const QString& directory) {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->setDirectory(directory);
}


LaTeXExportDialog::ImageMode LaTeXExportDialog::imageMode() const {
    ImageMode result;

    QRadioButton* noImagesRadioButton = widget<QRadioButton>("no_images_radio_button");
    QRadioButton* originalImageFormatRadioButton = widget<QRadioButton>("original_image_format_radio_button");
    QRadioButton* forcePngFormatRadioButton = widget<QRadioButton>("force_png_format_radio_button");
    QRadioButton* forceJpegFormatRadioButton = widget<QRadioButton>("force_Jpeg_format_radio_button");
    QRadioButton* preferPngFormatRadioButton = widget<QRadioButton>("prefer_png_format_radio_button");
    QRadioButton* preferJpegFormatRadioButton = widget<QRadioButton>("prefer_jpeg_format_radio_button");

    if (noImagesRadioButton->isChecked()) {
        result = ImageMode::NO_IMAGES;
    } else if (originalImageFormatRadioButton->isChecked()) {
        result = ImageMode::ORIGINAL;
    } else if (forcePngFormatRadioButton->isChecked()) {
        result = ImageMode::FORCE_PNG;
    } else if (forceJpegFormatRadioButton->isChecked()) {
        result = ImageMode::FORCE_JPEG;
    } else if (preferPngFormatRadioButton->isChecked()) {
        result = ImageMode::PREFER_PNG;
    } else if (preferJpegFormatRadioButton->isChecked()) {
        result = ImageMode::PREFER_JPEG;
    } else {
        result = ImageMode::PREFER_PNG;
    }

    return result;
}


void LaTeXExportDialog::setImageMode(ImageMode imageMode) {
    QRadioButton* noImagesRadioButton = widget<QRadioButton>("no_images_radio_button");
    QRadioButton* originalImageFormatRadioButton = widget<QRadioButton>("original_image_format_radio_button");
    QRadioButton* forcePngFormatRadioButton = widget<QRadioButton>("force_png_format_radio_button");
    QRadioButton* forceJpegFormatRadioButton = widget<QRadioButton>("force_Jpeg_format_radio_button");
    QRadioButton* preferPngFormatRadioButton = widget<QRadioButton>("prefer_png_format_radio_button");
    QRadioButton* preferJpegFormatRadioButton = widget<QRadioButton>("prefer_jpeg_format_radio_button");

    noImagesRadioButton->setChecked(imageMode == ImageMode::NO_IMAGES);
    originalImageFormatRadioButton->setChecked(imageMode == ImageMode::ORIGINAL);
    forcePngFormatRadioButton->setChecked(imageMode == ImageMode::FORCE_PNG);
    forceJpegFormatRadioButton->setChecked(imageMode == ImageMode::FORCE_JPEG);
    preferPngFormatRadioButton->setChecked(imageMode == ImageMode::PREFER_PNG);
    preferJpegFormatRadioButton->setChecked(imageMode == ImageMode::PREFER_JPEG);
}


bool LaTeXExportDialog::singleFile() const {
    QRadioButton* singleFileRadioButton = widget<QRadioButton>("single_file_radio_button");
    return singleFileRadioButton->isChecked();
}


bool LaTeXExportDialog::multipleFiles() const {
    QRadioButton* multipleFilesRadioButton = widget<QRadioButton>("multiple_files_radio_button");
    return multipleFilesRadioButton->isChecked();
}


void LaTeXExportDialog::setSingleFile(bool nowSingleFile) {
    QRadioButton* singleFileRadioButton = widget<QRadioButton>("single_file_radio_button");
    QRadioButton* multipleFilesRadioButton = widget<QRadioButton>("multiple_files_radio_button");

    singleFileRadioButton->setChecked(nowSingleFile);
    multipleFilesRadioButton->setChecked(!nowSingleFile);
}


void LaTeXExportDialog::setMultipleFiles(bool nowMultipleFiles) {
    setSingleFile(!nowMultipleFiles);
}


LaTeXExportDialog::UnicodeMode LaTeXExportDialog::unicodeTranslationMode() const {
    UnicodeMode result;

    QRadioButton* insertWithoutTranslationRadioButton = widget<QRadioButton>("insert_without_translation_radio_button");
    QRadioButton* convertToCaratNotationRadioButton   = widget<QRadioButton>("convert_to_carat_notation_radio_button");
    QRadioButton* convertToLaTeXCommandRadioButton    = widget<QRadioButton>("convert_to_latex_command_radio_button");

    if (insertWithoutTranslationRadioButton->isChecked()) {
        result = UnicodeMode::INSERT_UNICODE;
    } else if (convertToCaratNotationRadioButton->isChecked()) {
        result = UnicodeMode::CONVERT_TO_HAT_NOTATION;
    } else if (convertToLaTeXCommandRadioButton->isChecked()) {
        result = UnicodeMode::CONVERT_TO_LATEX_COMMAND;
    } else {
        result = UnicodeMode::CONVERT_TO_LATEX_COMMAND;
    }

    return result;
}


void LaTeXExportDialog::setUnicodeTranslationMode(LaTeXExportDialog::UnicodeMode newUnicodeTranslationMode) {
    QRadioButton* insertWithoutTranslationRadioButton = widget<QRadioButton>("insert_without_translation_radio_button");
    QRadioButton* convertToCaratNotationRadioButton   = widget<QRadioButton>("convert_to_carat_notation_radio_button");
    QRadioButton* convertToLaTeXCommandRadioButton    = widget<QRadioButton>("convert_to_latex_command_radio_button");

    insertWithoutTranslationRadioButton->setChecked(newUnicodeTranslationMode == UnicodeMode::INSERT_UNICODE);
    convertToCaratNotationRadioButton->setChecked(newUnicodeTranslationMode == UnicodeMode::CONVERT_TO_HAT_NOTATION);
    convertToLaTeXCommandRadioButton->setChecked(newUnicodeTranslationMode == UnicodeMode::CONVERT_TO_LATEX_COMMAND);
}


bool LaTeXExportDialog::includeImports() const {
    QCheckBox* includeExternalImportsCheckBox = widget<QCheckBox>("include_external_imports_check_box");
    return includeExternalImportsCheckBox->isChecked();
}


void LaTeXExportDialog::setIncludeImports(bool nowIncludeImports) {
    QCheckBox* includeExternalImportsCheckBox = widget<QCheckBox>("include_external_imports_check_box");
    includeExternalImportsCheckBox->setChecked(nowIncludeImports);
}


void LaTeXExportDialog::populate() {
    EQt::ProgrammaticDialog::populate();
}


void LaTeXExportDialog::bind() {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    connect(fileSelectorWidget, &EQt::FileSelectorWidget::filesSelected, this, &LaTeXExportDialog::filesSelected);
    connect(
        fileSelectorWidget,
        &EQt::FileSelectorWidget::fileNameTextChanged,
        this,
        &LaTeXExportDialog::fileNameEdited
    );

    QPushButton* exportPushButton = widget<QPushButton>("export_push_button");
    connect(exportPushButton, &QPushButton::clicked, this, &LaTeXExportDialog::accept);

    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");
    connect(cancelPushButton, &QPushButton::clicked, this, &LaTeXExportDialog::reject);
}


void LaTeXExportDialog::closeEvent(QCloseEvent* event) {
    EQt::ProgrammaticDialog::closeEvent(event);
}


void LaTeXExportDialog::filesSelected(const QStringList& fileList) {
    if (fileList.isEmpty()) {
        fileNameEdited(QString());
    } else {
        fileNameEdited(fileList.at(0));
    }
}


void LaTeXExportDialog::fileNameEdited(const QString& fileName) {
    QPushButton* exportPushButton = widget<QPushButton>("export_push_button");

    if (fileName.isEmpty()) {
        exportPushButton->setEnabled(false);
    } else {
        EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
        QDir    currentDirectory = fileSelectorWidget->directory();
        QString targetDirectory  = currentDirectory.canonicalPath() + "/" + fileName;

        QFileInfo targetFileInformation(targetDirectory);
        if (!targetFileInformation.exists()) {
            exportPushButton->setEnabled(true);
        } else {
            if (targetFileInformation.isDir() && targetFileInformation.isWritable()) {
                exportPushButton->setEnabled(true);
            } else {
                exportPushButton->setEnabled(false);
            }
        }
    }
}


void LaTeXExportDialog::configureWidget() {
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

    QVBoxLayout* additionalControlsLayout = newVBoxLayout("additional_controls_layout");
    mainHorizontalLayout->addLayout(additionalControlsLayout, 0);

    additionalControlsLayout->addSpacing(30);

    QGroupBox* latexImageModeGroupBox = new QGroupBox(tr("Image Export"));
    additionalControlsLayout->addWidget(latexImageModeGroupBox);
    registerWidget(latexImageModeGroupBox, "latex_image_mode_group_box");

    latexImageModeGroupBox->setToolTip(tr("Determines how images will be exported."));
    latexImageModeGroupBox->setWhatsThis(
        tr("You can use the options in this group box to determine how images will be exported.")
    );

    QVBoxLayout* imageModeLayout = newVBoxLayout("image_mode_layout");
    latexImageModeGroupBox->setLayout(imageModeLayout);

    QRadioButton* noImagesRadioButton = new QRadioButton(tr("Do not export images"));
    imageModeLayout->addWidget(noImagesRadioButton);
    registerWidget(noImagesRadioButton, "no_images_radio_button");

    noImagesRadioButton->setToolTip(
        tr("Do not export images, insert comment into LaTeX output where each image belongs")
    );
    noImagesRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause images to <b>not</b> be exported with the LaTeX output. The generated "
           "LaTeX output will include comments indicating where each image would have been placed."
          )
    );

    QRadioButton* originalImageFormatRadioButton = new QRadioButton(tr("Export images in their original format"));
    imageModeLayout->addWidget(originalImageFormatRadioButton);
    registerWidget(originalImageFormatRadioButton, "original_image_format_radio_button");

    originalImageFormatRadioButton->setToolTip(
        tr("Images are saved exactly as they were originally loaded into %1").arg(Application::applicationName())
    );
    originalImageFormatRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause images to be saved in the exact same format that they were in when "
           "first loaded into your %1 program.  The generated LaTeX output will include an appropriate "
           "\\includegraphics command to reference the image under the assumption that your LaTeX tool can properly "
           "parse the image."
           )
    );

    QRadioButton* forcePngFormatRadioButton = new QRadioButton(tr("Convert all images in PNG format"));
    imageModeLayout->addWidget(forcePngFormatRadioButton);
    registerWidget(forcePngFormatRadioButton, "force_png_format_radio_button");

    forcePngFormatRadioButton->setToolTip(tr("Images are always converted and save in PNG format"));
    forcePngFormatRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause images to be saved in PNG format.  If needed, the images will be "
           "translated to PNG format prior to saving.  The generated LaTeX output will include an appropriate "
           "\\includegraphics command to reference the image."
           )
    );

    QRadioButton* forceJpegFormatRadioButton = new QRadioButton(tr("Convert all images in JPEG format"));
    imageModeLayout->addWidget(forceJpegFormatRadioButton);
    registerWidget(forceJpegFormatRadioButton, "force_Jpeg_format_radio_button");

    forceJpegFormatRadioButton->setToolTip(tr("Images are always converted and save in JPEG format"));
    forceJpegFormatRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause images to be saved in JPEG format.  If needed, the images will be "
           "translated to JPEG format prior to saving.  The generated LaTeX output will include an appropriate "
           "\\includegraphics command to reference the image."
           )
    );

    QRadioButton* preferPngFormatRadioButton = new QRadioButton(tr("Convert images to PNG format if not PNG or JPEG"));
    imageModeLayout->addWidget(preferPngFormatRadioButton);
    registerWidget(preferPngFormatRadioButton, "prefer_png_format_radio_button");

    preferPngFormatRadioButton->setToolTip(
        tr("Images are converted to PNG format if not already in PNG or JPEG format")
    );
    preferPngFormatRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause images to be saved in either PNG or JPEG format.  If the image is not "
           "currently stored internally in either PNG or JPEG format, the image will be converted to PNG format when "
           "saved.  The generated LaTeX output will include an appropriate \\includegraphics command to reference the "
           "image."
           )
    );

    QRadioButton* preferJpegFormatRadioButton = new QRadioButton(
        tr("Convert images to JPEG format if not PNG or JPEG")
    );
    imageModeLayout->addWidget(preferJpegFormatRadioButton);
    registerWidget(preferJpegFormatRadioButton, "prefer_jpeg_format_radio_button");

    preferJpegFormatRadioButton->setToolTip(
        tr("Images are converted to JPEG format if not already in PNG or JPEG format")
    );
    preferJpegFormatRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause images to be saved in either PNG or JPEG format.  If the image is not "
           "currently stored internally in either PNG or JPEG format, the image will be converted to JPEG format when "
           "saved.  The generated LaTeX output will include an appropriate \\includegraphics command to reference the "
           "image."
           )
    );

    additionalControlsLayout->addStretch(0);

    QGroupBox* unicodeTranslationGroupBox = new QGroupBox(tr("Unicode Translation"));
    additionalControlsLayout->addWidget(unicodeTranslationGroupBox);
    registerWidget(unicodeTranslationGroupBox, "unicode_translation_group_box");

    unicodeTranslationGroupBox->setToolTip(tr("Determines how non-ASCII characters are handled."));
    unicodeTranslationGroupBox->setWhatsThis(
        tr(
           "You can use the options in this group box to determine how non-ASCII characters are handled in the "
           "exported LaTeX output."
          )
    );

    QVBoxLayout* unicodeTranslationLayout = newVBoxLayout("unicode_translation_layout");
    unicodeTranslationGroupBox->setLayout(unicodeTranslationLayout);

    QRadioButton* insertWithoutTranslationRadioButton = new QRadioButton(tr("Insert Unicode characters as-is"));
    unicodeTranslationLayout->addWidget(insertWithoutTranslationRadioButton);
    registerWidget(insertWithoutTranslationRadioButton, "insert_without_translation_radio_button");

    insertWithoutTranslationRadioButton->setToolTip(tr("All characters are inserted in the LaTeX output as-is"));
    insertWithoutTranslationRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause the LaTeX output to include Unicode characters directly in the output.  "
           "You should use this option if you are using a LaTeX tool, such as XeTeX or LuaTex, that properly handles "
           "UTF-8 encoded text and your editing environment can display UTF-8 encoded text."
          )
    );

    QRadioButton* convertToCaratNotationRadioButton = new QRadioButton(tr("Convert to LaTeX ^^^^ notation"));
    unicodeTranslationLayout->addWidget(convertToCaratNotationRadioButton);
    registerWidget(convertToCaratNotationRadioButton, "convert_to_carat_notation_radio_button");

    convertToCaratNotationRadioButton->setToolTip(
        tr("Non-ASCII characters are converted to ^^^^ notation in the LaTeX output")
    );
    convertToCaratNotationRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause the LaTeX output to include Unicode characters encoded using LaTeX's "
           "^^^^ notation.  As an example, the character \"%1\" would be encoded as ^^^^%2 with this option.  You "
           "You should use this option if you are using a LaTeX tool, such as XeTeX or LuaTex, that properly handles "
           "UTF-8 encoded text but do not want Unicode directly in the LaTeX source."
          ).arg(QChar(0x01E9E)).arg(0x1E9E, 4, 16, QChar('0'))
    );

    QRadioButton* convertToLaTeXCommandRadioButton = new QRadioButton(tr("Convert to LaTeX commands"));
    unicodeTranslationLayout->addWidget(convertToLaTeXCommandRadioButton);
    registerWidget(convertToLaTeXCommandRadioButton, "convert_to_latex_command_radio_button");

    convertToLaTeXCommandRadioButton->setToolTip(
        tr("Non-ASCII characters are converted to LaTeX commands (if possible) in the LaTeX output")
    );
    convertToLaTeXCommandRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause the LaTeX output to include Unicode characters converted to LaTeX "
           "commands.  Characters that %1 can not map to commands will be replaced with warning text in the LaTeX "
           "output.  As an example, the character \"%2\" would be encoded as \\SS{} using this option.  You should "
           "use this option if you are using a LaTeX tool, such as pdflatex, that has no or limited support for "
           "UTF-8 encoded text."
          ).arg(Application::applicationName()).arg(QChar(0x01E9E))
    );

    additionalControlsLayout->addStretch(0);

    QGroupBox* outputFormatGroupBox = new QGroupBox(tr("Output Format"));
    additionalControlsLayout->addWidget(outputFormatGroupBox);
    registerWidget(outputFormatGroupBox, "output_format_group_box");

    QVBoxLayout* outputFormatLayout = newVBoxLayout("output_format_layout");
    outputFormatGroupBox->setLayout(outputFormatLayout);

    unicodeTranslationGroupBox->setToolTip(tr("Determines how the LaTeX output will be structured"));
    unicodeTranslationGroupBox->setWhatsThis(
        tr(
           "You can use the options in this group box to determine how the LaTeX will be structured."
          )
    );

    QRadioButton* singleFileRadioButton = new QRadioButton(tr("Single LaTeX file"));
    outputFormatLayout->addWidget(singleFileRadioButton);
    registerWidget(singleFileRadioButton, "single_file_radio_button");

    singleFileRadioButton->setToolTip(tr("Places all of the LaTeX source into a single file"));
    singleFileRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause all of the generated LaTeX output to be placed into a single file named "
           "\"%1\" under the selected directory.  If enabled, images will also be placed in this directory."
          ).arg(Ld::LaTeXCodeGenerator::latexTopFilename)
    );

    QRadioButton* multipleFilesRadioButton = new QRadioButton(tr("Preamble and document body in separate files"));
    outputFormatLayout->addWidget(multipleFilesRadioButton);
    registerWidget(multipleFilesRadioButton, "multiple_files_radio_button");

    multipleFilesRadioButton->setToolTip(tr("Breaks the LaTeX source into multiple files"));
    multipleFilesRadioButton->setWhatsThis(
        tr(
           "Selecting this option will cause the LaTeX output to be broken up into three distinct files under the "
           "selected directory.  The LaTeX preamble will be placed in a file called \"%1\".  The majority of the "
           "LaTeX source will be placed in a file called \"%2\".  Lastly a top level file called \"%3\" will also "
           "be created that inputs the other files.  If enabled, images will also be placed in the selected directory."
          ).arg(
              Ld::LaTeXCodeGenerator::latexPreambleFilename,
              Ld::LaTeXCodeGenerator::latexBodyFilename,
              Ld::LaTeXCodeGenerator::latexTopFilename
          )
    );

    additionalControlsLayout->addStretch(0);

    QCheckBox* includeExternalImportsCheckBox = new QCheckBox(tr("Include all imports"));
    additionalControlsLayout->addWidget(includeExternalImportsCheckBox);
    registerWidget(includeExternalImportsCheckBox, "include_external_imports_check_box");

    includeExternalImportsCheckBox->setToolTip(
        tr("Indicates if the LaTeX output should include external imports")
    );
    multipleFilesRadioButton->setWhatsThis(
        tr(
           "Checking this checkbox will cause the LaTeX output to include referenced external programs in the same "
           "order that they are processed when compiling your model.  If this checkbox is unchecked, then the LaTeX "
           "output will include content from this program file only."
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
}
