/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BraceConditionalConfigurationDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>
#include <QSizePolicy>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFont>
#include <QFontMetricsF>
#include <QRectF>
#include <QPointF>
#include <QPoint>
#include <QPen>
#include <QColor>

#include <algorithm>

#include <eqt_graphics_math_group.h>

#include "application.h"
#include "brace_conditional_configuration_dialog.h"

BraceConditionalConfigurationDialog::BraceConditionalConfigurationDialog(
        QWidget* parent
    ):EQt::ProgrammaticDialog(
        "BraceConditionalConfigurationDialog",
        parent
    ) {
    configureWidget();
    runBuilders();
}


BraceConditionalConfigurationDialog::~BraceConditionalConfigurationDialog() {}


void BraceConditionalConfigurationDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void BraceConditionalConfigurationDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void BraceConditionalConfigurationDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void BraceConditionalConfigurationDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool BraceConditionalConfigurationDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool BraceConditionalConfigurationDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


void BraceConditionalConfigurationDialog::setOkButtonAlwaysEnabled(bool alwaysEnable) {
    currentOkAlwaysEnabled = alwaysEnable;
    valuesChanged();
}


void BraceConditionalConfigurationDialog::setOkButtonEnabledOnChange(bool conditionallyEnableOnly) {
    setOkButtonAlwaysEnabled(!conditionallyEnableOnly);
}


bool BraceConditionalConfigurationDialog::okButtonAlwaysEnabled() const {
    return currentOkAlwaysEnabled;
}


bool BraceConditionalConfigurationDialog::okButtonEnabledOnChange() const {
    return !okButtonAlwaysEnabled();
}


void BraceConditionalConfigurationDialog::setApplyButtonVisible(bool nowVisible) {
    #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setVisible(nowVisible);

    #elif (defined(Q_OS_DARWIN))

        (void) nowVisible;

    #else

        #error Unknown platform

    #endif
}


void BraceConditionalConfigurationDialog::setApplyButtonHidden(bool nowHidden) {
    setApplyButtonVisible(!nowHidden);
}


bool BraceConditionalConfigurationDialog::applyButtonVisible() const {
    bool result;

    #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        result = applyPushButton->isVisible();

    #elif (defined(Q_OS_DARWIN))

        result = false;

    #else

        #error Unknown platform

    #endif

    return result;
}


bool BraceConditionalConfigurationDialog::applyButtonHidden() const {
    return !applyButtonVisible();
}


void BraceConditionalConfigurationDialog::setNumberExplicitConditionsControlEnabled(bool nowEnabled) {
    QSpinBox* numberExplicitConditionsSpinBox = widget<QSpinBox>("number_explicit_conditionals_spin_box");
    numberExplicitConditionsSpinBox->setEnabled(nowEnabled);
}


void BraceConditionalConfigurationDialog::setNumberExplicitConditionsControlDisabled(bool nowDisabled) {
    setNumberExplicitConditionsControlEnabled(!nowDisabled);
}


bool BraceConditionalConfigurationDialog::isNumberExplicitConditionsControlEnabled() const {
    QSpinBox* numberExplicitConditionsSpinBox = widget<QSpinBox>("number_explicit_conditionals_spin_box");
    return numberExplicitConditionsSpinBox->isEnabled();
}


bool BraceConditionalConfigurationDialog::isNumberExplicitConditionsControlDisabled() const {
    return !isNumberExplicitConditionsControlEnabled();
}


unsigned long BraceConditionalConfigurationDialog::numberExplicitConditions() const {
    QSpinBox* numberExplicitConditionsSpinBox = widget<QSpinBox>("number_explicit_conditionals_spin_box");
    return static_cast<unsigned long>(numberExplicitConditionsSpinBox->value());
}


bool BraceConditionalConfigurationDialog::hasExplicitElseCondition() const {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");

    return explicitElseConditionRadioButton->isChecked() && !implicitElseConditionRadioButton->isChecked();
}


