/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ValueFieldViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSet>
#include <QVariant>
#include <QPalette>

#include <algorithm>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_spin_box.h>
#include <eqt_combo_box.h>
#include <eqt_line_edit.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format_structures.h>
#include <ld_format.h>

#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_numeric_data_type_format.h>
#include <ld_integer_data_type_format.h>
#include <ld_real_data_type_format.h>
#include <ld_complex_data_type_format.h>
#include <ld_list_data_type_format.h>
#include <ld_set_data_type_format.h>
#include <ld_tuple_data_type_format.h>
#include <ld_matrix_data_type_format.h>
#include <ld_value_field_format.h>

#include <ld_root_element.h>
#include <ld_value_field_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "view_proxy.h"
#include "main_window.h"
#include "document.h"
#include "metatypes.h"
#include "numeric_value_format_dialog.h"
#include "value_field_builder.h"
#include "value_field_view_proxy.h"


ValueFieldViewProxy::ValueFieldViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {
    currentNumericFormatDialog = Q_NULLPTR;
}


ValueFieldViewProxy::~ValueFieldViewProxy() {}


void ValueFieldViewProxy::bindDynamic() {
    updateValueFieldDockState(view()->aggregationsByCapabilityWithoutAncestors());
    connectSignals();

    if (currentNumericFormatDialog != Q_NULLPTR) {
        updateNumericValueFormatDialog(currentNumericFormatDialog);
    }
}


void ValueFieldViewProxy::unbindDynamic() {
    disconnectSignals();
}


void ValueFieldViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithAncestors */,
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
    ) {
    disconnectSignals();
    updateValueFieldDockState(aggregationsByCapabilityWithoutAncestors);
    connectSignals();

    if (currentNumericFormatDialog != Q_NULLPTR) {
        updateNumericValueFormatDialog(currentNumericFormatDialog);
    }
}


void ValueFieldViewProxy::insertValueFieldRequested() {
    const Cursor&                  cursor        = view()->cursor();
    Cursor::Type                   cursorType    = cursor.cursorType();
    QSharedPointer<Ld::FontFormat> fontFormat;
    if (cursorType == Cursor::Type::ELEMENT || cursorType == Cursor::Type::TEXT) {
        const Ld::ElementCursor& elementCursor = view()->cursor().elementCursor();
        Ld::ElementPointer       element       = elementCursor.element();
        Ld::FormatPointer        format        = element.isNull() ? Ld::FormatPointer() : element->format();

        if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
            fontFormat = format.dynamicCast<Ld::FontFormat>();
        }
    }

    if (fontFormat.isNull()) {
        fontFormat = Ld::CharacterFormat::applicationDefaultTextFont().dynamicCast<Ld::FontFormat>();
    }

    NumericValueFormatDialog numericValueFormatDialog(window());
    numericValueFormatDialog.setWindowTitle(tr("Insert Field"));
    numericValueFormatDialog.setApplyButtonHidden();

    QFont applicationFont = Application::font();
    QFont sampleFont      = fontFormat->toQFont();
    sampleFont.setPointSize(std::min(3 * applicationFont.pointSize(), 2 * sampleFont.pointSize()));

    numericValueFormatDialog.setSampleFont(sampleFont);

    QSet<NumericValueFormatDialog::ValueType> supportedTypes;
    supportedTypes << NumericValueFormatDialog::ValueType::BOOLEAN
                   << NumericValueFormatDialog::ValueType::INTEGER
                   << NumericValueFormatDialog::ValueType::REAL
                   << NumericValueFormatDialog::ValueType::COMPLEX
                   << NumericValueFormatDialog::ValueType::SET
                   << NumericValueFormatDialog::ValueType::TUPLE
                   << NumericValueFormatDialog::ValueType::MATRIX_BOOLEAN
                   << NumericValueFormatDialog::ValueType::MATRIX_INTEGER
                   << NumericValueFormatDialog::ValueType::MATRIX_REAL
                   << NumericValueFormatDialog::ValueType::MATRIX_COMPLEX;

    numericValueFormatDialog.setSupportedDataTypes(supportedTypes);

    numericValueFormatDialog.setBooleanStyle(
        QSet<NumericValueFormatDialog::BooleanStyle>()
            << NumericValueFormatDialog::BooleanStyle::LOWER_CASE_TRUE_FALSE
    );

    numericValueFormatDialog.setMantissaDigitsIgnored();
    numericValueFormatDialog.setUpperCase();
    numericValueFormatDialog.setIntegerNumberStyles(
        QSet<NumericValueFormatDialog::IntegerNumberStyle>()
            << NumericValueFormatDialog::IntegerNumberStyle::NONE
    );
    numericValueFormatDialog.setBases(QSet<unsigned>() << 10U);
    numericValueFormatDialog.setRealNumberStyles(
        QSet<NumericValueFormatDialog::RealNumberStyle>() << NumericValueFormatDialog::RealNumberStyle::CONCISE
    );
    numericValueFormatDialog.setPrecisionInferred();
    numericValueFormatDialog.setMultiplicationFactor(1.0);
    numericValueFormatDialog.setImaginaryUnits(
        QSet<NumericValueFormatDialog::ImaginaryUnit>() << NumericValueFormatDialog::ImaginaryUnit::I
    );

    numericValueFormatDialog.setTupleDisplayMode(Ld::TupleDataTypeFormat::defaultDisplayMode);
    numericValueFormatDialog.setLeadingMemberCount(Ld::SetDataTypeFormat::defaultLeadingMemberCount);
    numericValueFormatDialog.setTrailingMemberCount(Ld::SetDataTypeFormat::defaultTrailingMemberCount);
    numericValueFormatDialog.setShowHiddenMemberCounts(Ld::SetDataTypeFormat::defaultShowHiddenMemberCount);

    numericValueFormatDialog.setLeadingRowsCount(Ld::MatrixDataTypeFormat::defaultLeadingMemberCount);
    numericValueFormatDialog.setTrailingRowsCount(Ld::MatrixDataTypeFormat::defaultTrailingMemberCount);
    numericValueFormatDialog.setLeadingColumnsCount(Ld::MatrixDataTypeFormat::defaultLeadingMemberCount);
    numericValueFormatDialog.setTrailingColumnsCount(Ld::MatrixDataTypeFormat::defaultTrailingMemberCount);

    int result = numericValueFormatDialog.exec();
    if (result == NumericValueFormatDialog::Accepted) {
        QSharedPointer<Ld::ValueFieldFormat> format = Ld::Format::create(Ld::ValueFieldFormat::formatName)
                                                      .dynamicCast<Ld::ValueFieldFormat>();

        format->setBooleanStyle(numericValueFormatDialog.booleanStyle());
        format->setVariableName(numericValueFormatDialog.variableName());
        format->setFamily(fontFormat->family());
        format->setFontSize(fontFormat->fontSize());
        format->setFontWeight(fontFormat->fontWeight());
        format->setItalics(fontFormat->italics());
        format->setUnderline(fontFormat->underline());
        format->setOverline(fontFormat->overline());
        format->setStrikeout(fontFormat->strikeout());
        format->setLetterSpacing(fontFormat->letterSpacing());
        format->setFontColor(fontFormat->fontColor());
        format->setFontBackgroundColor(fontFormat->fontBackgroundColor());

        unsigned mantissaDigits = numericValueFormatDialog.mantissaDigits();
        if (mantissaDigits == NumericValueFormatDialog::ignoreMantissaDigits) {
            format->setWidth(Ld::ValueFieldFormat::ignoreWidth);
        } else {
            format->setWidth(mantissaDigits);
        }

        format->setUpperCase(numericValueFormatDialog.upperCase());
        format->setBase(numericValueFormatDialog.base());
        format->setIntegerNumberStyle(numericValueFormatDialog.integerNumberStyle());

        unsigned precision = numericValueFormatDialog.precision();
        if (precision == NumericValueFormatDialog::inferPrecision) {
            format->setPrecision(Ld::ValueFieldFormat::inferPrecision);
        } else {
            format->setPrecision(precision);
        }

        format->setRealNumberStyle(numericValueFormatDialog.realNumberStyle());
        format->setMultiplier(numericValueFormatDialog.multiplicationFactor());
        format->setImaginaryUnit(numericValueFormatDialog.imaginaryUnit());

        format->setDisplayMode(numericValueFormatDialog.tupleDisplayMode());
        format->setLeadingMemberCount(numericValueFormatDialog.leadingMemberCount());
        format->setTrailingMemberCount(numericValueFormatDialog.trailingMemberCount());
        format->setHiddenMemberCountVisible(numericValueFormatDialog.showHiddenMemberCount());

        format->setLeadingRows(numericValueFormatDialog.leadingRowsCount());
        format->setTrailingRows(numericValueFormatDialog.trailingRowsCount());
        format->setLeadingColumns(numericValueFormatDialog.leadingColumnsCount());
        format->setTrailingColumns(numericValueFormatDialog.trailingColumnsCount());

        Ld::ElementPointer element = Ld::Element::create(Ld::ValueFieldElement::elementName);
        element->setFormat(format);

        view()->insertCommand(new InsertElementCommand(element));
    }
}


