/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MathViewProxyBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSet>

#include <algorithm>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format_structures.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>
#include <ld_multiplication_operator_format.h>
#include <ld_division_operator_format.h>
#include <ld_parenthesis_format.h>
#include <ld_function_format.h>

#include "insert_element_command.h"
#include "view_proxy.h"
#include "main_window.h"
#include "document.h"
#include "view_widget.h"
#include "math_view_proxy_base.h"

MathViewProxyBase::MathViewProxyBase(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {}


MathViewProxyBase::~MathViewProxyBase() {}


Ld::FormatPointer MathViewProxyBase::mathCharacterFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer               format = rootElement->defaultMathTextFormat()->clone();
    return format;
}


Ld::FormatPointer MathViewProxyBase::mathIdentifierCharacterFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer               format = rootElement->defaultMathIdentifierFormat()->clone();
    return format;
}


void MathViewProxyBase::copyFontData(Ld::FormatPointer outputFormat, Ld::FormatPointer inputFormat) {
    QSharedPointer<Ld::FontFormat> inputFontFormat  = inputFormat.dynamicCast<Ld::FontFormat>();
    QSharedPointer<Ld::FontFormat> outputFontFormat = outputFormat.dynamicCast<Ld::FontFormat>();

    outputFontFormat->setFamily(inputFontFormat->family());
    outputFontFormat->setFontSize(inputFontFormat->fontSize());
    outputFontFormat->setFontWeight(inputFontFormat->fontWeight());
    outputFontFormat->setItalics(inputFontFormat->italics());
    outputFontFormat->setUnderline(inputFontFormat->underline());
    outputFontFormat->setOverline(inputFontFormat->overline());
    outputFontFormat->setFontColor(inputFontFormat->fontColor());
    outputFontFormat->setFontBackgroundColor(inputFontFormat->fontBackgroundColor());
}


Ld::FormatPointer MathViewProxyBase::mathFunctionFontFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    return Ld::FormatPointer(new Ld::FontFormat(*rootElement->defaultMathTextFormat()));
}


Ld::FormatPointer MathViewProxyBase::operatorFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer format = Ld::Format::create(Ld::OperatorFormat::formatName);
    copyFontData(format, rootElement->defaultMathTextFormat());

    return format;
}


Ld::FormatPointer MathViewProxyBase::multiplicationOperatorFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer format = Ld::Format::create(Ld::MultiplicationOperatorFormat::formatName);
    copyFontData(format, rootElement->defaultMathTextFormat());

    return format;
}


Ld::FormatPointer MathViewProxyBase::divisionOperatorFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer format = Ld::Format::create(Ld::DivisionOperatorFormat::formatName);
    copyFontData(format, rootElement->defaultMathTextFormat());

    return format;
}


Ld::FormatPointer MathViewProxyBase::parenthesisOperatorFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer format = Ld::Format::create(Ld::ParenthesisFormat::formatName);
    copyFontData(format, rootElement->defaultMathTextFormat());

    return format;
}


Ld::FormatPointer MathViewProxyBase::functionFormat() const {
    ViewWidget*                     view        = MathViewProxyBase::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer format = Ld::Format::create(Ld::FunctionFormat::formatName);
    copyFontData(format, rootElement->defaultMathFunctionFormat());

    return format;
}


void MathViewProxyBase::insertElement(Ld::ElementPointer newElement) {
    InsertElementCommand::FinalCursorPosition finalPosition;

    if (newElement->numberTextRegions() > 0) {
        finalPosition = InsertElementCommand::FinalCursorPosition::ELEMENT_START_OF_TEXT;
    } else if (newElement->childPlacement() != Ld::Element::ChildPlacement::NONE) {
        finalPosition = InsertElementCommand::FinalCursorPosition::FIRST_CHILD_ELEMENT;
    } else {
        finalPosition = InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE;
    }

    insertElement(newElement, finalPosition);
}


void MathViewProxyBase::insertElement(
        Ld::ElementPointer                        newElement,
        InsertElementCommand::FinalCursorPosition finalCursorPosition
    ) {
    InsertElementCommand* newCommand = new InsertElementCommand(
        newElement,
        InsertElementCommand::InsertionLocation::AT_CURSOR,
        finalCursorPosition
    );

    view()->insertCommand(newCommand);
}


void MathViewProxyBase::restoreFocus() {
    view()->restoreFocus();
}
