/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SpecialCharactersBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QStringList>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>
#include <QMap>
#include <QColor>
#include <QPalette>
#include <QFont>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>

#include <ld_capabilities.h>
#include <ld_special_characters.h>

#include "application.h"
#include "main_window.h"
#include "special_characters_marshaller.h"
#include "special_characters_main_window_proxy.h"
#include "special_characters_view_proxy.h"
#include "special_characters_builder.h"

const char SpecialCharactersBuilder::builderName[] = "SpecialCharacterBuilder";
const char SpecialCharactersBuilder::greekCharactersDockName[] = "greek_characters";
const char SpecialCharactersBuilder::specialCharactersDockName[] = "special_characters";

#if (defined(Q_OS_DARWIN))

    const unsigned SpecialCharactersBuilder::numberHorizontalRows  = 4;
    const unsigned SpecialCharactersBuilder::numberVerticalColumns = 9;
    const unsigned SpecialCharactersBuilder::buttonPadding         = 6;
    const char     SpecialCharactersBuilder::buttonFontFamily[]    = "Lucida Grande";
    const unsigned SpecialCharactersBuilder::buttonFontSize        = 12;

#elif (defined(Q_OS_WIN))

    const unsigned SpecialCharactersBuilder::numberHorizontalRows  = 4;
    const unsigned SpecialCharactersBuilder::numberVerticalColumns = 8;
    const unsigned SpecialCharactersBuilder::buttonPadding         = 4;
    const char     SpecialCharactersBuilder::buttonFontFamily[]    = "Segoe UI";
    const unsigned SpecialCharactersBuilder::buttonFontSize        = 8;

#elif (defined(Q_OS_LINUX))

    const unsigned SpecialCharactersBuilder::numberHorizontalRows  = 4;
    const unsigned SpecialCharactersBuilder::numberVerticalColumns = 9;
    const unsigned SpecialCharactersBuilder::buttonPadding         = 5;
    const char     SpecialCharactersBuilder::buttonFontFamily[]    = "Liberation Serif";
    const unsigned SpecialCharactersBuilder::buttonFontSize        = 8;

#else

    #error Unknown platform

#endif

SpecialCharactersBuilder::SpecialCharactersBuilder(QObject* parent):EQt::Builder(parent) {
    specialButtonFont = QFont(buttonFontFamily, buttonFontSize);
    buttonEdgeWidth   = buttonEdgeSize();
}


SpecialCharactersBuilder::~SpecialCharactersBuilder() {}


unsigned SpecialCharactersBuilder::priority() const {
    return builderPriority;
}


const char* SpecialCharactersBuilder::builderIdentifier() const {
    return builderName;
}


const char* SpecialCharactersBuilder::plugInName() const {
    return "";
}


void SpecialCharactersBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QString menuPath = tr("&Edit | Special Character...");

        window->addMenuAction(
            menuPath,
            Ld::Capabilities::allAlphanumericSymbols,
            "special_characters_dialog"
        );

    #elif (defined(Q_OS_DARWIN))

        (void) window;

    #else

        #error Unknown platform.

    #endif
}


void SpecialCharactersBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QMap<UnicodeValue, QString> diacriticalDescriptionsByUnicode = characterDescriptionsByUnicodeValue(
        false,
        true,
        false,
        false
    );

    QMap<UnicodeValue, QString> diacriticalCommandsByUnicode = characterCommandsByUnicodeValue(
        false,
        true,
        false,
        false
    );

    for (  QMap<UnicodeValue, QString>::const_iterator pos = diacriticalCommandsByUnicode.constBegin(),
                                                       end = diacriticalCommandsByUnicode.constEnd()
         ; pos != end
         ; ++pos
        ) {
        UnicodeValue unicodeValue = pos.key();
        QString      command      = pos.value();
        QString      description  = diacriticalDescriptionsByUnicode.value(unicodeValue);
        QString      actionName   = QString("special_character_%1").arg(command);
        QAction*     action       = window->addAction(
            new QAction(this),
            Ld::Capabilities::nonAsciiAlphabeticSymbols,
            actionName
        );

        action->setText(QString(QChar(unicodeValue)));
        action->setToolTip(description);
        action->setWhatsThis(description);
    }

    QAction* backslashAction = window->addAction(
        new QAction(this),
        Ld::Capabilities::specialSymbols,
        "special_character_backslash"
    );

    backslashAction->setText(QString("\\"));
    backslashAction->setToolTip(tr("Backslash (\\)"));
    backslashAction->setWhatsThis(tr("Backslash (\\)"));
}


void SpecialCharactersBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    buildGreekCharactersDock(window);
    buildSpecialCharactersDock(window);

    buildMarshallers(window);
}


EQt::ProgrammaticMainWindowProxy* SpecialCharactersBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new SpecialCharactersMainWindowProxy(window);
}


void SpecialCharactersBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);
    Q_ASSERT(mainWindow != Q_NULLPTR);

    QMap<UnicodeValue, QString> commandsByUnicode = characterCommandsByUnicodeValue(true, true, true, true);
    for (  QMap<UnicodeValue, QString>::const_iterator pos = commandsByUnicode.constBegin(),
                                                       end = commandsByUnicode.constEnd()
         ; pos != end
         ; ++pos
        ) {
        QString  command    = pos.value();
        QString  actionName = QString("special_character_%1").arg(command);
        QAction* action     = mainWindow->action(actionName);

        Q_ASSERT(action != Q_NULLPTR);

        bool success = mainWindow->setCommand(command, action);
        Q_ASSERT(success);
    }

    QAction* backslashAction = window->action("special_character_backslash");
    bool success = mainWindow->setCommand(tr("backslash"), backslashAction);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("\\"), backslashAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* SpecialCharactersBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new SpecialCharactersViewProxy(view, window);
}


unsigned SpecialCharactersBuilder::buttonEdgeSize() const {
    QList<UnicodeValue> allSymbols = recommendedCharacterOrder(true, false, true, true);
    QFontMetrics        fontMetrics(specialButtonFont);

    unsigned maximumSide = fontMetrics.height();
    for (QList<UnicodeValue>::const_iterator it=allSymbols.constBegin(),end=allSymbols.constEnd() ; it!=end ; ++it) {
        unsigned width = static_cast<unsigned>(fontMetrics.horizontalAdvance(QChar(*it)));
        if (width > maximumSide) {
            maximumSide = width;
        }
    }

    return static_cast<unsigned>(maximumSide * buttonScaleFactor + 0.5F + buttonPadding);
}


void SpecialCharactersBuilder::buildGreekCharactersDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        greekCharactersDockName,
        tr("Greek"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION, "control_flow", true)
    );
    dock->setCloseButtonEnabled();

    QGridLayout* horizontalLayout = dock->newGridLayout("horizontal");
    dock->setHorizontalLayout(horizontalLayout);

    QGridLayout* verticalLayout = dock->newGridLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    QList<UnicodeValue> greekSymbols = recommendedCharacterOrder(true, false, false, false);
    addButtons(
        window,
        dock,
        greekSymbols,
        horizontalLayout,
        verticalLayout
    );

    QAction* greekCharactersDockToggleAction = window->addDockToggleMenuItem(
        greekCharactersDockName,
        tr("&View | &Docks"),
        "greek_characters_dock_toggle_action"
    );
    greekCharactersDockToggleAction->setToolTip("Show/hide the Greek Characters dock");
    greekCharactersDockToggleAction->setWhatsThis(
        tr(
           "You can use this menu item to show or hide the Greek Characters dock.  The Greek Characters dock includes "
           "functions used to add Greek symbols to your program."
          )
    );

    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);

//    window->tabifyDockWidget(window->dockWidget("Insert"), dock);
}


void SpecialCharactersBuilder::buildSpecialCharactersDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        specialCharactersDockName,
        tr("Symbols"),
        EQt::DockWidgetDefaults(
            EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
            greekCharactersDockName,
            true
        )
    );
    dock->setCloseButtonEnabled();

    QGridLayout* horizontalLayout = dock->newGridLayout("horizontal");
    dock->setHorizontalLayout(horizontalLayout);

    QGridLayout* verticalLayout = dock->newGridLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    QList<UnicodeValue> specialSymbolsAndCharacters = recommendedCharacterOrder(false, false, true, true);
    addButtons(
        window,
        dock,
        specialSymbolsAndCharacters,
        horizontalLayout,
        verticalLayout
    );

    QAction* specialCharactersDockToggleAction = window->addDockToggleMenuItem(
        specialCharactersDockName,
        tr("&View | &Docks"),
        "special_characters_dock_toggle_action"
    );
    specialCharactersDockToggleAction->setToolTip("Show/hide the Special Characters dock");
    specialCharactersDockToggleAction->setWhatsThis(
        tr(
           "You can use this menu item to show or hide the Special Characters dock.  The Special Characters dock "
           "includes functions used to add special symbols such as \"%1\" and \"%2\"."
          ).arg(QChar(0x2127)).arg(QChar(0x210F))
    );

    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);

