/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ViewProxy class.
***********************************************************************************************************************/

#include <QObject>

#include "eqt_programmatic_view.h"
#include "eqt_programmatic_main_window.h"
#include "eqt_programmatic_view_proxy.h"

#include "document.h"
#include "view_widget.h"
#include "main_window.h"
#include "view_proxy.h"

ViewProxy::ViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ProgrammaticViewProxy(
        view,
        window
    ) {}


ViewProxy::~ViewProxy() {}


ViewWidget* ViewProxy::view() const {
    return dynamic_cast<ViewWidget*>(EQt::ProgrammaticViewProxy::view());
}


MainWindow* ViewProxy::window() const {
    return dynamic_cast<MainWindow*>(EQt::ProgrammaticViewProxy::window());
}


void ViewProxy::aboutToSave(Document*) {}


void ViewProxy::loaded(Document*) {}
