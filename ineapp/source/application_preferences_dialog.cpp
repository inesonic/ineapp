/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ApplicationPreferencesDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QTabBar>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QMarginsF>
#include <QSizeF>
#include <QSize>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFont>
#include <QFontComboBox>
#include <QMessageBox>
#include <QStringList>
#include <QScreen>

#include <util_page_size.h>

#include <ud_usage_data.h>

#include <eqt_application.h>
#include <eqt_global_setting.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_programmatic_dialog.h>

#include <ld_page_format.h>
#include <ld_character_format.h>
#include <ld_brace_conditional_format.h>
#include <ld_root_element.h>

#include "application.h"
#include "application_settings.h"
#include "application_preferences_dialog.h"

const QSize ApplicationPreferencesDialog::minimumDisplayDpi = QSize( 48,  48);
const QSize ApplicationPreferencesDialog::maximumDisplayDpi = QSize(300, 300);

ApplicationPreferencesDialog::ApplicationPreferencesDialog(
        QWidget* parent
    ):ProgrammaticDialog(
        "ApplicationPreferencesDialog",
        parent
    ) {
    setWindowTitle(tr("Application Preferences"));
    setSizeGripEnabled(false);

    configureDialog();
    runBuilders();
}


ApplicationPreferencesDialog::~ApplicationPreferencesDialog() {}