//    window->tabifyDockWidget(window->dockWidget("Insert"), dock);
}


void SpecialCharactersBuilder::buildMarshallers(EQt::ProgrammaticMainWindow* window) {
    QMap<UnicodeValue, QString> commandsByUnicode = characterCommandsByUnicodeValue(true, true, true, true);
    for (  QMap<UnicodeValue, QString>::const_iterator pos = commandsByUnicode.constBegin(),
                                                       end = commandsByUnicode.constEnd()
         ; pos != end
         ; ++pos
        ) {
        UnicodeValue unicodeValue = pos.key();
        QString      command      = pos.value();
        QString      actionName   = QString("special_character_%1").arg(command);
        QAction*     action       = window->action(actionName);

        Q_ASSERT(action != Q_NULLPTR);

        SpecialCharactersMarshaller* marshaller = new SpecialCharactersMarshaller(unicodeValue, window);
        window->registerObject(marshaller, QString("special_character_marshaller_%1").arg(command));
    }

    SpecialCharactersMarshaller* backslashMarshaller = new SpecialCharactersMarshaller(QChar('\\').unicode(), window);
    window->registerObject(backslashMarshaller, "special_character_marshaller_backslash");
}


void SpecialCharactersBuilder::addButtons(
        EQt::ProgrammaticMainWindow* window,
        EQt::ProgrammaticDockWidget* dock,
        const QList<UnicodeValue>&   characterList,
        QGridLayout*                 horizontalLayout,
        QGridLayout*                 verticalLayout
    ) {
    QMap<UnicodeValue, QString> descriptionsByUnicode = characterDescriptionsByUnicodeValue(true, false, true, true);
    QMap<UnicodeValue, QString> commandsByUnicode     = characterCommandsByUnicodeValue(true, false, true, true);

    unsigned numberItems             = static_cast<unsigned>(characterList.size());
    unsigned numberHorizontalColumns = (numberItems + numberHorizontalRows - 1) / numberHorizontalRows;

    QList<UnicodeValue>::const_iterator it  = characterList.constBegin();
    QList<UnicodeValue>::const_iterator end = characterList.constEnd();

    unsigned buttonIndex = 0;

    while (it != end) {
        UnicodeValue unicode      = *it;
        QString      description  = descriptionsByUnicode.value(unicode);
        QString      command      = commandsByUnicode.value(unicode);
        Util::BitSet capabilities = Ld::SpecialCharacters::requiredCapabilities(QString(QChar(unicode)));

        QString  actionName = QString("special_character_%1").arg(command);
        QAction* action     = window->addAction(new QAction(this), capabilities, actionName);

        action->setText(QString(QChar(unicode)));
        action->setToolTip(description);
        action->setWhatsThis(description);

        EQt::ToolButton* toolButton = new EQt::ToolButton(action);
        toolButton->setGlobalScaleFactorSettingsDisabled();
        toolButton->setFont(specialButtonFont);
        toolButton->setFixedSize(QSize(buttonEdgeWidth, buttonEdgeWidth));

        dock->registerWidget(toolButton, QString("horizontal_tool_button_%1").arg(command));

        horizontalLayout->addWidget(
            toolButton,
            buttonIndex / numberHorizontalColumns,
            buttonIndex % numberHorizontalColumns
        );

        toolButton = new EQt::ToolButton(action);
        toolButton->setGlobalScaleFactorSettingsEnabled();
        toolButton->setFont(specialButtonFont);
        toolButton->setFixedSize(QSize(buttonEdgeWidth, buttonEdgeWidth));

        dock->registerWidget(toolButton, QString("vertical_tool_button_%1").arg(command));

        verticalLayout->addWidget(
            toolButton,
            buttonIndex / numberVerticalColumns,
            buttonIndex % numberVerticalColumns
        );

        ++buttonIndex;
        ++it;
    }
}
