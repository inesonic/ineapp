/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QList>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QMessageBox>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>
#include <eqt_tool_button_array.h>

#include <ld_capabilities.h>
#include <ld_operator_format.h>
#include <ld_function_variant.h>
#include <ld_function_data.h>
#include <ld_function_database.h>
#include <ld_function_element.h>

#include "application.h"
#include "main_window.h"
#include "function_dock_widget.h"
#include "function_marshaller.h"
#include "function_main_window_proxy.h"
#include "function_view_proxy.h"
#include "function_builder.h"

const char FunctionBuilder::builderName[] = "FunctionBuilder";
const char FunctionBuilder::functionDockName[] = "function";

FunctionBuilder::FunctionBuilder(QObject* parent):EQt::Builder(parent) {}


FunctionBuilder::~FunctionBuilder() {}


unsigned FunctionBuilder::priority() const {
    return builderPriority;
}


const char* FunctionBuilder::builderIdentifier() const {
    return builderName;
}


const char* FunctionBuilder::plugInName() const {
    return "";
}

#if (defined(Q_OS_DARWIN))

    #define MENU_TEXT(_text,_suffix) ((_text) + (_suffix))

#else

    #define MENU_TEXT(_text,_suffix) (_text)

#endif

void FunctionBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* functionOperatorAction = window->addMenuAction(
        EQt::Application::icon("function_operator"),
        MENU_TEXT(tr("&Insert | &Function"), tr("  f(?)")),
        Ld::FunctionElement::childProvides,
        "function_operator_action"
    );
    functionOperatorAction->setToolTip(tr("Define or use a function"));
    functionOperatorAction->setWhatsThis(
        tr(
           "You can use the function operator to define or call a function.  You can define a function by placing "
           "the function operator on the left side of an assignment.  You can call a function by placing the function "
           "operator on the right side of an assignment."
          )
    );
}


void FunctionBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QList<Ld::FunctionData> functionDataList = Ld::FunctionDatabase::functions();
    for (  QList<Ld::FunctionData>::const_iterator functionDataIterator    = functionDataList.constBegin(),
                                                   functionDataEndIterator = functionDataList.constEnd()
         ; functionDataIterator != functionDataEndIterator
         ; ++functionDataIterator
        ) {
        const Ld::FunctionData& functionData   = *functionDataIterator;
        const Ld::Capabilities& capabilities   = functionData.childCapabilities();
        const QString&          commandString  = functionData.functionCommand();
        const QString&          description    = functionData.description();
        QString                 actionName     = QString("%1_function_action").arg(commandString);
        QString                 marshallerName = QString("%1_function_marshaller").arg(commandString);

        QAction* action = new QAction(window);
        action->setToolTip(description);
        window->addAction(action, capabilities, actionName);

        FunctionMarshaller* marshaller = new FunctionMarshaller(functionData, window);
        window->registerObject(marshaller, marshallerName);

        connect(action, &QAction::triggered, marshaller, &FunctionMarshaller::triggered);
    }
}


void FunctionBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    FunctionDockWidget* dock = new FunctionDockWidget(window);
    window->addDockWidget(
        functionDockName,
        dock,
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER, "run_export", true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        functionDockName,
        tr("&View | &Docks"),
        "function_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Function dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Function dock.  You can use the Function dock to insert "
            "and use functions."
        )
    );
}


void FunctionBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* functionOperatorAction = mainWindow->action("function_operator_action");
    bool success = mainWindow->setCommand(tr("fn"), functionOperatorAction);
    Q_ASSERT(success);

    QList<Ld::FunctionData> functionDataList = Ld::FunctionDatabase::functions();
    for (  QList<Ld::FunctionData>::const_iterator functionDataIterator    = functionDataList.constBegin(),
                                                   functionDataEndIterator = functionDataList.constEnd()
         ; functionDataIterator != functionDataEndIterator
         ; ++functionDataIterator
        ) {
        const Ld::FunctionData& functionData   = *functionDataIterator;
        const QString&          commandString  = functionData.functionCommand();
        QString                 actionName     = QString("%1_function_action").arg(commandString);
        QAction*                action         = mainWindow->action(actionName);

        success = mainWindow->setCommand(commandString, action);
        if (!success) {
            Q_ASSERT(functionData.functionType() != Ld::FunctionData::Type::BUILT_IN);
            QMessageBox::warning(
                mainWindow,
                tr("Duplicate Function Command"),
                tr("Duplicate command \"%1\", function \"%2<sub>%3</sub>\".")
                .arg(commandString)
                .arg(functionData.userVisibleName().text1())
                .arg(functionData.userVisibleName().text2())
            );
        }
    }
}


EQt::ProgrammaticViewProxy* FunctionBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new FunctionViewProxy(view, window);
}


EQt::ProgrammaticMainWindowProxy* FunctionBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new FunctionMainWindowProxy(window);
}