void ValueFieldViewProxy::formatValueFieldRequested() {
    Q_ASSERT(currentNumericFormatDialog == Q_NULLPTR);

    NumericValueFormatDialog numericValueFormatDialog;
    numericValueFormatDialog.loadSettings("NumericValueFormatDialog");
    numericValueFormatDialog.setWindowTitle(tr("Format Field"));

    numericValueFormatDialog.setApplyButtonVisible();
    connect(
        &numericValueFormatDialog,
        &NumericValueFormatDialog::apply,
        this,
        &ValueFieldViewProxy::formatValueFieldDialogApplyUpdates
    );

    updateNumericValueFormatDialog(&numericValueFormatDialog);

    currentNumericFormatDialog = &numericValueFormatDialog;
    int result = numericValueFormatDialog.exec();
    if (result == NumericValueFormatDialog::Accepted) {
        formatValueFieldDialogApplyUpdates(&numericValueFormatDialog);
    }

    currentNumericFormatDialog = Q_NULLPTR;
}


void ValueFieldViewProxy::formatValueFieldDialogApplyUpdates(NumericValueFormatDialog* dialog) {
    dialog->saveSettings("NumericValueFormatDialog");

    Ld::VariableName                             variableName             = dialog->variableName();
    unsigned                                     mantissaDigits           = dialog->mantissaDigits();
    bool                                         upperCase                = dialog->upperCase();
    bool                                         caseIsSet                = dialog->caseIsSet();
    NumericValueFormatDialog::IntegerNumberStyle integerNumberStyle       = dialog->integerNumberStyle();
    unsigned                                     base                     = dialog->base();
    NumericValueFormatDialog::RealNumberStyle    realNumberStyle          = dialog->realNumberStyle();
    unsigned                                     precision                = dialog->precision();
    double                                       multiplicationFactor     = dialog->multiplicationFactor();
    NumericValueFormatDialog::ImaginaryUnit      imaginaryUnit            = dialog->imaginaryUnit();
    NumericValueFormatDialog::BooleanStyle       booleanStyle             = dialog->booleanStyle();
    unsigned long                                leadingMemberCount       = dialog->leadingMemberCount();
    unsigned long                                trailingMemberCount      = dialog->trailingMemberCount();
    bool                                         showHiddenMemberCount    = dialog->showHiddenMemberCount();
    bool                                         showHiddenMemberCountSet = dialog->showHideHiddenMemberCountIsSet();
    NumericValueFormatDialog::TupleDisplayMode   tupleDisplayMode         = dialog->tupleDisplayMode();
    unsigned long                                leadingRowsCount         = dialog->leadingRowsCount();
    unsigned long                                trailingRowsCount        = dialog->trailingRowsCount();
    unsigned long                                leadingColumnsCount      = dialog->leadingRowsCount();
    unsigned long                                trailingColumnsCount     = dialog->trailingRowsCount();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
    for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                           elementEndIterator = elements.end()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        if (!element.isNull()) {
            Ld::FormatPointer format = element->format();
            if (!format.isNull()) {
                Ld::FormatPointer newFormat     = format->clone();
                bool              formatUpdated = false;

                Ld::Format::Capabilities capabilities = format->capabilities();

                bool includesNumericDataTypeFormat = capabilities.contains(Ld::NumericDataTypeFormat::formatName);
                bool includesBooleanDataTypeFormat = capabilities.contains(Ld::BooleanDataTypeFormat::formatName);
                bool includesIntegerDataTypeFormat = capabilities.contains(Ld::IntegerDataTypeFormat::formatName);
                bool includesRealDataTypeFormat    = capabilities.contains(Ld::RealDataTypeFormat::formatName);
                bool includesComplexDataTypeFormat = capabilities.contains(Ld::ComplexDataTypeFormat::formatName);
                bool includesSetDataTypeFormat     = capabilities.contains(Ld::SetDataTypeFormat::formatName);
                bool includesTupleDataTypeFormat   = capabilities.contains(Ld::TupleDataTypeFormat::formatName);
                bool includeMatrixDataTypeFormat   = capabilities.contains(Ld::MatrixDataTypeFormat::formatName);
                bool includesValueFieldFormat      = capabilities.contains(Ld::ValueFieldFormat::formatName);

                if (includesNumericDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::NumericDataTypeFormat>
                        numericDataTypeFormat = newFormat.dynamicCast<Ld::NumericDataTypeFormat>();

                    if (caseIsSet && upperCase != numericDataTypeFormat->upperCase()) {
                        numericDataTypeFormat->setUpperCase(upperCase);
                        formatUpdated = true;
                    }
                }

                if (includesBooleanDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::BooleanDataTypeFormat>
                        booleanDataTypeFormat = newFormat.dynamicCast<Ld::BooleanDataTypeFormat>();

                    if (booleanStyle != Ld::BooleanDataTypeFormat::BooleanStyle::INVALID &&
                        booleanDataTypeFormat->booleanStyle() != booleanStyle               ) {
                        booleanDataTypeFormat->setBooleanStyle(booleanStyle);
                        formatUpdated = true;
                    }
                }

                if (includesIntegerDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::IntegerDataTypeFormat>
                        integerDataTypeFormat = newFormat.dynamicCast<Ld::IntegerDataTypeFormat>();

                    if (mantissaDigits != NumericValueFormatDialog::mantissaDigitsNotSet &&
                        mantissaDigits != integerDataTypeFormat->width()                    ) {
                        integerDataTypeFormat->setWidth(mantissaDigits);
                        formatUpdated = true;
                    }

                    if (integerNumberStyle != NumericValueFormatDialog::IntegerNumberStyle::INVALID &&
                        integerNumberStyle != integerDataTypeFormat->integerNumberStyle()              ) {
                        integerDataTypeFormat->setIntegerNumberStyle(integerNumberStyle);
                        formatUpdated = true;
                    }

                    if (base != NumericValueFormatDialog::invalidBase && base != integerDataTypeFormat->base()) {
                        integerDataTypeFormat->setBase(base);
                        formatUpdated = true;
                    }
                }

                if (includesRealDataTypeFormat | includesValueFieldFormat) {
                    QSharedPointer<Ld::RealDataTypeFormat>
                        realDataTypeFormat = newFormat.dynamicCast<Ld::RealDataTypeFormat>();

                    if (precision != NumericValueFormatDialog::precisionNotSet &&
                        precision != realDataTypeFormat->precision()              ) {
                        realDataTypeFormat->setPrecision(precision);
                        formatUpdated = true;
                    }

                    if (multiplicationFactor != NumericValueFormatDialog::multiplicationFactorNotSet &&
                        multiplicationFactor != realDataTypeFormat->multiplier()                        ) {
                        realDataTypeFormat->setMultiplier(multiplicationFactor);
                        formatUpdated = true;
                    }

                    if (realNumberStyle != NumericValueFormatDialog::RealNumberStyle::INVALID &&
                        realNumberStyle != realDataTypeFormat->realNumberStyle()                 ) {
                        realDataTypeFormat->setRealNumberStyle(realNumberStyle);
                        formatUpdated = true;
                    }
                }

                if (includesComplexDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::ComplexDataTypeFormat>
                        complexDataTypeFormat = newFormat.dynamicCast<Ld::ComplexDataTypeFormat>();

                    if (imaginaryUnit != NumericValueFormatDialog::ImaginaryUnit::INVALID &&
                        imaginaryUnit != complexDataTypeFormat->imaginaryUnit()              ) {
                        complexDataTypeFormat->setImaginaryUnit(imaginaryUnit);
                        formatUpdated = true;
                    }
                }

                if (includesSetDataTypeFormat || includesTupleDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::ListDataTypeFormat>
                        listDataTypeFormat = newFormat.dynamicCast<Ld::ListDataTypeFormat>();

                    if (leadingMemberCount != NumericValueFormatDialog::invalidMemberCount &&
                        leadingMemberCount != listDataTypeFormat->leadingMemberCount()        ) {
                        listDataTypeFormat->setLeadingMemberCount(leadingMemberCount);
                        formatUpdated = true;
                    }

                    if (trailingMemberCount != NumericValueFormatDialog::invalidMemberCount &&
                        trailingMemberCount != listDataTypeFormat->trailingMemberCount()        ) {
                        listDataTypeFormat->setTrailingMemberCount(trailingMemberCount);
                        formatUpdated = true;
                    }

                    if (showHiddenMemberCountSet                                                &&
                        showHiddenMemberCount != listDataTypeFormat->displayHiddenMemberCount()    ) {
                        listDataTypeFormat->setHiddenMemberCountVisible(showHiddenMemberCount);
                        formatUpdated = true;
                    }
                }

                if (includesTupleDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::TupleDataTypeFormat>
                        tupleDataTypeFormat = newFormat.dynamicCast<Ld::TupleDataTypeFormat>();

                    if (tupleDisplayMode != NumericValueFormatDialog::TupleDisplayMode::INVALID &&
                        tupleDisplayMode != tupleDataTypeFormat->displayMode()                     ) {
                        tupleDataTypeFormat->setDisplayMode(tupleDisplayMode);
                        formatUpdated = true;
                    }
                }

                if (includeMatrixDataTypeFormat || includesValueFieldFormat) {
                    QSharedPointer<Ld::MatrixDataTypeFormat>
                        matrixDataTypeFormat = newFormat.dynamicCast<Ld::MatrixDataTypeFormat>();

                    if (leadingRowsCount != NumericValueFormatDialog::invalidMemberCount &&
                        leadingRowsCount != matrixDataTypeFormat->leadingRows()             ) {
                        matrixDataTypeFormat->setLeadingRows(leadingRowsCount);
                        formatUpdated = true;
                    }

                    if (trailingRowsCount != NumericValueFormatDialog::invalidMemberCount &&
                        trailingRowsCount != matrixDataTypeFormat->trailingRows()            ) {
                        matrixDataTypeFormat->setTrailingRows(trailingRowsCount);
                        formatUpdated = true;
                    }

                    if (leadingColumnsCount != NumericValueFormatDialog::invalidMemberCount &&
                        leadingColumnsCount != matrixDataTypeFormat->leadingColumns()          ) {
                        matrixDataTypeFormat->setLeadingColumns(leadingColumnsCount);
                        formatUpdated = true;
                    }

                    if (trailingColumnsCount != NumericValueFormatDialog::invalidMemberCount &&
                        trailingColumnsCount != matrixDataTypeFormat->trailingColumns()         ) {
                        matrixDataTypeFormat->setTrailingColumns(trailingColumnsCount);
                        formatUpdated = true;
                    }
                }

                if (includesValueFieldFormat) {
                    QSharedPointer<Ld::ValueFieldFormat>
                        valueFieldFormat = newFormat.dynamicCast<Ld::ValueFieldFormat>();

                    if (variableName.isValid() && valueFieldFormat->variableName() != variableName) {
                        valueFieldFormat->setVariableName(variableName);
                        formatUpdated = true;
                    }
                }

                if (formatUpdated) {
                    formatsByElement.insert(element, newFormat);
                }
            }
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void ValueFieldViewProxy::integerNumberDigitsChanged() {
    QAction* integerNumberDigitsAction = window()->action("integer_number_digits_action");

    bool     ok;
    unsigned newValue = integerNumberDigitsAction->data().toUInt(&ok);

    if (ok) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
        for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                               elementEndIterator = elements.end()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Ld::FormatPointer format = element->format();
                if (!format.isNull()                                                             &&
                    (format->capabilities().contains(Ld::IntegerDataTypeFormat::formatName) ||
                     format->capabilities().contains(Ld::ValueFieldFormat::formatName)         ) &&
                    format.dynamicCast<Ld::IntegerDataTypeFormat>()->width() != newValue            ) {
                    QSharedPointer<Ld::IntegerDataTypeFormat>
                        newFormat = format->clone().dynamicCast<Ld::IntegerDataTypeFormat>();

                    newFormat->setWidth(newValue);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void ValueFieldViewProxy::integerBaseChanged() {
    QAction* integerBaseAction = window()->action("integer_base_action");

    bool     ok;
    unsigned newValue = integerBaseAction->data().toUInt(&ok);

    if (ok) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
        for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                               elementEndIterator = elements.end()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Ld::FormatPointer format = element->format();
                if (!format.isNull()                                                             &&
                    (format->capabilities().contains(Ld::IntegerDataTypeFormat::formatName) ||
                     format->capabilities().contains(Ld::ValueFieldFormat::formatName)         ) &&
                    format.dynamicCast<Ld::IntegerDataTypeFormat>()->base() != newValue             ) {
                    QSharedPointer<Ld::IntegerDataTypeFormat>
                        newFormat = format->clone().dynamicCast<Ld::IntegerDataTypeFormat>();

                    newFormat->setBase(newValue);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void ValueFieldViewProxy::integerNumberStyleChanged() {
    QAction* integerNumberStyleAction = window()->action("integer_numer_style_action");

    bool ok = integerNumberStyleAction->data().canConvert<Ld::IntegerDataTypeFormat::IntegerNumberStyle>();
    if (ok) {
        Ld::IntegerDataTypeFormat::IntegerNumberStyle
            newValue = integerNumberStyleAction->data().value<Ld::IntegerDataTypeFormat::IntegerNumberStyle>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
        for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                               elementEndIterator = elements.end()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Ld::FormatPointer format = element->format();
                if (!format.isNull()                                                                  &&
                    (format->capabilities().contains(Ld::IntegerDataTypeFormat::formatName) ||
                     format->capabilities().contains(Ld::ValueFieldFormat::formatName)         )      &&
                    format.dynamicCast<Ld::IntegerDataTypeFormat>()->integerNumberStyle() != newValue    ) {
                    QSharedPointer<Ld::IntegerDataTypeFormat>
                        newFormat = format->clone().dynamicCast<Ld::IntegerDataTypeFormat>();

                    newFormat->setIntegerNumberStyle(newValue);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void ValueFieldViewProxy::realNumberPrecisionChanged() {
    QAction* realNumberPrecisionAction = window()->action("real_number_precision_action");

    bool     ok;
    unsigned newValue = realNumberPrecisionAction->data().toUInt(&ok);

    if (ok) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
        for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                               elementEndIterator = elements.end()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Ld::FormatPointer format = element->format();
                if (!format.isNull()                                                             &&
                    (format->capabilities().contains(Ld::RealDataTypeFormat::formatName) ||
                     format->capabilities().contains(Ld::ValueFieldFormat::formatName)         ) &&
                    format.dynamicCast<Ld::RealDataTypeFormat>()->precision() != newValue           ) {
                    QSharedPointer<Ld::RealDataTypeFormat>
                        newFormat = format->clone().dynamicCast<Ld::RealDataTypeFormat>();

                    newFormat->setPrecision(newValue);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void ValueFieldViewProxy::realNumberStyleChanged() {
    QAction* realNumberStyleAction = window()->action("real_number_numer_style_action");

    bool ok = realNumberStyleAction->data().canConvert<Ld::RealDataTypeFormat::RealNumberStyle>();
    if (ok) {
        Ld::RealDataTypeFormat::RealNumberStyle
            newValue = realNumberStyleAction->data().value<Ld::RealDataTypeFormat::RealNumberStyle>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
        for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                               elementEndIterator = elements.end()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Ld::FormatPointer format = element->format();
                if (!format.isNull()                                                            &&
                    (format->capabilities().contains(Ld::RealDataTypeFormat::formatName) ||
                     format->capabilities().contains(Ld::ValueFieldFormat::formatName)      )   &&
                    format.dynamicCast<Ld::RealDataTypeFormat>()->realNumberStyle() != newValue    ) {
                    QSharedPointer<Ld::RealDataTypeFormat>
                        newFormat = format->clone().dynamicCast<Ld::RealDataTypeFormat>();

                    newFormat->setRealNumberStyle(newValue);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}

void ValueFieldViewProxy::realNumberMultiplierChanged() {
    QAction* realNumberMultiplierAction = window()->action("real_number_multiplier_action");

    bool   ok;
    double newValue = realNumberMultiplierAction->data().toDouble(&ok);

    if (ok) {
        EQt::ProgrammaticDockWidget* dock = window()->dockWidget(ValueFieldBuilder::valueFieldFormatDockName);
        EQt::LineEdit* horizontalRealNumberMultiplierLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::LineEdit>(
            "horizontal_real_number_multiplier_line_edit"
        );
        EQt::LineEdit* verticalRealNumberMultiplierLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::LineEdit>(
            "vertical_real_number_multiplier_line_edit"
        );

        QPalette palette;
        palette.setColor(QPalette::Text, newValue == double(0) ? Qt::red : Qt::black);
        horizontalRealNumberMultiplierLineEdit->setPalette(palette);
        verticalRealNumberMultiplierLineEdit->setPalette(palette);

        if (newValue != double(0)) {
            UpdateFormatCommand::FormatsByElement formatsByElement;

            Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
            for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                                   elementEndIterator = elements.end()
                 ; elementIterator != elementEndIterator
                 ; ++elementIterator
                ) {
                Ld::ElementPointer element = *elementIterator;
                if (!element.isNull()) {
                    Ld::FormatPointer format = element->format();
                    if (!format.isNull()                                                             &&
                        (format->capabilities().contains(Ld::RealDataTypeFormat::formatName) ||
                         format->capabilities().contains(Ld::ValueFieldFormat::formatName)         ) &&
                        format.dynamicCast<Ld::RealDataTypeFormat>()->multiplier() != newValue           ) {
                        QSharedPointer<Ld::RealDataTypeFormat>
                            newFormat = format->clone().dynamicCast<Ld::RealDataTypeFormat>();

                        newFormat->setMultiplier(newValue);
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


void ValueFieldViewProxy::complexImaginaryUnitISelected() {
    updateImaginaryUnit(Ld::ComplexDataTypeFormat::ImaginaryUnit::I);
}


void ValueFieldViewProxy::complexImaginaryUnitJSelected() {
    updateImaginaryUnit(Ld::ComplexDataTypeFormat::ImaginaryUnit::J);
}


void ValueFieldViewProxy::connectSignals() {
    MainWindow* window = ValueFieldViewProxy::window();
    ViewWidget* view   = ValueFieldViewProxy::view();

    sessionConnect(view, &ViewWidget::formatsAtCursorChanged, &ValueFieldViewProxy::formatsUnderCursorChanged);

    QAction* insertValueFieldDialogAction = window->action("insert_value_field_dialog");
    sessionConnect(insertValueFieldDialogAction, &QAction::triggered, &ValueFieldViewProxy::insertValueFieldRequested);

    QAction* integerNumberDigitsAction = window->action("integer_number_digits_action");
    sessionConnect(integerNumberDigitsAction, &QAction::triggered, &ValueFieldViewProxy::integerNumberDigitsChanged);

    QAction* integerBaseAction = window->action("integer_base_action");
    sessionConnect(integerBaseAction, &QAction::triggered, &ValueFieldViewProxy::integerBaseChanged);

    QAction* integerNumberStyleAction = window->action("integer_numer_style_action");
    sessionConnect(integerNumberStyleAction, &QAction::triggered, &ValueFieldViewProxy::integerNumberStyleChanged);

    QAction* realNumberPrecisionAction = window->action("real_number_precision_action");
    sessionConnect(realNumberPrecisionAction, &QAction::triggered, &ValueFieldViewProxy::realNumberPrecisionChanged);

    QAction* realNumberStyleAction = window->action("real_number_numer_style_action");
    sessionConnect(realNumberStyleAction, &QAction::triggered, &ValueFieldViewProxy::realNumberStyleChanged);

    QAction* realNumberMultiplerAction = window->action("real_number_multiplier_action");
    sessionConnect(realNumberMultiplerAction, &QAction::triggered, &ValueFieldViewProxy::realNumberMultiplierChanged);

    QAction* formatValueFieldDialogAction = window->action("format_value_field_dialog");
    sessionConnect(formatValueFieldDialogAction, &QAction::triggered, &ValueFieldViewProxy::formatValueFieldRequested);

    QAction* imaginaryUnitIAction = window->action("imaginary_unit_i_action");
    sessionConnect(imaginaryUnitIAction, &QAction::triggered, &ValueFieldViewProxy::complexImaginaryUnitISelected);

    QAction* imaginaryUnitJAction = window->action("imaginary_unit_j_action");
    sessionConnect(imaginaryUnitJAction, &QAction::triggered, &ValueFieldViewProxy::complexImaginaryUnitJSelected);
}


void ValueFieldViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void ValueFieldViewProxy::updateValueFieldDockState(const Ld::AggregationsByCapability& aggregationsByCapability) {
    currentAggregationsByCapability = aggregationsByCapability;

    MainWindow*                  window = ValueFieldViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(ValueFieldBuilder::valueFieldFormatDockName);

    QAction* integerNumberDigitsAction        = window->action("integer_number_digits_action");
    QAction* integerBaseAction                = window->action("integer_base_action");
    QAction* integerNumberStyleAction         = window->action("integer_numer_style_action");
    QAction* realNumberPrecisionAction        = window->action("real_number_precision_action");
    QAction* realNumberStyleAction            = window->action("real_number_numer_style_action");
    QAction* realNumberMultiplerAction        = window->action("real_number_multiplier_action");
    QAction* formatValueFieldDialogAction     = window->action("format_value_field_dialog");
    QAction* complexNumberImaginaryUnitAction = window->action("complex_number_imaginary_unit_action");
    QAction* imaginaryUnitIAction             = window->action("imaginary_unit_i_action");
    QAction* imaginaryUnitJAction             = window->action("imaginary_unit_j_action");

    bool hasDataTypePresentation = false;

    if (aggregationsByCapability.contains(Ld::IntegerDataTypeFormat::formatName) ||
        aggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)         ) {
        hasDataTypePresentation = true;

        const Ld::IntegerDataTypeFormat::Aggregation& aggregation =
              aggregationsByCapability.contains(Ld::IntegerDataTypeFormat::formatName)
            ? aggregationsByCapability.aggregationForFormat<Ld::IntegerDataTypeFormat>()
            : aggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        const QSet<unsigned>&                                      integerWidths = aggregation.widths();
        const QSet<unsigned>&                                      integerBases  = aggregation.bases();
        const QSet<Ld::IntegerDataTypeFormat::IntegerNumberStyle>& integerStyles = aggregation.integerNumberStyles();

        if (integerWidths.size() != 1) {
            EQt::SpinBox* horizontalIntegerNumberDigitsSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
                "horizontal_integer_number_digits_spin_box"
            );
            EQt::SpinBox* verticalIntegerNumberDigitsSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
                "vertical_integer_number_digits_spin_box"
            );

            horizontalIntegerNumberDigitsSpinBox->clear();
            verticalIntegerNumberDigitsSpinBox->clear();
        } else {
            integerNumberDigitsAction->setData(*integerWidths.begin());
            integerNumberDigitsAction->trigger();
        }

        if (integerBases.size() != 1) {
            EQt::SpinBox* horizontalIntegerBaseSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
                "horizontal_integer_base_spin_box"
            );
            EQt::SpinBox* verticalIntegerBaseSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
                "vertical_integer_base_spin_box"
            );

            horizontalIntegerBaseSpinBox->clear();
            verticalIntegerBaseSpinBox->clear();
        } else {
            integerBaseAction->setData(*integerBases.begin());
            integerBaseAction->trigger();
        }

        if (integerStyles.size() != 1) {
            EQt::ComboBox* horizontalIntegerStyleComobBox = dock->EQt::ProgrammaticWidget::widget<EQt::ComboBox>(
                "horizontal_integer_number_style_combo_box"
            );
            EQt::ComboBox* verticalIntegerStyleComobBox = dock->EQt::ProgrammaticWidget::widget<EQt::ComboBox>(
                "vertical_integer_number_style_combo_box"
            );

            horizontalIntegerStyleComobBox->clearEditText();
            verticalIntegerStyleComobBox->clearEditText();
        } else {
            integerNumberStyleAction->setData(QVariant::fromValue(*integerStyles.begin()));
            integerNumberStyleAction->trigger();
        }

        integerNumberDigitsAction->setEnabled(true);
        integerBaseAction->setEnabled(true);
        integerNumberStyleAction->setEnabled(true);
    } else {
        integerNumberDigitsAction->setEnabled(false);
        integerBaseAction->setEnabled(false);
        integerNumberStyleAction->setEnabled(false);
    }

    if (aggregationsByCapability.contains(Ld::RealDataTypeFormat::formatName) ||
        aggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)      ) {
        hasDataTypePresentation = true;

        const Ld::RealDataTypeFormat::Aggregation& aggregation =
              aggregationsByCapability.contains(Ld::RealDataTypeFormat::formatName)
            ? aggregationsByCapability.aggregationForFormat<Ld::RealDataTypeFormat>()
            : aggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        const QSet<unsigned>&                                precisions  = aggregation.precisions();
        const QSet<double>&                                  multipliers = aggregation.multipliers();
        const QSet<Ld::RealDataTypeFormat::RealNumberStyle>& styles      = aggregation.realNumberStyles();

        if (precisions.size() != 1) {
            EQt::SpinBox* horizontalRealNumberPrecisionSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
                "horizontal_real_number_precision_spin_box"
            );
            EQt::SpinBox* verticalRealNumberPrecisionSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
                "vertical_real_number_precision_spin_box"
            );

            horizontalRealNumberPrecisionSpinBox->clear();
            verticalRealNumberPrecisionSpinBox->clear();
        } else {
            realNumberPrecisionAction->setData(QVariant::fromValue(*precisions.begin()));
            realNumberPrecisionAction->trigger();
        }

        if (multipliers.size() != 1) {
            EQt::LineEdit* horizontalMultiplierLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::LineEdit>(
                "horizontal_real_number_multiplier_line_edit"
            );
            EQt::LineEdit* verticalMultiplierLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::LineEdit>(
                "vertical_real_number_multiplier_line_edit"
            );

            horizontalMultiplierLineEdit->clear();
            verticalMultiplierLineEdit->clear();
        } else {
            realNumberMultiplerAction->setData(QVariant::fromValue(*multipliers.begin()));
            realNumberMultiplerAction->trigger();
        }

        if (styles.size() != 1) {
            EQt::ComboBox* horizontalStyleComboBox = dock->EQt::ProgrammaticWidget::widget<EQt::ComboBox>(
                "horizontal_real_number_style_combo_box"
            );
            EQt::ComboBox* verticalStyleComboBox = dock->EQt::ProgrammaticWidget::widget<EQt::ComboBox>(
                "vertical_real_number_style_combo_box"
            );

            horizontalStyleComboBox->clearEditText();
            verticalStyleComboBox->clearEditText();
        } else {
            realNumberStyleAction->setData(QVariant::fromValue(*styles.begin()));
            realNumberStyleAction->trigger();
        }
    }

    if (aggregationsByCapability.contains(Ld::ComplexDataTypeFormat::formatName) ||
        aggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)         ) {
        hasDataTypePresentation = true;

        const Ld::ComplexDataTypeFormat::Aggregation& aggregation =
              aggregationsByCapability.contains(Ld::ComplexDataTypeFormat::formatName)
            ? aggregationsByCapability.aggregationForFormat<Ld::ComplexDataTypeFormat>()
            : aggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        QSet<Ld::ComplexDataTypeFormat::ImaginaryUnit> imaginaryUnits = aggregation.imaginaryUnits();

        imaginaryUnitIAction->setChecked(imaginaryUnits.contains(Ld::ComplexDataTypeFormat::ImaginaryUnit::I));
        imaginaryUnitJAction->setChecked(imaginaryUnits.contains(Ld::ComplexDataTypeFormat::ImaginaryUnit::J));

        complexNumberImaginaryUnitAction->setEnabled(true);
    } else {
        complexNumberImaginaryUnitAction->setEnabled(false);
    }

    formatValueFieldDialogAction->setEnabled(hasDataTypePresentation);
}


void ValueFieldViewProxy::updateNumericValueFormatDialog(NumericValueFormatDialog* numericValueFieldDialog) {
    QSet<NumericValueFormatDialog::ValueType> supportedTypes;

    if (currentAggregationsByCapability.contains(Ld::BooleanDataTypeFormat::formatName) ||
        currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)         ) {
        supportedTypes.insert(NumericValueFormatDialog::ValueType::BOOLEAN);

        const Ld::BooleanDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::BooleanDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::BooleanDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        const QSet<Ld::BooleanDataTypeFormat::BooleanStyle> booleanStyles = aggregation.booleanStyles();
        numericValueFieldDialog->setBooleanStyle(booleanStyles);
    }

    if (currentAggregationsByCapability.contains(Ld::IntegerDataTypeFormat::formatName) ||
        currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)         ) {
        supportedTypes.insert(NumericValueFormatDialog::ValueType::INTEGER);

        const Ld::IntegerDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::IntegerDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::IntegerDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        const QSet<unsigned>&                                      integerWidths = aggregation.widths();
        const QSet<unsigned>&                                      integerBases  = aggregation.bases();
        const QSet<Ld::IntegerDataTypeFormat::IntegerNumberStyle>& integerStyles = aggregation.integerNumberStyles();

        if (integerWidths.size() != 1) {
            numericValueFieldDialog->setMantissaDigits(NumericValueFormatDialog::mantissaDigitsNotSet);
        } else {
            numericValueFieldDialog->setMantissaDigits(*integerWidths.begin());
        }

        numericValueFieldDialog->setBases(integerBases);
        numericValueFieldDialog->setIntegerNumberStyles(integerStyles);
    }

    if (currentAggregationsByCapability.contains(Ld::RealDataTypeFormat::formatName) ||
        currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)      ) {
        supportedTypes.insert(NumericValueFormatDialog::ValueType::REAL);

        const Ld::RealDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::RealDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::RealDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        const QSet<unsigned>&                                precisions  = aggregation.precisions();
        const QSet<double>&                                  multipliers = aggregation.multipliers();
        const QSet<Ld::RealDataTypeFormat::RealNumberStyle>& styles      = aggregation.realNumberStyles();

        if (precisions.size() != 1) {
            numericValueFieldDialog->setPrecisionUnset();
        } else {
            unsigned precision = *precisions.begin();
            if (precision == Ld::RealDataTypeFormat::inferPrecision) {
                numericValueFieldDialog->setPrecisionInferred();
            } else {
                numericValueFieldDialog->setPrecision(precision);
            }
        }

        if (multipliers.size() != 1) {
            numericValueFieldDialog->setMultiplicationFactorUnset();
        } else {
            numericValueFieldDialog->setMultiplicationFactor(*multipliers.begin());
        }

        numericValueFieldDialog->setRealNumberStyles(styles);
    }

    if (currentAggregationsByCapability.contains(Ld::ComplexDataTypeFormat::formatName) ||
        currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)         ) {
        supportedTypes.insert(NumericValueFormatDialog::ValueType::COMPLEX);

        const Ld::ComplexDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::ComplexDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::ComplexDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        QSet<Ld::ComplexDataTypeFormat::ImaginaryUnit> imaginaryUnits = aggregation.imaginaryUnits();
        numericValueFieldDialog->setImaginaryUnits(imaginaryUnits);
    }

    if (currentAggregationsByCapability.contains(Ld::SetDataTypeFormat::formatName)   ||
        currentAggregationsByCapability.contains(Ld::TupleDataTypeFormat::formatName) ||
        currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)       ) {
        if (currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)) {
            supportedTypes.insert(NumericValueFormatDialog::ValueType::SET);
            supportedTypes.insert(NumericValueFormatDialog::ValueType::TUPLE);
        } else {
            if (currentAggregationsByCapability.contains(Ld::SetDataTypeFormat::formatName)) {
                supportedTypes.insert(NumericValueFormatDialog::ValueType::SET);
            }

            if (currentAggregationsByCapability.contains(Ld::TupleDataTypeFormat::formatName)) {
                supportedTypes.insert(NumericValueFormatDialog::ValueType::TUPLE);
            }
        }

        if (currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)    ||
            currentAggregationsByCapability.contains(Ld::TupleDataTypeFormat::formatName)    ) {
            const Ld::TupleDataTypeFormat::Aggregation& tupleAggregation =
                  currentAggregationsByCapability.contains(Ld::TupleDataTypeFormat::formatName)
                ? currentAggregationsByCapability.aggregationForFormat<Ld::TupleDataTypeFormat>()
                : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

            if (tupleAggregation.displayModes().size() != 1) {
                numericValueFieldDialog->setTupleDisplayModeUnset();
            } else {
                numericValueFieldDialog->setTupleDisplayMode(*tupleAggregation.displayModes().begin());
            }
        }

        const Ld::ListDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::ListDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::ListDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        if (aggregation.leadingMemberCounts().size() != 1) {
            numericValueFieldDialog->setLeadingMemberCountUnset();
        } else {
            numericValueFieldDialog->setLeadingMemberCount(*aggregation.leadingMemberCounts().begin());
        }

        if (aggregation.trailingMemberCounts().size() != 1) {
            numericValueFieldDialog->setTrailingMemberCountUnset();
        } else {
            numericValueFieldDialog->setTrailingMemberCount(*aggregation.trailingMemberCounts().begin());
        }

        if (aggregation.showHiddenMemberCounts().bothTrueAndFalse()) {
            numericValueFieldDialog->setShowHideHiddenMemberCountsUnset();
        } else {
            numericValueFieldDialog->setShowHiddenMemberCounts(aggregation.showHiddenMemberCounts().allTrue());
        }
    }

    if (currentAggregationsByCapability.contains(Ld::MatrixDataTypeFormat::formatName) ||
        currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)        ) {
        if (currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)) {
            supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_BOOLEAN);
            supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_INTEGER);
            supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_REAL);
            supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_COMPLEX);
        } else {
            if (currentAggregationsByCapability.contains(Ld::MatrixBooleanDataTypeFormat::formatName)) {
                supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_BOOLEAN);
            }

            if (currentAggregationsByCapability.contains(Ld::MatrixIntegerDataTypeFormat::formatName)) {
                supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_INTEGER);
            }

            if (currentAggregationsByCapability.contains(Ld::MatrixRealDataTypeFormat::formatName)) {
                supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_REAL);
            }

            if (currentAggregationsByCapability.contains(Ld::MatrixComplexDataTypeFormat::formatName)) {
                supportedTypes.insert(NumericValueFormatDialog::ValueType::MATRIX_COMPLEX);
            }
        }

        const Ld::MatrixDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::MatrixDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::MatrixDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        if (aggregation.leadingRowsCounts().size() != 1) {
            numericValueFieldDialog->setLeadingRowsCountUnset();
        } else {
            numericValueFieldDialog->setLeadingRowsCount(*aggregation.leadingRowsCounts().begin());
        }

        if (aggregation.trailingRowsCounts().size() != 1) {
            numericValueFieldDialog->setTrailingRowsCountUnset();
        } else {
            numericValueFieldDialog->setTrailingRowsCount(*aggregation.trailingRowsCounts().begin());
        }

        if (aggregation.leadingColumnsCounts().size() != 1) {
            numericValueFieldDialog->setLeadingColumnsCountUnset();
        } else {
            numericValueFieldDialog->setLeadingColumnsCount(*aggregation.leadingColumnsCounts().begin());
        }

        if (aggregation.trailingColumnsCounts().size() != 1) {
            numericValueFieldDialog->setTrailingColumnsCountUnset();
        } else {
            numericValueFieldDialog->setTrailingColumnsCount(*aggregation.trailingColumnsCounts().begin());
        }
    }

    if (currentAggregationsByCapability.contains(Ld::ValueFieldFormat::formatName)) {
        const Ld::ValueFieldFormat::Aggregation&
            aggregation = currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        const QSet<Ld::VariableName>& variableNames = aggregation.variableNames();
        if (variableNames.size() != 1) {
            numericValueFieldDialog->setVariableName(Ld::VariableName());
        } else {
            numericValueFieldDialog->setVariableName(*variableNames.begin());
        }
        numericValueFieldDialog->setVariableNameFieldsEnabled();
    } else {
        numericValueFieldDialog->setVariableNameFieldsDisabled();
    }

    numericValueFieldDialog->setSupportedDataTypes(supportedTypes);

    if (!supportedTypes.isEmpty()) {
        const Ld::NumericDataTypeFormat::Aggregation& aggregation =
              currentAggregationsByCapability.contains(Ld::NumericDataTypeFormat::formatName)
            ? currentAggregationsByCapability.aggregationForFormat<Ld::NumericDataTypeFormat>()
            : currentAggregationsByCapability.aggregationForFormat<Ld::ValueFieldFormat>();

        if (aggregation.upperCase().allTrue()) {
            numericValueFieldDialog->setUpperCase();
        } else if (aggregation.upperCase().allFalse()) {
            numericValueFieldDialog->setLowerCase();
        } else {
            numericValueFieldDialog->setCaseUnset();
        }

        const QSet<QFont>& qtFonts = aggregation.qtFonts();
        if (qtFonts.size() != 1) {
            QFont applicationFont = Ld::CharacterFormat::applicationDefaultTextFont()->toQFont();
            numericValueFieldDialog->setSampleFont(applicationFont);
        } else {
            numericValueFieldDialog->setSampleFont(*qtFonts.begin());
        }
    }
}


