/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlugInManager class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLUG_IN_MANAGER_H
#define PLUG_IN_MANAGER_H

#include <QObject>
#include <QString>

#include <ld_plug_in_information.h>
#include <ld_plug_in_manager.h>

#include "app_common.h"

struct PlugInData;
class PlugInRegistrar;

/**
 * Class that loads and manages language plug-in.
 */
class APP_PUBLIC_API PlugInManager:public QObject, public Ld::PlugInManager {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlugInManager(QObject* parent = Q_NULLPTR);

        ~PlugInManager() override;

    signals:
        /**
         * Signal that is emitted when a plug-in is about to be loaded.
         *
         * \param[out] loadPath The path used to load the library.
         */
        void aboutToLoadPlugIn(const QString& loadPath);

        /**
         * Signal that is emitted after a plug-in is successfully loaded.
         *
         * \param[out] loadPath          File path to the loaded plug-in.
         *
         * \param[out] plugInInformation Information used to describe the plug-in.
         */
        void loadedPlugIn(const QString& loadPath, Ld::PlugInInformation plugInInformation);

        /**
         * Signal that is emitted when the plug-in manager could not successfully load a plug-in.
         *
         * \param[in] loadPath The path that was used to load the library.
         */
        void failedToLoadPlugIn(const QString& loadPath);

    protected:
        /**
         * Method you can overload to receiving notification that a plug-in is about to be loaded.  The default
         * implementation simply returns.
         *
         * \param[in] loadPath The path used to load the library.
         */
        void aboutToLoad(const QString& loadPath) final;

        /**
         * Method you can overload to receiving notification that a plug-in was successfully loaded.  The default
         * implementation simply returns.
         *
         * \param[in] loadPath          The path used to load the library.
         *
         * \param[in] plugInInformation The information associated with the plug-in.
         */
        void plugInLoaded(const QString& loadPath, Ld::PlugInInformation plugInInformation) final;

        /**
         * Method you can overload to receive notification that a plug-in was not successfully loaded.  The default
         * implementation dumps an error message to stderr.
         *
         * \param[in] loadPath The path that was used to load the library.
         */
        void plugInFailedToLoad(const QString& loadPath) final;
};

#endif
