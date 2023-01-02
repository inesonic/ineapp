/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlugInRegistrar class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLUG_IN_REGISTRAR_H
#define PLUG_IN_REGISTRAR_H

#include <QString>

#include "ld_data_type.h"
#include "ld_plug_in_registrar.h"

#include "app_common.h"
#include "inspector_widget.h"

namespace EQt {
    class Builder;
}

/**
 * Pure virtual class used to register application plug-ins.
 */
class APP_PUBLIC_API PlugInRegistrar:public Ld::PlugInRegistrar {
    public:
        virtual ~PlugInRegistrar() = default;

        /**
         * Pure virtual method you can call to register a builder with the application.  The builder will be used by
         * the application customize the user interface for this plug-in.
         *
         * Note that the application will take ownership of the builder and will destroy it automatically when the
         * application terminates.
         *
         * \param[in] newBuilder The builder instance to be used.
         */
        virtual void registerBuilder(EQt::Builder* newBuilder) = 0;

        /**
         * Method you can use to register new inspector widget factory function.
         *
         * \param[in] valueType       The value type to associate with this factory function.
         *
         * \param[in] creatorFunction The factory function.
         */
        virtual void registerInspectorWidget(
            Ld::DataType::ValueType          valueType,
            InspectorWidget::CreatorFunction creatorFunction
        ) = 0;
};

#endif
