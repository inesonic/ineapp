/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SpecialCharactersViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>

#include <ld_special_characters.h>

#include "command_container.h"
#include "main_window.h"
#include "view_widget.h"
#include "special_characters_marshaller.h"
#include "special_characters_view_proxy.h"

SpecialCharactersViewProxy::SpecialCharactersViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {}


SpecialCharactersViewProxy::~SpecialCharactersViewProxy() {}


void SpecialCharactersViewProxy::bindDynamic() {
    MainWindow* mainWindow = window();
    Q_ASSERT(mainWindow != Q_NULLPTR);

    ViewWidget* viewWidget = view();
    Q_ASSERT(viewWidget != Q_NULLPTR);

    QMap<UnicodeValue, QString> commandsByUnicode = characterCommandsByUnicodeValue(true, true, true, true);

    for (  QMap<UnicodeValue, QString>::const_iterator pos = commandsByUnicode.constBegin(),
                                                       end = commandsByUnicode.constEnd()
         ; pos != end
         ; ++pos
        ) {
        QString                      command        = pos.value();
        QString                      marshallerName = QString("special_character_marshaller_%1").arg(command);
        SpecialCharactersMarshaller* marshaller     = mainWindow->object<SpecialCharactersMarshaller>(marshallerName);

        connect(
            marshaller,
            &SpecialCharactersMarshaller::commandIssued,
            viewWidget,
            static_cast<void (ViewWidget::*)(const CommandContainer&)>(&ViewWidget::insertCommand)
        );
    }

    SpecialCharactersMarshaller* backslashMarshaller = mainWindow->object<SpecialCharactersMarshaller>(
        "special_character_marshaller_backslash"
    );

    connect(
        backslashMarshaller,
        &SpecialCharactersMarshaller::commandIssued,
        viewWidget,
        static_cast<void (ViewWidget::*)(const CommandContainer&)>(&ViewWidget::insertCommand)
    );
}


void SpecialCharactersViewProxy::unbindDynamic() {
    MainWindow* mainWindow = window();
    Q_ASSERT(mainWindow != Q_NULLPTR);

    ViewWidget* viewWidget = view();
    Q_ASSERT(viewWidget != Q_NULLPTR);

    QMap<UnicodeValue, QString> commandsByUnicode = characterCommandsByUnicodeValue(true, true, true, true);

    for (  QMap<UnicodeValue, QString>::const_iterator pos = commandsByUnicode.constBegin(),
                                                       end = commandsByUnicode.constEnd()
         ; pos != end
         ; ++pos
        ) {
        QString                      command        = pos.value();
        QString                      marshallerName = QString("special_character_marshaller_%1").arg(command);
        SpecialCharactersMarshaller* marshaller     = mainWindow->object<SpecialCharactersMarshaller>(marshallerName);

        disconnect(
            marshaller,
            &SpecialCharactersMarshaller::commandIssued,
            viewWidget,
            static_cast<void (ViewWidget::*)(const CommandContainer&)>(&ViewWidget::insertCommand)
        );
    }

    SpecialCharactersMarshaller* backslashMarshaller = mainWindow->object<SpecialCharactersMarshaller>(
        "special_character_marshaller_backslash"
    );

    disconnect(
        backslashMarshaller,
        &SpecialCharactersMarshaller::commandIssued,
        viewWidget,
        static_cast<void (ViewWidget::*)(const CommandContainer&)>(&ViewWidget::insertCommand)
    );
}
