/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DocumentPreferencesDialog class.
***********************************************************************************************************************/

#include <QSharedPointer>
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
#include <QRegularExpressionValidator>
#include <QRegularExpressionValidator>
#include <QScreen>

#include <cstring>

#include <util_page_size.h>

#include <ud_usage_data.h>

#include <eqt_application.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_programmatic_dialog.h>

#include <ld_page_format.h>
#include <ld_character_format.h>

#include "application.h"
#include "document_preferences_dialog.h"

DocumentPreferencesDialog::DocumentPreferencesDialog(
        QWidget* parent
    ):EQt::ProgrammaticDialog(
        "DocumentPreferencesDialog",
        parent
    ) {
    configureDialog();
    runBuilders();
}


DocumentPreferencesDialog::~DocumentPreferencesDialog() {}


QSharedPointer<Ld::PageFormat> DocumentPreferencesDialog::pageFormat() const {
    return QSharedPointer<Ld::PageFormat>(pageFormatFromInput().clone().dynamicCast<Ld::PageFormat>());
}


bool DocumentPreferencesDialog::pageFormatChanged() const {
    return pageFormatFromInput().toString() != inputPageFormat->toString();
}


QFont DocumentPreferencesDialog::textFont() const {
    QFontComboBox* textFontComboBox         = widget<QFontComboBox>("font_text_combo_box");
    QSpinBox*      textFontSpinBox          = widget<QSpinBox>("font_text_spin_box");
    QCheckBox*     textFontItalicsCheckBox  = widget<QCheckBox>("font_text_italics_check_box");
    QCheckBox*     textFontBoldfaceCheckBox = widget<QCheckBox>("font_text_bold_check_box");

    QFont font = textFontComboBox->currentFont();
    font.setPointSize(textFontSpinBox->value());
    font.setItalic(textFontItalicsCheckBox->isChecked());
    font.setWeight(textFontBoldfaceCheckBox->isChecked() ? QFont::Bold : QFont::Normal);

    return font;
}


bool DocumentPreferencesDialog::textFontChanged() const {
    return textFont() != inputTextFont;
}


QFont DocumentPreferencesDialog::mathFont() const {
    QFontComboBox* mathFontComboBox         = widget<QFontComboBox>("font_math_combo_box");
    QSpinBox*      mathFontSpinBox          = widget<QSpinBox>("font_math_spin_box");
    QCheckBox*     mathFontItalicsCheckBox  = widget<QCheckBox>("font_math_italics_check_box");
    QCheckBox*     mathFontBoldfaceCheckBox = widget<QCheckBox>("font_math_bold_check_box");

    QFont font = mathFontComboBox->currentFont();
    font.setPointSize(mathFontSpinBox->value());
    font.setItalic(mathFontItalicsCheckBox->isChecked());
    font.setWeight(mathFontBoldfaceCheckBox->isChecked() ? QFont::Bold : QFont::Normal);

    return font;
}


bool DocumentPreferencesDialog::mathFontChanged() const {
    return mathFont() != inputMathFont;
}


QFont DocumentPreferencesDialog::mathIdentifierFont() const {
    QFontComboBox* mathIdentifierFontComboBox         = widget<QFontComboBox>("font_math_identifier_combo_box");
    QSpinBox*      mathIdentifierFontSpinBox          = widget<QSpinBox>("font_math_identifier_spin_box");
    QCheckBox*     mathIdentifierFontItalicsCheckBox  = widget<QCheckBox>("font_math_identifier_italics_check_box");
    QCheckBox*     mathIdentifierFontBoldfaceCheckBox = widget<QCheckBox>("font_math_identifier_bold_check_box");

    QFont font = mathIdentifierFontComboBox->currentFont();
    font.setPointSize(mathIdentifierFontSpinBox->value());
    font.setItalic(mathIdentifierFontItalicsCheckBox->isChecked());
    font.setWeight(mathIdentifierFontBoldfaceCheckBox->isChecked() ? QFont::Bold : QFont::Normal);

    return font;
}


bool DocumentPreferencesDialog::mathIdentifierFontChanged() const {
    return mathIdentifierFont() != inputMathIdentifierFont;
}


