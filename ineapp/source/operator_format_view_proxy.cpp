/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref OperatorFormatViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QList>
#include <QMap>
#include <QSet>
#include <QScreen>
#include <QDialog>
#include <QToolButton>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_operator_format.h>
#include <ld_function_format.h>
#include <ld_multiplication_operator_format.h>
#include <ld_division_operator_format.h>
#include <ld_logical_unary_not_operator_format.h>
#include <ld_brace_conditional_format.h>
#include <ld_element_with_grid_children.h>
#include <ld_matrix_operator_element.h>
#include <ld_brace_conditional_operator_element.h>
#include <ld_placeholder_element.h>

#include "application.h"
#include "update_format_command.h"
#include "operator_format_dialog.h"
#include "brace_conditional_configuration_dialog.h"
#include "parenthesis_style_popup_dialog.h"
#include "multiplication_style_popup_dialog.h"
#include "division_style_popup_dialog.h"
#include "logical_not_style_popup_dialog.h"
#include "matrix_insert_row_column_dialog.h"
#include "modify_grid_add_rows_command.h"
#include "modify_grid_add_columns_command.h"
#include "modify_grid_remove_row_command.h"
#include "modify_grid_remove_column_command.h"
#include "document.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "view_widget.h"
#include "operator_format_builder.h"
#include "operator_format_view_proxy.h"

OperatorFormatViewProxy::OperatorFormatViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):MathViewProxyBase(
        view,
        window
    ) {
    currentParenthesisStylePopupDialog = new ParenthesisStylePopupDialog(window);
    connect(
        currentParenthesisStylePopupDialog,
        &ParenthesisStylePopupDialog::parenthesisStyleSelected,
        this,
        &OperatorFormatViewProxy::parenthesisStyleSelected
    );

    connect(
        currentParenthesisStylePopupDialog,
        &ParenthesisStylePopupDialog::forceParenthesisButtonChecked,
        this,
        &OperatorFormatViewProxy::forceParenthesisButtonChecked
    );

    connect(
        currentParenthesisStylePopupDialog,
        &ParenthesisStylePopupDialog::parameterParenthesisStyleSelected,
        this,
        &OperatorFormatViewProxy::parameterParenthesisStyleSelected
    );

    connect(
        currentParenthesisStylePopupDialog,
        &ParenthesisStylePopupDialog::forceParameterParenthesisButtonChecked,
        this,
        &OperatorFormatViewProxy::forceParameterParenthesisButtonChecked
    );

    connect(
        currentParenthesisStylePopupDialog,
        &ParenthesisStylePopupDialog::focusLost,
        this,
        &OperatorFormatViewProxy::parenthesisStyleDialogFocusLost
    );

    currentMultiplicationStylePopupDialog = new MultiplicationStylePopupDialog(window);
    connect(
        currentMultiplicationStylePopupDialog,
        &MultiplicationStylePopupDialog::multiplicationSymbolSelected,
        this,
        &OperatorFormatViewProxy::multiplicationSymbolSelected
    );

    connect(
        currentMultiplicationStylePopupDialog,
        &MultiplicationStylePopupDialog::focusLost,
        this,
        &OperatorFormatViewProxy::multiplicationSymbolFocusLost
    );

    currentDivisionStylePopupDialog = new DivisionStylePopupDialog(window);
    connect(
        currentDivisionStylePopupDialog,
        &DivisionStylePopupDialog::divisionSymbolSelected,
        this,
        &OperatorFormatViewProxy::divisionSymbolSelected
    );

    connect(
        currentDivisionStylePopupDialog,
        &DivisionStylePopupDialog::focusLost,
        this,
        &OperatorFormatViewProxy::divisionSymbolFocusLost
    );

    currentLogicalNotStylePopupDialog = new LogicalNotStylePopupDialog(window);
    connect(
        currentLogicalNotStylePopupDialog,
        &LogicalNotStylePopupDialog::logicalNotSymbolSelected,
        this,
        &OperatorFormatViewProxy::logicalNotSymbolSelected
    );

    connect(
        currentLogicalNotStylePopupDialog,
        &LogicalNotStylePopupDialog::focusLost,
        this,
        &OperatorFormatViewProxy::logicalNotSymbolFocusLost
    );

    currentOperatorFormatDialog                = Q_NULLPTR;
    currentBraceConditionalConfigurationDialog = Q_NULLPTR;
}


OperatorFormatViewProxy::~OperatorFormatViewProxy() {}


void OperatorFormatViewProxy::bindDynamic() {
    ViewWidget* view = OperatorFormatViewProxy::view();
    updateOperatorFormatDockState(view->aggregationsByCapabilityWithoutAncestors());
    elementStackChanged();

    connectSignals();
}


void OperatorFormatViewProxy::unbindDynamic() {
    if (currentParenthesisStylePopupDialog->isVisible()) {
        currentParenthesisStylePopupDialog->hide();
    }

    if (currentMultiplicationStylePopupDialog->isVisible()) {
        currentMultiplicationStylePopupDialog->hide();
    }

    if (currentDivisionStylePopupDialog->isVisible()) {
        currentDivisionStylePopupDialog->hide();
    }

    if (currentLogicalNotStylePopupDialog->isVisible()) {
        currentLogicalNotStylePopupDialog->hide();
    }

    disconnectSignals();
}


void OperatorFormatViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithAncestors */,
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
    ) {
    disconnectSignals();
    updateOperatorFormatDockState(aggregationsByCapabilityWithoutAncestors);
    connectSignals();

    if (currentOperatorFormatDialog != Q_NULLPTR) {
        updateOperatorFormatDialog(aggregationsByCapabilityWithoutAncestors, currentOperatorFormatDialog);
    }

    if (currentBraceConditionalConfigurationDialog != Q_NULLPTR) {
        updateBraceConditionalOperatorFormatDialog(
            aggregationsByCapabilityWithoutAncestors,
            currentBraceConditionalConfigurationDialog
        );
    }
}


