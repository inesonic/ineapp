/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageBreakViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QAction>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>

#include <ld_page_break_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "view_widget.h"
#include "page_break_view_proxy.h"

PageBreakViewProxy::PageBreakViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticViewProxy(
        view,
        window
    ) {}


PageBreakViewProxy::~PageBreakViewProxy() {}


void PageBreakViewProxy::bindDynamic() {
    QAction* insertPageBreakAction = window()->action("insert_page_break_action");
    connect(insertPageBreakAction, &QAction::triggered, this, &PageBreakViewProxy::insertPageBreakRequested);
}


void PageBreakViewProxy::unbindDynamic() {
    QAction* insertPageBreakAction = window()->action("insert_page_break_action");
    disconnect(insertPageBreakAction, &QAction::triggered, this, &PageBreakViewProxy::insertPageBreakRequested);
}


void PageBreakViewProxy::insertPageBreakRequested() {
    QSharedPointer<Ld::PageBreakElement> newElement = Ld::Element::createElement<Ld::PageBreakElement>();

    ViewWidget* view = dynamic_cast<ViewWidget*>(PageBreakViewProxy::view());
    view->insertCommand(
        new InsertElementCommand(newElement, InsertElementCommand::InsertionLocation::UNDER_ROOT)
    );
}