QFont DocumentPreferencesDialog::mathFunctionFont() const {
    QFontComboBox* mathFunctionFontComboBox         = widget<QFontComboBox>("font_math_function_combo_box");
    QSpinBox*      mathFunctionFontSpinBox          = widget<QSpinBox>("font_math_function_spin_box");
    QCheckBox*     mathFunctionFontItalicsCheckBox  = widget<QCheckBox>("font_math_function_italics_check_box");
    QCheckBox*     mathFunctionFontBoldfaceCheckBox = widget<QCheckBox>("font_math_function_bold_check_box");

    QFont font = mathFunctionFontComboBox->currentFont();
    font.setPointSize(mathFunctionFontSpinBox->value());
    font.setItalic(mathFunctionFontItalicsCheckBox->isChecked());
    font.setWeight(mathFunctionFontBoldfaceCheckBox->isChecked() ? QFont::Bold : QFont::Normal);

    return font;
}


bool DocumentPreferencesDialog::mathFunctionFontChanged() const {
    return mathFunctionFont() != inputMathFunctionFont;
}


QFont DocumentPreferencesDialog::braceConditionalFont() const {
    QFontComboBox* braceConditionalFontComboBox         = widget<QFontComboBox>("brace_conditional_font_combo_box");
    QSpinBox*      braceConditionalFontSpinBox          = widget<QSpinBox>("brace_conditional_font_spin_box");
    QCheckBox*     braceConditionalFontItalicsCheckBox  = widget<QCheckBox>("brace_conditional_font_italics_check_box");
    QCheckBox*     braceConditionalFontBoldfaceCheckBox = widget<QCheckBox>("brace_conditional_font_bold_check_box");

    QFont font = braceConditionalFontComboBox->currentFont();
    font.setPointSize(braceConditionalFontSpinBox->value());
    font.setItalic(braceConditionalFontItalicsCheckBox->isChecked());
    font.setWeight(braceConditionalFontBoldfaceCheckBox->isChecked() ? QFont::Bold : QFont::Normal);

    return font;
}


bool DocumentPreferencesDialog::braceConditionalFontChanged() const {
    return braceConditionalFont() != inputBraceConditionalFont;
}


QString DocumentPreferencesDialog::braceConditionalPreambleString() const {
    QLineEdit* braceConditionalPreambleLineEdit = widget<QLineEdit>("brace_conditional_preamble_line_edit");
    return braceConditionalPreambleLineEdit->text();
}


bool DocumentPreferencesDialog::braceConditionalPreambleStringChanged() const {
    return inputBraceConditionalIfString != braceConditionalPreambleString();
}


QString DocumentPreferencesDialog::braceConditionalElseString() const {
    QLineEdit* braceConditionalElseLineEdit = widget<QLineEdit>("brace_conditional_else_line_edit");
    return braceConditionalElseLineEdit->text();
}


bool DocumentPreferencesDialog::braceConditionalElseStringChanged() const {
    return inputBraceConditionalIfString != braceConditionalElseString();
}


bool DocumentPreferencesDialog::braceConditionalElseClauseIncluded() const {
    QCheckBox* showConditionalElseCheckbox = widget<QCheckBox>("show_conditional_else_checkbox");
    return showConditionalElseCheckbox->isChecked();
}


DocumentPreferencesDialog::RngType DocumentPreferencesDialog::rngType() const {
    QComboBox* rngComboBox   = widget<QComboBox>("rng_combo_box");
    return static_cast<RngType>(rngComboBox->currentData().toInt());
}


const DocumentPreferencesDialog::RngSeed& DocumentPreferencesDialog::rngSeed() const {
    QLineEdit* rngSeed1LineEdit = widget<QLineEdit>("rngSeed1LineEdit");
    QLineEdit* rngSeed2LineEdit = widget<QLineEdit>("rngSeed2LineEdit");
    QLineEdit* rngSeed3LineEdit = widget<QLineEdit>("rngSeed3LineEdit");
    QLineEdit* rngSeed4LineEdit = widget<QLineEdit>("rngSeed4LineEdit");

    bool ok1;
    bool ok2;
    bool ok3;
    bool ok4;

    std::uint64_t seed1 = rngSeed1LineEdit->text().toULongLong(&ok1);
    std::uint64_t seed2 = rngSeed2LineEdit->text().toULongLong(&ok2);
    std::uint64_t seed3 = rngSeed3LineEdit->text().toULongLong(&ok3);
    std::uint64_t seed4 = rngSeed4LineEdit->text().toULongLong(&ok4);

    outputRngSeed[0] = ok1 ? seed1 : 0;
    outputRngSeed[1] = ok2 ? seed2 : 0;
    outputRngSeed[2] = ok3 ? seed3 : 0;
    outputRngSeed[3] = ok4 ? seed4 : 0;

    return outputRngSeed;
}