void ApplicationPreferencesDialog::populate() {
    // Document Defaults

    QString        defaultPageFormatIdentifier = EQt::GlobalSetting::setting("default_page_format")->toString();
    Ld::PageFormat defaultPageFormat           = Ld::PageFormat::fromString(defaultPageFormatIdentifier);
    Util::PageSize defaultPageSize             = defaultPageFormat.pageSize();
    bool           landscape                   = defaultPageFormat.landscape();

    bool useMetric = (Application::locale()->measurementSystem() != QLocale::ImperialSystem);

    widget<QComboBox>("document_defaults_page_size_combo_box")->setCurrentText(defaultPageSize.description(useMetric));
    widget<QCheckBox>("document_defaults_page_landscape_orientation_check_box")->setChecked(landscape);

    widget<EQt::DimensionLineEdit>("document_defaults_page_left_line_edit")->setValue(defaultPageFormat.leftMargin());
    widget<EQt::DimensionLineEdit>("document_defaults_page_right_line_edit")->setValue(defaultPageFormat.rightMargin());
    widget<EQt::DimensionLineEdit>("document_defaults_page_top_line_edit")->setValue(defaultPageFormat.topMargin());

    widget<EQt::DimensionLineEdit>("document_defaults_page_bottom_line_edit")->setValue(
        defaultPageFormat.bottomMargin()
    );

    QString             defaultTextFontString = EQt::GlobalSetting::setting("default_text_font")->toString();
    Ld::CharacterFormat textFormat            = Ld::CharacterFormat::fromString(defaultTextFontString);
    QFont               defaultTextFont       = textFormat.toQFont();

    QString             defaultMathFontString = EQt::GlobalSetting::setting("default_math_font")->toString();
    Ld::CharacterFormat mathFormat            = Ld::CharacterFormat::fromString(defaultMathFontString);
    QFont               defaultMathFont       = mathFormat.toQFont();

    QString             defaultMathIdentifierFontString  = EQt::GlobalSetting::setting(
                                                               "default_math_identifier_font"
                                                           )->toString();
    Ld::CharacterFormat mathIdentifierFormat             = Ld::CharacterFormat::fromString(
                                                               defaultMathIdentifierFontString
                                                           );
    QFont               defaultMathIdentifierFont        = mathIdentifierFormat.toQFont();

    QString             defaultMathFunctionFontString  = EQt::GlobalSetting::setting(
                                                             "default_math_function_font"
                                                         )->toString();
    Ld::CharacterFormat mathFunctionFormat             = Ld::CharacterFormat::fromString(
                                                             defaultMathFunctionFontString
                                                         );
    QFont               defaultMathFunctionFont        = mathFunctionFormat.toQFont();

    QFontComboBox* textFontComboBox         = widget<QFontComboBox>("document_defaults_font_text_combo_box");
    QSpinBox*      textFontSpinBox          = widget<QSpinBox>("document_defaults_font_text_spin_box");
    QCheckBox*     textFontItalicsCheckBox  = widget<QCheckBox>("document_defaults_font_text_italics_check_box");
    QCheckBox*     textFontBoldfaceCheckBox = widget<QCheckBox>("document_defaults_font_text_boldface_check_box");

    textFontComboBox->setCurrentFont(defaultTextFont);
    textFontSpinBox->setValue(defaultTextFont.pointSize());
    textFontItalicsCheckBox->setChecked(defaultTextFont.italic());
    textFontBoldfaceCheckBox->setChecked(defaultTextFont.bold());

    QFontComboBox* mathFontComboBox         = widget<QFontComboBox>("document_defaults_font_math_combo_box");
    QSpinBox*      mathFontSpinBox          = widget<QSpinBox>("document_defaults_font_math_spin_box");
    QCheckBox*     mathFontItalicsCheckBox  = widget<QCheckBox>("document_defaults_font_math_italics_check_box");
    QCheckBox*     mathFontBoldfaceCheckBox = widget<QCheckBox>("document_defaults_font_math_boldface_check_box");

    mathFontComboBox->setCurrentFont(defaultMathFont);
    mathFontSpinBox->setValue(defaultMathFont.pointSize());
    mathFontItalicsCheckBox->setChecked(defaultMathFont.italic());
    mathFontBoldfaceCheckBox->setChecked(defaultMathFont.bold());

    QFontComboBox* mathIdentifierFontComboBox =
        widget<QFontComboBox>("document_defaults_font_math_identifier_combo_box");
    QSpinBox* mathIdentifierFontSpinBox =
        widget<QSpinBox>("document_defaults_font_math_identifier_spin_box");
    QCheckBox* mathIdentifierFontItalicsCheckBox =
        widget<QCheckBox>("document_defaults_font_math_identifier_italics_check_box");
    QCheckBox* mathIdentifierFontBoldfaceCheckBox =
        widget<QCheckBox>("document_defaults_font_math_identifier_boldface_check_box");

    mathIdentifierFontComboBox->setCurrentFont(defaultMathIdentifierFont);
    mathIdentifierFontSpinBox->setValue(defaultMathIdentifierFont.pointSize());
    mathIdentifierFontItalicsCheckBox->setChecked(defaultMathIdentifierFont.italic());
    mathIdentifierFontBoldfaceCheckBox->setChecked(defaultMathIdentifierFont.bold());

    QFontComboBox* mathFunctionFontComboBox =
        widget<QFontComboBox>("document_defaults_font_math_function_combo_box");
    QSpinBox* mathFunctionFontSpinBox =
        widget<QSpinBox>("document_defaults_font_math_function_spin_box");
    QCheckBox* mathFunctionFontItalicsCheckBox =
        widget<QCheckBox>("document_defaults_font_math_function_italics_check_box");
    QCheckBox* mathFunctionFontBoldfaceCheckBox =
        widget<QCheckBox>("document_defaults_font_math_function_boldface_check_box");

    mathFunctionFontComboBox->setCurrentFont(defaultMathFunctionFont);
    mathFunctionFontSpinBox->setValue(defaultMathFunctionFont.pointSize());
    mathFunctionFontItalicsCheckBox->setChecked(defaultMathFunctionFont.italic());
    mathFunctionFontBoldfaceCheckBox->setChecked(defaultMathFunctionFont.bold());

    Ld::BraceConditionalFormat braceConditionalFormat = Ld::BraceConditionalFormat::fromString(
        EQt::GlobalSetting::setting("brace_conditional_font")->toString()
    );
    QFont   braceConditionalFont = braceConditionalFormat.toQFont();
    QString preambleString       = braceConditionalFormat.conditionPreambleString();
    QString elseString           = braceConditionalFormat.elseConditionString();

    bool showElseClause = EQt::GlobalSetting::setting("brace_conditional_show_else_clause")->toBool();

    QFontComboBox* braceFontComboBox = widget<QFontComboBox>("brace_conditional_font_combo_box");
    QSpinBox*      braceFontSpinBox = widget<QSpinBox>("brace_conditional_font_spin_box");
    QCheckBox*     braceFontItalicsCheckBox = widget<QCheckBox>("brace_conditional_font_italics_check_box");
    QCheckBox*     braceFontBoldfaceCheckBox = widget<QCheckBox>("brace_conditional_font_bold_check_box");
    QLineEdit*     bracePreambleLineEdit = widget<QLineEdit>("brace_conditional_preamble_line_edit");
    QLineEdit*     braceElseLineEdit = widget<QLineEdit>("brace_conditional_else_line_edit");
    QCheckBox*     showElseCheckbox = widget<QCheckBox>("show_conditional_else_checkbox");

    braceFontComboBox->setCurrentFont(braceConditionalFont);
    braceFontSpinBox->setValue(braceConditionalFont.pointSize());
    braceFontItalicsCheckBox->setChecked(braceConditionalFont.italic());
    braceFontBoldfaceCheckBox->setChecked(braceConditionalFont.bold());
    bracePreambleLineEdit->setText(preambleString);
    braceElseLineEdit->setText(elseString);
    showElseCheckbox->setChecked(showElseClause);
    braceElseLineEdit->setEnabled(showElseClause);

    QString                  rngTypeName  = EQt::GlobalSetting::setting("default_rng")->toString();
    Ld::RootElement::RngType rngType      = Ld::RootElement::toRngType(rngTypeName);
    int                      rngTypeValue = static_cast<int>(rngType);
    int                      optionIndex  = 0;
    QComboBox*               rngComboBox  = widget<QComboBox>("rng_combo_box");
    while (optionIndex < rngComboBox->count() && rngComboBox->itemData(optionIndex).toInt() != rngTypeValue) {
        ++optionIndex;
    }

    if (optionIndex < rngComboBox->count()) {
        rngComboBox->setCurrentIndex(optionIndex);
    } else {
        rngComboBox->setCurrentIndex(0);
    }

    // System Settings

    QCheckBox* useSystemPhysicalDisplaySettingsCheckBox = widget<QCheckBox>(
        "system_settings_physical_display_use_defaults_check_box"
    );
    QSpinBox* horizontalPhysicalResolutionSpinBox = widget<QSpinBox>(
        "system_settings_physical_display_horizontal_resolution_spin_box"
    );
    QSpinBox* verticalPhysicalResolutionSpinBox = widget<QSpinBox>(
        "system_settings_physical_display_vertical_resolution_spin_box"
    );

    bool useCustomPhysicalDisplayResolution = EQt::GlobalSetting::setting(
        "use_custom_physical_display_resolution"
    )->toBool();

    useSystemPhysicalDisplaySettingsCheckBox->setChecked(!useCustomPhysicalDisplayResolution);

    horizontalPhysicalResolutionSpinBox->setSuffix(tr(" dpi"));
    verticalPhysicalResolutionSpinBox->setSuffix(tr(" dpi"));

    horizontalPhysicalResolutionSpinBox->setRange(minimumDisplayDpi.width(), maximumDisplayDpi.width());
    verticalPhysicalResolutionSpinBox->setRange(minimumDisplayDpi.height(), maximumDisplayDpi.height());

    QSize customPhysicalDisplayResolution = Application::physicalDpi();

    horizontalPhysicalResolutionSpinBox->setEnabled(useCustomPhysicalDisplayResolution);
    verticalPhysicalResolutionSpinBox->setEnabled(useCustomPhysicalDisplayResolution);

    horizontalPhysicalResolutionSpinBox->setValue(customPhysicalDisplayResolution.width());
    verticalPhysicalResolutionSpinBox->setValue(customPhysicalDisplayResolution.width());


    QCheckBox* useSystemLogicalDisplaySettingsCheckBox = widget<QCheckBox>(
        "system_settings_logical_display_use_defaults_check_box"
    );
    QSpinBox* horizontalLogicalResolutionSpinBox = widget<QSpinBox>(
        "system_settings_logical_display_horizontal_resolution_spin_box"
    );
    QSpinBox* verticalLogicalResolutionSpinBox = widget<QSpinBox>(
        "system_settings_logical_display_vertical_resolution_spin_box"
    );

    bool useCustomLogicalDisplayResolution = EQt::GlobalSetting::setting(
        "use_custom_logical_display_resolution"
    )->toBool();

    useSystemLogicalDisplaySettingsCheckBox->setChecked(!useCustomLogicalDisplayResolution);

    horizontalLogicalResolutionSpinBox->setSuffix(tr(" dpi"));
    verticalLogicalResolutionSpinBox->setSuffix(tr(" dpi"));

    horizontalLogicalResolutionSpinBox->setRange(minimumDisplayDpi.width(), maximumDisplayDpi.width());
    verticalLogicalResolutionSpinBox->setRange(minimumDisplayDpi.height(), maximumDisplayDpi.height());

    QSize customLogicalDisplayResolution = Application::logicalDpi();

    horizontalLogicalResolutionSpinBox->setEnabled(useCustomLogicalDisplayResolution);
    verticalLogicalResolutionSpinBox->setEnabled(useCustomLogicalDisplayResolution);

    horizontalLogicalResolutionSpinBox->setValue(customLogicalDisplayResolution.width());
    verticalLogicalResolutionSpinBox->setValue(customLogicalDisplayResolution.width());

    // Usage Data

    widget<QCheckBox>("send_usage_data_check_box")->setChecked(Application::usageData()->reportingEnabled());

    // Application defaults

    bool hideWelcomeMessage = EQt::GlobalSetting::setting("hide_welcome_screen")->toBool();
    widget<QCheckBox>("hide_welcome_message_check_box")->setChecked(hideWelcomeMessage);

    ProgrammaticDialog::populate();

    acceptCalled = false;
}