void OperatorFormatViewProxy::elementStackChanged() {
    MainWindow* window = OperatorFormatViewProxy::window();

    QAction* matrixInsertRowsAction             = window->action("matrix_insert_row_action");
    QAction* matrixInsertColumnsAction          = window->action("matrix_insert_column_action");
    QAction* matrixRemoveRowAction              = window->action("matrix_remove_row_action");
    QAction* matrixRemoveColumnAction           = window->action("matrix_remove_column_action");
    QAction* braceConditionalFormatDialogAction = window->action("brace_conditional_format_dialog");

    const Cursor& cursor    = view()->cursor();
    Cursor::Type  cursorType = cursor.cursorType();
    if (cursorType == Cursor::Type::ELEMENT || cursorType == Cursor::Type::TEXT) {
        Ld::ElementPointerList elements         = cursor.elementsUnderCursor();
        unsigned               cursorStackDepth = static_cast<unsigned>(elements.size());

        unsigned i = 0;
        while (i < cursorStackDepth                                                 &&
               !elements.at(i).isNull()                                             &&
               elements.at(i)->typeName() != Ld::MatrixOperatorElement::elementName    ) {
            ++i;
        }

        if (i < cursorStackDepth && !elements.at(i).isNull()) {
            QSharedPointer<Ld::MatrixOperatorElement> matrixElement = elements.at(i)
                                                                      .dynamicCast<Ld::MatrixOperatorElement>();

            matrixInsertRowsAction->setEnabled(true);
            matrixInsertColumnsAction->setEnabled(true);
            matrixRemoveRowAction->setEnabled(matrixElement->numberRows() > 1);
            matrixRemoveColumnAction->setEnabled(matrixElement->numberColumns() > 1);
        } else {
            matrixInsertRowsAction->setEnabled(false);
            matrixInsertColumnsAction->setEnabled(false);
            matrixRemoveRowAction->setEnabled(false);
            matrixRemoveColumnAction->setEnabled(false);
        }

        i = 0;
        while (i < cursorStackDepth                                                           &&
               !elements.at(i).isNull()                                                       &&
               elements.at(i)->typeName() != Ld::BraceConditionalOperatorElement::elementName    ) {
            ++i;
        }

        braceConditionalFormatDialogAction->setEnabled(i < cursorStackDepth && !elements.at(i).isNull());
    } else {
        matrixInsertRowsAction->setEnabled(false);
        matrixInsertColumnsAction->setEnabled(false);
        matrixRemoveRowAction->setEnabled(false);
        matrixRemoveColumnAction->setEnabled(false);
    }
}


void OperatorFormatViewProxy::horizontalDockParenthesisStyleButtonClicked() {
    QToolButton* horizontalParenthesisToolButton =
        window()->widget<QToolButton>("horizontal_format_parenthesis_tool_button");

    displayDialog(horizontalParenthesisToolButton, currentParenthesisStylePopupDialog);
}


void OperatorFormatViewProxy::verticalDockParenthesisStyleButtonClicked() {
    QToolButton* verticalParenthesisToolButton =
        window()->widget<QToolButton>("vertical_format_parenthesis_tool_button");

    displayDialog(verticalParenthesisToolButton, currentParenthesisStylePopupDialog);
}


void OperatorFormatViewProxy::horizontalDockMultiplicationSymbolStyleButtonClicked() {
    QToolButton* horizontalMultiplicationSymbolToolButton =
        window()->widget<QToolButton>("horizontal_format_multiplication_symbol_tool_button");

    displayDialog(horizontalMultiplicationSymbolToolButton, currentMultiplicationStylePopupDialog);
}


void OperatorFormatViewProxy::verticalDockMultiplicationSymbolStyleButtonClicked() {
    QToolButton* verticalMultiplicationSymbolToolButton =
        window()->widget<QToolButton>("vertical_format_multiplication_symbol_tool_button");

    displayDialog(verticalMultiplicationSymbolToolButton, currentMultiplicationStylePopupDialog);
}


void OperatorFormatViewProxy::horizontalDockDivisionSymbolStyleButtonClicked() {
    QToolButton* horizontalDivisionSymbolToolButton =
        window()->widget<QToolButton>("horizontal_format_division_symbol_tool_button");

    displayDialog(horizontalDivisionSymbolToolButton, currentDivisionStylePopupDialog);
}


void OperatorFormatViewProxy::verticalDockDivisionSymbolStyleButtonClicked() {
    QToolButton* verticalDivisionSymbolToolButton =
        window()->widget<QToolButton>("vertical_format_division_symbol_tool_button");

    displayDialog(verticalDivisionSymbolToolButton, currentDivisionStylePopupDialog);
}


void OperatorFormatViewProxy::horizontalDockLogicalNotSymbolStyleButtonClicked() {
    QToolButton* horizontalLogicalNotSymbolToolButton =
        window()->widget<QToolButton>("horizontal_format_logical_not_symbol_tool_button");

    displayDialog(horizontalLogicalNotSymbolToolButton, currentLogicalNotStylePopupDialog);
}


void OperatorFormatViewProxy::verticalDockLogicalNotSymbolStyleButtonClicked() {
    QToolButton* verticalLogicalNotSymbolToolButton =
        window()->widget<QToolButton>("vertical_format_logical_not_symbol_tool_button");

    displayDialog(verticalLogicalNotSymbolToolButton, currentLogicalNotStylePopupDialog);
}


void OperatorFormatViewProxy::operatorFormatDialogRequested() {
    OperatorFormatDialog operatorFormatDialog;

    updateOperatorFormatDialog(currentAggregationsByCapability, &operatorFormatDialog);

    currentOperatorFormatDialog = &operatorFormatDialog;
    connect(
        &operatorFormatDialog,
        &OperatorFormatDialog::apply,
        this,
        &OperatorFormatViewProxy::applyOperatorFormatDialogUpdates
    );

    int result = operatorFormatDialog.exec();
    if (result == OperatorFormatDialog::Accepted) {
        applyOperatorFormatDialogUpdates(&operatorFormatDialog);
    }

    currentOperatorFormatDialog = Q_NULLPTR;
}


void OperatorFormatViewProxy::braceConditionalFormatDialogRequested() {
    BraceConditionalConfigurationDialog braceConditionalConfigurationDialog;

    updateBraceConditionalOperatorFormatDialog(currentAggregationsByCapability, &braceConditionalConfigurationDialog);

    currentBraceConditionalConfigurationDialog = &braceConditionalConfigurationDialog;
    connect(
        &braceConditionalConfigurationDialog,
        &BraceConditionalConfigurationDialog::apply,
        this,
        &OperatorFormatViewProxy::applyBraceConditionalOperatorFormatDialogUpdates
    );

    int result = braceConditionalConfigurationDialog.exec();
    if (result == BraceConditionalConfigurationDialog::Accepted) {
        applyBraceConditionalOperatorFormatDialogUpdates(&braceConditionalConfigurationDialog);
    }

    currentBraceConditionalConfigurationDialog = Q_NULLPTR;
}


void OperatorFormatViewProxy::matrixInsertRowsButtonClicked() {
    unsigned long rowIndex;
    unsigned long columnIndex;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = locationInGridElement(
        Ld::MatrixOperatorElement::elementName,
        rowIndex,
        columnIndex
    );

    if (!gridElement.isNull()                         &&
        rowIndex != Ld::Element::invalidChildIndex    &&
        columnIndex != Ld::Element::invalidChildIndex    ) {
        MatrixInsertRowColumnDialog dialog(MatrixInsertRowColumnDialog::Orientation::ROWS, window());
        int result = dialog.exec();

        if (result == MatrixInsertRowColumnDialog::Accepted) {
            unsigned count        = dialog.count();
            bool     insertBefore = dialog.location() == MatrixInsertRowColumnDialog::Location::BEFORE;

            if (insertBefore) {
                view()->insertCommand(new ModifyGridAddRowsCommand(gridElement, rowIndex, count));
            } else {
                view()->insertCommand(new ModifyGridAddRowsCommand(gridElement, rowIndex + 1, count));
            }
        }
    } else {
        Application::beep();
    }
}