bool DocumentPreferencesDialog::useSeedAsPreset() const {
    QCheckBox* rngUseAsPresetCheckBox = widget<QCheckBox>("rng_use_as_preset_check_box");
    return rngUseAsPresetCheckBox->isChecked();
}


void DocumentPreferencesDialog::populate() {
    QComboBox*              pageSizeComboBox             = widget<QComboBox>("page_size_combo_box");
    QCheckBox*              landscapeOrientationCheckBox = widget<QCheckBox>("page_landscape_orientation_check_box");
    EQt::DimensionLineEdit* leftDimensionLineEdit        = widget<EQt::DimensionLineEdit>("page_left_line_edit");
    EQt::DimensionLineEdit* rightDimensionLineEdit       = widget<EQt::DimensionLineEdit>("page_right_line_edit");
    EQt::DimensionLineEdit* topDimensionLineEdit         = widget<EQt::DimensionLineEdit>("page_top_line_edit");
    EQt::DimensionLineEdit* bottomDimensionLineEdit      = widget<EQt::DimensionLineEdit>("page_bottom_line_edit");

    Util::PageSize defaultPageSize = inputPageFormat->pageSize();
    bool           landscape       = inputPageFormat->landscape();

    bool           useMetric       = (Application::locale()->measurementSystem() != QLocale::ImperialSystem);

    pageSizeComboBox->setCurrentText(defaultPageSize.description(useMetric));
    landscapeOrientationCheckBox->setChecked(landscape);

    leftDimensionLineEdit->setValue(inputPageFormat->leftMargin());
    rightDimensionLineEdit->setValue(inputPageFormat->rightMargin());
    topDimensionLineEdit->setValue(inputPageFormat->topMargin());
    bottomDimensionLineEdit->setValue(inputPageFormat->bottomMargin());


    QFontComboBox* textFontComboBox         = widget<QFontComboBox>("font_text_combo_box");
    QSpinBox*      textFontSpinBox          = widget<QSpinBox>("font_text_spin_box");
    QCheckBox*     textFontItalicsCheckBox  = widget<QCheckBox>("font_text_italics_check_box");
    QCheckBox*     textFontBoldfaceCheckBox = widget<QCheckBox>("font_text_bold_check_box");

    QFontComboBox* mathFontComboBox         = widget<QFontComboBox>("font_math_combo_box");
    QSpinBox*      mathFontSpinBox          = widget<QSpinBox>("font_math_spin_box");
    QCheckBox*     mathFontItalicsCheckBox  = widget<QCheckBox>("font_math_italics_check_box");
    QCheckBox*     mathFontBoldfaceCheckBox = widget<QCheckBox>("font_math_bold_check_box");

    QFontComboBox* mathIdentifierFontComboBox         = widget<QFontComboBox>("font_math_identifier_combo_box");
    QSpinBox*      mathIdentifierFontSpinBox          = widget<QSpinBox>("font_math_identifier_spin_box");
    QCheckBox*     mathIdentifierFontItalicsCheckBox  = widget<QCheckBox>("font_math_identifier_italics_check_box");
    QCheckBox*     mathIdentifierFontBoldfaceCheckBox = widget<QCheckBox>("font_math_identifier_bold_check_box");

    QFontComboBox* mathFunctionFontComboBox         = widget<QFontComboBox>("font_math_function_combo_box");
    QSpinBox*      mathFunctionFontSpinBox          = widget<QSpinBox>("font_math_function_spin_box");
    QCheckBox*     mathFunctionFontItalicsCheckBox  = widget<QCheckBox>("font_math_function_italics_check_box");
    QCheckBox*     mathFunctionFontBoldfaceCheckBox = widget<QCheckBox>("font_math_function_bold_check_box");

    QFontComboBox* braceConditionalFontComboBox         = widget<QFontComboBox>("brace_conditional_font_combo_box");
    QSpinBox*      braceConditionalFontSpinBox          = widget<QSpinBox>("brace_conditional_font_spin_box");
    QCheckBox*     braceConditionalFontItalicsCheckBox  = widget<QCheckBox>("brace_conditional_font_italics_check_box");
    QCheckBox*     braceConditionalFontBoldfaceCheckBox = widget<QCheckBox>("brace_conditional_font_bold_check_box");

    textFontComboBox->setCurrentFont(inputTextFont);
    textFontSpinBox->setValue(inputTextFont.pointSize());
    textFontItalicsCheckBox->setChecked(inputTextFont.italic());
    textFontBoldfaceCheckBox->setChecked(inputTextFont.bold());

    mathFontComboBox->setCurrentFont(inputMathFont);
    mathFontSpinBox->setValue(inputMathFont.pointSize());
    mathFontItalicsCheckBox->setChecked(inputMathFont.italic());
    mathFontBoldfaceCheckBox->setChecked(inputMathFont.bold());

    mathIdentifierFontComboBox->setCurrentFont(inputMathIdentifierFont);
    mathIdentifierFontSpinBox->setValue(inputMathIdentifierFont.pointSize());
    mathIdentifierFontItalicsCheckBox->setChecked(inputMathIdentifierFont.italic());
    mathIdentifierFontBoldfaceCheckBox->setChecked(inputMathIdentifierFont.bold());

    mathFunctionFontComboBox->setCurrentFont(inputMathFunctionFont);
    mathFunctionFontSpinBox->setValue(inputMathFunctionFont.pointSize());
    mathFunctionFontItalicsCheckBox->setChecked(inputMathFunctionFont.italic());
    mathFunctionFontBoldfaceCheckBox->setChecked(inputMathFunctionFont.bold());

    braceConditionalFontComboBox->setCurrentFont(inputBraceConditionalFont);
    braceConditionalFontSpinBox->setValue(inputBraceConditionalFont.pointSize());
    braceConditionalFontItalicsCheckBox->setChecked(inputBraceConditionalFont.italic());
    braceConditionalFontBoldfaceCheckBox->setChecked(inputBraceConditionalFont.bold());

    QLineEdit* braceConditionalPreambleLineEdit = widget<QLineEdit>("brace_conditional_preamble_line_edit");
    braceConditionalPreambleLineEdit->setText(inputBraceConditionalIfString);

    QLineEdit* braceConditionalElseLineEdit = widget<QLineEdit>("brace_conditional_else_line_edit");
    braceConditionalElseLineEdit->setText(inputBraceConditionalElseString);

    QCheckBox* showConditionalElseCheckbox = widget<QCheckBox>("show_conditional_else_checkbox");
    showConditionalElseCheckbox->setChecked(inputBraceConditionalElseClauseIncluded);
    braceConditionalElseLineEdit->setEnabled(inputBraceConditionalElseClauseIncluded);

    QComboBox* rngComboBox   = widget<QComboBox>("rng_combo_box");
    int        rngTypeValue  = static_cast<int>(inputRngType);
    int        comboBoxIndex = 0;
    while (comboBoxIndex < rngComboBox->count() && rngComboBox->itemData(comboBoxIndex).toInt() != rngTypeValue) {
        ++comboBoxIndex;
    }

    if (comboBoxIndex < rngComboBox->count()) {
        rngComboBox->setCurrentIndex(comboBoxIndex);
    } else {
        rngComboBox->setCurrentIndex(0);
    }

    QCheckBox* rngUseAsPresetCheckBox = widget<QCheckBox>("rng_use_as_preset_check_box");
    rngUseAsPresetCheckBox->setChecked(inputUseRngSeedAsPreset);

    QLineEdit* rngSeed1LineEdit = widget<QLineEdit>("rngSeed1LineEdit");
    rngSeed1LineEdit->setText(QString::number(inputRngSeed[0]));

    QLineEdit* rngSeed2LineEdit = widget<QLineEdit>("rngSeed2LineEdit");
    rngSeed2LineEdit->setText(QString::number(inputRngSeed[1]));

    QLineEdit* rngSeed3LineEdit = widget<QLineEdit>("rngSeed3LineEdit");
    rngSeed3LineEdit->setText(QString::number(inputRngSeed[2]));

    QLineEdit* rngSeed4LineEdit = widget<QLineEdit>("rngSeed4LineEdit");
    rngSeed4LineEdit->setText(QString::number(inputRngSeed[3]));

    rngTypeChanged(comboBoxIndex);
}


