/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionMarshaller class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FUNCTION_MARSHALLER_H
#define FUNCTION_MARSHALLER_H

#include <QObject>

#include <ld_function_data.h>

/**
 * Marshaller for the function actions.  Re-emits actions with function data used to instantiate the function included.
 */
class FunctionMarshaller:public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] functionData The function data used to define this function.
         *
         * \param[in] parent       Pointer to the parent object.
         */
        explicit FunctionMarshaller(const Ld::FunctionData& functionData, QObject* parent = Q_NULLPTR);

        ~FunctionMarshaller() override;

    signals:
        /**
         * Signal that is emitted when a function should be inserted.  You can use this signal to issue the appropriate
         * signal to a \ref ViewWidget instance.
         *
         * \param[out] function The function to be inserted.
         */
        void insertFunction(const Ld::FunctionData& function);

    public slots:
        /**
         * Slot that can be triggered by the action.
         */
        void triggered();

    private:
        /**
         * The function data to be marshalled by this class.
         */
        Ld::FunctionData currentFunctionData;
};

#endif
