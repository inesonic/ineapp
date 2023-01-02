/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BooleanDataTypePresentationGenerator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BOOLEAN_DATA_TYPE_PRESENTATION_GENERATOR_H
#define BOOLEAN_DATA_TYPE_PRESENTATION_GENERATOR_H

#include <QCoreApplication>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>

#include <model_variant.h>

#include <ld_boolean_data_type_decoder.h>

#include "app_common.h"
#include "simple_text_data_type_presentation_generator_base.h"

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Ld {
    class DataTypeFormat;
}

/**
 * Presentation generator used to render boolean values.
 */
class APP_PUBLIC_API BooleanDataTypePresentationGenerator:public virtual Ld::BooleanDataTypeDecoder,
                                                          public virtual SimpleTextDataTypePresentationGeneratorBase {
    public:
        /**
         * Static instance you can use for the application configuration.
         */
        static const BooleanDataTypePresentationGenerator instance;

        BooleanDataTypePresentationGenerator();

        ~BooleanDataTypePresentationGenerator() override;

    protected:
        /**
         * Method that returns a string representation of the provided value.
         *
         * \param[in]  value  A variant type containing the value to be translated.
         *
         * \param[in]  format Format information used to control how the data-type is translated.  Note that the
         *                    translation engine must be able to perform a reasonable translation even if a format has
         *                    not been supplied.
         *
         * \return Returns a string representation of the provided value.
         */
        QString toDisplayedString(const Model::Variant& value, Ld::FormatPointer format) const override;
};


#endif
