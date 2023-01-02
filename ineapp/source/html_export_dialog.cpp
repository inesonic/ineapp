/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HtmlExportDialog class.
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
#include "html_export_dialog.h"

HtmlExportDialog::HtmlExportDialog(QWidget* parent):EQt::ProgrammaticDialog("HtmlExportDialog", parent) {
    configureWidget();
    runBuilders();
}


HtmlExportDialog::~HtmlExportDialog() {}


void HtmlExportDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->loadSettings(settings);

    QString mathMode = settings->value("math_mode", "MATHJAX_LATEX").toString();
    if (mathMode == "NO_MATH") {
        setMathMode(MathMode::NO_MATH);
    } else if (mathMode == "MATHML") {
        setMathMode(MathMode::MATHML);
    } else if (mathMode == "MATHJAX_MATHML") {
        setMathMode(MathMode::MATHJAX_MATHML);
    } else if (mathMode == "MATHJAX_LATEX") {
        setMathMode(MathMode::MATHJAX_LATEX);
    } else {
        setMathMode(MathMode::MATHJAX_LATEX);
    }

    QString htmlStyle = settings->value("html_style", "HTML5_WITH_CSS").toString();
    if (htmlStyle == "HTML5_WITH_CSS") {
        setHtmlStyle(HtmlStyle::HTML5_WITH_CSS);
    } else if (htmlStyle == "HTML4_WITHOUT_CSS") {
        setHtmlStyle(HtmlStyle::HTML4_WITHOUT_CSS);
    } else {
        setHtmlStyle(HtmlStyle::HTML5_WITH_CSS);
    }

    QString imageMode = settings->value("image_mode", "SEPARATE_FILES").toString();
    if (imageMode == "EXCLUDE") {
        setImageHandlingMode(ImageHandlingMode::EXCLUDE);
    } else if (imageMode == "SEPARATE_FILES") {
        setImageHandlingMode(ImageHandlingMode::SEPARATE_PAYLOADS);
    } else if (imageMode == "EMBEDDED") {
        setImageHandlingMode(ImageHandlingMode::EMBEDDED);
    } else {
        setImageHandlingMode(ImageHandlingMode::SEPARATE_PAYLOADS);
    }

    bool includeExternalImports = settings->value("include_imports", false).toBool();
    setIncludeImports(includeExternalImports);

    settings->endGroup();
}


void HtmlExportDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->saveSettings(settings);

    QString mathMode;
    switch (HtmlExportDialog::mathMode()) {
        case MathMode::NO_MATH:        { mathMode = "NO_MATH";        break; }
        case MathMode::MATHML:         { mathMode = "MATHML";         break; }
        case MathMode::MATHJAX_MATHML: { mathMode = "MATHJAX_MATHML"; break; }
        case MathMode::MATHJAX_LATEX:  { mathMode = "MATHJAX_LATEX";  break; }
        case MathMode::IMAGES:         { mathMode = "IMAGES";         break; }
        default: {
            Q_ASSERT(false);
            break;
        }
    }

    settings->setValue("math_mode", mathMode);

    QString htmlStyle;
    switch (HtmlExportDialog::htmlStyle()) {
        case HtmlStyle::HTML5_WITH_CSS:    { htmlStyle = "HTML5_WITH_CSS";    break; }
        case HtmlStyle::HTML4_WITHOUT_CSS: { htmlStyle = "HTML4_WITHOUT_CSS"; break; }
        default: {
            Q_ASSERT(false);
            break;
        }
    }

    settings->setValue("html_style", htmlStyle);

    QString imageMode;
    switch (imageHandlingMode()) {
        case ImageHandlingMode::EXCLUDE:           { imageMode = "EXCLUDE";        break; }
        case ImageHandlingMode::SEPARATE_PAYLOADS: { imageMode = "SEPARATE_FILES"; break; }
        case ImageHandlingMode::EMBEDDED:          { imageMode = "EMBEDDED";       break; }
        default: {
            Q_ASSERT(false);
            break;
        }
    }

    settings->setValue("image_mode", imageMode);

    settings->setValue("include_imports", includeImports());

    settings->endGroup();
}


