/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SpecialCharactersViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SPECIAL_CHARACTERS_VIEW_PROXY_H
#define SPECIAL_CHARACTERS_VIEW_PROXY_H

#include <QObject>

#include <ld_special_characters.h>

#include "view_proxy.h"

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

/**
 * Main window proxy used to insert special characters.
 */
class SpecialCharactersViewProxy:public ViewProxy, private Ld::SpecialCharacters {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        SpecialCharactersViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~SpecialCharactersViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         *
         * The default implementation simply returns.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         *
         * The default implementation simply returns.
         */
        void unbindDynamic() final;
};

#endif