void OperatorFormatViewProxy::matrixInsertColumnsButtonClicked() {
    unsigned long rowIndex;
    unsigned long columnIndex;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = locationInGridElement(
        Ld::MatrixOperatorElement::elementName,
        rowIndex,
        columnIndex
    );

    if (!gridElement.isNull()                         &&
        rowIndex != Ld::Element::invalidChildIndex    &&
        columnIndex != Ld::Element::invalidChildIndex    ) {
        MatrixInsertRowColumnDialog dialog(MatrixInsertRowColumnDialog::Orientation::COLUMNS, window());
        int result = dialog.exec();

        if (result == MatrixInsertRowColumnDialog::Accepted) {
            unsigned count        = dialog.count();
            bool     insertBefore = dialog.location() == MatrixInsertRowColumnDialog::Location::BEFORE;

            if (insertBefore) {
                view()->insertCommand(new ModifyGridAddColumnsCommand(gridElement, columnIndex, count));
            } else {
                view()->insertCommand(new ModifyGridAddColumnsCommand(gridElement, columnIndex + 1, count));
            }
        }
    } else {
        Application::beep();
    }
}


void OperatorFormatViewProxy::matrixRemoveRowButtonClicked() {
    unsigned long rowIndex;
    unsigned long columnIndex;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = locationInGridElement(
        Ld::MatrixOperatorElement::elementName,
        rowIndex,
        columnIndex
    );

    if (!gridElement.isNull()                         &&
        rowIndex != Ld::Element::invalidChildIndex    &&
        columnIndex != Ld::Element::invalidChildIndex    ) {
        view()->insertCommand(new ModifyGridRemoveRowCommand(gridElement, rowIndex));
    } else {
        Application::beep();
    }
}


void OperatorFormatViewProxy::matrixRemoveColumnButtonClicked() {
    unsigned long rowIndex;
    unsigned long columnIndex;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = locationInGridElement(
        Ld::MatrixOperatorElement::elementName,
        rowIndex,
        columnIndex
    );

    if (!gridElement.isNull()                         &&
        rowIndex != Ld::Element::invalidChildIndex    &&
        columnIndex != Ld::Element::invalidChildIndex    ) {
        view()->insertCommand(new ModifyGridRemoveColumnCommand(gridElement, columnIndex));
    } else {
        Application::beep();
    }
}


