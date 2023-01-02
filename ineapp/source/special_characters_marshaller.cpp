/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SpecialCharactersMarshaller class.
***********************************************************************************************************************/

#include <QObject>

#include <ld_special_characters.h>

#include "command_container.h"
#include "insert_string_command.h"
#include "special_characters_marshaller.h"

SpecialCharactersMarshaller::SpecialCharactersMarshaller(
        Ld::SpecialCharacters::UnicodeValue unicodeValue,
        QObject*                            parent
    ):QObject(
        parent
    ) {
    currentUnicodeValue = unicodeValue;
}


SpecialCharactersMarshaller::~SpecialCharactersMarshaller() {}


void SpecialCharactersMarshaller::triggered() {
    emit commandIssued(CommandContainer(new InsertStringCommand(currentUnicodeValue)));
}