bool BraceConditionalConfigurationDialog::hasImplicitElseCondition() const {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");

    return !explicitElseConditionRadioButton->isChecked() && implicitElseConditionRadioButton->isChecked();
}


bool BraceConditionalConfigurationDialog::isElseConditionSet() const {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");

    return explicitElseConditionRadioButton->isChecked() || implicitElseConditionRadioButton->isChecked();
}


QString BraceConditionalConfigurationDialog::conditionPreambleText() const {
    QLineEdit* conditionPreambleLineEdit = widget<QLineEdit>("condition_preamble_line_edit");
    return conditionPreambleLineEdit->text();
}


QString BraceConditionalConfigurationDialog::implicitElseConditionText() const {
    QLineEdit* implicitElseConditionLineEdit = widget<QLineEdit>("implicit_else_condition_line_edit");
    return implicitElseConditionLineEdit->text();
}


void BraceConditionalConfigurationDialog::setElseConditionControlsEnabled(bool nowEnabled) {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");
    QLineEdit*    implicitElseConditionLineEdit    = widget<QLineEdit>("implicit_else_condition_line_edit");

    explicitElseConditionRadioButton->setEnabled(nowEnabled);
    implicitElseConditionRadioButton->setEnabled(nowEnabled);
    implicitElseConditionLineEdit->setEnabled(
           !nowEnabled
        || (nowEnabled && implicitElseConditionRadioButton->isChecked())
    );
}


void BraceConditionalConfigurationDialog::setElseConditionControlsDisabled(bool nowDisabled) {
    setElseConditionControlsEnabled(!nowDisabled);
}


bool BraceConditionalConfigurationDialog::isElseConditionControlsEnabled() const {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    return explicitElseConditionRadioButton->isEnabled();
}


bool BraceConditionalConfigurationDialog::isElseConditionControlsDisabled() const {
    return !isElseConditionControlsEnabled();
}


QFont BraceConditionalConfigurationDialog::mathTextFont() const {
    return currentMathTextFont;
}


QFont BraceConditionalConfigurationDialog::mathIdentifierFont() const {
    return currentMathIdentifierFont;
}


void BraceConditionalConfigurationDialog::populate() {
    QSpinBox* numberExplicitConditionsSpinBox = widget<QSpinBox>("number_explicit_conditionals_spin_box");
    if (inputNumberExplicitConditionals == 0) {
        numberExplicitConditionsSpinBox->clear();
    } else {
        numberExplicitConditionsSpinBox->setValue(inputNumberExplicitConditionals);
    }

    QLineEdit* conditionPreambleLineEdit = widget<QLineEdit>("condition_preamble_line_edit");
    conditionPreambleLineEdit->setText(inputConditionPrefixText);

    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    explicitElseConditionRadioButton->setChecked(inputExplicitElseClause == TriState::TRUE);

    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");
    implicitElseConditionRadioButton->setChecked(inputExplicitElseClause == TriState::FALSE);

    if (inputExplicitElseClause != TriState::UNSET) {
        explicitElseConditionRadioButton->setAutoExclusive(true);
        implicitElseConditionRadioButton->setAutoExclusive(true);
    }

    QLineEdit* implicitElseConditionLineEdit = widget<QLineEdit>("implicit_else_condition_line_edit");
    implicitElseConditionLineEdit->setText(inputImplicitElseClauseText);
    implicitElseConditionLineEdit->setEnabled(inputExplicitElseClause != TriState::TRUE);

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(currentOkAlwaysEnabled);

    #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    updateSample();
}


void BraceConditionalConfigurationDialog::setNumberExplicitConditions(unsigned long newNumberExplicitConditions) {
    inputNumberExplicitConditionals = newNumberExplicitConditions;

    if (isVisible()) {
        populate();
    }
}


void BraceConditionalConfigurationDialog::clearNumberExplicitConditions() {
    setNumberExplicitConditions(0);
}


