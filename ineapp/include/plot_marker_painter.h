/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotMarkerPainter class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_MARKER_PAINTER_H
#define PLOT_MARKER_PAINTER_H

#include <ld_plot_series.h>

#include "plot_engine.h"
#include "app_common.h"

class QPaintEvent;
class QPainter;
class QRect;
class QColor;

/**
 * Helper class you can use to paint plot markers.
 */
class APP_PUBLIC_API PlotMarkerPainter {
    public:
        /**
         * The marker style.
         */
        typedef Ld::PlotSeries::MarkerStyle MarkerStyle;

        PlotMarkerPainter();

        ~PlotMarkerPainter();

    protected:
        /**
         * Static method you can use to paint a marker onto an arbitrary surface.
         *
         * \param[in] painter           Painter used to draw the line style image.
         *
         * \param[in] boundingRectangle The bounding rectangle to contain the marker.
         *
         * \param[in] lineWidth         The line widht.
         *
         * \param[in] lineColor         The line color.
         *
         * \param[in] markerStyle       The marker style.
         */
        static void paintMarker(
            QPainter*      painter,
            const QRect&   boundingRectangle,
            float          lineWidth,
            const QColor&  lineColor,
            MarkerStyle    markerStyle
        );

    private:
        /**
         * Magnitude of cosine of 18 degrees.
         */
        static const float starA;

        /**
         * Magnitude of sine of 18 degrees.
         */
        static const float starB;

        /**
         * Magnitude of cosine of 234 degrees.
         */
        static const float starC;

        /**
         * Magnitide of sine of 234 degrees.
         */
        static const float starD;

        /**
         * Scale factor for 45 degree values.
         */
        static const float oneOverSqrt2;
};

#endif