void ApplicationPreferencesDialog::accept() {
    if (!acceptCalled) {
        // Document Defaults

        Ld::PageFormat defaultPageFormat = generatePageFormat();
        Q_ASSERT(defaultPageFormat.isValid());

        EQt::GlobalSetting::setting("default_page_format")->setValue(defaultPageFormat.toString());

        QFontComboBox* textFontComboBox = widget<QFontComboBox>("document_defaults_font_text_combo_box");
        QSpinBox*      textFontSpinBox  = widget<QSpinBox>("document_defaults_font_text_spin_box");
        QCheckBox*     textFontItalicsCheckBox  = widget<QCheckBox>("document_defaults_font_text_italics_check_box");
        QCheckBox*     textFontBoldfaceCheckBox = widget<QCheckBox>("document_defaults_font_text_boldface_check_box");

        Ld::CharacterFormat::Weight textFontWeight  =   textFontBoldfaceCheckBox->isChecked()
                                                      ? Ld::CharacterFormat::Weight::BOLD
                                                      : Ld::CharacterFormat::Weight::NORMAL;
        bool                        textFontItalics = textFontItalicsCheckBox->isChecked();

        Ld::CharacterFormat textFontFormat(
            textFontComboBox->currentFont().family(),
            textFontSpinBox->value(),
            textFontWeight,
            textFontItalics
        );

        EQt::GlobalSetting::setting("default_text_font")->setValue(textFontFormat.toString());

        QFontComboBox* mathFontComboBox         = widget<QFontComboBox>("document_defaults_font_math_combo_box");
        QSpinBox*      mathFontSpinBox          = widget<QSpinBox>("document_defaults_font_math_spin_box");
        QCheckBox*     mathFontItalicsCheckBox  = widget<QCheckBox>("document_defaults_font_math_italics_check_box");
        QCheckBox*     mathFontBoldfaceCheckBox = widget<QCheckBox>("document_defaults_font_math_boldface_check_box");

        Ld::CharacterFormat::Weight mathFontWeight  =   mathFontBoldfaceCheckBox->isChecked()
                                                      ? Ld::CharacterFormat::Weight::BOLD
                                                      : Ld::CharacterFormat::Weight::NORMAL;
        bool                        mathFontItalics = mathFontItalicsCheckBox->isChecked();

        Ld::CharacterFormat mathFontFormat(
            mathFontComboBox->currentFont().family(),
            mathFontSpinBox->value(),
            mathFontWeight,
            mathFontItalics
        );

        EQt::GlobalSetting::setting("default_math_font")->setValue(mathFontFormat.toString());

        QFontComboBox* mathIdentifierFontComboBox =
            widget<QFontComboBox>("document_defaults_font_math_identifier_combo_box");
        QSpinBox*      mathIdentifierFontSpinBox =
            widget<QSpinBox>("document_defaults_font_math_identifier_spin_box");
        QCheckBox*     mathIdentifierFontItalicsCheckBox =
            widget<QCheckBox>("document_defaults_font_math_identifier_italics_check_box");
        QCheckBox*     mathIdentifierFontBoldfaceCheckBox =
            widget<QCheckBox>("document_defaults_font_math_identifier_boldface_check_box");

        Ld::CharacterFormat::Weight mathIdentifierFontWeight  =   mathIdentifierFontBoldfaceCheckBox->isChecked()
                                                                ? Ld::CharacterFormat::Weight::BOLD
                                                                : Ld::CharacterFormat::Weight::NORMAL;
        bool                        mathIdentifierFontItalics = mathIdentifierFontItalicsCheckBox->isChecked();

        Ld::CharacterFormat mathIdentifierFontFormat(
            mathIdentifierFontComboBox->currentFont().family(),
            mathIdentifierFontSpinBox->value(),
            mathIdentifierFontWeight,
            mathIdentifierFontItalics
        );

        EQt::GlobalSetting::setting("default_math_identifier_font")->setValue(mathIdentifierFontFormat.toString());

        QFontComboBox* mathFunctionFontComboBox =
            widget<QFontComboBox>("document_defaults_font_math_function_combo_box");
        QSpinBox*      mathFunctionFontSpinBox =
            widget<QSpinBox>("document_defaults_font_math_function_spin_box");
        QCheckBox*     mathFunctionFontItalicsCheckBox =
            widget<QCheckBox>("document_defaults_font_math_function_italics_check_box");
        QCheckBox*     mathFunctionFontBoldfaceCheckBox =
            widget<QCheckBox>("document_defaults_font_math_function_boldface_check_box");

        Ld::CharacterFormat::Weight mathFunctionFontWeight  =   mathFunctionFontBoldfaceCheckBox->isChecked()
                                                                ? Ld::CharacterFormat::Weight::BOLD
                                                                : Ld::CharacterFormat::Weight::NORMAL;
        bool                        mathFunctionFontItalics = mathFunctionFontItalicsCheckBox->isChecked();

        Ld::CharacterFormat mathFunctionFontFormat(
            mathFunctionFontComboBox->currentFont().family(),
            mathFunctionFontSpinBox->value(),
            mathFunctionFontWeight,
            mathFunctionFontItalics
        );

        EQt::GlobalSetting::setting("default_math_function_font")->setValue(mathFunctionFontFormat.toString());


        QFontComboBox* braceFontComboBox = widget<QFontComboBox>("brace_conditional_font_combo_box");
        QSpinBox*      braceFontSpinBox = widget<QSpinBox>("brace_conditional_font_spin_box");
        QCheckBox*     braceFontItalicsCheckBox = widget<QCheckBox>("brace_conditional_font_italics_check_box");
        QCheckBox*     braceFontBoldfaceCheckBox = widget<QCheckBox>("brace_conditional_font_bold_check_box");
        QLineEdit*     bracePreambleLineEdit = widget<QLineEdit>("brace_conditional_preamble_line_edit");
        QLineEdit*     braceElseLineEdit = widget<QLineEdit>("brace_conditional_else_line_edit");
        QCheckBox*     showElseCheckbox = widget<QCheckBox>("show_conditional_else_checkbox");

        Ld::BraceConditionalFormat braceConditionalFormat;
        braceConditionalFormat.setFamily(braceFontComboBox->currentFont().family());
        braceConditionalFormat.setFontSize(braceFontSpinBox->value());
        braceConditionalFormat.setItalics(braceFontItalicsCheckBox->isChecked());
        braceConditionalFormat.setFontWeight(
              braceFontBoldfaceCheckBox->isChecked()
            ? Ld::BraceConditionalFormat::Weight::BOLD
            : Ld::BraceConditionalFormat::Weight::NORMAL
        );
        braceConditionalFormat.setConditionPreambleString(bracePreambleLineEdit->text());
        braceConditionalFormat.setElseConditionString(braceElseLineEdit->text());

        EQt::GlobalSetting::setting("brace_conditional_font")->setValue(braceConditionalFormat.toString());
        EQt::GlobalSetting::setting("brace_conditional_show_else_clause")->setValue(showElseCheckbox->isChecked());

        QComboBox*               rngComboBox   = widget<QComboBox>("rng_combo_box");
        int                      rngTypeNumber = rngComboBox->currentData().toInt();
        Ld::RootElement::RngType rngType       = static_cast<Ld::RootElement::RngType>(rngTypeNumber);

        EQt::GlobalSetting::setting("default_rng")->setValue(Ld::RootElement::toString(rngType));

        // System Settings

        QCheckBox* useSystemPhysicalDisplaySettingsCheckBox = widget<QCheckBox>(
            "system_settings_physical_display_use_defaults_check_box"
        );

        QCheckBox* useSystemLogicalDisplaySettingsCheckBox = widget<QCheckBox>(
            "system_settings_logical_display_use_defaults_check_box"
        );

        bool customPhysicalDisplayResolution = !useSystemPhysicalDisplaySettingsCheckBox->isChecked();
        bool customLogicalDisplayResolution = !useSystemLogicalDisplaySettingsCheckBox->isChecked();

        QSize physicalDpiValue = QSize(
            widget<QSpinBox>("system_settings_physical_display_horizontal_resolution_spin_box")->value(),
            widget<QSpinBox>("system_settings_physical_display_horizontal_resolution_spin_box")->value()
        );

        QSize logicalDpiValue = QSize(
            widget<QSpinBox>("system_settings_logical_display_horizontal_resolution_spin_box")->value(),
            widget<QSpinBox>("system_settings_logical_display_horizontal_resolution_spin_box")->value()
        );

        bool settingsChanged = false;

        bool customPhysicalDpiSetting = EQt::GlobalSetting::setting(
            "use_custom_physical_display_resolution"
        )->toBool();

        bool customLogicalDpiSetting  = EQt::GlobalSetting::setting(
            "use_custom_logical_display_resolution"
        )->toBool();

        if (customPhysicalDisplayResolution != customPhysicalDpiSetting) {
            EQt::GlobalSetting::setting("use_custom_physical_display_resolution")->setValue(
                customPhysicalDisplayResolution
            );
            settingsChanged = true;
        }

        if (customPhysicalDisplayResolution && physicalDpiValue != Application::physicalDpi()) {
            EQt::GlobalSetting::setting("custom_physical_display_resolution")->setValue(physicalDpiValue);
            settingsChanged = true;
        }

        if (customLogicalDisplayResolution != customLogicalDpiSetting) {
            EQt::GlobalSetting::setting("use_custom_logical_display_resolution")->setValue(
                customLogicalDisplayResolution
            );

            settingsChanged = true;
        }

        if (customLogicalDisplayResolution && logicalDpiValue != Application::logicalDpi()) {
            EQt::GlobalSetting::setting("custom_logical_display_resolution")->setValue(logicalDpiValue);
            settingsChanged = true;
        }

        if (settingsChanged) {
            QMessageBox::information(
                this,
                tr("Display Settings Changed"),
                tr("Please restart the application to use your new display settings.")
            );
        }

        // Privacy

        Application::usageData()->setReportingEnabled(widget<QCheckBox>("send_usage_data_check_box")->isChecked());
        ProgrammaticDialog::accept();

        // Application defaults

        bool hideWelcomeMessage = widget<QCheckBox>("hide_welcome_message_check_box")->isChecked();
        EQt::GlobalSetting::setting("hide_welcome_screen")->setValue(hideWelcomeMessage);

        acceptCalled = true;
    }
}