void BraceConditionalConfigurationDialog::setHaveExplicitElseCondition(bool nowHaveExplicitElseCondition) {
    inputExplicitElseClause = nowHaveExplicitElseCondition ? TriState::TRUE : TriState::FALSE;

    if (isVisible()) {
        populate();
    }
}


void BraceConditionalConfigurationDialog::setHaveImplicitElseCondition(bool nowHaveImplicitElseCondition) {
    setHaveExplicitElseCondition(!nowHaveImplicitElseCondition);
}


void BraceConditionalConfigurationDialog::setElseConditionUndefined() {
    inputExplicitElseClause = TriState::UNSET;

    if (isVisible()) {
        populate();
    }
}


void BraceConditionalConfigurationDialog::setConditionPreambleText(const QString& newPreambleText) {
    inputConditionPrefixText = newPreambleText;

    if (isVisible()) {
        populate();
    }
}


void BraceConditionalConfigurationDialog::setImplicitElseConditionText(const QString& newElseConditionText) {
    inputImplicitElseClauseText = newElseConditionText;

    if (isVisible()) {
        populate();
    }
}


void BraceConditionalConfigurationDialog::setMathTextFont(const QFont& newFont) {
    currentMathTextFont = newFont;
    updateSample();
}


void BraceConditionalConfigurationDialog::setMathIdentifierFont(const QFont& newFont) {
    currentMathIdentifierFont = newFont;
    updateSample();
}


void BraceConditionalConfigurationDialog::bind() {
    QSpinBox* numberExplicitConditionsSpinBox = widget<QSpinBox>("number_explicit_conditionals_spin_box");
    connect(
        numberExplicitConditionsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &BraceConditionalConfigurationDialog::numberExplicitConditionsChanged
    );

    QLineEdit* conditionPreambleLineEdit = widget<QLineEdit>("condition_preamble_line_edit");
    connect(
        conditionPreambleLineEdit,
        &QLineEdit::textEdited,
        this,
        &BraceConditionalConfigurationDialog::conditionPreambleLineEditModified
    );

    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    connect(
        explicitElseConditionRadioButton,
        &QRadioButton::clicked,
        this,
        &BraceConditionalConfigurationDialog::explicitElseConditionRadioButtonClicked
    );

    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");
    connect(
        implicitElseConditionRadioButton,
        &QRadioButton::clicked,
        this,
        &BraceConditionalConfigurationDialog::implicitElseConditionRadioButtonClicked
    );

    QLineEdit* implicitElseConditionLineEdit = widget<QLineEdit>("implicit_else_condition_line_edit");
    connect(
        implicitElseConditionLineEdit,
        &QLineEdit::textEdited,
        this,
        &BraceConditionalConfigurationDialog::implicitElseCondtionTextModified
    );

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    connect(okPushButton, &QPushButton::clicked, this, &BraceConditionalConfigurationDialog::accept);

    #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        connect(applyPushButton, &QPushButton::clicked, this, &BraceConditionalConfigurationDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");
    connect(cancelPushButton, &QPushButton::clicked, this, &BraceConditionalConfigurationDialog::reject);
}


void BraceConditionalConfigurationDialog::numberExplicitConditionsChanged(int /* newValue */) {
    updateSample();
    updateDialogControls();
}


void BraceConditionalConfigurationDialog::explicitElseConditionRadioButtonClicked() {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");
    QLineEdit*    implicitElseConditionLineEdit    = widget<QLineEdit>("implicit_else_condition_line_edit");

    explicitElseConditionRadioButton->setChecked(true);
    implicitElseConditionRadioButton->setChecked(false);

    explicitElseConditionRadioButton->setAutoExclusive(true);
    implicitElseConditionRadioButton->setAutoExclusive(true);

    implicitElseConditionLineEdit->setEnabled(false);

    updateSample();
    updateDialogControls();
}


