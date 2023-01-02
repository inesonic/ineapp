/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RuntimeDiagnostic class.
***********************************************************************************************************************/

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QString>
#include <QSharedPointer>

#include <ld_element_structures.h>
#include <ld_translation_phase.h>
#include <ld_diagnostic.h>

#include <ld_element_structures.h>
#include <ld_translation_phase.h>
#include <ld_diagnostic.h>

#include "runtime_diagnostic.h"

// Static assert below will fire if new Model::AbortReason values are inserted without also updating
// RuntimeDiagnostic::Code to match.

static_assert(
       static_cast<RuntimeDiagnostic::Code>(Model::AbortReason::NUMBER_ABORT_REASONS)
    == RuntimeDiagnostic::Code::PAUSED_ON_USER_REQUEST,
    "RuntimeDiagnostic::Code is missing values from Model::AbortReason.  Update RuntimeDiagnostic::Code to remove "
    "this error."
);

const Ld::TranslationPhase RuntimeDiagnostic::dummyTranslationPhase;

RuntimeDiagnostic::RuntimeDiagnostic() {
    currentDiagnosticCode = Code::NO_DIAGNOSTIC;
}


RuntimeDiagnostic::RuntimeDiagnostic(
        Ld::ElementPointer      element,
        RuntimeDiagnostic::Type diagnosticType,
        RuntimeDiagnostic::Code diagnosticCode
    ):Ld::Diagnostic(
        element,
        QString(),
        diagnosticType
    ) {
    currentDiagnosticCode = diagnosticCode;
}


RuntimeDiagnostic::RuntimeDiagnostic(const RuntimeDiagnostic& other):Ld::Diagnostic(other) {
    currentDiagnosticCode = other.currentDiagnosticCode;
}


RuntimeDiagnostic::~RuntimeDiagnostic() {}


const Ld::TranslationPhase& RuntimeDiagnostic::translationPhase() const {
    return dummyTranslationPhase;
}


RuntimeDiagnostic::Code RuntimeDiagnostic::diagnosticCode() const {
    return currentDiagnosticCode;
}


QString RuntimeDiagnostic::diagnosticMessage() const {
    QString message;
    QString description;

    switch (currentDiagnosticCode) {
        case Code::NO_DIAGNOSTIC: {
            description = tr("No diagnostic message.");
            break;
        }

        case Code::USER_REQUEST: {
            description = tr("Aborted at user request.");
            break;
        }

        case Code::INSUFFICIENT_MEMORY: {
            description = tr("Insufficient memory.");
            break;
        }

        case Code::SYSTEM: {
            description = tr("System error reported.");
            break;
        }

        case Code::INTERNAL_ERROR: {
            description = tr("Internal math library error.");
            break;
        }

        case Code::MEMORY_ALLOCATION_ERROR: {
            description = tr("Memory allocation error.");
            break;
        }

        case Code::FILE_OPEN_ERROR: {
            description = tr("File open error.");
            break;
        }

        case Code::FILE_READ_ERROR: {
            description = tr("File read error.");
            break;
        }

        case Code::FILE_WRITE_ERROR: {
            description = tr("File write error.");
            break;
        }

        case Code::FILE_SEEK_ERROR: {
            description = tr("File seek error.");
            break;
        }

        case Code::FILE_CLOSE_ERROR: {
            description = tr("File close error.");
            break;
        }

        case Code::INVALID_FILE_NUMBER: {
            description = tr("Invalid file number");
            break;
        }

        case Code::UNKNOWN_FILE_TYPE: {
            description = tr("Unknown file type");
            break;
        }

        case Code::MALFORMED_STRING: {
            description = tr("Malformed string");
            break;
        }

        case Code::CAN_NOT_CONVERT_TO_STRING: {
            description = tr("Can not convert data to string");
            break;
        }

        case Code::INVALID_RUNTIME_CONVERSION: {
            description = tr("Invalid runtime type conversion.");
            break;
        }

        case Code::INVALID_RANGE_PARAMETER: {
            description = tr("Invalid range parameter.");
            break;
        }

        case Code::INVALID_CONTAINER_CONTENTS: {
            description = tr("Invalid container object contents.");
            break;
        }

        case Code::INVALID_ROW_INDEX: {
            description = tr("Invalid row index value.");
            break;
        }

        case Code::INVALID_COLUMN_INDEX: {
            description = tr("Invalid column index value.");
            break;
        }

        case Code::INVALID_INDEX: {
            description = tr("Invalid index value.");
            break;
        }

        case Code::INCOMPATIBLE_MATRIX_DIMENSIONS: {
            description = tr("Incompatible matrix dimensions.");
            break;
        }

        case Code::INVALID_MATRIX_DIMENSIONS: {
            description = tr("Invalid matrix dimensions.");
            break;
        }

        case Code::MATRIX_IS_SINGULAR: {
            description= tr("Matrix is singular.");
            break;
        }

        case Code::TYPE_DOES_NOT_SUPPORT_SUBSCRIPTS: {
            description = tr("Contained type does not support this subscripting operation.");
            break;
        }

        case Code::INVALID_NUMERIC_VALUE: {
            description = tr("Invalid numeric value.");
            break;
        }

        case Code::RESULT_IS_NAN: {
            description = tr("Result is NaN.");
            break;
        }

        case Code::RESULT_IS_INFINITE: {
            description = tr("Result is infinite.");
            break;
        }

        case Code::CAN_NOT_CONVERGE: {
            description = tr("Can not converge on value.");
            break;
        }

        case Code::PAUSED_ON_USER_REQUEST: {
            description = tr("User request");
            break;
        }

        case Code::PAUSED_AT_INSTRUCTION_BREAKPOINT: {
            description = tr("Instruction breakpoint");
            break;
        }

        case Code::PAUSED_ON_VARIABLE_UPDATE: {
            description = tr("Variable update");
            break;
        }

        default: {
            description = tr("Undefined error %1.").arg(static_cast<unsigned>(diagnosticCode()));
            break;
        }
    }

    message = tr("%1: %2").arg(diagnosticTypeMessage()).arg(description);
    return message;
}


RuntimeDiagnostic& RuntimeDiagnostic::operator=(const RuntimeDiagnostic& other) {
    Ld::Diagnostic::operator=(other);
    currentDiagnosticCode = other.currentDiagnosticCode;

    return *this;
}
