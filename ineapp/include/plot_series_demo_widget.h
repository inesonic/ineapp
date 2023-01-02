/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotSeriesDemoWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_SERIES_DEMO_WIDGET_H
#define PLOT_SERIES_DEMO_WIDGET_H

#include <QWidget>
#include <QFont>
#include <QLineEdit>

#include <ld_plot_series.h>
#include <ld_special_characters.h>

#include "plot_engine.h"
#include "plot_marker_painter.h"
#include "app_common.h"

class QPaintEvent;
class QPainter;

namespace EQt {
    class LineEdit;
    class ToolButton;
    class ToolButtonArray;
}

/**
 * Widget you can use to display a plot series as a line with marker.  The class also contains a static method you can
 * use to draw the demo widget onto other surfaces.
 */
class APP_PUBLIC_API PlotSeriesDemoWidget:public QWidget, public PlotMarkerPainter {
    Q_OBJECT

    public:
        /**
         * The plot line style.
         */
        typedef Ld::PlotSeries::LineStyle LineStyle;

        /**
         * The line spline type.
         */
        typedef Ld::PlotSeries::SplineType SplineType;

        /**
         * The gradient type.
         */
        typedef Ld::PlotSeries::GradientType GradientType;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlotSeriesDemoWidget(QWidget* parent = Q_NULLPTR);

        ~PlotSeriesDemoWidget() override;

        /**
         * Method you can use to obtain the plot series line style.
         *
         * \return Returns the current plot series line style.
         */
        LineStyle lineStyle() const;

        /**
         * Method you can use to obtain the current line width.
         *
         * \return Returns the current line width setting.
         */
        float lineWidth() const;

        /**
         * Method you can use to obtain the current line color.
         *
         * \return Returns the currently selected line color.
         */
        QColor lineColor() const;

        /**
         * Method you can use to obtain the marker style.
         *
         * \return Returns the currently selected marker style.
         */
        MarkerStyle markerStyle() const;

        /**
         * Method you can use to obtain the selected spline type.
         *
         * \return Returns the selected spline type.
         */
        SplineType splineType() const;

        /**
         * Method you can use to obtain the selected gradient type.
         *
         * \return Returns the selected gradient type.
         */
        GradientType gradientType() const;

        /**
         * Static method you can use to paint onto an arbitrary surface.
         *
         * \param[in] painter           Painter used to draw the line style image.
         *
         * \param[in] boundingRectangle The bounding rectangle to contain the line style image in.
         *
         * \param[in] lineStyle         The line style.
         *
         * \param[in] lineWidth         The line widht.
         *
         * \param[in] lineColor         The line color.
         *
         * \param[in] markerStyle       The marker style.
         *
         * \param[in] splineType        The spline type.
         *
         * \param[in] gradientType      The gradient type.
         */
        static void paint(
            QPainter*      painter,
            const QRect&   boundingRectangle,
            LineStyle      lineStyle,
            float          lineWidth,
            const QColor&  lineColor,
            MarkerStyle    markerStyle,
            SplineType     splineType,
            GradientType   gradientType
        );

    public slots:
        /**
         * Slot you can trigger to set the plot series line style.
         *
         * \param[in] newLineStyle The new plot series line style.
         */
        void setLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to change the current line width.
         *
         * \param[in] newLineWidth The new line width setting.
         */
        void setLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to change the current line color.
         *
         * \param[in] newColor The new line color.
         */
        void setLineColor(const QColor& newColor);

        /**
         * Slot you can trigger to change the marker style.
         *
         * \param[in] newMarkerStyle The new marker style.
         */
        void setMarkerStyle(MarkerStyle newMarkerStyle);

        /**
         * Slot you can trigger to change the selected spline type.
         *
         * \param[in] newSplineType The new spline type.
         */
        void setSplineType(SplineType newSplineType);

        /**
         * Slot you can trigger to change the selected gradient type.
         *
         * \param[in] newGradientType The new gradient type.
         */
        void setGradientType(GradientType newGradientType);

    protected:
        /**
         * Method that is triggered to paint onto this widget.
         *
         * \param[in] event Event that triggered the call to this method.
         */
        void paintEvent(QPaintEvent* event) override;

    private:
        /**
         * The current line style.
         */
        LineStyle currentLineStyle;

        /**
         * The current line width.
         */
        float currentLineWidth;

        /**
         * The current line color.
         */
        QColor currentLineColor;

        /**
         * The current marker style.
         */
        MarkerStyle currentMarkerStyle;

        /**
         * The current spline type.
         */
        SplineType currentSplineType;

        /**
         * The current gradient type.
         */
        GradientType currentGradientType;
};

#endif