void ApplicationPreferencesDialog::bind() {
    // Document Defaults

    connect(
        widget<QComboBox>("document_defaults_page_size_combo_box"),
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(validateDocumentDefaultsTab(int))
    );

    connect(
        widget<QCheckBox>("document_defaults_page_landscape_orientation_check_box"),
        SIGNAL(clicked()),
        this,
        SLOT(validateDocumentDefaultsTab())
    );

    connect(
        widget<EQt::DimensionLineEdit>("document_defaults_page_left_line_edit"),
        SIGNAL(editingFinished()),
        this,
        SLOT(validateDocumentDefaultsTab())
    );

    connect(
        widget<EQt::DimensionLineEdit>("document_defaults_page_top_line_edit"),
        SIGNAL(editingFinished()),
        this,
        SLOT(validateDocumentDefaultsTab())
    );

    connect(
        widget<EQt::DimensionLineEdit>("document_defaults_page_right_line_edit"),
        SIGNAL(editingFinished()),
        this,
        SLOT(validateDocumentDefaultsTab())
    );

    connect(
        widget<EQt::DimensionLineEdit>("document_defaults_page_bottom_line_edit"),
        SIGNAL(editingFinished()),
        this,
        SLOT(validateDocumentDefaultsTab())
    );

    // Document Default Settings.

    QCheckBox* showConditionalElseCheckbox = widget<QCheckBox>("show_conditional_else_checkbox");
    QLineEdit* braceConditionalElseLineEdit = widget<QLineEdit>("brace_conditional_else_line_edit");

    connect(showConditionalElseCheckbox, &QCheckBox::clicked, braceConditionalElseLineEdit, &QLineEdit::setEnabled);

    // System Settings

    connect(
        widget<QCheckBox>("system_settings_physical_display_use_defaults_check_box"),
        SIGNAL(toggled(bool)),
        widget<QSpinBox>("system_settings_physical_display_horizontal_resolution_spin_box"),
        SLOT(setDisabled(bool))
    );

    connect(
        widget<QCheckBox>("system_settings_physical_display_use_defaults_check_box"),
        SIGNAL(toggled(bool)),
        widget<QSpinBox>("system_settings_physical_display_vertical_resolution_spin_box"),
        SLOT(setDisabled(bool))
    );

    connect(
        widget<QCheckBox>("system_settings_logical_display_use_defaults_check_box"),
        SIGNAL(toggled(bool)),
        widget<QSpinBox>("system_settings_logical_display_horizontal_resolution_spin_box"),
        SLOT(setDisabled(bool))
    );

    connect(
        widget<QCheckBox>("system_settings_logical_display_use_defaults_check_box"),
        SIGNAL(toggled(bool)),
        widget<QSpinBox>("system_settings_logical_display_vertical_resolution_spin_box"),
        SLOT(setDisabled(bool))
    );

    connect(widget<QPushButton>("ok_push_button"), SIGNAL(clicked()), this, SLOT(accept()));
    connect(widget<QPushButton>("cancel_push_button"), SIGNAL(clicked()), this, SLOT(reject()));

    // Dialog global

    connect(widget<QPushButton>("ok_push_button"), SIGNAL(clicked()), this, SLOT(accept()));
    connect(widget<QPushButton>("cancel_push_button"), SIGNAL(clicked()), this, SLOT(reject()));
    ProgrammaticDialog::bind();
}


