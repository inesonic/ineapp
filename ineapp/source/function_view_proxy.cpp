/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSet>

#include <algorithm>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_view.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format_structures.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>
#include <ld_brace_conditional_format.h>
#include <ld_multiplication_operator_format.h>
#include <ld_parenthesis_format.h>

#include <ld_element_cursor.h>

#include <ld_root_element.h>
#include <ld_placeholder_element.h>
#include <ld_variable_name.h>
#include <ld_function_variant.h>
#include <ld_function_data.h>
#include <ld_function_database.h>
#include <ld_function_format.h>
#include <ld_function_element.h>
#include <ld_function_placeholder_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "document.h"
#include "view_widget.h"
#include "cursor.h"
#include "function_marshaller.h"
#include "function_dock_widget.h"
#include "insert_element_command.h"
#include "math_view_proxy_base.h"
#include "function_builder.h"
#include "function_view_proxy.h"

FunctionViewProxy::FunctionViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):MathViewProxyBase(
        view,
        window
    ) {}


FunctionViewProxy::~FunctionViewProxy() {}


void FunctionViewProxy::bindDynamic() {
    MainWindow* window     = FunctionViewProxy::window();
    ViewWidget* viewWidget = FunctionViewProxy::view();

    QAction* functionOperatorAction = window->action("function_operator_action");
    sessionConnect(
        functionOperatorAction,
        &QAction::triggered,
        &FunctionViewProxy::functionOperatorButtonClicked
    );

    QList<Ld::FunctionData> functionDataList = Ld::FunctionDatabase::functions();
    for (  QList<Ld::FunctionData>::const_iterator functionDataIterator    = functionDataList.constBegin(),
                                                   functionDataEndIterator = functionDataList.constEnd()
         ; functionDataIterator != functionDataEndIterator
         ; ++functionDataIterator
        ) {
        const Ld::FunctionData& functionData   = *functionDataIterator;
        const QString&          commandString  = functionData.functionCommand();
        QString                 marshallerName = QString("%1_function_marshaller").arg(commandString);

        FunctionMarshaller* marshaller = window->object<FunctionMarshaller>(marshallerName);
        sessionConnect(
            marshaller,
            &FunctionMarshaller::insertFunction,
            static_cast<void (FunctionViewProxy::*)(const Ld::FunctionData&)>(&FunctionViewProxy::insertFunction)
        );
    }

    EQt::ProgrammaticDockWidget* dockWidget = window->dockWidget(FunctionBuilder::functionDockName);
    FunctionDockWidget*          functionDockWidget = dynamic_cast<FunctionDockWidget*>(dockWidget);

    functionDockWidget->requirementsChanged(viewWidget->requirementsAtCursor());

    sessionConnect(
        functionDockWidget,
        static_cast<void (FunctionDockWidget::*)(const Ld::FunctionData&, const QStringList&)>(
            &FunctionDockWidget::functionSelected
        ),
        static_cast<void (FunctionViewProxy::*)(const Ld::FunctionData&, const QStringList&)>(
            &FunctionViewProxy::insertFunction
        )
    );

    sessionConnect(
        viewWidget,
        &ViewWidget::requirementsChanged,
        functionDockWidget,
        &FunctionDockWidget::requirementsChanged
    );
}


void FunctionViewProxy::unbindDynamic() {
    sessionDisconnectAll();
}


void FunctionViewProxy::functionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::FunctionElement::elementName);
    element->setFormat(functionFormat());

    insertElement(element);
}


void FunctionViewProxy::insertFunction(const Ld::FunctionData& functionData) {
    Ld::FunctionVariant variant               = functionData.variants().first();
    QList<QString>      parameterDescriptions = variant.parameterDescriptions();

    insertFunction(functionData, parameterDescriptions);
}


void FunctionViewProxy::insertFunction(
        const Ld::FunctionData& functionData,
        const QStringList&      parameterDescriptions
    ) {
    const Ld::VariableName& variableName                = functionData.userVisibleName();
    bool                    firstParameterIsSubscripted = functionData.includeSubscriptParameter();

    QSharedPointer<Ld::FunctionElement> element = Ld::Element::create(Ld::FunctionElement::elementName)
                                                  .dynamicCast<Ld::FunctionElement>();

    QSharedPointer<Ld::FunctionFormat> format  = functionFormat().dynamicCast<Ld::FunctionFormat>();
    format->setSubscriptedParameter(firstParameterIsSubscripted);

    element->setFormat(format);
    element->setText(variableName.text1(), 0);
    element->setText(variableName.text2(), 1);

    for (  QStringList::const_iterator it = parameterDescriptions.constBegin(),end = parameterDescriptions.constEnd()
         ; it != end
         ; ++it
        ) {
        const QString& description = *it;

        QSharedPointer<Ld::FunctionPlaceholderElement>
            placeholder = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName)
                          .dynamicCast<Ld::FunctionPlaceholderElement>();

        placeholder->setFieldString(tr("<%1>").arg(description));
        element->append(placeholder, Q_NULLPTR);
    }

    QSharedPointer<Ld::FunctionPlaceholderElement>
        placeholder = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName)
                      .dynamicCast<Ld::FunctionPlaceholderElement>();

    element->append(placeholder, Q_NULLPTR);

    insertElement(element, InsertElementCommand::FinalCursorPosition::FIRST_CHILD_ELEMENT);
}
