/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RealDataTypePresentationGeneratorBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef REAL_DATA_TYPE_PRESENTATION_GENERATOR_BASE_H
#define REAL_DATA_TYPE_PRESENTATION_GENERATOR_BASE_H

#include <QCoreApplication>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QFont>

#include <model_intrinsic_types.h>

#include <ld_real_data_type_format.h>
#include <ld_real_data_type_decoder.h>

#include "app_common.h"
#include "data_type_presentation_generator.h"

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Ld {
    class DataTypeFormat;
}

/**
 * Base class for presentation generators used to render real and complex values.  The class provides several
 * protected methods to facilitate those derived classes.
 */
class APP_PUBLIC_API RealDataTypePresentationGeneratorBase:public virtual DataTypePresentationGenerator {
    Q_DECLARE_TR_FUNCTIONS(RealDataTypePresentationGeneratorBase)

    public:
        RealDataTypePresentationGeneratorBase();

        ~RealDataTypePresentationGeneratorBase() override;

    protected:
        /**
         * Method that inserts a real number with a specified format and related settings.
         *
         * \param[in]     graphicsItem The graphics item to populate.
         *
         * \param[in]     value        The value to insert into the graphics item.
         *
         * \param[in]     upperCase    If true, the computer exponent "E" will be presented in upper case.  If false,
         *                             the computer exponent "e" will be presented in lower case.
         *
         * \param[in]     includeSign  If true, a sign will always be included on the mantissa, even if it's not
         *                             needed.
         *
         * \param[in]     font         The font used to render the text.
         *
         * \param[in]     precision    The precision to apply to the rendered value.
         *
         * \param[in]     style        The style used to display the value.
         *
         * \param[in,out] xOffset      The horizontal offset where this entry should start.  The value will be upated
         *                             on exit.
         *
         * \param[out]    size         The size for the provided real value, in scene units.
         *
         * \param[out]    ascent       The ascent for the provided real value, in scene units.
         *
         * \param[in,out] index        A running index into the graphics item used to handle inserting text entries.
         */
        static void insertRealValue(
            EQt::GraphicsMultiTextGroup*            graphicsItem,
            Model::Real                             value,
            bool                                    upperCase,
            bool                                    includeSign,
            const QFont&                            font,
            unsigned                                precision,
            Ld::RealDataTypeFormat::RealNumberStyle style,
            float&                                  xOffset,
            QSizeF&                                 size,
            float&                                  ascent,
            unsigned&                               index
        );
};

#endif
