/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandBase class.
***********************************************************************************************************************/

#include <QPointer>
#include <QSharedPointer>
#include <QString>

#include "cursor.h"
#include "edit_helpers.h"
#include "command.h"
#include "command_base.h"

CommandBase::CommandBase() {}


CommandBase::CommandBase(CursorPointer newCursor):Command(newCursor) {}


CommandBase::~CommandBase() {}
