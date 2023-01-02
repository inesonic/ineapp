/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SimpleTextDataTypePresentationGeneratorBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SIMPLE_STRING_DATA_TYPE_PRESENTATION_GENERATOR_BASE_H
#define SIMPLE_STRING_DATA_TYPE_PRESENTATION_GENERATOR_BASE_H

#include <QCoreApplication>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>

#include <model_variant.h>

#include "app_common.h"
#include "data_type_presentation_generator.h"

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Ld {
    class DataTypeFormat;
}

class PlacementTracker;

/**
 * Base class for presentation generators that can render types with a simple string.
 */
class APP_PUBLIC_API SimpleTextDataTypePresentationGeneratorBase:public virtual DataTypePresentationGenerator {
    public:
        SimpleTextDataTypePresentationGeneratorBase();

        ~SimpleTextDataTypePresentationGeneratorBase() override;

        /**
         * Method that calculates the height and ascent of the expected representation of an instance of this data
         * type.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update status
         *                              during a time-consuming placement operation.
         *
         * \param[in]  variant          The value to be rendered.
         *
         * \param[in]  format           The format used to render the data type.
         *
         * \param[in]  relativeScale    An optional relative scale factor used to adjust the size.
         *
         * \param[out] ascent           An optional location that will be populated with the ascent.
         *
         * \return Returns the expected height, in scene units.
         */
        float calculateHeight(
            PlacementTracker*     placementTracker,
            const Model::Variant& variant,
            Ld::FormatPointer     format,
            float                 relativeScale,
            float*                ascent = Q_NULLPTR
        ) const override;

        /**
         * Method you can overload to create a graphic representation of a value of this data-type.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update
         *                              status during a time-consuming placement operation.
         *
         * \param[in]  value            A variant type containing the value to be translated.
         *
         * \param[in]  format           Format information used to control how the data-type is translated.  Note that
         *                              the translation engine must be able to perform a reasonable translation even if
         *                              a format has not been supplied.
         *
         * \param[in]  presenter        Pointer to the presenter requesting the rendering operation.
         *
         * \param[out] size             A reference to a location to hold the size of the graphic representation in
         *                              scene units.
         *
         * \param[out] ascent           The ascent to apply to the graphics representation in scene units.
         *
         * \return Returns a graphics item containing the graphic representation of the data-type.
         */
        EQt::GraphicsMultiTextGroup* toGraphicsItem(
            PlacementTracker*     placementTracker,
            const Model::Variant& value,
            Ld::FormatPointer     format,
            DataTypePresenter*    presenter,
            QSizeF&               size,
            float&                ascent
        ) const override;

        /**
         * Method you can optionally overload to update an existing graphic representation of a value of this
         * data-type.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update
         *                              status during a time-consuming placement operation.
         *
         * \param[in]  graphicsItem     The graphics item to be updated (or replaced).  Note that you do not need to
         *                              delete this graphics item if you intend to replace it.  The calling methods
         *                              will automatically detect that the graphics item is different and delete this
         *                              instance.
         *
         * \param[in]  value            A variant type containing the value to be translated.
         *
         * \param[in]  format           Format information used to control how the data-type is translated.  Note that
         *                              the translation engine must be able to perform a reasonable translation even
         *                              if a format has not been supplied.
         *
         * \param[in]  presenter        Pointer to the presenter requesting the rendering operation.
         *
         * \param[out] size             A reference to a location to hold the size of the graphic representation in
         *                              scene units.
         *
         * \param[out] ascent           The ascent to apply to the graphics representation in scene units.
         *
         * \return Returns a graphics item containing the graphic representation of the data-type.  Note that this may
         *         be a different graphics item than the one supplied.
         */
        EQt::GraphicsMultiTextGroup* updateGraphicsItem(
            PlacementTracker*            placementTracker,
            EQt::GraphicsMultiTextGroup* graphicsItem,
            const Model::Variant&        value,
            Ld::FormatPointer            format,
            DataTypePresenter*           presenter,
            QSizeF&                      size,
            float&                       ascent
        ) const override;

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
        virtual QString toDisplayedString(const Model::Variant& value, Ld::FormatPointer format) const = 0;
};


#endif
