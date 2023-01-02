/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionMarshaller class.
***********************************************************************************************************************/

#include <QObject>

#include <ld_function_data.h>

#include "function_marshaller.h"

FunctionMarshaller::FunctionMarshaller(
        const Ld::FunctionData& functionData,
        QObject*                parent
    ):QObject(
        parent
    ),currentFunctionData(
        functionData
    ) {}


FunctionMarshaller::~FunctionMarshaller() {}


void FunctionMarshaller::triggered() {
    emit insertFunction(currentFunctionData);
}