void ApplicationPreferencesDialog::validateDocumentDefaultsTab() {
    Ld::PageFormat pageFormat      = generatePageFormat();
    bool           pageFormatValid = pageFormat.isValid();

    widget<QLabel>("document_defaults_page_warning_label")->setVisible(!pageFormatValid);

    bool     validTab = pageFormatValid;

    QTabBar* tabBar = widget<QTabBar>("tab_bar");
    int      count  = tabBar->count();
    int      index  = 0;

    while (index<count && tabBar->tabText(index) != tr("Defaults For New &Documents")) {
        ++index;
    }

    if (validTab) {
        tabBar->setTabTextColor(index, QColor(Qt::black));
    } else {
        tabBar->setTabTextColor(index, QColor(Qt::red));
    }

    Q_ASSERT(index < count);


    conditionallyEnableOkButton("document_defaults_tab", validTab);
}


void ApplicationPreferencesDialog::validateDocumentDefaultsTab(int /* index */) {
    validateDocumentDefaultsTab();
}


QWidget* ApplicationPreferencesDialog::buildDocumentDefaultsTab() {
    QWidget* documentDefaultsWidget = new QWidget();

    QVBoxLayout* verticalLayout = newVBoxLayout("document_defaults_vertical_layout");
    documentDefaultsWidget->setLayout(verticalLayout);

    QGroupBox* pageGroupBox = new QGroupBox(tr("Page"));
    registerWidget(pageGroupBox, "document_defaults_page_group_box");
    verticalLayout->addWidget(pageGroupBox);

    QVBoxLayout* pageVerticalLayout = newVBoxLayout("document_defaults_page_vertical_layout");
    pageGroupBox->setLayout(pageVerticalLayout);

    QComboBox* pageSizeComboBox = new QComboBox;
    registerWidget(pageSizeComboBox, "document_defaults_page_size_combo_box");
    pageVerticalLayout->addWidget(pageSizeComboBox);

    QStringList descriptions = EQt::Application::paperSizeDescriptions();
    for (QStringList::const_iterator it=descriptions.constBegin(),end=descriptions.constEnd() ; it!=end ; ++it) {
        if (it->isEmpty()) {
            pageSizeComboBox->insertSeparator(descriptions.size());
        } else {
            pageSizeComboBox->addItem(*it);
        }
    }

    QCheckBox* landscapeOrientationCheckBox = new QCheckBox(tr("Landscape Orientation"));
    registerWidget(landscapeOrientationCheckBox, "document_defaults_page_landscape_orientation_check_box");
    pageVerticalLayout->addWidget(landscapeOrientationCheckBox);

    QHBoxLayout* marginsLayout = newHBoxLayout("document_defaults_page_margins_layout");
    pageVerticalLayout->addLayout(marginsLayout);

    QFormLayout* leftRightLayout = newFormLayout("document_defaults_page_left_right_layout");
    marginsLayout->addLayout(leftRightLayout);

    EQt::DimensionLineEdit* leftDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(leftDimensionLineEdit, "document_defaults_page_left_line_edit");
    leftRightLayout->addRow(tr("Left"), leftDimensionLineEdit);

    EQt::DimensionLineEdit* rightDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(rightDimensionLineEdit, "document_defaults_page_right_line_edit");
    leftRightLayout->addRow(tr("Right"), rightDimensionLineEdit);

    QFormLayout* topBottomLayout = newFormLayout("document_defaults_page_top_bottom_layout");
    marginsLayout->addLayout(topBottomLayout);

    EQt::DimensionLineEdit* topDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(topDimensionLineEdit, "document_defaults_page_top_line_edit");
    topBottomLayout->addRow(tr("Top"), topDimensionLineEdit);

    EQt::DimensionLineEdit* bottomDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(bottomDimensionLineEdit, "document_defaults_page_bottom_line_edit");
    topBottomLayout->addRow(tr("Bottom"), bottomDimensionLineEdit);

    QLabel* warningLabel = new QLabel(tr("<font color='red'>Note: Default page settings are incompatible</font>"));
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->setVisible(false);
    registerWidget(warningLabel, "document_defaults_page_warning_label");
    pageVerticalLayout->addWidget(warningLabel);

    verticalLayout->addStretch();

    // Paragraph settings

    // Font settings
    QGroupBox* fontGroupBox = new QGroupBox(tr("Font"));
    registerWidget(fontGroupBox, "document_defaults_font_group_box");
    verticalLayout->addWidget(fontGroupBox);

    QFormLayout* fontFormLayout = newFormLayout("document_defaults_font_form_layout");
    fontGroupBox->setLayout(fontFormLayout);

    QFontComboBox* textFontComboBox = new QFontComboBox;
    registerWidget(textFontComboBox, "document_defaults_font_text_combo_box");

    QSpinBox* textFontSpinBox = new QSpinBox;
    registerWidget(textFontSpinBox, "document_defaults_font_text_spin_box");
    textFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* textFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(textFontItalicsCheckBox, "document_defaults_font_text_italics_check_box");

    QCheckBox* textFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(textFontBoldfaceCheckBox, "document_defaults_font_text_boldface_check_box");

    QHBoxLayout* textFontLayout = newHBoxLayout("document_Defaults_font_text_layout");
    textFontLayout->addSpacing(2);
    textFontLayout->addWidget(textFontComboBox);
    textFontLayout->addWidget(textFontSpinBox);
    textFontLayout->addWidget(textFontItalicsCheckBox);
    textFontLayout->addWidget(textFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Text Font: "), textFontLayout);

    QFontComboBox* mathFontComboBox = new QFontComboBox;
    registerWidget(mathFontComboBox, "document_defaults_font_math_combo_box");

    QSpinBox* mathFontSpinBox = new QSpinBox;
    registerWidget(mathFontSpinBox, "document_defaults_font_math_spin_box");
    mathFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* mathFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(mathFontItalicsCheckBox, "document_defaults_font_math_italics_check_box");

    QCheckBox* mathFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(mathFontBoldfaceCheckBox, "document_defaults_font_math_boldface_check_box");

    QHBoxLayout* mathFontLayout = newHBoxLayout("document_defaults_font_math_layout");
    mathFontLayout->addSpacing(2);
    mathFontLayout->addWidget(mathFontComboBox);
    mathFontLayout->addWidget(mathFontSpinBox);
    mathFontLayout->addWidget(mathFontItalicsCheckBox);
    mathFontLayout->addWidget(mathFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Math Font: "), mathFontLayout);

    QFontComboBox* mathIdentifierFontComboBox = new QFontComboBox;
    registerWidget(mathIdentifierFontComboBox, "document_defaults_font_math_identifier_combo_box");

    QSpinBox* mathIdentifierFontSpinBox = new QSpinBox;
    registerWidget(mathIdentifierFontSpinBox, "document_defaults_font_math_identifier_spin_box");
    mathIdentifierFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* mathIdentifierFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(mathIdentifierFontItalicsCheckBox, "document_defaults_font_math_identifier_italics_check_box");

    QCheckBox* mathIdentifierFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(mathIdentifierFontBoldfaceCheckBox, "document_defaults_font_math_identifier_boldface_check_box");

    QHBoxLayout* mathIdentifierFontLayout = newHBoxLayout("document_defaults_font_math_identifier_layout");
    mathIdentifierFontLayout->addSpacing(2);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontComboBox);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontSpinBox);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontItalicsCheckBox);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Identifier Font: "), mathIdentifierFontLayout);


    QFontComboBox* mathFunctionFontComboBox = new QFontComboBox;
    registerWidget(mathFunctionFontComboBox, "document_defaults_font_math_function_combo_box");

    QSpinBox* mathFunctionFontSpinBox = new QSpinBox;
    registerWidget(mathFunctionFontSpinBox, "document_defaults_font_math_function_spin_box");
    mathFunctionFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* mathFunctionFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(mathFunctionFontItalicsCheckBox, "document_defaults_font_math_function_italics_check_box");

    QCheckBox* mathFunctionFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(mathFunctionFontBoldfaceCheckBox, "document_defaults_font_math_function_boldface_check_box");

    QHBoxLayout* mathFunctionFontLayout = newHBoxLayout("document_defaults_font_math_function_layout");
    mathFunctionFontLayout->addSpacing(2);
    mathFunctionFontLayout->addWidget(mathFunctionFontComboBox);
    mathFunctionFontLayout->addWidget(mathFunctionFontSpinBox);
    mathFunctionFontLayout->addWidget(mathFunctionFontItalicsCheckBox);
    mathFunctionFontLayout->addWidget(mathFunctionFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Function Font: "), mathFunctionFontLayout);


    QGroupBox* braceConditionalGroupBox = new QGroupBox(tr("Brace Conditional / Case Operator"));
    registerWidget(braceConditionalGroupBox, "brace_conditional_group_box");
    verticalLayout->addWidget(braceConditionalGroupBox);

    QVBoxLayout* braceConditionalVerticalLayout = newVBoxLayout("brace_conditional_vertical_layout");
    braceConditionalGroupBox->setLayout(braceConditionalVerticalLayout);

    QHBoxLayout* braceConditionalFontLayout = newHBoxLayout("brace_conditional_font_layout");
    braceConditionalVerticalLayout->addLayout(braceConditionalFontLayout);

    QFontComboBox* braceConditionalFontComboBox = new QFontComboBox;
    registerWidget(braceConditionalFontComboBox, "brace_conditional_font_combo_box");

    QSpinBox* braceConditionalFontSpinBox = new QSpinBox;
    registerWidget(braceConditionalFontSpinBox, "brace_conditional_font_spin_box");
    braceConditionalFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* braceConditionalFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(braceConditionalFontItalicsCheckBox, "brace_conditional_font_italics_check_box");

    QCheckBox* braceConditionalFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(braceConditionalFontBoldfaceCheckBox, "brace_conditional_font_bold_check_box");

    braceConditionalFontLayout->addSpacing(2);
    braceConditionalFontLayout->addWidget(braceConditionalFontComboBox);
    braceConditionalFontLayout->addWidget(braceConditionalFontSpinBox);
    braceConditionalFontLayout->addWidget(braceConditionalFontItalicsCheckBox);
    braceConditionalFontLayout->addWidget(braceConditionalFontBoldfaceCheckBox);

    QFormLayout* braceConditionalTextLayout = newFormLayout("brace_conditional_text_layout");
    braceConditionalVerticalLayout->addLayout(braceConditionalTextLayout);

    QLineEdit* braceConditionalPreambleLineEdit = new QLineEdit;
    registerWidget(braceConditionalPreambleLineEdit, "brace_conditional_preamble_line_edit");

    QLineEdit* braceConditionalElseLineEdit = new QLineEdit;
    registerWidget(braceConditionalElseLineEdit, "brace_conditional_else_line_edit");

    QCheckBox* showConditionalElseCheckbox = new QCheckBox(tr("Show"));
    registerWidget(showConditionalElseCheckbox, "show_conditional_else_checkbox");

    QHBoxLayout* braceConditionalElseLayout = newHBoxLayout("brace_conditional_else_layout");
    braceConditionalElseLayout->addWidget(braceConditionalElseLineEdit);
    braceConditionalElseLayout->addSpacing(20);
    braceConditionalElseLayout->addWidget(showConditionalElseCheckbox);

    braceConditionalTextLayout->addRow(tr("Preamble: "), braceConditionalPreambleLineEdit);
    braceConditionalTextLayout->addRow(tr("Else/Otherwise: "), braceConditionalElseLayout);

    QGroupBox* rngGroupBox = new QGroupBox(tr("Random Number Generator"));
    registerWidget(rngGroupBox, "rng_group_box");
    verticalLayout->addWidget(rngGroupBox);

    QFormLayout* rngFormLayout = newFormLayout("rng_form_layout");
    rngGroupBox->setLayout(rngFormLayout);

    QComboBox* rngComboBox = new QComboBox;
    rngComboBox->setEditable(false);
    rngComboBox->addItem(tr("Mercenne Twister 19937-64"), static_cast<int>(Ld::RootElement::RngType::MT19937));
    rngComboBox->addItem(tr("Mercenne Twister 216091 (SIMD)"), static_cast<int>(Ld::RootElement::RngType::MT216091));
    rngComboBox->addItem(tr("XOR-Shift-Rotate 256+"), static_cast<int>(Ld::RootElement::RngType::XORSHIRO256_PLUS));
    rngComboBox->addItem(tr("XOR-Shift-Rotate 256**"), static_cast<int>(Ld::RootElement::RngType::XORSHIRO256_STARS));
    rngComboBox->addItem(tr("True Random Number Generator"), static_cast<int>(Ld::RootElement::RngType::TRNG));
    registerWidget(rngComboBox, "rng_combo_box");

    rngFormLayout->addRow(tr("Default: "), rngComboBox);

    return documentDefaultsWidget;
}


