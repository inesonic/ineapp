/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixViewProxy class.
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
#include <ld_parenthesis_format.h>

#include <ld_element_cursor.h>

#include <ld_root_element.h>
#include <ld_matrix_operator_element.h>
#include <ld_matrix_combine_left_to_right_operator_element.h>
#include <ld_matrix_combine_top_to_bottom_operator_element.h>
#include <ld_hadamard_product_operator_element.h>
#include <ld_kronecker_product_operator_element.h>
#include <ld_matrix_transpose_operator_element.h>
#include <ld_matrix_conjugate_transpose_operator_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "document.h"
#include "view_widget.h"
#include "cursor.h"
#include "matrix_size_dialog.h"
#include "math_view_proxy_base.h"
#include "matrix_view_proxy.h"

MatrixViewProxy::MatrixViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):MathViewProxyBase(
        view,
        window
    ) {}


MatrixViewProxy::~MatrixViewProxy() {}


void MatrixViewProxy::bindDynamic() {
    MainWindow* window     = MatrixViewProxy::window();
//    ViewWidget* viewWidget = MatrixViewProxy::view();

    QAction* matrixOperatorAction = window->action("matrix_operator_action");
    sessionConnect(
        matrixOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::matrixOperatorButtonClicked
    );

    QAction* matrixCombineLeftToRightOperatorAction = window->action("matrix_combine_left_to_right_operator_action");
    sessionConnect(
        matrixCombineLeftToRightOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::matrixCombineLeftToRightButtonClicked
    );

    QAction* matrixCombineTopToBottomOperatorAction = window->action("matrix_combine_top_to_bottom_operator_action");
    sessionConnect(
        matrixCombineTopToBottomOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::matrixCombineTopToBottomButtonClicked
    );

    QAction* hadamardProductOperatorAction = window->action("hadamard_product_operator_action");
    sessionConnect(
        hadamardProductOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::hadamardProductOperatorButtonClicked
    );

    QAction* kroneckerProductOperatorAction = window->action("kronecker_product_operator_action");
    sessionConnect(
        kroneckerProductOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::kroneckerProductOperatorButtonClicked
    );

    QAction* matrixTransposeOperatorAction = window->action("matrix_transpose_operator_action");
    sessionConnect(
        matrixTransposeOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::matrixTransposeOperatorButtonClicked
    );

    QAction* matrixConjugateTransposeOperatorAction = window->action("matrix_conjugate_transpose_operator_action");
    sessionConnect(
        matrixConjugateTransposeOperatorAction,
        &QAction::triggered,
        &MatrixViewProxy::matrixConjugateTransposeOperatorButtonClicked
    );
}


void MatrixViewProxy::unbindDynamic() {
    sessionDisconnectAll();
}


void MatrixViewProxy::matrixOperatorButtonClicked() {
    MatrixSizeDialog sizeDialog(window());

    int result = sizeDialog.exec();
    if (result == MatrixSizeDialog::Accepted) {
        QSharedPointer<Ld::OperatorFormat> format = operatorFormat().dynamicCast<Ld::OperatorFormat>();
        format->setParenthesisStyle(Ld::OperatorFormat::ParenthesisStyle::BRACKETS);

        QSharedPointer<Ld::MatrixOperatorElement> element = Ld::Element::create(Ld::MatrixOperatorElement::elementName)
                                                            .dynamicCast<Ld::MatrixOperatorElement>();

        element->setNumberRows(sizeDialog.numberRows(), nullptr);
        element->setNumberColumns(sizeDialog.numberColumns(), nullptr);
        element->setFormat(format);

        insertElement(element);
    }
}


void MatrixViewProxy::matrixCombineLeftToRightButtonClicked() {
    QSharedPointer<Ld::MatrixCombineLeftToRightOperatorElement>
        element = Ld::Element::create(Ld::MatrixCombineLeftToRightOperatorElement::elementName)
                  .dynamicCast<Ld::MatrixCombineLeftToRightOperatorElement>();


    QSharedPointer<Ld::OperatorFormat> format = operatorFormat().dynamicCast<Ld::OperatorFormat>();
    format->setParenthesisStyle(Ld::OperatorFormat::ParenthesisStyle::BRACKETS);

    element->setFormat(format);
    insertElement(element);
}


void MatrixViewProxy::matrixCombineTopToBottomButtonClicked() {
    QSharedPointer<Ld::MatrixCombineTopToBottomOperatorElement>
        element = Ld::Element::create(Ld::MatrixCombineTopToBottomOperatorElement::elementName)
                  .dynamicCast<Ld::MatrixCombineTopToBottomOperatorElement>();


    QSharedPointer<Ld::OperatorFormat> format = operatorFormat().dynamicCast<Ld::OperatorFormat>();
    format->setParenthesisStyle(Ld::OperatorFormat::ParenthesisStyle::BRACKETS);

    element->setFormat(format);
    insertElement(element);
}


void MatrixViewProxy::hadamardProductOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::HadamardProductOperatorElement::elementName);
    element->setFormat(multiplicationOperatorFormat());

    insertElement(element);
}


void MatrixViewProxy::kroneckerProductOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::KroneckerProductOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void MatrixViewProxy::matrixTransposeOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::MatrixTransposeOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void MatrixViewProxy::matrixConjugateTransposeOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::MatrixConjugateTransposeOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}