void OperatorFormatViewProxy::applyOperatorFormatDialogUpdates(OperatorFormatDialog* dialog) {
    if (currentAggregationsByCapability.contains(Ld::OperatorFormat::formatName)) {
        Ld::Element::ParenthesisStyle parenthesisStyle    = dialog->parenthesisStyle();
        bool                          forceParenthesisSet = dialog->forceParenthesisSet();
        bool                          forceParenthesis    = dialog->forceParenthesis();

        Ld::Element::ParenthesisStyle parameterParenthesisStyle    = dialog->parameterParenthesisStyle();
        bool                          forceParameterParenthesisSet = dialog->forceParameterParenthesisSet();
        bool                          forceParameterParenthesis    = dialog->forceParameterParenthesis();
        bool                          subscriptedFirstParameter    = dialog->subscriptedFirstParameter();
        bool                          subscriptedFirstParameterSet = dialog->subscriptedFirstParameterSet();

        Ld::MultiplicationOperatorFormat::MultiplicationSymbol multiplicationSymbol = dialog->multiplicationSymbol();
        Ld::DivisionOperatorFormat::DivisionSymbol             divisionSymbol       = dialog->divisionSymbol();
        Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol    logicalNotSymbol     = dialog->logicalNotSymbol();

        const Ld::OperatorFormat::Aggregation& operatorFormatAggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::OperatorFormat>();

        Ld::ElementWeakPointerSet elements = operatorFormatAggregation;
        if (currentAggregationsByCapability.contains(Ld::MultiplicationOperatorFormat::formatName)) {
            const Ld::OperatorFormat::Aggregation& multiplicationOperatorFormatAggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::MultiplicationOperatorFormat>();

            elements |= multiplicationOperatorFormatAggregation;
        }

        UpdateFormatCommand::FormatsByElement formatsByElement;
        for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = elements.constBegin(),
                                                         elementEndIterator = elements.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer format        = element->format()->clone();
                bool              formatChanged = false;

                if (format->capabilities().contains(Ld::ParenthesisFormat::formatName)) {
                    QSharedPointer<Ld::ParenthesisFormat>
                        parenthesisFormat = format.dynamicCast<Ld::ParenthesisFormat>();

                    if (parenthesisStyle != Ld::Element::ParenthesisStyle::INVALID_OR_NONE &&
                        parenthesisFormat->parenthesisStyle() != parenthesisStyle          &&
                        (parenthesisFormat->supportsDefaultParenthesisStyle()                   ||
                         parenthesisStyle != Ld::Element::ParenthesisStyle::DEFAULT           )    ) {
                        parenthesisFormat->setParenthesisStyle(parenthesisStyle);
                        formatChanged = true;
                    }
                }

                if (format->capabilities().contains(Ld::OperatorFormat::formatName)) {
                    QSharedPointer<Ld::OperatorFormat> operatorFormat = format.dynamicCast<Ld::OperatorFormat>();
                    if (forceParenthesisSet && operatorFormat->forcedParenthesis() != forceParenthesis) {
                        operatorFormat->setForcedParenthesis(forceParenthesis);
                        formatChanged = true;
                    }
                }

                if (format->capabilities().contains(Ld::MultiplicationOperatorFormat::formatName)) {
                    QSharedPointer<Ld::MultiplicationOperatorFormat>
                        multiplicationOperatorFormat = format.dynamicCast<Ld::MultiplicationOperatorFormat>();

                    if (multiplicationSymbol != Ld::MultiplicationOperatorFormat::MultiplicationSymbol::INVALID &&
                        multiplicationOperatorFormat->multiplicationSymbol() != multiplicationSymbol               ) {
                        multiplicationOperatorFormat->setMultiplicationSymbol(multiplicationSymbol);
                        formatChanged = true;
                    }
                }

                if (format->capabilities().contains(Ld::DivisionOperatorFormat::formatName)) {
                    QSharedPointer<Ld::DivisionOperatorFormat>
                        divisionOperatorFormat = format.dynamicCast<Ld::DivisionOperatorFormat>();

                    if (divisionSymbol != Ld::DivisionOperatorFormat::DivisionSymbol::INVALID &&
                        divisionOperatorFormat->divisionSymbol() != divisionSymbol               ) {
                        divisionOperatorFormat->setDivisionSymbol(divisionSymbol);
                        formatChanged = true;
                    }
                }

                if (format->capabilities().contains(Ld::LogicalUnaryNotOperatorFormat::formatName)) {
                    QSharedPointer<Ld::LogicalUnaryNotOperatorFormat>
                        logicalUnaryNotOperatorFormat = format.dynamicCast<Ld::LogicalUnaryNotOperatorFormat>();

                    if (logicalNotSymbol != Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::INVALID &&
                        logicalUnaryNotOperatorFormat->logicalNotSymbol() != logicalNotSymbol               ) {
                        logicalUnaryNotOperatorFormat->setLogicalNotSymbol(logicalNotSymbol);
                        formatChanged = true;
                    }
                }

                if (format->capabilities().contains(Ld::FunctionFormat::formatName)) {
                    QSharedPointer<Ld::FunctionFormat> functionFormat = format.dynamicCast<Ld::FunctionFormat>();

                    if ((parameterParenthesisStyle != Ld::Element::ParenthesisStyle::INVALID_OR_NONE &&
                         parameterParenthesisStyle != functionFormat->parameterParenthesisStyle()       ) ||
                        (forceParameterParenthesisSet                                             &&
                         forceParameterParenthesis != functionFormat->forceParameterParenthesis()    )    ||
                        (subscriptedFirstParameterSet                                        &&
                         subscriptedFirstParameter != functionFormat->subscriptedParameter()    )            ) {
                        functionFormat->setParameterParenthesisStyle(parameterParenthesisStyle);

                        if (forceParameterParenthesisSet) {
                            functionFormat->setForceParameterParenthesis(forceParameterParenthesis);
                        }

                        if (subscriptedFirstParameterSet) {
                            functionFormat->setSubscriptedParameter(subscriptedFirstParameter);
                        }

                        formatChanged = true;
                    }
                }

                if (formatChanged) {
                    formatsByElement.insert(element, format);
                }
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void OperatorFormatViewProxy::applyBraceConditionalOperatorFormatDialogUpdates(
        BraceConditionalConfigurationDialog* dialog
    ) {
    if (currentAggregationsByCapability.contains(Ld::BraceConditionalFormat::formatName)) {
        const Ld::BraceConditionalFormat::Aggregation&
            aggregation = currentAggregationsByCapability.aggregationForFormat<Ld::BraceConditionalFormat>();

        QString conditionPreambleString = dialog->conditionPreambleText();
        QString elseConditionString     = dialog->implicitElseConditionText();

        if (aggregation.size() == 1 && dialog->isNumberExplicitConditionsControlEnabled()) {
            QSharedPointer<Ld::BraceConditionalOperatorElement>
                element = aggregation.begin()->toStrongRef().dynamicCast<Ld::BraceConditionalOperatorElement>();

            Ld::ElementPointer newElement;
            unsigned long newNumberExplicitConditions = dialog->numberExplicitConditions();
            if (newNumberExplicitConditions > 0 && dialog->isElseConditionSet()) {
                bool newExplicitElseCondition = dialog->hasExplicitElseCondition();
                if (element->numberExplicitConditions() != newNumberExplicitConditions ||
                    element->hasExplicitElseCondition() != newExplicitElseCondition       ) {

                    QSharedPointer<Ld::BraceConditionalOperatorElement>
                        newBraceElement = element->clone(true).dynamicCast<Ld::BraceConditionalOperatorElement>();

                    Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::PlaceholderElement::elementName);
                    newBraceElement->setNumberConditions(
                        newNumberExplicitConditions,
                        newExplicitElseCondition,
                        placeholderElement,
                        nullptr
                    );

                    newElement = newBraceElement;
                }
            }

            QSharedPointer<Ld::BraceConditionalFormat>
                format = element->format()->clone().dynamicCast<Ld::BraceConditionalFormat>();

            if (format->conditionPreambleString() != conditionPreambleString ||
                format->elseConditionString() != elseConditionString            ) {
                format->setConditionPreambleString(conditionPreambleString);
                format->setElseConditionString(elseConditionString);

                if (newElement.isNull()) {
                    UpdateFormatCommand::FormatsByElement formatsByElement;
                    formatsByElement.insert(element, format);
                    issueUpdateFormatCommand(formatsByElement);
                } else {
                    newElement->setFormat(format);
                    insertElement(newElement, InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE);
                }
            } else {
                if (!newElement.isNull()) {
                    insertElement(newElement, InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE);
                }
            }
        } else {
            UpdateFormatCommand::FormatsByElement formatsByElement;

            for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                             elementEndIterator = aggregation.constEnd()
                 ; elementIterator != elementEndIterator
                 ; ++elementIterator
                ) {
                Ld::ElementPointer element = elementIterator->toStrongRef();
                if (!element.isNull()) {
                    Ld::FormatPointer oldFormat = element->format();

                    if (oldFormat->typeName() == Ld::BraceConditionalFormat::formatName) {
                        QSharedPointer<Ld::BraceConditionalFormat>
                            newFormat = oldFormat->clone().dynamicCast<Ld::BraceConditionalFormat>();

                        newFormat->setConditionPreambleString(conditionPreambleString);
                        newFormat->setElseConditionString(elseConditionString);

                        formatsByElement.insert(element, newFormat);
                    }
                }
            }

            if (!formatsByElement.isEmpty()) {
                issueUpdateFormatCommand(formatsByElement);
            }
        }
    }
}