QWidget* ApplicationPreferencesDialog::buildSystemSettingsTab() {
    QWidget* systemSettingsWidget = new QWidget();

    QVBoxLayout* verticalLayout = newVBoxLayout("system_settings_vertical_layout");
    systemSettingsWidget->setLayout(verticalLayout);

    QHBoxLayout* displayHorizontalLayout = newHBoxLayout("system_settings_display_horizontal_layout");
    verticalLayout->addLayout(displayHorizontalLayout);

    QGroupBox* physicalDisplayGroupBox = new QGroupBox(tr("Physical Display Settings"));
    registerWidget(physicalDisplayGroupBox, "system_settings_physical_display_group_box");
    displayHorizontalLayout->addWidget(physicalDisplayGroupBox);

    QVBoxLayout* physicalDisplayLayout = newVBoxLayout("system_settings_physical_display_vertical_layout");
    physicalDisplayGroupBox->setLayout(physicalDisplayLayout);

    QCheckBox* useSystemPhysicalDisplayDefaultsCheckBox = new QCheckBox(tr("Use system defaults"));
    registerWidget(useSystemPhysicalDisplayDefaultsCheckBox, "system_settings_physical_display_use_defaults_check_box");
    physicalDisplayLayout->addWidget(useSystemPhysicalDisplayDefaultsCheckBox);

    QFormLayout* physicalDisplayDpiFormLayout = newFormLayout("system_settings_physical_display_dpi_layout");
    physicalDisplayLayout->addLayout(physicalDisplayDpiFormLayout);

    QSpinBox* horizontalPhysicalResolutionSpinBox = new QSpinBox;
    registerWidget(horizontalPhysicalResolutionSpinBox, "system_settings_physical_display_horizontal_resolution_spin_box");
    physicalDisplayDpiFormLayout->addRow(tr("Horizontal Resolution:"), horizontalPhysicalResolutionSpinBox);

    QSpinBox* verticalPhysicalResolutionSpinBox = new QSpinBox;
    registerWidget(verticalPhysicalResolutionSpinBox, "system_settings_physical_display_vertical_resolution_spin_box");
    physicalDisplayDpiFormLayout->addRow(tr("Vertical Resolution:"), verticalPhysicalResolutionSpinBox);


    QGroupBox* logicalDisplayGroupBox = new QGroupBox(tr("Logical (Font) Display Settings"));
    registerWidget(logicalDisplayGroupBox, "system_settings_logical_display_group_box");
    displayHorizontalLayout->addWidget(logicalDisplayGroupBox);

    QVBoxLayout* logicalDisplayLayout = newVBoxLayout("system_settings_logical_display_vertical_layout");
    logicalDisplayGroupBox->setLayout(logicalDisplayLayout);

    QCheckBox* useSystemLogicalDisplayDefaultsCheckBox = new QCheckBox(tr("Use system defaults"));
    registerWidget(useSystemLogicalDisplayDefaultsCheckBox, "system_settings_logical_display_use_defaults_check_box");
    logicalDisplayLayout->addWidget(useSystemLogicalDisplayDefaultsCheckBox);

    QFormLayout* logicalDisplayDpiFormLayout = newFormLayout("system_settings_logical_display_dpi_layout");
    logicalDisplayLayout->addLayout(logicalDisplayDpiFormLayout);

    QSpinBox* horizontalLogicalResolutionSpinBox = new QSpinBox;
    registerWidget(horizontalLogicalResolutionSpinBox, "system_settings_logical_display_horizontal_resolution_spin_box");
    logicalDisplayDpiFormLayout->addRow(tr("Horizontal Resolution:"), horizontalLogicalResolutionSpinBox);

    QSpinBox* verticalLogicalResolutionSpinBox = new QSpinBox;
    registerWidget(verticalLogicalResolutionSpinBox, "system_settings_logical_display_vertical_resolution_spin_box");
    logicalDisplayDpiFormLayout->addRow(tr("Vertical Resolution:"), verticalLogicalResolutionSpinBox);


    return systemSettingsWidget;
}