QString HtmlExportDialog::selectedDirectory() const {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    QStringList              selectedFiles      = fileSelectorWidget->selectedFiles();

    return selectedFiles.isEmpty() ? QString() : selectedFiles.at(0);
}


QString HtmlExportDialog::directory() const {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    return fileSelectorWidget->directory().absolutePath();
}


void HtmlExportDialog::setDirectory(const QString& directory) {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    fileSelectorWidget->setDirectory(directory);
}


HtmlExportDialog::MathMode HtmlExportDialog::mathMode() const {
    MathMode result;

    QRadioButton* noMathRadioButton            = widget<QRadioButton>("no_math_radio_button");
    QRadioButton* mathMlRadioButton            = widget<QRadioButton>("math_ml_radio_button");
    QRadioButton* mathMlWithMathJaxRadioButton = widget<QRadioButton>("math_ml_with_math_jax_radio_button");
    QRadioButton* latexWithMathJaxRadioButton  = widget<QRadioButton>("latex_with_math_jax_radio_button");
    QRadioButton* imageMathRadioButton         = widget<QRadioButton>("image_math_radio_button");

    if (noMathRadioButton->isChecked()) {
        result = MathMode::NO_MATH;
    } else if (mathMlRadioButton->isChecked()) {
        result = MathMode::MATHML;
    } else if (mathMlWithMathJaxRadioButton->isChecked()) {
        result = MathMode::MATHJAX_MATHML;
    } else if (latexWithMathJaxRadioButton->isChecked()) {
        result = MathMode::MATHJAX_LATEX;
    } else if (imageMathRadioButton->isChecked()) {
        result = MathMode::IMAGES;
    } else {
        result = MathMode::NO_MATH;
    }

    return result;
}


void HtmlExportDialog::setMathMode(HtmlExportDialog::MathMode mathMode) {
    QRadioButton* noMathRadioButton            = widget<QRadioButton>("no_math_radio_button");
    QRadioButton* mathMlRadioButton            = widget<QRadioButton>("math_ml_radio_button");
    QRadioButton* mathMlWithMathJaxRadioButton = widget<QRadioButton>("math_ml_with_math_jax_radio_button");
    QRadioButton* latexWithMathJaxRadioButton  = widget<QRadioButton>("latex_with_math_jax_radio_button");
    QRadioButton* imageMathRadioButton         = widget<QRadioButton>("image_math_radio_button");

    noMathRadioButton->setChecked(mathMode == MathMode::NO_MATH);
    mathMlRadioButton->setChecked(mathMode == MathMode::MATHML);
    mathMlWithMathJaxRadioButton->setChecked(mathMode == MathMode::MATHJAX_MATHML);
    latexWithMathJaxRadioButton->setChecked(mathMode == MathMode::MATHJAX_LATEX);
    imageMathRadioButton->setChecked(mathMode == MathMode::IMAGES);
}


HtmlExportDialog::HtmlStyle HtmlExportDialog::htmlStyle() const {
    HtmlStyle result;

    QRadioButton* html5RadioButton             = widget<QRadioButton>("html5_radio_button");
    QRadioButton* html4TransitionalRadioButton = widget<QRadioButton>("html4_transitional_radio_button");

    if (html5RadioButton->isChecked()) {
        result = HtmlStyle::HTML5_WITH_CSS;
    } else if (html4TransitionalRadioButton->isChecked()) {
        result = HtmlStyle::HTML4_WITHOUT_CSS;
    } else {
        result = HtmlStyle::HTML5_WITH_CSS;
    }

    return result;
}


void HtmlExportDialog::setHtmlStyle(HtmlStyle newHtmlStyle) {
    QRadioButton* html5RadioButton             = widget<QRadioButton>("html5_radio_button");
    QRadioButton* html4TransitionalRadioButton = widget<QRadioButton>("html4_transitional_radio_button");

    html5RadioButton->setChecked(newHtmlStyle == HtmlStyle::HTML5_WITH_CSS);
    html4TransitionalRadioButton->setChecked(newHtmlStyle == HtmlStyle::HTML4_WITHOUT_CSS);
}