void BraceConditionalConfigurationDialog::implicitElseConditionRadioButtonClicked() {
    QRadioButton* explicitElseConditionRadioButton = widget<QRadioButton>("explicit_else_condition_radio_button");
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");
    QLineEdit*    implicitElseConditionLineEdit    = widget<QLineEdit>("implicit_else_condition_line_edit");

    explicitElseConditionRadioButton->setChecked(false);
    implicitElseConditionRadioButton->setChecked(true);

    explicitElseConditionRadioButton->setAutoExclusive(true);
    implicitElseConditionRadioButton->setAutoExclusive(true);

    implicitElseConditionLineEdit->setEnabled(true);

    updateSample();
    updateDialogControls();

}


void BraceConditionalConfigurationDialog::conditionPreambleLineEditModified(const QString& /* newText */) {
    updateSample();
    updateDialogControls();
}


void BraceConditionalConfigurationDialog::implicitElseCondtionTextModified(const QString& /* newText */) {
    QRadioButton* implicitElseConditionRadioButton = widget<QRadioButton>("implicit_condition_radio_button");

    if (implicitElseConditionRadioButton->isEnabled() && !implicitElseConditionRadioButton->isChecked()) {
        implicitElseConditionRadioButtonClicked();
    } else {
        updateSample();
        updateDialogControls();
    }
}


void BraceConditionalConfigurationDialog::applyClicked() {
    emit apply(this);

    inputNumberExplicitConditionals = numberExplicitConditions();
    inputConditionPrefixText        = conditionPreambleText();
    inputImplicitElseClauseText     = implicitElseConditionText();

    if (isElseConditionSet()) {
        inputExplicitElseClause = hasExplicitElseCondition() ? TriState::TRUE : TriState::FALSE;
    } else {
        inputExplicitElseClause = TriState::UNSET;
    }

    updateDialogControls();;
}


