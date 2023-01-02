/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PageBreakViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PAGE_BREAK_VIEW_PROXY_H
#define PAGE_BREAK_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>

#include <eqt_programmatic_view_proxy.h>

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

/**
 * View proxy used to manage the insert page break button.
 */
class PageBreakViewProxy:public EQt::ProgrammaticViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        PageBreakViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~PageBreakViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

    private slots:
        /**
         * Slot that is triggered when the insert page break button or menu item is clicked.
         */
        void insertPageBreakRequested();
};

#endif