HtmlExportDialog::ImageHandlingMode HtmlExportDialog::imageHandlingMode() const {
    ImageHandlingMode result;

    QRadioButton* excludeImagesRadioButton         = widget<QRadioButton>("exclude_images_radio_button");
    QRadioButton* imagesInSeparateFilesRadioButton = widget<QRadioButton>("images_in_separate_files_radio_button");
    QRadioButton* imagesEmbeddedRadioButton        = widget<QRadioButton>("images_embedded_radio_button");

    if (excludeImagesRadioButton->isChecked()) {
        result = ImageHandlingMode::EXCLUDE;
    } else if (imagesInSeparateFilesRadioButton->isChecked()) {
        result = ImageHandlingMode::SEPARATE_PAYLOADS;
    } else if (imagesEmbeddedRadioButton->isChecked()) {
        result = ImageHandlingMode::EMBEDDED;
    } else {
        result = ImageHandlingMode::SEPARATE_PAYLOADS;
    }

    return result;
}


void HtmlExportDialog::setImageHandlingMode(HtmlExportDialog::ImageHandlingMode newImageHandlingMode) {
    QRadioButton* excludeImagesRadioButton         = widget<QRadioButton>("exclude_images_radio_button");
    QRadioButton* imagesInSeparateFilesRadioButton = widget<QRadioButton>("images_in_separate_files_radio_button");
    QRadioButton* imagesEmbeddedRadioButton        = widget<QRadioButton>("images_embedded_radio_button");

    excludeImagesRadioButton->setChecked(newImageHandlingMode == ImageHandlingMode::EXCLUDE);
    imagesInSeparateFilesRadioButton->setChecked(newImageHandlingMode == ImageHandlingMode::SEPARATE_PAYLOADS);
    imagesEmbeddedRadioButton->setChecked(newImageHandlingMode == ImageHandlingMode::EMBEDDED);
}


bool HtmlExportDialog::includeImports() const {
    QCheckBox* includeExternalImportsCheckBox = widget<QCheckBox>("include_external_imports_check_box");
    return includeExternalImportsCheckBox->isChecked();
}


void HtmlExportDialog::setIncludeImports(bool nowIncludeImports) {
    QCheckBox* includeExternalImportsCheckBox = widget<QCheckBox>("include_external_imports_check_box");
    includeExternalImportsCheckBox->setChecked(nowIncludeImports);
}


void HtmlExportDialog::populate() {
    EQt::ProgrammaticDialog::populate();
}


void HtmlExportDialog::bind() {
    EQt::FileSelectorWidget* fileSelectorWidget = widget<EQt::FileSelectorWidget>("file_selector_widget");
    connect(fileSelectorWidget, &EQt::FileSelectorWidget::filesSelected, this, &HtmlExportDialog::filesSelected);
    connect(
        fileSelectorWidget,
        &EQt::FileSelectorWidget::fileNameTextChanged,
        this,
        &HtmlExportDialog::fileNameEdited
    );

    QPushButton* exportPushButton = widget<QPushButton>("export_push_button");
    connect(exportPushButton, &QPushButton::clicked, this, &HtmlExportDialog::accept);

    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");
    connect(cancelPushButton, &QPushButton::clicked, this, &HtmlExportDialog::reject);
}


void HtmlExportDialog::closeEvent(QCloseEvent* event) {
    EQt::ProgrammaticDialog::closeEvent(event);
}


void HtmlExportDialog::filesSelected(const QStringList& fileList) {
    if (fileList.isEmpty()) {
        fileNameEdited(QString());
    } else {
        fileNameEdited(fileList.at(0));
    }
}


