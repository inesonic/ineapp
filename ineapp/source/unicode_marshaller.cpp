/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref UnicodeMarshaller class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <cstdint>

#include "unicode_marshaller.h"

UnicodeMarshaller::UnicodeMarshaller(UnicodeMarshaller::UnicodeValue unicodeValue, QObject* parent):QObject(parent) {
    currentUnicodeValue = unicodeValue;
}


UnicodeMarshaller::~UnicodeMarshaller() {}


void UnicodeMarshaller::setUnicodeValue(UnicodeValue newUnicodeValue) {
    currentUnicodeValue = newUnicodeValue;
}


UnicodeMarshaller::UnicodeValue UnicodeMarshaller::unicodeValue() const {
    return currentUnicodeValue;
}


void UnicodeMarshaller::trigger() {
    emit triggered(currentUnicodeValue);
    emit triggered(QString(QChar(currentUnicodeValue)));
}
