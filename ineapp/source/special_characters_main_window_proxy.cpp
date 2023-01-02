/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SpecialCharactersMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QGridLayout>
#include <QGroupBox>

#include <eqt_tool_button.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include <ld_special_characters.h>

#include "main_window.h"
#include "special_characters_builder.h"
#include "special_characters_marshaller.h"
#include "special_characters_main_window_proxy.h"

SpecialCharactersMainWindowProxy::SpecialCharactersMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


SpecialCharactersMainWindowProxy::~SpecialCharactersMainWindowProxy() {}


void SpecialCharactersMainWindowProxy::bind() {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window());
    Q_ASSERT(mainWindow != Q_NULLPTR);

    QMap<UnicodeValue, QString> commandsByUnicode = characterCommandsByUnicodeValue(true, true, true, true);

    for (  QMap<UnicodeValue, QString>::const_iterator pos = commandsByUnicode.constBegin(),
                                                       end = commandsByUnicode.constEnd()
         ; pos != end
         ; ++pos
        ) {
        QString                      command        = pos.value();
        QString                      actionName     = QString("special_character_%1").arg(command);
        QString                      marshallerName = QString("special_character_marshaller_%1").arg(command);
        QAction*                     action         = mainWindow->action(actionName);
        SpecialCharactersMarshaller* marshaller     = mainWindow->object<SpecialCharactersMarshaller>(marshallerName);

        Q_ASSERT(action != Q_NULLPTR);
        Q_ASSERT(marshaller != Q_NULLPTR);

        connect(action, &QAction::triggered, marshaller, &SpecialCharactersMarshaller::triggered);
    }

    QAction*                     backslashAction     = mainWindow->action("special_character_backslash");
    SpecialCharactersMarshaller* backslashMarshaller = mainWindow->object<SpecialCharactersMarshaller>(
        "special_character_marshaller_backslash"
    );

    connect(backslashAction, &QAction::triggered, backslashMarshaller, &SpecialCharactersMarshaller::triggered);
}


void SpecialCharactersMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* specialCharactersDock =
        window()->dockWidget(SpecialCharactersBuilder::specialCharactersDockName);

    EQt::ProgrammaticDockWidget* greekCharactersDock =
        window()->dockWidget(SpecialCharactersBuilder::greekCharactersDockName);

    QGridLayout* specialCharactersHorizontalLayout =
        specialCharactersDock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal");

    QGridLayout* specialCharactersVerticalLayout =
        specialCharactersDock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical");

    EQt::ToolButton* horizontalSpecialCharactersSampleToolButton =
        specialCharactersDock->EQt::ProgrammaticWidget::widget<EQt::ToolButton>("horizontal_tool_button_aleph");

    EQt::ToolButton* verticalSpecialCharactersSampleToolButton =
        specialCharactersDock->EQt::ProgrammaticWidget::widget<EQt::ToolButton>("vertical_tool_button_aleph");

    unsigned specialCharactersMinimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              (specialCharactersHorizontalLayout->rowCount() - 1) * specialCharactersHorizontalLayout->spacing()
            + specialCharactersHorizontalLayout->rowCount() * horizontalSpecialCharactersSampleToolButton->height()
            + specialCharactersHorizontalLayout->contentsMargins().top()
            + specialCharactersHorizontalLayout->contentsMargins().bottom()
        ),
        specialCharactersDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned specialCharactersMinimumWidthVerticalOrientation = (
          (specialCharactersVerticalLayout->columnCount() - 1) * specialCharactersVerticalLayout->spacing()
        + specialCharactersVerticalLayout->columnCount() * verticalSpecialCharactersSampleToolButton->height()
        + specialCharactersVerticalLayout->contentsMargins().top()
        + specialCharactersVerticalLayout->contentsMargins().bottom()
    );

    specialCharactersDock->setMinimumDockHeight(specialCharactersMinimumHeightHorizontalOrientation);
    specialCharactersDock->setMinimumDockWidth(specialCharactersMinimumWidthVerticalOrientation);

    QGridLayout* greekCharactersHorizontalLayout =
        greekCharactersDock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal");

    QGridLayout* greekCharactersVerticalLayout =
        greekCharactersDock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical");

    EQt::ToolButton* horizontalGreekCharactersSampleToolButton =
        greekCharactersDock->EQt::ProgrammaticWidget::widget<EQt::ToolButton>("horizontal_tool_button_alpha");

    EQt::ToolButton* verticalGreekCharactersSampleToolButton =
        greekCharactersDock->EQt::ProgrammaticWidget::widget<EQt::ToolButton>("vertical_tool_button_alpha");

    unsigned greekCharactersMinimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              (greekCharactersHorizontalLayout->rowCount() - 1) * greekCharactersHorizontalLayout->spacing()
            + greekCharactersHorizontalLayout->rowCount() * horizontalGreekCharactersSampleToolButton->height()
            + greekCharactersHorizontalLayout->contentsMargins().top()
            + greekCharactersHorizontalLayout->contentsMargins().bottom()
        ),
        greekCharactersDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned greekCharactersMinimumWidthVerticalOrientation = (
          (greekCharactersVerticalLayout->columnCount() - 1) * greekCharactersVerticalLayout->spacing()
        + greekCharactersVerticalLayout->columnCount() * verticalGreekCharactersSampleToolButton->height()
        + greekCharactersVerticalLayout->contentsMargins().top()
        + greekCharactersVerticalLayout->contentsMargins().bottom()
    );

    greekCharactersDock->setMinimumDockHeight(greekCharactersMinimumHeightHorizontalOrientation);
    greekCharactersDock->setMinimumDockWidth(greekCharactersMinimumWidthVerticalOrientation);
}


void SpecialCharactersMainWindowProxy::screenResized() {
    windowConfigured();
}
