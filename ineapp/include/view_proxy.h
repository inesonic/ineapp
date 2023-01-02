/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VIEW_PROXY_H
#define VIEW_PROXY_H

#include "eqt_programmatic_view_proxy.h"

#include "app_common.h"

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainWindow;
}

class MainWindow;
class ViewWidget;
class Document;

/**
 * Trivial base class for application view proxies.  You can use this class to extend the functionality of the
 * \ref ViewWidget class.
 */
class APP_PUBLIC_API ViewProxy:public EQt::ProgrammaticViewProxy {
    Q_OBJECT

    friend class ViewWidget;

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        ViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~ViewProxy() override;

        /**
         * Determines the associated view class instance.
         *
         * \returns Returns the view instance associated with this proxy.
         */
        ViewWidget* view() const;

        /**
         * Determines the window class instance that owns this view.
         *
         * \return Returns the window instance that manages the view this proxy is supporting.
         */
        MainWindow* window() const;

    protected:
        /**
         * Method you can overload to receive notification that the document under this view is about to be saved.
         *
         * The default implementation performs no action.
         *
         * \param[in] document Pointer to the document instance that is about to be saved.
         */
        virtual void aboutToSave(Document* document);

        /**
         * Method you can overload to receive notification that the document under this view was just loaded from a
         * file.
         *
         * The default implementation performs no action.
         *
         * \param[in] document Pointer to the newly loaded document instance.
         */
        virtual void loaded(Document* document);
};

#endif
