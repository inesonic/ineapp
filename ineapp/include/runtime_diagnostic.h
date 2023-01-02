/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RuntimeDiagnostic class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RUNTIME_DIAGNOSTIC_H
#define RUNTIME_DIAGNOSTIC_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QSharedPointer>
#include <QString>

#include <model_api_types.h>

#include <ld_element_structures.h>
#include <ld_translation_phase.h>
#include <ld_diagnostic.h>

#include "app_common.h"

/**
 * That that reports run-time diagnostic conditions, including pause events.  This object uses a pimpl design pattern
 * allowing you to efficiently pass the diagnostic by value, if desired.
 */
class RuntimeDiagnostic:public Ld::Diagnostic {
    Q_DECLARE_TR_FUNCTIONS(RuntimeDiagnostic)

    public:
        /**
         * Type representing the diagnostic being reported.  Note values must align with Model::AbortReason.
         */
        enum class Code {
            /**
             * Indicates no diagnostic.
             */
            NO_DIAGNOSTIC = static_cast<int>(Model::AbortReason::NONE),

            /**
             * Indicates the model was aborted on user request.
             */
            USER_REQUEST = static_cast<int>(Model::AbortReason::USER_REQUEST),

            /**
             * Indicates that there is insufficient memory available.
             */
            INSUFFICIENT_MEMORY = static_cast<int>(Model::AbortReason::INSUFFICIENT_MEMORY),

            /**
             * Indicates an unknown system exception.
             */
            SYSTEM = static_cast<int>(Model::AbortReason::SYSTEM),

            /**
             * Indicates an undefined model internal error.
             */
            INTERNAL_ERROR = static_cast<int>(Model::AbortReason::INTERNAL_ERROR),

            /**
             * Indicates a memory allocation error.
             */
            MEMORY_ALLOCATION_ERROR = static_cast<int>(Model::AbortReason::MEMORY_ALLOCATION_ERROR),

            /**
             * Indicates a file open error.
             */
            FILE_OPEN_ERROR = static_cast<int>(Model::AbortReason::FILE_OPEN_ERROR),

            /**
             * Indicates a file read error.
             */
            FILE_READ_ERROR = static_cast<int>(Model::AbortReason::FILE_READ_ERROR),

            /**
             * Indicates a file write error.
             */
            FILE_WRITE_ERROR = static_cast<int>(Model::AbortReason::FILE_WRITE_ERROR),

            /**
             * Indicates an error during a seek.
             */
            FILE_SEEK_ERROR = static_cast<int>(Model::AbortReason::FILE_SEEK_ERROR),

            /**
             * Indicates an error during file close.
             */
            FILE_CLOSE_ERROR = static_cast<int>(Model::AbortReason::FILE_CLOSE_ERROR),

            /**
             * Indicates an invalid file number.
             */
            INVALID_FILE_NUMBER = static_cast<int>(Model::AbortReason::INVALID_FILE_NUMBER),

            /**
             * Indicates an unrecognized file type.
             */
            UNKNOWN_FILE_TYPE = static_cast<int>(Model::AbortReason::UNKNOWN_FILE_TYPE),

            /**
             * Indicates a string was not properly encoded.
             */
            MALFORMED_STRING = static_cast<int>(Model::AbortReason::MALFORMED_STRING),

            /**
             * Indicates a tuple can-not be converted into a string.
             */
            CAN_NOT_CONVERT_TO_STRING = static_cast<int>(Model::AbortReason::CAN_NOT_CONVERT_TO_STRING),

            /**
             * Indicates an invalid run-time type conversion.
             */
            INVALID_RUNTIME_CONVERSION = static_cast<int>(Model::AbortReason::INVALID_RUNTIME_CONVERSION),

            /**
             * Indicates type used as a parameter for a range is invalid.
             */
            INVALID_RANGE_PARAMETER = static_cast<int>(Model::AbortReason::INVALID_RANGE_PARAMETER),

            /**
             * Indicates a container class holds contents of an invalid type.
             */
            INVALID_CONTAINER_CONTENTS = static_cast<int>(Model::AbortReason::INVALID_CONTAINER_CONTENTS),

            /**
             * Indicates an invalid row was specified for a matrix.
             */
            INVALID_ROW_INDEX = static_cast<int>(Model::AbortReason::INVALID_ROW_INDEX),