void BraceConditionalConfigurationDialog::configureWidget() {
    setWindowTitle(tr("Create Brace Conditional"));
    setSizeGripEnabled(false);

    currentAutoApplyEnabled         = false;
    inputNumberExplicitConditionals = 1;
    inputExplicitElseClause         = TriState::FALSE;
    inputConditionPrefixText        = tr("if ");
    inputImplicitElseClauseText     = tr("otherwise");

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* mainHorizontalLayout = newHBoxLayout("main_horizontal_layout");
    mainLayout->addLayout(mainHorizontalLayout);

    QVBoxLayout* leftVerticalLayout = newVBoxLayout("left_vertical_layout");
    mainHorizontalLayout->addLayout(leftVerticalLayout);

    QFormLayout* controlsLayout = newFormLayout("controls_layout");
    leftVerticalLayout->addLayout(controlsLayout);

    QSpinBox* numberExplicitConditionsSpinBox = new QSpinBox;
    numberExplicitConditionsSpinBox->setMinimum(1);
    numberExplicitConditionsSpinBox->setValue(inputNumberExplicitConditionals);
    registerWidget(numberExplicitConditionsSpinBox, "number_explicit_conditionals_spin_box");

    controlsLayout->addRow(tr("Number conditions (Excluding final): "), numberExplicitConditionsSpinBox);

    QLineEdit* conditionPreambleLineEdit = new QLineEdit(inputConditionPrefixText);
    controlsLayout->addRow(tr("Preamble for all conditions: "), conditionPreambleLineEdit);
    registerWidget(conditionPreambleLineEdit, "condition_preamble_line_edit");

    QGroupBox* elseClauseGroupBox = new QGroupBox(tr("Final Condition"));
    leftVerticalLayout->addWidget(elseClauseGroupBox);
    registerWidget(elseClauseGroupBox, "else_clause_group_box");

    QVBoxLayout* elseClauseVerticalLayout = newVBoxLayout("else_clause_vertical_layout");
    elseClauseGroupBox->setLayout(elseClauseVerticalLayout);

    QRadioButton* explicitElseConditionRadioButton = new QRadioButton(
        tr("Show operators for final condition (operators ignored, not evaluated)")
    );
    explicitElseConditionRadioButton->setAutoExclusive(false);
    elseClauseVerticalLayout->addWidget(explicitElseConditionRadioButton);
    registerWidget(explicitElseConditionRadioButton, "explicit_else_condition_radio_button");

    QRadioButton* implicitElseConditionRadioButton = new QRadioButton(tr("Show only text for the final condition"));
    explicitElseConditionRadioButton->setAutoExclusive(false);
    elseClauseVerticalLayout->addWidget(implicitElseConditionRadioButton);
    registerWidget(implicitElseConditionRadioButton, "implicit_condition_radio_button");

    QFormLayout* elseTextFormLayout = newFormLayout("else_text_form_layout");
    leftVerticalLayout->addLayout(elseTextFormLayout);

    QLineEdit* implicitElseConditionLineEdit = new QLineEdit(inputImplicitElseClauseText);
    elseTextFormLayout->addRow(tr("Final condition text: "), implicitElseConditionLineEdit);
    registerWidget(implicitElseConditionLineEdit, "implicit_else_condition_line_edit");

    QVBoxLayout* rightVerticalLayout = newVBoxLayout("right_vertical_layout");
    mainHorizontalLayout->addLayout(rightVerticalLayout);

    QGroupBox* sampleGroupBox = new QGroupBox(tr("Sample"));
    rightVerticalLayout->addWidget(sampleGroupBox);
    registerWidget(sampleGroupBox, "sample_group_box");

    rightVerticalLayout->addStretch();

    QGridLayout* sampleGridLayout = newGridLayout("grid_layout");
    sampleGroupBox->setLayout(sampleGridLayout);

    QGraphicsView* sampleGraphicsView = new QGraphicsView;
    sampleGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sampleGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sampleGraphicsView->setTransform(QTransform(1, 0, 0, 1, 0, 0));
    sampleGridLayout->addWidget(sampleGraphicsView);
    registerWidget(sampleGraphicsView, "sample_graphics_view");

    QGraphicsScene* sampleGraphicsScene = new QGraphicsScene(sampleGraphicsView);
    sampleGraphicsView->setScene(sampleGraphicsScene);

    EQt::GraphicsMathGroup* graphicsMathGroup = new EQt::GraphicsMathGroup;
    sampleGraphicsScene->addItem(graphicsMathGroup);

    #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
              QDialogButtonBox::Ok
            | QDialogButtonBox::Apply
            | QDialogButtonBox::Cancel
        );


        registerWidget(buttonBox->button(QDialogButtonBox::StandardButton::Ok), "ok_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::StandardButton::Apply), "apply_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::StandardButton::Cancel), "cancel_push_button");

    #elif (defined(Q_OS_DARWIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        registerWidget(buttonBox->button(QDialogButtonBox::StandardButton::Ok), "ok_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::StandardButton::Cancel), "cancel_push_button");

    #else

        #error Unknown platform

    #endif

    mainLayout->addWidget(buttonBox);
    registerWidget(buttonBox, "button_box");
}