QWidget* ApplicationPreferencesDialog::buildPrivacyTab() {
    QWidget* privacyWidget = new QWidget();

    QVBoxLayout* verticalLayout = newVBoxLayout("privacy_vertical_layout");
    privacyWidget->setLayout(verticalLayout);

    QString message = tr(
        "<p>%1 is committed to protecting our customer's privacy.</p>"
        "<p>For details on how we collect and use your "
        "information, please review our privacy policy at <a href=\"%2\">%2</a></p>"
    ).arg(Application::organizationName(), PRIVACY_POLICY_URL);

    verticalLayout->addStretch(1);

    QLabel* privacyMessageLabel = new QLabel(message);
    privacyMessageLabel->setWordWrap(true);
    privacyMessageLabel->setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);
    privacyMessageLabel->setOpenExternalLinks(true);
    verticalLayout->addWidget(privacyMessageLabel);

    verticalLayout->addSpacing(20);

    QString checkBoxText = tr("Send anonymized usage statistics to %1").arg(Application::organizationName());
    QCheckBox* sendUsageDataCheckBox = new QCheckBox(checkBoxText);
    registerWidget(sendUsageDataCheckBox, "send_usage_data_check_box");

    verticalLayout->addWidget(sendUsageDataCheckBox);

    verticalLayout->addStretch(1);

    return privacyWidget;
}