void ValueFieldViewProxy::updateImaginaryUnit(Ld::ComplexDataTypeFormat::ImaginaryUnit newValue) {
    UpdateFormatCommand::FormatsByElement formatsByElement;

    Ld::ElementPointerSet elements = currentAggregationsByCapability.elements();
    for (  Ld::ElementPointerSet::iterator elementIterator    = elements.begin(),
                                           elementEndIterator = elements.end()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        if (!element.isNull()) {
            Ld::FormatPointer format = element->format();
            if (!format.isNull()                                                             &&
                (format->capabilities().contains(Ld::ComplexDataTypeFormat::formatName) ||
                 format->capabilities().contains(Ld::ValueFieldFormat::formatName)         ) &&
                format.dynamicCast<Ld::ComplexDataTypeFormat>()->imaginaryUnit() != newValue    ) {
                QSharedPointer<Ld::ComplexDataTypeFormat>
                    newFormat = format->clone().dynamicCast<Ld::ComplexDataTypeFormat>();

                newFormat->setImaginaryUnit(newValue);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void ValueFieldViewProxy::issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement) {
    ViewWidget* view = ValueFieldViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement));
}


void ValueFieldViewProxy::insertElement(Ld::ElementPointer newElement) {
    InsertElementCommand::FinalCursorPosition finalPosition;

    if (newElement->numberTextRegions() > 0) {
        finalPosition = InsertElementCommand::FinalCursorPosition::ELEMENT_START_OF_TEXT;
    } else if (newElement->childPlacement() != Ld::Element::ChildPlacement::NONE) {
        finalPosition = InsertElementCommand::FinalCursorPosition::FIRST_CHILD_ELEMENT;
    } else {
        finalPosition = InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE;
    }

    InsertElementCommand* newCommand = new InsertElementCommand(
        newElement,
        InsertElementCommand::InsertionLocation::AT_CURSOR,
        finalPosition
    );

    view()->insertCommand(newCommand);
}


void ValueFieldViewProxy::restoreFocus() {
    view()->restoreFocus();
}