void BraceConditionalConfigurationDialog::updateSample() {
    QGraphicsView*          graphicsView      = widget<QGraphicsView>("sample_graphics_view");
    QGraphicsScene*         graphicsScene     = graphicsView->scene();
    EQt::GraphicsMathGroup* graphicsMathGroup = dynamic_cast<EQt::GraphicsMathGroup*>(graphicsScene->items().first());

    graphicsMathGroup->clearText();

    unsigned long currentNumberExplicitConditions = numberExplicitConditions();
    bool          elseConditionIsSet              = isElseConditionSet();

    if (elseConditionIsSet && currentNumberExplicitConditions > 0) {
        QFont identifierFont = currentMathIdentifierFont;
        QFont textFont       = currentMathTextFont;

        identifierFont.setPointSizeF(identifierFont.pointSizeF() * Application::fontScaleFactor());
        textFont.setPointSizeF(textFont.pointSizeF() * Application::fontScaleFactor());

        QFontMetricsF identifierFontMetrics(identifierFont);
        QFontMetricsF textFontMetrics(textFont);

        float identifierFontAscent  = identifierFontMetrics.ascent();
        float identifierFontDescent = identifierFontMetrics.descent();
        float textFontAscent        = textFontMetrics.ascent();
        float textFontDescent       = textFontMetrics.descent();

        float maximumAscent         = std::max(identifierFontAscent, textFontAscent);
        float maximumDescent        = std::max(identifierFontDescent, textFontDescent);
        float rowHeight             = maximumAscent + maximumDescent;
        float totalHeight           = rowHeight * (currentNumberExplicitConditions + 1);

        float valueSpacing         = textFontMetrics.horizontalAdvance(tr("MM"));
        float leftParenthesisWidth = valueSpacing / 4.0F;

        graphicsMathGroup->clearText();
        graphicsMathGroup->setLeftParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::BRACES);
        graphicsMathGroup->setRightParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);
        graphicsMathGroup->setLeftParenthesisBoundingRectangle(QRectF(0, 0, leftParenthesisWidth, totalHeight));
        graphicsMathGroup->setParenthesisPen(QPen(Qt::black));
        graphicsMathGroup->setTextPen(QPen(Qt::black));

        float   currentY          = 0;
        float   maximumValueWidth = 0;
        QString multiply          = tr("%1").arg(QChar(0x2009));
        QString add               = tr(" + ");
        QString subtract          = tr(" - ");

        for (unsigned long i=0 ; i<=currentNumberExplicitConditions ; ++i) {
            QString variableA      = QString(QChar(static_cast<unsigned>('a' + i % 26)));
            float   variableAWidth = identifierFontMetrics.horizontalAdvance(variableA);

            graphicsMathGroup->append(
                variableA,
                identifierFont,
                QPointF(leftParenthesisWidth, currentY + maximumAscent)
            );

            float valueWidth;
            unsigned m = i % 4;
            if (m == 0) {
                valueWidth = variableAWidth;
            } else {
                QString op       = m == 1 ? multiply : m == 2 ? add : subtract;
                float   opWidth  = textFontMetrics.horizontalAdvance(op);

                graphicsMathGroup->append(
                    op,
                    textFont,
                    QPointF(leftParenthesisWidth + variableAWidth, currentY + maximumAscent)
                );

                QString variableB = QString(QChar(static_cast<unsigned>('z' - (i + 11) % 26)));
                if (variableB == variableA) {
                    variableB = QString(QChar(static_cast<unsigned>('z' - (i + 12) % 26)));
                }

                float variableBWidth = identifierFontMetrics.horizontalAdvance(variableB);

                graphicsMathGroup->append(
                    variableB,
                    identifierFont,
                    QPointF(leftParenthesisWidth + variableAWidth + opWidth, currentY + maximumAscent)
                );

                valueWidth = variableAWidth + opWidth + variableBWidth;
            }

            if (valueWidth > maximumValueWidth) {
                maximumValueWidth = valueWidth;
            }

            currentY += rowHeight;
        }

        QString conditionPreambleText  = BraceConditionalConfigurationDialog::conditionPreambleText();
        float   conditionPreambleWidth = textFontMetrics.horizontalAdvance(conditionPreambleText);

        bool explicitElseCondition = BraceConditionalConfigurationDialog::hasExplicitElseCondition();

        QString  conditionVariable      = tr("c");
        float    conditionVariableWidth = identifierFontMetrics.horizontalAdvance(conditionVariable);
        unsigned numberExplicitConditions =   explicitElseCondition
                                            ? currentNumberExplicitConditions + 1
                                            : currentNumberExplicitConditions;
        float    maximumConditionWidth = 0;
        float    conditionPreambleX    = leftParenthesisWidth + maximumValueWidth + valueSpacing;

        currentY = 0;

        for (unsigned long i=0 ; i<numberExplicitConditions ; ++i) {
            float currentX = conditionPreambleX;
            graphicsMathGroup->append(
                conditionPreambleText,
                textFont,
                QPointF(currentX, currentY + maximumAscent)
            );
            currentX += conditionPreambleWidth;

            graphicsMathGroup->append(
                conditionVariable,
                identifierFont,
                QPointF(currentX, currentY + maximumAscent)
            );
            currentX += conditionVariableWidth;

            if (i == 0) {
                QString opString = tr(" < 0");
                graphicsMathGroup->append(
                    opString,
                    textFont,
                    QPointF(currentX, currentY + maximumAscent)
                );
                currentX += textFontMetrics.horizontalAdvance(opString);
            } else {
                QString op1String = tr(" %1 %2 %3 ").arg(QChar(0x2265)).arg(i-1).arg(QChar(0x2227));
                graphicsMathGroup->append(
                    op1String,
                    textFont,
                    QPointF(currentX, currentY + maximumAscent)
                );
                currentX += textFontMetrics.horizontalAdvance(op1String);

                graphicsMathGroup->append(
                    conditionVariable,
                    identifierFont,
                    QPointF(currentX, currentY + maximumAscent)
                );
                currentX += conditionVariableWidth;

                QString op2String = tr(" < %1").arg(i);
                graphicsMathGroup->append(
                    op2String,
                    textFont,
                    QPointF(currentX, currentY + maximumAscent)
                );
                currentX += textFontMetrics.horizontalAdvance(op2String);
            }

            float conditionWidth = currentX - conditionPreambleX;
            if (conditionWidth > maximumConditionWidth) {
                maximumConditionWidth = conditionWidth;
            }

            currentY += rowHeight;
        }

        if (!explicitElseCondition) {
            QString elseConditionText = implicitElseConditionText();
            graphicsMathGroup->append(
                elseConditionText,
                textFont,
                QPointF(conditionPreambleX, currentY + maximumAscent)
            );

            float conditionWidth = textFontMetrics.horizontalAdvance(elseConditionText);
            if (conditionWidth > maximumConditionWidth) {
                maximumConditionWidth = conditionWidth;
            }
        }

        float totalWidth = leftParenthesisWidth + maximumValueWidth + valueSpacing + maximumConditionWidth;

        graphicsView->setVisible(true);

        graphicsScene->setSceneRect(0, 0, totalWidth, totalHeight);
        QPoint sceneTopLeft     = graphicsView->mapFromScene(0, 0);
        QPoint sceneBottomRight = graphicsView->mapFromScene(totalWidth, totalHeight);

        graphicsView->setFixedSize(
            sceneBottomRight.x() - sceneTopLeft.x() + 10,
            sceneBottomRight.y() - sceneTopLeft.y() + 10
        );
        graphicsView->centerOn(totalWidth / 2.0, totalHeight / 2.0);
        graphicsView->updateGeometry();
    } else {
        graphicsView->setVisible(false);
    }
}


void BraceConditionalConfigurationDialog::updateDialogControls() {
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");

    if (currentAutoApplyEnabled) {
        applyClicked();
        okPushButton->setEnabled(currentOkAlwaysEnabled);
    } else {
        bool valuesHaveChanged = valuesChanged();

        okPushButton->setEnabled(currentOkAlwaysEnabled || valuesHaveChanged);

        #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

            QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
            applyPushButton->setEnabled(valuesHaveChanged);

        #elif (!defined(Q_OS_DARWIN))

            #error Unknown platform

        #endif
    }
}


bool BraceConditionalConfigurationDialog::valuesChanged() const {
    bool result = false;

    if (inputNumberExplicitConditionals != numberExplicitConditions()  ||
        inputConditionPrefixText        != conditionPreambleText()     ||
        inputImplicitElseClauseText     != implicitElseConditionText()    ) {
        result = true;
    } else {
        switch (inputExplicitElseClause) {
            case TriState::FALSE: {
                result = hasExplicitElseCondition();
                break;
            }

            case TriState::TRUE: {
                result = hasImplicitElseCondition();
                break;
            }

            case TriState::UNSET: {
                result = isElseConditionSet();
                break;
            }
        }
    }

    return result;
}