QWidget* ApplicationPreferencesDialog::buildApplicationDefaultsTab() {
    QWidget* applicationDefaultsWidget = new QWidget();

    QVBoxLayout* verticalLayout = newVBoxLayout("application_defaults_vertical_layout");
    applicationDefaultsWidget->setLayout(verticalLayout);

    QGroupBox* visibleDialogsGroupBox = new QGroupBox(tr("Dialogs"));
    registerWidget(visibleDialogsGroupBox, "visible_dialogs_group_box");
    verticalLayout->addWidget(visibleDialogsGroupBox);

    QVBoxLayout* visibleDialogsLayout = newVBoxLayout("application_defaults_visible_dialogs_vertical_layout");
    visibleDialogsGroupBox->setLayout(visibleDialogsLayout);

    QCheckBox* hideWelcomeMessageDialogCheckBox = new QCheckBox(tr("Hide Welcome Message Dialog"));
    registerWidget(hideWelcomeMessageDialogCheckBox, "hide_welcome_message_check_box");
    visibleDialogsLayout->addWidget(hideWelcomeMessageDialogCheckBox);

    verticalLayout->addStretch(1);

    return applicationDefaultsWidget;
}


void ApplicationPreferencesDialog::configureDialog() {
    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setLayout(verticalLayout);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setAcceptDrops(false);
    tabWidget->setMovable(false);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setTabsClosable(false);
    tabWidget->setElideMode(Qt::ElideNone);
    registerWidget(tabWidget, "tab_widget");
    registerWidget(tabWidget->tabBar(), "tab_bar");

    verticalLayout->addWidget(tabWidget);

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    registerWidget(dialogButtonBox, "dialog_button_box");
    registerWidget(dialogButtonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(dialogButtonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    verticalLayout->addWidget(dialogButtonBox);

    tabWidget->addTab(buildDocumentDefaultsTab(), tr("Defaults For New &Documents"));
    tabWidget->addTab(buildSystemSettingsTab(), tr("&System Settings"));
    tabWidget->addTab(buildPrivacyTab(), tr("&Privacy"));
    tabWidget->addTab(buildApplicationDefaultsTab(), tr("Application Defaults"));
}


Ld::PageFormat ApplicationPreferencesDialog::generatePageFormat() {
    EQt::DimensionLineEdit* leftLineEdit = widget<EQt::DimensionLineEdit>("document_defaults_page_left_line_edit");
    EQt::DimensionLineEdit* topLineEdit  = widget<EQt::DimensionLineEdit>("document_defaults_page_top_line_edit");
    EQt::DimensionLineEdit* rightLineEdit = widget<EQt::DimensionLineEdit>("document_defaults_page_right_line_edit");
    EQt::DimensionLineEdit* bottomLineEdit  = widget<EQt::DimensionLineEdit>("document_defaults_page_bottom_line_edit");

    bool valuesValid =    leftLineEdit->valueOk()
                       && topLineEdit->valueOk()
                       && rightLineEdit->valueOk()
                       && bottomLineEdit->valueOk();

    Ld::PageFormat result;

    if (valuesValid) {
        QCheckBox* orientationCheckBox  = widget<QCheckBox>("document_defaults_page_landscape_orientation_check_box");
        QComboBox* pageSizeComboBox     = widget<QComboBox>("document_defaults_page_size_combo_box");

        bool           landscapeOrientation = orientationCheckBox->isChecked();
        QString        pageDescription      = pageSizeComboBox->currentText();
        Util::PageSize pageSize             = Util::PageSize::fromDescription(pageDescription);

        double leftMargin   = leftLineEdit->value();
        double rightMargin  = rightLineEdit->value();
        double topMargin    = topLineEdit->value();
        double bottomMargin = bottomLineEdit->value();

        result = Ld::PageFormat(pageSize);
        result.setLandscape(landscapeOrientation);
        result.setLeftMargin(leftMargin);
        result.setRightMargin(rightMargin);
        result.setTopMargin(topMargin);
        result.setBottomMargin(bottomMargin);

        if (result.isInvalid()) {
            result = Ld::PageFormat();
        }
    }

    return result;
}
