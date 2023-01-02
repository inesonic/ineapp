/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SpecialCharactersMarshaller class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SPECIAL_CHARACTERS_MARSHALLER_H
#define SPECIAL_CHARACTERS_MARSHALLER_H

#include <QObject>

#include <ld_special_characters.h>

class CommandContainer;

/**
 * Marshaller for the special characters actions.  Re-emits actions with the special character unicode value included.
 */
class SpecialCharactersMarshaller:public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] unicodeValue The unicode value to be marshalled to the proxy.
         *
         * \param[in] parent       Pointer to the parent object.
         */
        explicit SpecialCharactersMarshaller(
            Ld::SpecialCharacters::UnicodeValue unicodeValue,
            QObject*                            parent = Q_NULLPTR
        );

        ~SpecialCharactersMarshaller() override;

    signals:
        /**
         * Signal that is emitted when a special character should be inserted.  Issues the appropriate command to a
         * \ref ViewWidget instance.
         *
         * \param[out] command The command to be issued to the view widget.
         */
        void commandIssued(const CommandContainer& command);

    public slots:
        /**
         * Slot that can be triggered by the action.
         */
        void triggered();

    private:
        /**
         * The unicode value to be marshalled.
         */
        Ld::SpecialCharacters::UnicodeValue currentUnicodeValue;
};

#endif