void OperatorFormatViewProxy::parenthesisStyleSelected(
        ParenthesisStylePopupDialog::ParenthesisStyle newParenthesisStyle
    ) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::ParenthesisFormat::formatName));

    const Ld::ParenthesisFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ParenthesisFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;
    for (  Ld::OperatorFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                           elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Q_ASSERT(!element.isNull());

        Ld::FormatPointer                     format            = element->format();
        QSharedPointer<Ld::ParenthesisFormat> parenthesisFormat = format.dynamicCast<Ld::ParenthesisFormat>();
        Q_ASSERT(!parenthesisFormat.isNull());

        if (parenthesisFormat->parenthesisStyle() != newParenthesisStyle   &&
            (parenthesisFormat->supportsDefaultParenthesisStyle()               ||
             newParenthesisStyle != Ld::Element::ParenthesisStyle::DEFAULT    )    ) {
            QSharedPointer<Ld::ParenthesisFormat> clonedFormat = parenthesisFormat->clone()
                                                                 .dynamicCast<Ld::ParenthesisFormat>();

            clonedFormat->setParenthesisStyle(newParenthesisStyle);
            formatsByElement.insert(element, clonedFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    currentParenthesisStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::forceParenthesisButtonChecked(bool nowChecked) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::OperatorFormat::formatName));

    const Ld::OperatorFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::OperatorFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;
    for (  Ld::OperatorFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                           elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Q_ASSERT(!element.isNull());

        Ld::FormatPointer                  format         = element->format();
        QSharedPointer<Ld::OperatorFormat> operatorFormat = format.dynamicCast<Ld::OperatorFormat>();
        Q_ASSERT(!operatorFormat.isNull());

        if (operatorFormat->forcedParenthesis() != nowChecked) {
            QSharedPointer<Ld::OperatorFormat> clonedFormat = operatorFormat->clone()
                                                              .dynamicCast<Ld::OperatorFormat>();

            clonedFormat->setForcedParenthesis(nowChecked);
            formatsByElement.insert(element, clonedFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void OperatorFormatViewProxy::parameterParenthesisStyleSelected(
        ParenthesisStylePopupDialog::ParenthesisStyle newParameterParenthesisStyle
    ) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::FunctionFormat::formatName));

    const Ld::ParenthesisFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::FunctionFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;
    for (  Ld::OperatorFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                           elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Q_ASSERT(!element.isNull());

        Ld::FormatPointer                  format         = element->format();
        QSharedPointer<Ld::FunctionFormat> functionFormat = format.dynamicCast<Ld::FunctionFormat>();
        Q_ASSERT(!functionFormat.isNull());

        if (functionFormat->parameterParenthesisStyle() != newParameterParenthesisStyle) {
            QSharedPointer<Ld::FunctionFormat> clonedFormat = functionFormat->clone()
                                                              .dynamicCast<Ld::FunctionFormat>();

            clonedFormat->setParameterParenthesisStyle(newParameterParenthesisStyle);
            formatsByElement.insert(element, clonedFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    currentParenthesisStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::forceParameterParenthesisButtonChecked(bool nowChecked) {
    if (currentAggregationsByCapability.contains(Ld::FunctionFormat::formatName)) {

        const Ld::FunctionFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FunctionFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;
        for (  Ld::FunctionFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                               elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            Q_ASSERT(!element.isNull());

            Ld::FormatPointer                  format         = element->format();
            QSharedPointer<Ld::FunctionFormat> functionFormat = format.dynamicCast<Ld::FunctionFormat>();
            Q_ASSERT(!functionFormat.isNull());

            if (functionFormat->forceParameterParenthesis() != nowChecked) {
                QSharedPointer<Ld::FunctionFormat> clonedFormat = functionFormat->clone()
                                                                  .dynamicCast<Ld::FunctionFormat>();

                clonedFormat->setForceParameterParenthesis(nowChecked);
                formatsByElement.insert(element, clonedFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void OperatorFormatViewProxy::parenthesisStyleDialogFocusLost() {
    currentParenthesisStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::multiplicationSymbolSelected(
        MultiplicationStylePopupDialog::MultiplicationSymbol newMultiplicationSymbol
    ) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::MultiplicationOperatorFormat::formatName));

    const Ld::OperatorFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::MultiplicationOperatorFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;
    for (  Ld::OperatorFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                           elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Q_ASSERT(!element.isNull());

        Ld::FormatPointer format = element->format();
        QSharedPointer<Ld::MultiplicationOperatorFormat>
            multiplicationOperatorFormat = format.dynamicCast<Ld::MultiplicationOperatorFormat>();
        Q_ASSERT(!multiplicationOperatorFormat.isNull());

        if (multiplicationOperatorFormat->multiplicationSymbol() != newMultiplicationSymbol) {
            QSharedPointer<Ld::MultiplicationOperatorFormat>
                clonedFormat = multiplicationOperatorFormat->clone().dynamicCast<Ld::MultiplicationOperatorFormat>();

            clonedFormat->setMultiplicationSymbol(newMultiplicationSymbol);
            formatsByElement.insert(element, clonedFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    currentMultiplicationStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::multiplicationSymbolFocusLost() {
    currentMultiplicationStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::divisionSymbolSelected(
        DivisionStylePopupDialog::DivisionSymbol newDivisionSymbol
    ) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::DivisionOperatorFormat::formatName));

    const Ld::OperatorFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::DivisionOperatorFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;
    for (  Ld::OperatorFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                           elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Q_ASSERT(!element.isNull());

        Ld::FormatPointer format = element->format();
        QSharedPointer<Ld::DivisionOperatorFormat>
            divisionOperatorFormat = format.dynamicCast<Ld::DivisionOperatorFormat>();
        Q_ASSERT(!divisionOperatorFormat.isNull());

        if (divisionOperatorFormat->divisionSymbol() != newDivisionSymbol) {
            QSharedPointer<Ld::DivisionOperatorFormat>
                clonedFormat = divisionOperatorFormat->clone().dynamicCast<Ld::DivisionOperatorFormat>();

            clonedFormat->setDivisionSymbol(newDivisionSymbol);
            formatsByElement.insert(element, clonedFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    currentDivisionStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::divisionSymbolFocusLost() {
    currentDivisionStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::logicalNotSymbolSelected(
        LogicalNotStylePopupDialog::LogicalNotSymbol newLogicalNotSymbol
    ) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::LogicalUnaryNotOperatorFormat::formatName));

    const Ld::OperatorFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::LogicalUnaryNotOperatorFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;
    for (  Ld::OperatorFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                           elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Q_ASSERT(!element.isNull());

        Ld::FormatPointer format = element->format();
        QSharedPointer<Ld::LogicalUnaryNotOperatorFormat>
            logicalUnaryNotOperatorFormat = format.dynamicCast<Ld::LogicalUnaryNotOperatorFormat>();
        Q_ASSERT(!logicalUnaryNotOperatorFormat.isNull());

        if (logicalUnaryNotOperatorFormat->logicalNotSymbol() != newLogicalNotSymbol) {
            QSharedPointer<Ld::LogicalUnaryNotOperatorFormat>
                clonedFormat = logicalUnaryNotOperatorFormat->clone().dynamicCast<Ld::LogicalUnaryNotOperatorFormat>();

            clonedFormat->setLogicalNotSymbol(newLogicalNotSymbol);
            formatsByElement.insert(element, clonedFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    currentLogicalNotStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::logicalNotSymbolFocusLost() {
    currentLogicalNotStylePopupDialog->hide();
    restoreFocus();
}


void OperatorFormatViewProxy::updateOperatorFormatDockState(
        const Ld::AggregationsByCapability& aggregationsByCapability
    ) {
    currentAggregationsByCapability = aggregationsByCapability;

    EQt::ProgrammaticMainWindow* window = OperatorFormatViewProxy::window();

    QAction* parenthesisStyleAction             = window->action("format_parenthesis_style");
    QAction* operatorFormatDialogAction         = window->action("operator_format_dialog");
    QAction* multiplicationSymbolAction         = window->action("format_multiplication_symbol");
    QAction* divisionSymbolAction               = window->action("format_division_symbol");
    QAction* logicalNotSymbolAction             = window->action("format_logical_not_symbol");
    QAction* braceConditionalFormatDialogAction = window->action("brace_conditional_format_dialog");

    bool enableOperatorFormatDialog = false;

    if (aggregationsByCapability.contains(Ld::ParenthesisFormat::formatName)) {
        const Ld::ParenthesisFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::ParenthesisFormat>();

        const QSet<Ld::Element::ParenthesisStyle>& parenthesisStyles = aggregation.parenthesisStyles();

        currentParenthesisStylePopupDialog->setParenthesisStyle(parenthesisStyles);
        currentParenthesisStylePopupDialog->setDefaultParenthesisStyleButtonVisible(
            aggregation.defaultParenthesisStyleAllowed().includesTrue()
        );

        parenthesisStyleAction->setEnabled(true);

        enableOperatorFormatDialog = true;
    } else {
        parenthesisStyleAction->setEnabled(false);
        currentParenthesisStylePopupDialog->setDefaultParenthesisStyleButtonHidden();

        if (currentParenthesisStylePopupDialog->isVisible()) {
            currentParenthesisStylePopupDialog->hide();
        }
    }

    if (aggregationsByCapability.contains(Ld::OperatorFormat::formatName)) {
        const Ld::OperatorFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::OperatorFormat>();

        const Ld::Format::BooleanAggregation& forcedParenthesisStates = aggregation.forceParenthesis();

        if (forcedParenthesisStates.bothTrueAndFalse()) {
            currentParenthesisStylePopupDialog->setForceParenthesisTriStated();
        } else {
            currentParenthesisStylePopupDialog->setForceParenthesisButtonChecked(forcedParenthesisStates.allTrue());
        }

        currentParenthesisStylePopupDialog->setForceParenthesisCheckboxVisible();

        enableOperatorFormatDialog = true;
    } else {
        currentParenthesisStylePopupDialog->setForceParenthesisCheckboxHidden();
    }

    if (aggregationsByCapability.contains(Ld::FunctionFormat::formatName)) {
        const Ld::FunctionFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::FunctionFormat>();

        const QSet<Ld::FunctionFormat::ParenthesisStyle>&
            parameterParenthesisStyle = aggregation.parameterParenthesisStyles();

        const Ld::Format::BooleanAggregation&
            forcedParameterParenthesisStates = aggregation.forceParameterParenthesis();

        currentParenthesisStylePopupDialog->setParameterParenthesisButtonsVisible(true);
        currentParenthesisStylePopupDialog->setForceParameterParenthesisCheckboxVisible();

        currentParenthesisStylePopupDialog->setParameterParenthesisStyle(parameterParenthesisStyle);

        if (forcedParameterParenthesisStates.bothTrueAndFalse()) {
            currentParenthesisStylePopupDialog->setForceParameterParenthesisTriStated();
        } else {
            currentParenthesisStylePopupDialog->setForceParameterParenthesisButtonChecked(
                forcedParameterParenthesisStates.allTrue()
            );
        }

        enableOperatorFormatDialog = true;
    } else {
        currentParenthesisStylePopupDialog->setParameterParenthesisButtonsHidden();
        currentParenthesisStylePopupDialog->setForceParameterParenthesisCheckboxHidden();
    }

    if (aggregationsByCapability.contains(Ld::MultiplicationOperatorFormat::formatName)) {
        const Ld::MultiplicationOperatorFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::MultiplicationOperatorFormat>();

        QSet<Ld::MultiplicationOperatorFormat::MultiplicationSymbol>
            multiplicationSymbols = aggregation.multiplicationSymbols();

        currentMultiplicationStylePopupDialog->setMultiplicationSymbol(multiplicationSymbols);
        multiplicationSymbolAction->setEnabled(true);

        enableOperatorFormatDialog = true;
    } else {
        multiplicationSymbolAction->setEnabled(false);

        if (currentMultiplicationStylePopupDialog->isVisible()) {
            currentMultiplicationStylePopupDialog->hide();
        }
    }

    if (aggregationsByCapability.contains(Ld::DivisionOperatorFormat::formatName)) {
        const Ld::DivisionOperatorFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::DivisionOperatorFormat>();

        QSet<Ld::DivisionOperatorFormat::DivisionSymbol> divisionSymbols = aggregation.divisionSymbols();
        currentDivisionStylePopupDialog->setDivisionSymbol(divisionSymbols);
        divisionSymbolAction->setEnabled(true);

        enableOperatorFormatDialog = true;
    } else {
        divisionSymbolAction->setEnabled(false);

        if (currentDivisionStylePopupDialog->isVisible()) {
            currentDivisionStylePopupDialog->hide();
        }
    }

    if (aggregationsByCapability.contains(Ld::LogicalUnaryNotOperatorFormat::formatName)) {
        const Ld::LogicalUnaryNotOperatorFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::LogicalUnaryNotOperatorFormat>();

        QSet<Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol> logicalNotSymbols = aggregation.logicalNotSymbols();
        currentLogicalNotStylePopupDialog->setLogicalNotSymbol(logicalNotSymbols);
        logicalNotSymbolAction->setEnabled(true);

        enableOperatorFormatDialog = true;
    } else {
        logicalNotSymbolAction->setEnabled(false);

        if (currentLogicalNotStylePopupDialog->isVisible()) {
            currentLogicalNotStylePopupDialog->hide();
        }
    }

    if (aggregationsByCapability.contains(Ld::BraceConditionalFormat::formatName)) {
        braceConditionalFormatDialogAction->setEnabled(true);
    } else {
        braceConditionalFormatDialogAction->setEnabled(false);
    }

    operatorFormatDialogAction->setEnabled(enableOperatorFormatDialog);
}


void OperatorFormatViewProxy::updateOperatorFormatDialog(
        const Ld::AggregationsByCapability& aggregationsByCapability,
        OperatorFormatDialog*               operatorFormatDialog
    ) {
    if (aggregationsByCapability.contains(Ld::ParenthesisFormat::formatName)) {
        const Ld::ParenthesisFormat::Aggregation&
            aggregation = aggregationsByCapability.aggregationForFormat<Ld::ParenthesisFormat>();

        operatorFormatDialog->setParenthesisStyles(aggregation.parenthesisStyles());
        operatorFormatDialog->setDefaultParenthesisStyleButtonVisible(
            aggregation.defaultParenthesisStyleAllowed().includesTrue()
        );
    } else {
        operatorFormatDialog->setDefaultParenthesisStyleButtonHidden();
    }

    if (aggregationsByCapability.contains(Ld::OperatorFormat::formatName)) {
        const Ld::OperatorFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::OperatorFormat>();

        if (aggregation.forceParenthesis().bothTrueAndFalse()) {
            operatorFormatDialog->setForceParenthesisTristated();
        } else {
            operatorFormatDialog->setForceParenthesisChecked(aggregation.forceParenthesis().allTrue());
        }

        if (currentAggregationsByCapability.contains(Ld::MultiplicationOperatorFormat::formatName)) {
            const Ld::MultiplicationOperatorFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::MultiplicationOperatorFormat>();

            operatorFormatDialog->setMultiplicationSymbols(aggregation.multiplicationSymbols());
            operatorFormatDialog->setMultiplicationSymbolsVisible();
        } else {
            operatorFormatDialog->setMultiplicationSymbolsHidden();
        }

        if (currentAggregationsByCapability.contains(Ld::DivisionOperatorFormat::formatName)) {
            const Ld::DivisionOperatorFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::DivisionOperatorFormat>();

            operatorFormatDialog->setDivisionSymbols(aggregation.divisionSymbols());
            operatorFormatDialog->setDivisionSymbolsVisible();
        } else {
            operatorFormatDialog->setDivisionSymbolsHidden();
        }

        if (currentAggregationsByCapability.contains(Ld::LogicalUnaryNotOperatorFormat::formatName)) {
            const Ld::LogicalUnaryNotOperatorFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::LogicalUnaryNotOperatorFormat>();

            operatorFormatDialog->setLogicalNotSymbols(aggregation.logicalNotSymbols());
            operatorFormatDialog->setLogicalNotSymbolsVisible();
        } else {
            operatorFormatDialog->setLogicalNotSymbolsHidden();
        }

        if (currentAggregationsByCapability.contains(Ld::FunctionFormat::formatName)) {
            const Ld::FunctionFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::FunctionFormat>();

            operatorFormatDialog->setParameterParenthesisStyle(aggregation.parameterParenthesisStyles());

            if (aggregation.forceParameterParenthesis().bothTrueAndFalse()) {
                operatorFormatDialog->setForceParameterParenthesisTristated();
            } else {
                operatorFormatDialog->setForceParameterParenthesisChecked(
                    aggregation.forceParameterParenthesis().allTrue()
                );
            }

            if (aggregation.subscriptedParameters().bothTrueAndFalse()) {
                operatorFormatDialog->setSubscriptedFirstParameterTristated();
            } else {
                operatorFormatDialog->setSubscriptedFirstParameterChecked(
                    aggregation.subscriptedParameters().allTrue()
                );
            }

            operatorFormatDialog->setParameterParenthesisButtonsVisible();
            operatorFormatDialog->setForceParameterParenthesisCheckboxVisible();
            operatorFormatDialog->setSubscriptedFirstParameterCheckboxVisible();
        } else {
            operatorFormatDialog->setParameterParenthesisButtonsHidden();
            operatorFormatDialog->setForceParameterParenthesisCheckboxHidden();
            operatorFormatDialog->setSubscriptedFirstParameterCheckboxHidden();
        }

        operatorFormatDialog->setForceParenthesisCheckboxVisible();
    } else {
        operatorFormatDialog->setForceParenthesisCheckboxHidden();
    }
}


void OperatorFormatViewProxy::updateBraceConditionalOperatorFormatDialog(
        const Ld::AggregationsByCapability&  aggregationsByCapability,
        BraceConditionalConfigurationDialog* braceConditionalFormatDialog
    ) {
    if (currentAggregationsByCapability.contains(Ld::BraceConditionalFormat::formatName)) {
        const Ld::BraceConditionalFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::BraceConditionalFormat>();

        unsigned long numberElements = static_cast<unsigned long>(aggregation.size());
        if (numberElements == 1) {
            QSharedPointer<Ld::BraceConditionalOperatorElement>
                element = aggregation.begin()->toStrongRef().dynamicCast<Ld::BraceConditionalOperatorElement>();

            if (!element.isNull()) {
                braceConditionalFormatDialog->setNumberExplicitConditionsControlEnabled();
                braceConditionalFormatDialog->setElseConditionControlsEnabled();

                braceConditionalFormatDialog->setNumberExplicitConditions(element->numberExplicitConditions());
                braceConditionalFormatDialog->setHaveExplicitElseCondition(element->hasExplicitElseCondition());
            } else {
                braceConditionalFormatDialog->setNumberExplicitConditionsControlDisabled();
                braceConditionalFormatDialog->setElseConditionControlsDisabled();

                braceConditionalFormatDialog->setNumberExplicitConditions(0);
                braceConditionalFormatDialog->setElseConditionUndefined();
            }
        } else {
            braceConditionalFormatDialog->setNumberExplicitConditionsControlDisabled();
            braceConditionalFormatDialog->setElseConditionControlsDisabled();

            braceConditionalFormatDialog->setNumberExplicitConditions(0);
            braceConditionalFormatDialog->setElseConditionUndefined();
        }

        if (aggregation.conditionPreambleStrings().size() == 1) {
            braceConditionalFormatDialog->setConditionPreambleText(*aggregation.conditionPreambleStrings().begin());
        } else {
            braceConditionalFormatDialog->setConditionPreambleText(QString());
        }

        if (aggregation.elseConditionStrings().size() == 1) {
            braceConditionalFormatDialog->setImplicitElseConditionText(*aggregation.elseConditionStrings().begin());
        } else {
            braceConditionalFormatDialog->setImplicitElseConditionText(QString());
        }
    } else {
        // Nothing to edit, then close the dialog.
        braceConditionalFormatDialog->reject();
    }
}


void OperatorFormatViewProxy::issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement) {
    ViewWidget* view = OperatorFormatViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement));
}


void OperatorFormatViewProxy::connectSignals() {
    MainWindow* window = OperatorFormatViewProxy::window();
    ViewWidget* view   = OperatorFormatViewProxy::view();

    sessionConnect(view, &ViewWidget::formatsAtCursorChanged, &OperatorFormatViewProxy::formatsUnderCursorChanged);
    sessionConnect(view, &ViewWidget::elementStackChanged, &OperatorFormatViewProxy::elementStackChanged);

    QToolButton* horizontalMultiplicationSymbolToolButton =
        window->widget<QToolButton>("horizontal_format_multiplication_symbol_tool_button");

    sessionConnect(
        horizontalMultiplicationSymbolToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::horizontalDockMultiplicationSymbolStyleButtonClicked
    );

    QToolButton* horizontalDivisionSymbolToolButton =
        window->widget<QToolButton>("horizontal_format_division_symbol_tool_button");

    sessionConnect(
        horizontalDivisionSymbolToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::horizontalDockDivisionSymbolStyleButtonClicked
    );

    QToolButton* horizontalLogicalNotSymbolToolButton =
        window->widget<QToolButton>("horizontal_format_logical_not_symbol_tool_button");

    sessionConnect(
        horizontalLogicalNotSymbolToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::horizontalDockLogicalNotSymbolStyleButtonClicked
    );

    QToolButton* horizontalParenthesisToolButton =
        window->widget<QToolButton>("horizontal_format_parenthesis_tool_button");

    sessionConnect(
        horizontalParenthesisToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::horizontalDockParenthesisStyleButtonClicked
    );

    QToolButton* verticalMultiplicationSymbolToolButton =
        window->widget<QToolButton>("vertical_format_multiplication_symbol_tool_button");

    sessionConnect(
        verticalMultiplicationSymbolToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::verticalDockMultiplicationSymbolStyleButtonClicked
    );

    QToolButton* verticalDivisionSymbolToolButton =
        window->widget<QToolButton>("vertical_format_division_symbol_tool_button");

    sessionConnect(
        verticalDivisionSymbolToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::verticalDockDivisionSymbolStyleButtonClicked
    );

    QToolButton* verticalLogicalNotSymbolToolButton =
        window->widget<QToolButton>("vertical_format_logical_not_symbol_tool_button");

    sessionConnect(
        verticalLogicalNotSymbolToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::verticalDockLogicalNotSymbolStyleButtonClicked
    );

    QToolButton* verticalParenthesisToolButton =
        window->widget<QToolButton>("vertical_format_parenthesis_tool_button");

    sessionConnect(
        verticalParenthesisToolButton,
        &QToolButton::clicked,
        &OperatorFormatViewProxy::verticalDockParenthesisStyleButtonClicked
    );

    QAction* operatorFormatDialogAction = window->action("operator_format_dialog");
    sessionConnect(
        operatorFormatDialogAction,
        &QAction::triggered,
        &OperatorFormatViewProxy::operatorFormatDialogRequested
    );

    QAction* braceConditionalFormatDialogAction = window->action("brace_conditional_format_dialog");
    sessionConnect(
        braceConditionalFormatDialogAction,
        &QAction::triggered,
        &OperatorFormatViewProxy::braceConditionalFormatDialogRequested
    );


    QAction* matrixInsertRowsAction = window->action("matrix_insert_row_action");
    sessionConnect(
        matrixInsertRowsAction,
        &QAction::triggered,
        &OperatorFormatViewProxy::matrixInsertRowsButtonClicked
    );

    QAction* matrixInsertColumnsAction = window->action("matrix_insert_column_action");
    sessionConnect(
        matrixInsertColumnsAction,
        &QAction::triggered,
        &OperatorFormatViewProxy::matrixInsertColumnsButtonClicked
    );

    QAction* matrixRemoveRowAction = window->action("matrix_remove_row_action");
    sessionConnect(
        matrixRemoveRowAction,
        &QAction::triggered,
        &OperatorFormatViewProxy::matrixRemoveRowButtonClicked
    );

    QAction* matrixRemoveColumnAction = window->action("matrix_remove_column_action");
    sessionConnect(
        matrixRemoveColumnAction,
        &QAction::triggered,
        &OperatorFormatViewProxy::matrixRemoveColumnButtonClicked
    );
}


void OperatorFormatViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void OperatorFormatViewProxy::restoreFocus() {
    view()->restoreFocus();
}


void OperatorFormatViewProxy::displayDialog(const QToolButton* button, QDialog* dialogWindow) {
    QPoint buttonBottomLeft  = button->rect().bottomLeft();
    QPoint buttonBottomRight = button->rect().bottomRight();
    QPoint buttonBottom      = QLine(buttonBottomLeft, buttonBottomRight).center();

    QPoint position = button->mapToGlobal(buttonBottom);

    unsigned dialogWidth  = static_cast<unsigned>(dialogWindow->width());
    unsigned dialogHeight = static_cast<unsigned>(dialogWindow->height());

    int      rightEdge   = position.x() + dialogWidth / 2;
    int      leftEdge    = rightEdge - dialogWidth;
    int      topEdge     = position.y();
    int      bottomEdge  = position.y() + dialogHeight;

    QScreen* screen = Application::screenAt(position);
    if (screen == Q_NULLPTR) {
        Q_ASSERT(!Application::screens().isEmpty());
        screen = Application::screens().at(0);
    }

    QRect screenRectangle = screen->geometry();

    if (topEdge < screenRectangle.top()) {
        topEdge     = screenRectangle.top();
        bottomEdge = topEdge + dialogHeight;
    } else if (bottomEdge > screenRectangle.bottom()) {
        bottomEdge = screenRectangle.bottom();
        topEdge    = bottomEdge - dialogHeight;
    }

    if (leftEdge < screenRectangle.left()) {
        leftEdge = screenRectangle.left();
        rightEdge = leftEdge + dialogWidth;
    } else if (rightEdge > screenRectangle.right()) {
        rightEdge = screenRectangle.right();
        leftEdge  = rightEdge - dialogWidth;
    }
    if (leftEdge < 0) {
        leftEdge = 0;
    }

    dialogWindow->move(leftEdge, topEdge);
    dialogWindow->show();
    dialogWindow->raise();
    dialogWindow->setFocus(Qt::ActiveWindowFocusReason);
}


QSharedPointer<Ld::ElementWithGridChildren> OperatorFormatViewProxy::locationInGridElement(
        const QString& elementName,
        unsigned long& rowIndex,
        unsigned long& columnIndex
    ) const {
    QSharedPointer<Ld::ElementWithGridChildren> gridElement;

    rowIndex    = Ld::Element::invalidChildIndex;
    columnIndex = Ld::Element::invalidChildIndex;

    const Cursor& cursor    = view()->cursor();
    Cursor::Type  cursorType = cursor.cursorType();

    if (cursorType == Cursor::Type::ELEMENT || cursorType == Cursor::Type::TEXT) {
        Ld::ElementPointerList elements         = cursor.elementsUnderCursor();
        unsigned               cursorStackDepth = static_cast<unsigned>(elements.size());

        unsigned i = 0;
        while (i < cursorStackDepth && !elements.at(i).isNull() && elements.at(i)->typeName() != elementName) {
            ++i;
        }

        if (i < cursorStackDepth && !elements.at(i).isNull()) {
            gridElement = elements.at(i).dynamicCast<Ld::ElementWithGridChildren>();

            if ((i + 1) < cursorStackDepth) {
                Ld::ElementPointer childElement = elements.at(i + 1);
                rowIndex    = gridElement->rowIndexOfChild(childElement);
                columnIndex = gridElement->columnIndexOfChild(childElement);
            }
        }
    }

    return gridElement;
}