void HtmlExportDialog::fileNameEdited(const QString& fileName) {
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


void HtmlExportDialog::configureWidget() {
    setWindowTitle(tr("Export HTML"));
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

    QGroupBox* htmlMathModeGroupBox = new QGroupBox(tr("Math Rendering"));
    additionalControlsLayout->addWidget(htmlMathModeGroupBox);
    registerWidget(htmlMathModeGroupBox, "html_math_mode_group_box");

    QVBoxLayout* mathModeLayout = newVBoxLayout("math_mode_layout");
    htmlMathModeGroupBox->setLayout(mathModeLayout);

    QRadioButton* noMathRadioButton = new QRadioButton(tr("Exclude Math"));
    mathModeLayout->addWidget(noMathRadioButton);
    registerWidget(noMathRadioButton, "no_math_radio_button");

    QRadioButton* mathMlRadioButton = new QRadioButton(tr("Use browser MathML support"));
    mathModeLayout->addWidget(mathMlRadioButton);
    registerWidget(mathMlRadioButton, "math_ml_radio_button");

    QRadioButton* mathMlWithMathJaxRadioButton = new QRadioButton(tr("Use MathJax MathML rendering"));
    mathModeLayout->addWidget(mathMlWithMathJaxRadioButton);
    registerWidget(mathMlWithMathJaxRadioButton, "math_ml_with_math_jax_radio_button");

    QRadioButton* latexWithMathJaxRadioButton = new QRadioButton(tr("Use MathJax LaTeX rendering"));
    mathModeLayout->addWidget(latexWithMathJaxRadioButton);
    registerWidget(latexWithMathJaxRadioButton, "latex_with_math_jax_radio_button");

    QRadioButton* imageMathRadioButton = new QRadioButton(tr("As images"));
    mathModeLayout->addWidget(imageMathRadioButton);
    registerWidget(imageMathRadioButton, "image_math_radio_button");

    additionalControlsLayout->addStretch(0);

    QGroupBox* imageHandlingGroupBox = new QGroupBox(tr("Images"));
    additionalControlsLayout->addWidget(imageHandlingGroupBox);
    registerWidget(imageHandlingGroupBox, "image_handling_group_box");

    QVBoxLayout* imageHandlingLayout = newVBoxLayout("image_handling_layout");
    imageHandlingGroupBox->setLayout(imageHandlingLayout);

    QRadioButton* excludeImagesRadioButton = new QRadioButton(tr("Exclude images"));
    imageHandlingLayout->addWidget(excludeImagesRadioButton);
    registerWidget(excludeImagesRadioButton, "exclude_images_radio_button");

    QRadioButton* imagesInSeparateFilesRadioButton = new QRadioButton(tr("Export to separate files"));
    imageHandlingLayout->addWidget(imagesInSeparateFilesRadioButton);
    registerWidget(imagesInSeparateFilesRadioButton, "images_in_separate_files_radio_button");

    QRadioButton* imagesEmbeddedRadioButton = new QRadioButton(tr("Embed directly in the HTML"));
    imageHandlingLayout->addWidget(imagesEmbeddedRadioButton);
    registerWidget(imagesEmbeddedRadioButton, "images_embedded_radio_button");

    additionalControlsLayout->addStretch(0);

    QGroupBox* htmlStyleGroupBox = new QGroupBox(tr("HTML Version"));
    additionalControlsLayout->addWidget(htmlStyleGroupBox);
    registerWidget(htmlStyleGroupBox, "html_style_group_box");

    QVBoxLayout* htmlStyleLayout = newVBoxLayout("html_style_layout");
    htmlStyleGroupBox->setLayout(htmlStyleLayout);

    QRadioButton* html5RadioButton = new QRadioButton(tr("HTML5 with CSS"));
    htmlStyleLayout->addWidget(html5RadioButton);
    registerWidget(html5RadioButton, "html5_radio_button");

    QRadioButton* html4TransitionalRadioButton = new QRadioButton(tr("HTML4.01 Transitional without CSS"));
    htmlStyleLayout->addWidget(html4TransitionalRadioButton);
    registerWidget(html4TransitionalRadioButton, "html4_transitional_radio_button");

    additionalControlsLayout->addStretch(0);

    QCheckBox* includeExternalImportsCheckBox = new QCheckBox(tr("Include all imports"));
    additionalControlsLayout->addWidget(includeExternalImportsCheckBox);
    registerWidget(includeExternalImportsCheckBox, "include_external_imports_check_box");

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
