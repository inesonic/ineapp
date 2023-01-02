/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*f
* This file implements the \ref ValueFieldBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QImageReader>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QVariant>
#include <QDoubleValidator>
#include <QFontMetrics>

#include <util_string.h>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_spin_box.h>
#include <eqt_line_edit.h>
#include <eqt_combo_box.h>
#include <eqt_tool_button.h>
#include <eqt_stacking_layout.h>

#include <model_intrinsic_types.h>

#include <ld_capabilities.h>

#include "application.h"
#include "metatypes.h"
#include "main_window.h"
#include "value_field_main_window_proxy.h"
#include "value_field_view_proxy.h"
#include "value_field_builder.h"

const char       ValueFieldBuilder::builderName[] = "ValueFieldBuilder";
const char       ValueFieldBuilder::valueFieldFormatDockName[] = "value_field_format";
QDoubleValidator ValueFieldBuilder::doubleValidator;

ValueFieldBuilder::ValueFieldBuilder(QObject* parent):EQt::Builder(parent) {}


ValueFieldBuilder::~ValueFieldBuilder() {}


unsigned ValueFieldBuilder::priority() const {
    return builderPriority;
}


const char* ValueFieldBuilder::builderIdentifier() const {
    return builderName;
}


const char* ValueFieldBuilder::plugInName() const {
    return "";
}


void ValueFieldBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* insertValueFieldDialogAction = window->addMenuAction(
        EQt::Application::icon("insert_value_field"),
        tr("&Insert | &Field..."),
        Ld::Capabilities::nonTextAnnotations,
        "insert_value_field_dialog"
    );
    insertValueFieldDialogAction->setToolTip(tr("Insert a field that displays the value of a variable"));
    insertValueFieldDialogAction->setWhatsThis(
        tr(
            "You can use this option to insert a field that displays the value of a variable.  The variable's value "
            "will be presented when you run your program and will be updated each time the program is stopped or "
            "paused."
        )
    );

    QAction* formatValueFieldDialog = window->addMenuAction(
        EQt::Application::icon("format_value_field"),
        tr("F&ormat | &Field..."),
        "format_value_field_dialog"
    );
    formatValueFieldDialog->setToolTip(tr("Adjusts the presentation of numerical values"));
    formatValueFieldDialog->setWhatsThis(
        tr("Use this option to display a dialog you can use to change how numerical values are displayed.")
    );

    QMenu* imaginaryUnitMenu = new QMenu(window);
    window->registerWidget(imaginaryUnitMenu, "imaginary_unit_menu");

    QAction* imaginaryUnitIAction = imaginaryUnitMenu->addAction(tr("i"));
    imaginaryUnitIAction->setCheckable(true);
    window->addAction(imaginaryUnitIAction, "imaginary_unit_i_action");

    QAction* imaginaryUnitJAction = imaginaryUnitMenu->addAction(tr("j"));
    imaginaryUnitIAction->setCheckable(true);
    window->addAction(imaginaryUnitJAction, "imaginary_unit_j_action");
}


void ValueFieldBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
//    QCOMPARE(format3->imaginaryUnit(), Ld::ValueFieldFormat::ImaginaryUnit::J);

    QAction* integerNumberDigitsAction = new QAction(window);
    integerNumberDigitsAction->setToolTip(tr("Set the width of integer values, in digits"));
    integerNumberDigitsAction->setWhatsThis(
        tr("You can use this field to set the width of integer values, in digits.")
    );
    window->addAction(integerNumberDigitsAction, "integer_number_digits_action");

    QAction* integerBaseAction = new QAction(window);
    integerBaseAction->setToolTip(tr("Set the integer base to display"));
    integerBaseAction->setWhatsThis(tr("You can use this field to set the integer base."));
    window->addAction(integerBaseAction, "integer_base_action");

    QAction* integerNumberStyleAction = new QAction(window);
    integerNumberStyleAction->setToolTip(tr("Change the way integer values are presented"));
    integerNumberStyleAction->setWhatsThis(tr("You can use this option to change how integer vales are presented."));
    window->addAction(integerNumberStyleAction, "integer_numer_style_action");

    QAction* realNumberPrecisionAction = new QAction(window);
    realNumberPrecisionAction->setToolTip(tr("Sets the real value precision"));
    realNumberPrecisionAction->setWhatsThis(
        tr("You can use this option to specify the number of digits to present after the decimal point.")
    );
    window->addAction(realNumberPrecisionAction, "real_number_precision_action");

    QAction* realNumberNumberStyleAction = new QAction(window);
    realNumberNumberStyleAction->setToolTip(tr("Change the way real number values are presented"));
    realNumberNumberStyleAction->setWhatsThis(
        tr("You can use this option to change how real number vales are presented.")
    );
    window->addAction(realNumberNumberStyleAction, "real_number_numer_style_action");

    QAction* realNumberMultiplerAction = new QAction(window);
    realNumberMultiplerAction->setToolTip(tr("Sets the real/complex value multiplier"));
    realNumberMultiplerAction->setWhatsThis(
        tr("You can use this option to specify a multiplier applied to real/complex values.")
    );
    window->addAction(realNumberMultiplerAction, "real_number_multiplier_action");

    QAction* complexNumberImaginaryUnitAction = new QAction(window);
    complexNumberImaginaryUnitAction->setIcon(Application::icon("format_value_field_imaginary_unit"));
    complexNumberImaginaryUnitAction->setToolTip(tr("Sets use of <i>i</i> or <i>j</i> for imarginary numbers"));
    complexNumberImaginaryUnitAction->setWhatsThis(
        tr(
            "You can use this control to specify whether <i>i</i> or <i>j</i> should be used as the imaginary unit "
            "for imaginary numbers."
          )
    );
    window->addAction(complexNumberImaginaryUnitAction, "complex_number_imaginary_unit_action");
}


void ValueFieldBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        valueFieldFormatDockName,
        tr("Field"),
        EQt::DockWidgetDefaults(
            EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
            "operator_format",
            false
        )
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        valueFieldFormatDockName,
        tr("&View | &Docks"),
        "format_value_field_dock_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Value Field Format dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Value Field Format dock.  You can use the Value Field "
            "Format dock to control how numeric values are displayed in value fields."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);

    updateInsertDock(window);
}


EQt::ProgrammaticMainWindowProxy* ValueFieldBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new ValueFieldMainWindowProxy(window);
}


void ValueFieldBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* formatValueFieldDockToggleAction = mainWindow->action("format_value_field_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("vfdock"), formatValueFieldDockToggleAction);
    Q_ASSERT(success);

    QAction* formatValueFieldDialogAction = mainWindow->action("format_value_field_dialog");
    success = mainWindow->setCommand(tr("vffmt"), formatValueFieldDialogAction);
    Q_ASSERT(success);

    QAction* insertValueFieldDialogAction = mainWindow->action("insert_value_field_dialog");
    success = mainWindow->setCommand(tr("vf"), insertValueFieldDialogAction);
    Q_ASSERT(success);

    QMenu* rightClickMenu = mainWindow->widget<QMenu>("right_click_menu");
    rightClickMenu->addAction(formatValueFieldDialogAction);
}


EQt::ProgrammaticViewProxy* ValueFieldBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new ValueFieldViewProxy(view, window);
}


void ValueFieldBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QAction* integerNumberDigitsAction        = window->action("integer_number_digits_action");
    QAction* integerBaseAction                = window->action("integer_base_action");
    QAction* integerNumberStyleAction         = window->action("integer_numer_style_action");
    QAction* realNumberPrecisionAction        = window->action("real_number_precision_action");
    QAction* realNumberStyleAction            = window->action("real_number_numer_style_action");
    QAction* realNumberMultiplerAction        = window->action("real_number_multiplier_action");
    QAction* complexNumberImaginaryUnitAction = window->action("complex_number_imaginary_unit_action");
    QAction* formatValueFieldDialogAction     = window->action("format_value_field_dialog");

    QMenu* imaginaryUnitMenu = window->widget<QMenu>("imaginary_unit_menu");

    EQt::SpinBox*    integerNumberDigitsSpinBox       = newIntegerNumberDigitsSpinBox(integerNumberDigitsAction);
    EQt::SpinBox*    integerBaseSpinBox               = newIntegerNumberBaseSpinBox(integerBaseAction);
    EQt::ComboBox*   integerNumberStyleComboBox       = newIntegerNumberStyleComboBox(integerNumberStyleAction);
    EQt::SpinBox*    realNumberPrecisionSpinBox       = newRealNumberPrecisionSpinBox(realNumberPrecisionAction);
    EQt::ComboBox*   realNumberStyleComboBox          = newRealNumberStyleComboBox(realNumberStyleAction);
    EQt::LineEdit*   realNumberMultiplierLineEdit     = newRealNumberMultiplierLineEdit(realNumberMultiplerAction);
    EQt::ToolButton* complexImaginaryUnitToolButton   = new EQt::ToolButton(
        complexNumberImaginaryUnitAction,
        imaginaryUnitMenu
    );
    EQt::ToolButton* formatValueFieldDialogToolButton = new EQt::ToolButton(formatValueFieldDialogAction);

    dock->registerWidget(integerNumberDigitsSpinBox, "horizontal_integer_number_digits_spin_box");
    dock->registerWidget(integerBaseSpinBox, "horizontal_integer_base_spin_box");
    dock->registerWidget(integerNumberStyleComboBox, "horizontal_integer_number_style_combo_box");
    dock->registerWidget(realNumberPrecisionSpinBox, "horizontal_real_number_precision_spin_box");
    dock->registerWidget(realNumberStyleComboBox, "horizontal_real_number_style_combo_box");
    dock->registerWidget(realNumberMultiplierLineEdit, "horizontal_real_number_multiplier_line_edit");
    dock->registerWidget(complexImaginaryUnitToolButton, "horizontal_complex_imaginary_unit_tool_button");
    dock->registerWidget(formatValueFieldDialogToolButton, "horizontal_format_value_field_dialog_tool_button");

    QVBoxLayout* integerControlsVerticalLayout = dock->newVBoxLayout("horizontal_integer_controls_vertical_layout");
    horizontalLayout->addLayout(integerControlsVerticalLayout);

    integerControlsVerticalLayout->addStretch(2);

    QLabel* integerControlsLabel = new QLabel(tr("Integer Values"));
    integerControlsLabel->setAlignment(Qt::AlignHCenter);
    dock->registerWidget(integerControlsLabel, "horizontal_integer_controls_label");
    integerControlsVerticalLayout->addWidget(integerControlsLabel, Qt::AlignHCenter);

    QFormLayout* integerControlsLayout = dock->newFormLayout("horizontal_integer_controls_layout");
    integerControlsVerticalLayout->addLayout(integerControlsLayout);

    integerControlsLayout->addRow(tr("Digits: "), integerNumberDigitsSpinBox);
    integerControlsLayout->addRow(tr("Base: "), integerBaseSpinBox);
    integerControlsLayout->addRow(tr("Style:"), integerNumberStyleComboBox);

    integerControlsVerticalLayout->addStretch(2);

    QVBoxLayout* realComplexControlsVerticalLayout = dock->newVBoxLayout("horizontal_real_controls_vertical_layout");
    horizontalLayout->addLayout(realComplexControlsVerticalLayout);

    realComplexControlsVerticalLayout->addStretch(2);

    QLabel* realComplexControlsLabel = new QLabel(tr("Real/Complex Values"));
    realComplexControlsLabel->setAlignment(Qt::AlignHCenter);
    dock->registerWidget(realComplexControlsLabel, "horizontal_real_controls_label");
    realComplexControlsVerticalLayout->addWidget(realComplexControlsLabel, Qt::AlignHCenter);

    QFormLayout* realComplexControlsLayout = dock->newFormLayout("horizontal_real_controls_layout");
    realComplexControlsVerticalLayout->addLayout(realComplexControlsLayout);

    realComplexControlsLayout->addRow(tr("Multiplier: "), realNumberMultiplierLineEdit);
    realComplexControlsLayout->addRow(tr("Precision: "), realNumberPrecisionSpinBox);
    realComplexControlsLayout->addRow(tr("Notation: "), realNumberStyleComboBox);

    realComplexControlsVerticalLayout->addStretch(2);

    QVBoxLayout* buttonsLayout = dock->newVBoxLayout("horizontal_buttons");
    horizontalLayout->addLayout(buttonsLayout);

    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(complexImaginaryUnitToolButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(formatValueFieldDialogToolButton);
    buttonsLayout->addStretch(1);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(buttonsLayout, 2);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void ValueFieldBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QAction* integerNumberDigitsAction        = window->action("integer_number_digits_action");
    QAction* integerBaseAction                = window->action("integer_base_action");
    QAction* integerNumberStyleAction         = window->action("integer_numer_style_action");
    QAction* realNumberPrecisionAction        = window->action("real_number_precision_action");
    QAction* realNumberStyleAction            = window->action("real_number_numer_style_action");
    QAction* realNumberMultiplerAction        = window->action("real_number_multiplier_action");
    QAction* complexNumberImaginaryUnitAction = window->action("complex_number_imaginary_unit_action");
    QAction* formatValueFieldDialogAction     = window->action("format_value_field_dialog");

    QMenu* imaginaryUnitMenu = window->widget<QMenu>("imaginary_unit_menu");

    EQt::SpinBox*    integerNumberDigitsSpinBox       = newIntegerNumberDigitsSpinBox(integerNumberDigitsAction);
    EQt::SpinBox*    integerBaseSpinBox               = newIntegerNumberBaseSpinBox(integerBaseAction);
    EQt::ComboBox*   integerNumberStyleComboBox       = newIntegerNumberStyleComboBox(integerNumberStyleAction);
    EQt::SpinBox*    realNumberPrecisionSpinBox       = newRealNumberPrecisionSpinBox(realNumberPrecisionAction);
    EQt::ComboBox*   realNumberStyleComboBox          = newRealNumberStyleComboBox(realNumberStyleAction);
    EQt::LineEdit*   realNumberMultiplierLineEdit     = newRealNumberMultiplierLineEdit(realNumberMultiplerAction);
    EQt::ToolButton* complexImaginaryUnitToolButton   = new EQt::ToolButton(
        complexNumberImaginaryUnitAction,
        imaginaryUnitMenu
    );
    EQt::ToolButton* formatValueFieldDialogToolButton = new EQt::ToolButton(formatValueFieldDialogAction);

    dock->registerWidget(integerNumberDigitsSpinBox, "vertical_integer_number_digits_spin_box");
    dock->registerWidget(integerBaseSpinBox, "vertical_integer_base_spin_box");
    dock->registerWidget(integerNumberStyleComboBox, "vertical_integer_number_style_combo_box");
    dock->registerWidget(realNumberPrecisionSpinBox, "vertical_real_number_precision_spin_box");
    dock->registerWidget(realNumberStyleComboBox, "vertical_real_number_style_combo_box");
    dock->registerWidget(realNumberMultiplierLineEdit, "vertical_real_number_multiplier_line_edit");
    dock->registerWidget(complexImaginaryUnitToolButton, "vertical_complex_imaginary_unit_tool_button");
    dock->registerWidget(formatValueFieldDialogToolButton, "vertical_format_value_field_dialog_tool_button");

    QVBoxLayout* integerControlsVerticalLayout = dock->newVBoxLayout("vertical_integer_controls_vertical_layout");
    verticalLayout->addLayout(integerControlsVerticalLayout);

    QLabel* integerControlsLabel = new QLabel(tr("Integer Values"));
    integerControlsLabel->setAlignment(Qt::AlignLeft);
    dock->registerWidget(integerControlsLabel, "vertical_integer_controls_label");
    integerControlsVerticalLayout->addWidget(integerControlsLabel, Qt::AlignLeft);

    QFormLayout* integerControlsLayout = dock->newFormLayout("vertical_integer_controls_layout");
    integerControlsVerticalLayout->addLayout(integerControlsLayout);

    integerControlsLayout->addRow(tr("Digits: "), integerNumberDigitsSpinBox);
    integerControlsLayout->addRow(tr("Base: "), integerBaseSpinBox);
    integerControlsLayout->addRow(tr("Style:"), integerNumberStyleComboBox);

    verticalLayout->addSpacing(20);

    QVBoxLayout* realComplexControlsVerticalLayout = dock->newVBoxLayout("vertical_real_controls_layout");
    verticalLayout->addLayout(realComplexControlsVerticalLayout);

    QLabel* realComplexControlsLabel = new QLabel(tr("Real/Complex Values"));
    realComplexControlsLabel->setAlignment(Qt::AlignLeft);
    dock->registerWidget(realComplexControlsLabel, "vertical_real_controls_label");
    realComplexControlsVerticalLayout->addWidget(realComplexControlsLabel, Qt::AlignLeft);

    QFormLayout* realComplexControlsLayout = dock->newFormLayout("vertical_real_controls_vertical_layout");
    realComplexControlsVerticalLayout->addLayout(realComplexControlsLayout);

    realComplexControlsLayout->addRow(tr("Multiplier: "), realNumberMultiplierLineEdit);
    realComplexControlsLayout->addRow(tr("Precision: "), realNumberPrecisionSpinBox);
    realComplexControlsLayout->addRow(tr("Notation: "), realNumberStyleComboBox);

    verticalLayout->addSpacing(20);

    QHBoxLayout* buttonsLayout = dock->newHBoxLayout("vertical_buttons");
    verticalLayout->addLayout(buttonsLayout);

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(complexImaginaryUnitToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(formatValueFieldDialogToolButton);
    buttonsLayout->addStretch();

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setHeightAdjustmentLayout(buttonsLayout, 2);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void ValueFieldBuilder::updateInsertDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* insertDock = window->dockWidget("insert");

    EQt::StackingLayout* horizontalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("horizontal_annotations_stacking_layout");
    EQt::StackingLayout* verticalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("vertical_annotations_stacking_layout");

    QAction* insertValueFieldDialogAction = window->action("insert_value_field_dialog");

    horizontalStackingLayout->appendWidget(new EQt::ToolButton(insertValueFieldDialogAction));
    verticalStackingLayout->appendWidget(new EQt::ToolButton(insertValueFieldDialogAction));
}


EQt::SpinBox* ValueFieldBuilder::newIntegerNumberDigitsSpinBox(QAction* action) {
    EQt::SpinBox* newSpinBox = new EQt::SpinBox(action);
    newSpinBox->setMinimum(0);
    newSpinBox->setMaximum(8 * sizeof(long long));
    newSpinBox->setSpecialValueText(tr("Optimal"));

    return newSpinBox;
}


EQt::SpinBox* ValueFieldBuilder::newIntegerNumberBaseSpinBox(QAction* action) {
    EQt::SpinBox* newSpinBox = new EQt::SpinBox(action);
    newSpinBox->setMinimum(2);
    newSpinBox->setMaximum(16);

    return newSpinBox;
}


EQt::ComboBox* ValueFieldBuilder::newIntegerNumberStyleComboBox(QAction* action) {
    EQt::ComboBox* newComboBox = new EQt::ComboBox(action);

    newComboBox->setInsertPolicy(QComboBox::NoInsert);
    newComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    newComboBox->setReportedValueType(EQt::ComboBox::ReportedValue::VARIANT);

    newComboBox->addItem(tr("No prefix or suffix"), QVariant::fromValue(Util::IntegerNumberStyle::NONE));
    newComboBox->addItem(tr("C style prefix"), QVariant::fromValue(Util::IntegerNumberStyle::C_STYLE));
    newComboBox->addItem(tr("Verilog style prefix"), QVariant::fromValue(Util::IntegerNumberStyle::VERILOG_STYLE));
    newComboBox->addItem(tr("Base subscript"), QVariant::fromValue(Util::IntegerNumberStyle::SUBSCRIPT_STYLE));

    return newComboBox;
}


EQt::SpinBox* ValueFieldBuilder::newRealNumberPrecisionSpinBox(QAction* action) {
    EQt::SpinBox* newSpinBox = new EQt::SpinBox(action);
    newSpinBox->setMinimum(0);
    newSpinBox->setMaximum(std::numeric_limits<Model::Real>::digits10);

    return newSpinBox;
}


EQt::ComboBox* ValueFieldBuilder::newRealNumberStyleComboBox(QAction* action) {
    EQt::ComboBox* newComboBox = new EQt::ComboBox(action);

    newComboBox->setInsertPolicy(QComboBox::NoInsert);
    newComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    newComboBox->setReportedValueType(EQt::ComboBox::ReportedValue::VARIANT);

    newComboBox->addItem(tr("Fixed point"), QVariant::fromValue(Util::RealNumberStyle::FLOATING_POINT));
    newComboBox->addItem(tr("Scientific"), QVariant::fromValue(Util::RealNumberStyle::SCIENTIFIC));
    newComboBox->addItem(tr("Engineering"), QVariant::fromValue(Util::RealNumberStyle::ENGINEERING));
    newComboBox->addItem(
        tr("Computer scientific"),
        QVariant::fromValue(Util::RealNumberStyle::COMPUTER_CONCISE)
    );
    newComboBox->addItem(tr("Most Concise"), QVariant::fromValue(Util::RealNumberStyle::CONCISE));
    newComboBox->addItem(
        tr("Computer concise"),
        QVariant::fromValue(Util::RealNumberStyle::COMPUTER_CONCISE)
    );

    return newComboBox;
}


EQt::LineEdit* ValueFieldBuilder::newRealNumberMultiplierLineEdit(QAction* action) {
    EQt::LineEdit* newLineEdit = new EQt::LineEdit(action);
    newLineEdit->setValidator(&doubleValidator);

    return newLineEdit;
}