void DocumentPreferencesDialog::setPageFormat(QSharedPointer<Ld::PageFormat> newPageFormat) {
    inputPageFormat = newPageFormat->clone().dynamicCast<Ld::PageFormat>();

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setTextFont(const QFont& newTextFont) {
    inputTextFont = newTextFont;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setMathFont(const QFont& newMathFont) {
    inputMathFont = newMathFont;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setMathIdentifierFont(const QFont& newMathIdentifierFont) {
    inputMathIdentifierFont = newMathIdentifierFont;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setMathFunctionFont(const QFont& newMathFunctionFont) {
    inputMathFunctionFont = newMathFunctionFont;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setBraceConditionalFont(const QFont& newBraceConditionalFont) {
    inputBraceConditionalFont = newBraceConditionalFont;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setBraceConditionalPreambleString(const QString& newBraceConditionalIfString) {
    inputBraceConditionalIfString = newBraceConditionalIfString;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setBraceConditionalElseString(const QString& newBraceConditionalElseString) {
    inputBraceConditionalElseString = newBraceConditionalElseString;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setBraceConditionalElseClauseIncluded(bool includeElseClause) {
    inputBraceConditionalElseClauseIncluded = includeElseClause;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setRngType(DocumentPreferencesDialog::RngType newRngType) {
    inputRngType = newRngType;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setRngSeed(const DocumentPreferencesDialog::RngSeed& newRngSeed) {
    std::memcpy(inputRngSeed, newRngSeed, sizeof(RngSeed));

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::setUseSeedAsPreset(bool nowUseSeedAsPreset) {
    inputUseRngSeedAsPreset = nowUseSeedAsPreset;

    if (isVisible()) {
        populate();
    }
}


void DocumentPreferencesDialog::apply() {
    emit applyClicked(this);
}


void DocumentPreferencesDialog::bind() {
    QComboBox*              pageSizeComboBox             = widget<QComboBox>("page_size_combo_box");
    QCheckBox*              landscapeOrientationCheckBox = widget<QCheckBox>("page_landscape_orientation_check_box");
    EQt::DimensionLineEdit* leftDimensionLineEdit        = widget<EQt::DimensionLineEdit>("page_left_line_edit");
    EQt::DimensionLineEdit* rightDimensionLineEdit       = widget<EQt::DimensionLineEdit>("page_right_line_edit");
    EQt::DimensionLineEdit* topDimensionLineEdit         = widget<EQt::DimensionLineEdit>("page_top_line_edit");
    EQt::DimensionLineEdit* bottomDimensionLineEdit      = widget<EQt::DimensionLineEdit>("page_bottom_line_edit");

    connect(
        pageSizeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [=](int) {
            pageSettingsChanged();
        }
    );

    connect(
        landscapeOrientationCheckBox,
        &QCheckBox::clicked,
        this,
        &DocumentPreferencesDialog::pageSettingsChanged
    );

    connect(
        leftDimensionLineEdit,
        &QLineEdit::editingFinished,
        this,
        &DocumentPreferencesDialog::pageSettingsChanged
    );

    connect(
        rightDimensionLineEdit,
        &QLineEdit::editingFinished,
        this,
        &DocumentPreferencesDialog::pageSettingsChanged
    );

    connect(
        topDimensionLineEdit,
        &QLineEdit::editingFinished,
        this,
        &DocumentPreferencesDialog::pageSettingsChanged
    );

    connect(
        bottomDimensionLineEdit,
        &QLineEdit::editingFinished,
        this,
        &DocumentPreferencesDialog::pageSettingsChanged
    );

    QCheckBox* showConditionalElseCheckbox = widget<QCheckBox>("show_conditional_else_checkbox");
    QLineEdit* braceConditionalElseLineEdit = widget<QLineEdit>("brace_conditional_else_line_edit");

    connect(showConditionalElseCheckbox, &QCheckBox::clicked, braceConditionalElseLineEdit, &QLineEdit::setEnabled);

    QComboBox* rngComboBox = widget<QComboBox>("rng_combo_box");
    connect(
        rngComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &DocumentPreferencesDialog::rngTypeChanged
    );

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(okPushButton, &QPushButton::clicked, this, &DocumentPreferencesDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &DocumentPreferencesDialog::reject);

    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    if (applyPushButton != Q_NULLPTR) {
        connect(applyPushButton, &QPushButton::clicked, this, &DocumentPreferencesDialog::apply);
    }
}


void DocumentPreferencesDialog::pageSettingsChanged() {
    Ld::PageFormat pageFormat      = pageFormatFromInput();
    bool           pageFormatValid = pageFormat.isValid();

    QLabel* warningLabel = widget<QLabel>("page_warning_label");
    warningLabel->setVisible(!pageFormatValid);

    conditionallyEnableOkButton("document_defaults_tab", pageFormatValid);
}


void DocumentPreferencesDialog::rngTypeChanged(int index) {
    QComboBox* rngComboBox = widget<QComboBox>("rng_combo_box");
    RngType    rngType     = static_cast<RngType>(rngComboBox->itemData(index).toInt());
    bool       enableField = (rngType != RngType::TRNG);

    QCheckBox* rngUseAsPresetCheckBox = widget<QCheckBox>("rng_use_as_preset_check_box");
    rngUseAsPresetCheckBox->setEnabled(enableField);

    QLineEdit* rngSeed1LineEdit = widget<QLineEdit>("rngSeed1LineEdit");
    rngSeed1LineEdit->setEnabled(enableField);

    QLineEdit* rngSeed2LineEdit = widget<QLineEdit>("rngSeed2LineEdit");
    rngSeed2LineEdit->setEnabled(enableField);

    QLineEdit* rngSeed3LineEdit = widget<QLineEdit>("rngSeed3LineEdit");
    rngSeed3LineEdit->setEnabled(enableField);

    QLineEdit* rngSeed4LineEdit = widget<QLineEdit>("rngSeed4LineEdit");
    rngSeed4LineEdit->setEnabled(enableField);

    QGroupBox* rngSeedsGroupBox = widget<QGroupBox>("rng_seeds_group_box");
    rngSeedsGroupBox->setEnabled(enableField);
}


void DocumentPreferencesDialog::configureDialog() {
    setWindowTitle(tr("Document Preferences"));

    inputBraceConditionalElseClauseIncluded = true;

    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setLayout(verticalLayout);

    QGroupBox* pageGroupBox = new QGroupBox(tr("Page"));
    registerWidget(pageGroupBox, "page_group_box");
    verticalLayout->addWidget(pageGroupBox);

    QVBoxLayout* pageVerticalLayout = newVBoxLayout("page_vertical_layout");
    pageGroupBox->setLayout(pageVerticalLayout);

    QComboBox* pageSizeComboBox = new QComboBox;
    registerWidget(pageSizeComboBox, "page_size_combo_box");
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
    registerWidget(landscapeOrientationCheckBox, "page_landscape_orientation_check_box");
    pageVerticalLayout->addWidget(landscapeOrientationCheckBox);

    QHBoxLayout* marginsLayout = newHBoxLayout("page_margins_layout");
    pageVerticalLayout->addLayout(marginsLayout);

    QFormLayout* leftRightLayout = newFormLayout("page_left_right_layout");
    marginsLayout->addLayout(leftRightLayout);

    EQt::DimensionLineEdit* leftDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(leftDimensionLineEdit, "page_left_line_edit");
    leftRightLayout->addRow(tr("Left"), leftDimensionLineEdit);

    EQt::DimensionLineEdit* rightDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(rightDimensionLineEdit, "page_right_line_edit");
    leftRightLayout->addRow(tr("Right"), rightDimensionLineEdit);

    QFormLayout* topBottomLayout = newFormLayout("page_top_bottom_layout");
    marginsLayout->addLayout(topBottomLayout);

    EQt::DimensionLineEdit* topDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(topDimensionLineEdit, "page_top_line_edit");
    topBottomLayout->addRow(tr("Top"), topDimensionLineEdit);

    EQt::DimensionLineEdit* bottomDimensionLineEdit = new EQt::DimensionLineEdit;
    registerWidget(bottomDimensionLineEdit, "page_bottom_line_edit");
    topBottomLayout->addRow(tr("Bottom"), bottomDimensionLineEdit);

    QLabel* warningLabel = new QLabel(tr("<font color='red'>Note: Default page settings are incompatible</font>"));
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->setVisible(false);
    registerWidget(warningLabel, "page_warning_label");
    pageVerticalLayout->addWidget(warningLabel);


    verticalLayout->addStretch();


    QGroupBox* fontGroupBox = new QGroupBox(tr("Font"));
    registerWidget(fontGroupBox, "font_group_box");
    verticalLayout->addWidget(fontGroupBox);

    QFormLayout* fontFormLayout = newFormLayout("font_form_layout");
    fontGroupBox->setLayout(fontFormLayout);

    QFontComboBox* textFontComboBox = new QFontComboBox;
    registerWidget(textFontComboBox, "font_text_combo_box");

    QSpinBox* textFontSpinBox = new QSpinBox;
    registerWidget(textFontSpinBox, "font_text_spin_box");
    textFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* textFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(textFontItalicsCheckBox, "font_text_italics_check_box");

    QCheckBox* textFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(textFontBoldfaceCheckBox, "font_text_bold_check_box");

    QHBoxLayout* textFontLayout = newHBoxLayout("font_text_layout");
    textFontLayout->addSpacing(2);
    textFontLayout->addWidget(textFontComboBox);
    textFontLayout->addWidget(textFontSpinBox);
    textFontLayout->addWidget(textFontItalicsCheckBox);
    textFontLayout->addWidget(textFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Text Font: "), textFontLayout);

    QFontComboBox* mathFontComboBox = new QFontComboBox;
    registerWidget(mathFontComboBox, "font_math_combo_box");

    QSpinBox* mathFontSpinBox = new QSpinBox;
    registerWidget(mathFontSpinBox, "font_math_spin_box");
    mathFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* mathFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(mathFontItalicsCheckBox, "font_math_italics_check_box");

    QCheckBox* mathFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(mathFontBoldfaceCheckBox, "font_math_bold_check_box");

    QHBoxLayout* mathFontLayout = newHBoxLayout("font_math_layout");
    mathFontLayout->addSpacing(2);
    mathFontLayout->addWidget(mathFontComboBox);
    mathFontLayout->addWidget(mathFontSpinBox);
    mathFontLayout->addWidget(mathFontItalicsCheckBox);
    mathFontLayout->addWidget(mathFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Math Font: "), mathFontLayout);

    QFontComboBox* mathIdentifierFontComboBox = new QFontComboBox;
    registerWidget(mathIdentifierFontComboBox, "font_math_identifier_combo_box");

    QSpinBox* mathIdentifierFontSpinBox = new QSpinBox;
    registerWidget(mathIdentifierFontSpinBox, "font_math_identifier_spin_box");
    mathIdentifierFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* mathIdentifierFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(mathIdentifierFontItalicsCheckBox, "font_math_identifier_italics_check_box");

    QCheckBox* mathIdentifierFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(mathIdentifierFontBoldfaceCheckBox, "font_math_identifier_bold_check_box");

    QHBoxLayout* mathIdentifierFontLayout = newHBoxLayout("font_math_identifier_layout");
    mathIdentifierFontLayout->addSpacing(2);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontComboBox);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontSpinBox);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontItalicsCheckBox);
    mathIdentifierFontLayout->addWidget(mathIdentifierFontBoldfaceCheckBox);

    fontFormLayout->addRow(tr("Identifier Font: "), mathIdentifierFontLayout);

    QFontComboBox* mathFunctionFontComboBox = new QFontComboBox;
    registerWidget(mathFunctionFontComboBox, "font_math_function_combo_box");

    QSpinBox* mathFunctionFontSpinBox = new QSpinBox;
    registerWidget(mathFunctionFontSpinBox, "font_math_function_spin_box");
    mathFunctionFontSpinBox->setMinimum(Ld::CharacterFormat::minimumFontSize);

    QCheckBox* mathFunctionFontItalicsCheckBox = new QCheckBox(tr("Italics"));
    registerWidget(mathFunctionFontItalicsCheckBox, "font_math_function_italics_check_box");

    QCheckBox* mathFunctionFontBoldfaceCheckBox = new QCheckBox(tr("Bold"));
    registerWidget(mathFunctionFontBoldfaceCheckBox, "font_math_function_bold_check_box");

    QHBoxLayout* mathFunctionFontLayout = newHBoxLayout("font_math_function_layout");
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

    QHBoxLayout* rngHorizontalLayout = newHBoxLayout("rng_horizontal_layout");
    rngGroupBox->setLayout(rngHorizontalLayout);

    QVBoxLayout* rngLeftVerticalLayout = newVBoxLayout("rng_left_vertical_layout");
    rngHorizontalLayout->addLayout(rngLeftVerticalLayout);

    rngLeftVerticalLayout->addStretch(1);

    QComboBox* rngComboBox = new QComboBox;
    rngComboBox->setEditable(false);
    rngComboBox->addItem(tr("Mercenne Twister 19937-64"), static_cast<int>(Ld::RootElement::RngType::MT19937));
    rngComboBox->addItem(tr("Mercenne Twister 216091 (SIMD)"), static_cast<int>(Ld::RootElement::RngType::MT216091));
    rngComboBox->addItem(tr("XOR-Shift-Rotate 256+"), static_cast<int>(Ld::RootElement::RngType::XORSHIRO256_PLUS));
    rngComboBox->addItem(tr("XOR-Shift-Rotate 256**"), static_cast<int>(Ld::RootElement::RngType::XORSHIRO256_STARS));
    rngComboBox->addItem(tr("True Random Number Generator"), static_cast<int>(Ld::RootElement::RngType::TRNG));
    registerWidget(rngComboBox, "rng_combo_box");

    rngLeftVerticalLayout->addWidget(rngComboBox);

    QCheckBox* rngUseAsPresetCheckBox = new QCheckBox(tr("Use seed for future runs"));
    registerWidget(rngUseAsPresetCheckBox, "rng_use_as_preset_check_box");
    rngLeftVerticalLayout->addWidget(rngUseAsPresetCheckBox);

    rngLeftVerticalLayout->addStretch(2);

    QGroupBox* rngSeedsGroupBox = new QGroupBox(tr("Seeds"));
    registerWidget(rngSeedsGroupBox, "rng_seeds_group_box");
    rngHorizontalLayout->addWidget(rngSeedsGroupBox);

    QVBoxLayout* rngSeedsVerticalLayout = newVBoxLayout("rng_seeds_vertical_layout");
    rngSeedsGroupBox->setLayout(rngSeedsVerticalLayout);

    QRegularExpression regularExpression("[1-9]?[0-9]*");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regularExpression, this);

    QLineEdit* rngSeed1LineEdit = new QLineEdit();
    rngSeed1LineEdit->setValidator(validator);
    registerWidget(rngSeed1LineEdit, "rngSeed1LineEdit");
    rngSeedsVerticalLayout->addWidget(rngSeed1LineEdit);

    QLineEdit* rngSeed2LineEdit = new QLineEdit();
    rngSeed2LineEdit->setValidator(validator);
    registerWidget(rngSeed2LineEdit, "rngSeed2LineEdit");
    rngSeedsVerticalLayout->addWidget(rngSeed2LineEdit);

    QLineEdit* rngSeed3LineEdit = new QLineEdit();
    rngSeed3LineEdit->setValidator(validator);
    registerWidget(rngSeed3LineEdit, "rngSeed3LineEdit");
    rngSeedsVerticalLayout->addWidget(rngSeed3LineEdit);

    QLineEdit* rngSeed4LineEdit = new QLineEdit();
    rngSeed4LineEdit->setValidator(validator);
    registerWidget(rngSeed4LineEdit, "rngSeed4LineEdit");
    rngSeedsVerticalLayout->addWidget(rngSeed4LineEdit);


    #if (defined(Q_OS_DARWIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    #elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

    #else

        #error Unknown platform

    #endif

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    verticalLayout->addWidget(buttonBox);
}


Ld::PageFormat DocumentPreferencesDialog::pageFormatFromInput() const {
    EQt::DimensionLineEdit* leftLineEdit   = widget<EQt::DimensionLineEdit>("page_left_line_edit");
    EQt::DimensionLineEdit* topLineEdit    = widget<EQt::DimensionLineEdit>("page_top_line_edit");
    EQt::DimensionLineEdit* rightLineEdit  = widget<EQt::DimensionLineEdit>("page_right_line_edit");
    EQt::DimensionLineEdit* bottomLineEdit = widget<EQt::DimensionLineEdit>("page_bottom_line_edit");

    bool valuesValid = (
           leftLineEdit->valueOk()
        && topLineEdit->valueOk()
        && rightLineEdit->valueOk()
        && bottomLineEdit->valueOk()
    );

    Ld::PageFormat result;

    if (valuesValid) {
        QCheckBox* orientationCheckBox  = widget<QCheckBox>("page_landscape_orientation_check_box");
        QComboBox* pageSizeComboBox     = widget<QComboBox>("page_size_combo_box");

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
    }

    return result;
}

