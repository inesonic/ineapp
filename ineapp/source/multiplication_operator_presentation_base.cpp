/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MultiplicationOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_multiplication_operator_format.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "multiplication_operator_presentation_base.h"

MultiplicationOperatorPresentationBase::MultiplicationOperatorPresentationBase() {}


MultiplicationOperatorPresentationBase::~MultiplicationOperatorPresentationBase() {}


QString MultiplicationOperatorPresentationBase::operatorString(Ld::FormatPointer operatorFormat) const {
    QString result;
    Ld::MultiplicationOperatorFormat::MultiplicationSymbol multiplicationSymbol;

    if (!operatorFormat.isNull()                                                              &&
        operatorFormat->capabilities().contains(Ld::MultiplicationOperatorFormat::formatName)    ) {
        QSharedPointer<Ld::MultiplicationOperatorFormat> multiplicationFormat =
            operatorFormat.dynamicCast<Ld::MultiplicationOperatorFormat>();

        multiplicationSymbol = multiplicationFormat->multiplicationSymbol();

        if (multiplicationSymbol == Ld::MultiplicationOperatorFormat::MultiplicationSymbol::DEFAULT) {
            multiplicationSymbol = defaultSymbol();
        }
    } else {
        multiplicationSymbol = defaultSymbol();
    }

    switch (multiplicationSymbol) {
        case Ld::MultiplicationOperatorFormat::MultiplicationSymbol::INVALID: {
            assert(false);
            break;
        }

        case Ld::MultiplicationOperatorFormat::MultiplicationSymbol::DEFAULT: {
            assert(false);
            break;
        }

        case Ld::MultiplicationOperatorFormat::MultiplicationSymbol::SPACE: {
            result = QString(QChar(0x2009));
            break;
        }

        case Ld::MultiplicationOperatorFormat::MultiplicationSymbol::DOT: {
            result = tr(" %1 ").arg(QChar(0x2219));
            break;
        }

        case Ld::MultiplicationOperatorFormat::MultiplicationSymbol::CROSS: {
            result = tr(" %1 ").arg(QChar(0x00D7));
            break;
        }

        case Ld::MultiplicationOperatorFormat::MultiplicationSymbol::CIRCLE: {
            result = tr(" %1 ").arg(QChar(0x2218));
            break;
        }

        default: {
            assert(false);
            break;
        }
    }

    return result;
}
