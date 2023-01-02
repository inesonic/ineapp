/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CmykImagePresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CMYK_IMAGE_PRESENTATION_DATA_H
#define CMYK_IMAGE_PRESENTATION_DATA_H

#include <QCoreApplication>
#include <QImage>

#include <ld_plot_series.h>

#include "app_common.h"
#include "plot_presentation_data.h"
#include "image_render_presentation_data.h"

namespace Model {
    class MatrixBoolean;
    class MatrixInteger;
    class MatrixReal;
    class MatrixComplex;
}

namespace QtCharts {
    class QScatterSeries;
    class QValueAxis;
}

namespace EQt {
    class GraphicsPixmapItem;
}

namespace Ld {
    class ChartAxisFormat;
}

/**
 * The \ref PlotPresentationData instance to use for CMYK images rendered as plots.
 */
class APP_PUBLIC_API CmykImagePresentationData:public ImageRenderPresentationData {
    Q_DECLARE_TR_FUNCTIONS(CmykImagePresentationData)

    public:
        CmykImagePresentationData();

        ~CmykImagePresentationData() override;

        /**
         * Method that indicates the number of series for this type of image.
         *
         * \return Returns the number of data series for this type of image.
         */
        unsigned numberSeries() const override;

        /**
         * Method that renders an image using a collection of real matrices.
         *
         * \param[in]  matrixByAxisLocation A map of matrices based on axis location.
         *
         * \param[out] errorString          An error string that is populated if an error is found.
         *
         * \return Returns the rendered image.
         */
        QImage render(
            const QMap<AxisLocation, Model::MatrixInteger>& matrixByAxisLocation,
            QString&                                        errorString
        ) const override;

        /**
         * Method that renders an image using a collection of real matrices.
         *
         * \param[in] matrixByAxisLocation A map of matrices based on axis location.
         *
         * \param[out] errorString          An error string that is populated if an error is found.
         *
         * \return Returns the rendered image.
         */
        QImage render(
            const QMap<AxisLocation, Model::MatrixReal>& matrixByAxisLocation,
            QString&                                     errorString
        ) const override;
};

#endif
