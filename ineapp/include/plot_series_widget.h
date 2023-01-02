/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotSeriesWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_SERIES_WIDGET_H
#define PLOT_SERIES_WIDGET_H

#include <QWidget>
#include <QSize>
#include <QRect>

#include <ld_plot_series.h>
#include <ld_special_characters.h>

#include "plot_engine.h"
#include "app_common.h"

class QWidget;
class QComboBox;
class QLabel;
class QDoubleSpinBox;
class QShowEvent;
class QTimer;

namespace EQt {
    class ColorToolButton;
}

class PlotSeriesDemoWidget;

/**
 * Widget you can use to edit values tied to a plot series.
 */
class APP_PUBLIC_API PlotSeriesWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * The plot line style.
         */
        typedef Ld::PlotSeries::LineStyle LineStyle;

        /**
         * The marker style.
         */
        typedef Ld::PlotSeries::MarkerStyle MarkerStyle;

        /**
         * The line spline type.
         */
        typedef Ld::PlotSeries::SplineType SplineType;

        /**
         * The gradient type.
         */
        typedef Ld::PlotSeries::GradientType GradientType;

        /**
         * The display mode.
         */
        typedef PlotEngine::PlotSeriesMode PlotSeriesMode;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlotSeriesWidget(QWidget* parent = Q_NULLPTR);

        ~PlotSeriesWidget() override;

        /**
         * Method you can use to obtain the current demo image location in screen coordinants.
         *
         * \return Returns the current demo image location in screen coordinants.
         */
        QRect demoImageGeometry() const;

        /**
         * Method you can use to determine the current plot series mode.
         *
         * \return Returns the current plot series mode.
         */
        PlotSeriesMode plotSeriesMode() const;

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

    public slots:
        /**
         * Slot you can trigger to specify the size of the demo image.
         *
         * \param[in] newDemoImageSize The new demo image size.
         */
        void setDemoImageSize(const QSize& newDemoImageSize);

        /**
         * Slot you can trigger to change the current plot series mode.
         *
         * \param[in] newPlotSeriesMode The new plot series mode.
         */
        void setPlotSeriesMode(PlotSeriesMode newPlotSeriesMode);

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
         * Method that is triggered when the widget is shown.
         *
         * \param[in] event Event that triggered the call to this method.
         */
        void showEvent(QShowEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the color tool button is clicked.
         */
        void colorToolButtonClicked();

        /**
         * Slot that is triggered when the line style is changed.
         *
         * \param[in] index The new index into the combo box.
         */
        void lineStyleChanged(int index);

        /**
         * Slot that is triggered when the line width is changed.
         *
         * \param[in] newValue The new line width value.
         */
        void lineWidthChanged(double newValue);

        /**
         * Slot that is triggered when the line color is changed.
         *
         * \param[in] newColor The new line color.
         */
        void lineColorChanged(const QColor& newColor);

        /**
         * Slot that is triggered when the marker style is changed.
         *
         * \param[in] index The index of the newly selected marker style.
         */
        void markerStyleChanged(int index);

        /**
         * Slot that is triggered when the spline style is changed.
         *
         * \param[in] index The index of the newly selected spline style.
         */
        void splineStyleChanged(int index);

        /**
         * Slot that is triggered when teh gradient type is changed.
         *
         * \param[in] index The index of the newly selected gradient type.
         */
        void gradientTypeChanged(int index);

        /**
         * Slot that is triggered after this widget becomes visible.  We use this to conditionally steal the
         * applicaiton focus.
         */
        void nowVisible();

    private:
        /**
         * Method that is called to disconnect all signals.
         */
        void disconnectAll();

        /**
         * Method that is called to connect all signals.
         */
        void connectAll();

        /**
         * Method that updates the demo widget.
         */
        void updateDemoWidget();

        /**
         * Method that populates the line style combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateLineStyleComboBox(bool manageSignals = true);

        /**
         * Method that populates the spline type combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateSplineTypeComboBox(bool manageSignals = true);

        /**
         * Method that populates the gradient type combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateGradientTypeComboBox(bool manageSignals = true);

        /**
         * Method that populates the marker style combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateMarkerStyleComboBox(bool manageSignals = true);

        /**
         * The current plot series mode.  Determines what functions are visible and what functions are hidden.
         */
        PlotSeriesMode currentPlotSeriesMode;

        /**
         * The line style combo box.
         */
        QComboBox* lineStyleComboBox;

        /**
         * The line width spin box.
         */
        QDoubleSpinBox* lineWidthSpinBox;

        /**
         * The gradient style combo box.
         */
        QComboBox* gradientTypeComboBox;

        /**
         * The right arrow associated with the line style and line width.
         */
        QLabel* rightArrowLabel;

        /**
         * The demo widget.
         */
        PlotSeriesDemoWidget* plotSeriesDemoWidget;

        /**
         * The left arrow associated with the spline type.
         */
        QLabel* leftArrowLabel;

        /**
         * The spline type combo box.
         */
        QComboBox* splineTypeComboBox;

        /**
         * The up arrow associated with the marker style.
         */
        QLabel* upArrowLabel;

        /**
         * The marker style.
         */
        QComboBox* markerStyleComboBox;

        /**
         * The color tool button.
         */
        EQt::ColorToolButton* colorToolButton;

        /**
         * Timer used to defer focus changes when this widget is shown.
         */
        QTimer* showTimer;
};

#endif