            /**
             * Indicates an invalid column index was specified for a matrix.
             */
            INVALID_COLUMN_INDEX = static_cast<int>(Model::AbortReason::INVALID_COLUMN_INDEX),

            /**
             * Indicates an invalid index was specified for a matrix or tuple.
             */
            INVALID_INDEX = static_cast<int>(Model::AbortReason::INVALID_INDEX),

            /**
             * Indicates matrix dimensions were incompatible for the given operation.  This exception is mostly associated
             * with matrix multiplication operations.
             */
            INCOMPATIBLE_MATRIX_DIMENSIONS = static_cast<int>(Model::AbortReason::INCOMPATIBLE_MATRIX_DIMENSIONS),

            /**
             * Indicates matrix dimensions were invalid for the given operation.  This exception is mostly associated
             * with such things as calculation of the determinant or inverse of a matrix.
             */
            INVALID_MATRIX_DIMENSIONS = static_cast<int>(Model::AbortReason::INVALID_MATRIX_DIMENSIONS),

            /**
             * Indicates an attempt to invert a singular matrix.
             */
            MATRIX_IS_SINGULAR = static_cast<int>(Model::AbortReason::MATRIX_IS_SINGULAR),

            /**
             * Indicates attempt to access a subscript of a scalar type.
             */
            TYPE_DOES_NOT_SUPPORT_SUBSCRIPTS = static_cast<int>(Model::AbortReason::TYPE_DOES_NOT_SUPPORT_SUBSCRIPTS),

            /**
             * Indicates an invalid value.  This value is generally tied to probabilities and other values with an
             * expected limited range.
             */
            INVALID_NUMERIC_VALUE = static_cast<int>(Model::AbortReason::INVALID_NUMERIC_VALUE),

            /**
             * Indicates an NaN result.
             */
            RESULT_IS_NAN = static_cast<int>(Model::AbortReason::RESULT_IS_NAN),

            /**
             * Indicates an infinite result.
             */
            RESULT_IS_INFINITE = static_cast<int>(Model::AbortReason::RESULT_IS_INFINITE),

            /**
             * Indicates a function can not converge on a value.
             */
            CAN_NOT_CONVERGE = static_cast<int>(Model::AbortReason::CAN_NOT_CONVERGE),

            /**
             * Indicates we've paused at this position on user request.
             */
            PAUSED_ON_USER_REQUEST,

            /**
             * Indicates we've paused at this position due to an instruction breakpoint.
             */
            PAUSED_AT_INSTRUCTION_BREAKPOINT,

            /**
             * Indicates we've paused at this position due to a variable update.
             */
            PAUSED_ON_VARIABLE_UPDATE,
        };

        RuntimeDiagnostic();

        /**
         * Constructor
         *
         * \param[in] element        The element that is associated with this diagnostic.
         *
         * \param[in] diagnosticType Indicates if this diagnostic is fatal, a warning, etc.
         *
         * \param[in] diagnosticCode Indicates the type of diagnostic being reported.
         */
        RuntimeDiagnostic(
            Ld::ElementPointer element,
            Type               diagnosticType,
            Code               diagnosticCode
        );

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        RuntimeDiagnostic(const RuntimeDiagnostic& other);

        ~RuntimeDiagnostic() final;

        /**
         * Method that returns the translation phase where the error was detected.
         *
         * \return Returns the translation phase where the error was detected.
         */
        const Ld::TranslationPhase& translationPhase() const final;

        /**
         * Method you can use to obtain the diagnostic code.
         *
         * \return Returns the diagnostic code.
         */
        Code diagnosticCode() const;

        /**
         * Method you can use to obtain a diagnostic message.
         *
         * \return Returns a usable message describing the diagnostic.
         */
        QString diagnosticMessage() const final;

        /**
         * Assignment operator
         *
         * \param[in] other The instance to be copied.
         */
        RuntimeDiagnostic& operator=(const RuntimeDiagnostic& other);

    private:
        /**
         * A static dummy translation phase to make the \ref RuntimeDiagnostic::translationPhase method happy.
         */
        static const Ld::TranslationPhase dummyTranslationPhase;

        /**
         * The diagnostic code.
         */
        Code currentDiagnosticCode;
};

#endif
