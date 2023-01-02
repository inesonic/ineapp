/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotSettingsWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_SETTINGS_WIDGET_H
#define PLOT_SETTINGS_WIDGET_H

#include <QWizard>
#include <QString>
#include <QList>
#include <QStringList>
#include <QSize>

#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>
#include <ld_variable_name.h>

#include "app_common.h"
#include "plot_engine.h"

class QWidget;

class PlotStyleWidget;
class PlotDataSourcesWidget;
class PlotAxisSettingsWidget;
class PlotAppearanceWidget;

/**
 * Pure virtual base class used to abstract away differences between the \ref PlotWizard and \ref PlotFormatDialog
 * classes.
 */
class APP_PUBLIC_API PlotSettingsWidget {
    public:
        /**
         * Enumeration of supported axis positions.
         */
        typedef Ld::PlotFormat::AxisLocation AxisLocation;

        /**
         * Enumeration of supported plot series modes.
         */
        typedef PlotEngine::PlotSeriesMode PlotSeriesMode;

        /**
         * Enumeration of axis location styles.
         */
        typedef PlotEngine::AxisLocationStyle AxisLocationStyle;

        /**
         * The type used to indicate a line style.
         */
        typedef Ld::ChartLineStyle::LineStyle LineStyle;

        /**
         * Enumeration indicating the desired axis scale.
         */
        typedef Ld::ChartAxisFormat::AxisScale AxisScale;

        /**
         * Enumeration indicating the desired axis tick style.
         */
        typedef Ld::ChartAxisFormat::TickStyle TickStyle;

        /**
         * Enumeration indicating different marker styles.
         */
        typedef Ld::PlotSeries::MarkerStyle MarkerStyle;

        /**
         * The legend location.
         */
        typedef Ld::PlotFormat::LegendLocation LegendLocation;

        /**
         * Value used to represent automatic scaling of values.  Maps to infinity.
         */
        static const double automaticScaling;

        PlotSettingsWidget();

        virtual ~PlotSettingsWidget();

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        virtual void loadSettings(const QString& groupName) = 0;

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        virtual void saveSettings(const QString& groupName) = 0;

        /**
         * Method that provides the currently selected plot style.
         *
         * \return Returns the currently selected plotting engine.  A null pointer is returned if a singular plotting
         *         engine has not been selected.
         */
        virtual const PlotEngine* plotStyle() const = 0;

        /**
         * Method that provides the currently selected plot width, in points.
         *
         * \return Returns the currently selected plot width, in points.  A zero or negative value indicates that no
         *         plot width has been entered.
         */
        virtual float plotWidth() const = 0;

        /**
         * Method that provides the currently selected plot height, in points.
         *
         * \return Returns the currently selected plot height, in points.  A zero or negative value indicates that no
         *         plot height has been entered.
         */
        virtual float plotHeight() const = 0;

        /**
         * Method you can use to determine the current number of data sources per data series.
         *
         * \return Returns the current number of data sources per data series.
         */
        virtual unsigned dataSourcesPerDataSeries() const = 0;

        /**
         * Method you can use to determine the current number of data series.
         *
         * \return Returns the number of data series.
         */
        virtual unsigned numberDataSeries() const = 0;

        /**
         * Method you can use to determine the style to use for axis locations.
         *
         * \returns the currently selected axis location style.
         */
        virtual AxisLocationStyle axisLocationStyle() const = 0;

        /**
         * Method you can use to determine the currently supported plot series mode.
         *
         * \return Returns the currently supported plot series mode.
         */
        virtual PlotSeriesMode supportedPlotSeriesMode() const = 0;

        /**
         * Method you can use to obtain a list of all the data sources.
         *
         * \return Returns a list of all the data sources.  Sources will be in source order by series order.
         */
        virtual QList<Ld::VariableName> dataSources() const = 0;

        /**
         * Method you can use to obtain a specific data source.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \param[in] sourceIndex The zero based index of the source within the data series.
         *
         * \return Returns the name of the requested data source.
         */
        virtual Ld::VariableName dataSource(unsigned seriesIndex, unsigned sourceIndex) const = 0;

        /**
         * Method you can use to determine the axis assigned to each data source.
         *
         * \return Returns a list of axis assigned to each data source.
         */
        virtual QList<AxisLocation> configuredAxisLocations() const = 0;

        /**
         * Method you can use to determine the axis location associated with a specific data source.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \param[in] sourceIndex The zero based index of the source within the data series.
         *
         * \return Returns the axis location associated with the data soruce and data series.
         */
        virtual AxisLocation axisLocation(unsigned seriesIndex, unsigned sourceIndex) const = 0;

        /**
         * Method you can use to determine the chart line style and markers.  Note that the plot series do not have the
         * axis for each data source assigned.
         *
         * \return Returns the list of chart line styles and markers for each data source or pair of sources.
         */
        virtual QList<Ld::PlotSeries> plotSeries() const = 0;

        /**
         * Method youc an use to determine the chart line style and markers for a specifiec series.  Note that the
         * plot series do not have the axis for each data source assigned.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \return Returns the requested plot series data.
         */
        virtual Ld::PlotSeries plotSeries(unsigned seriesIndex) const = 0;

        /**
         * Method you can use to determine the full set of plot series labels.
         *
         * \return Returns a string list containing all the plot series labels.
         */
        virtual const QStringList& seriesLabels() const = 0;

        /**
         * Method you can use to obtain a label for a specific plot series.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \return Returns the label associated with the plot series.
         */
        virtual QString seriesLabel(unsigned seriesIndex) const = 0;

        /**
         * Method you can use to obtain a list of all the supported axis locations.
         *
         * \return Returns a list of every supported axis location based on the user input.
         */
        virtual QList<AxisLocation> axisLocations() const = 0;

        /**
         * Method you can use to determine the baseline axis for the plot.  This value is intended primarily for use
         * with bar charts.
         *
         * \return Returns the baseline axis location for the plot.
         */
        virtual AxisLocation baselineAxis() const = 0;

        /**
         * Method you can use to determine if the first data series should be used as a source for data labels.
         *
         * \return Returns true if the first data series should be used as a source of data labels.
         */
        virtual bool firstDataSeriesIsDataLabels() const = 0;

        /**
         * Method you can use to obtain the desired axis scaling.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the desired axis scaling.
         */
        virtual AxisScale axisScale(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the minimum axis value.  This is the value presented at the left or bottom
         * of the axis.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the minimum axis value.  The value \ref PlotSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        virtual double minimumValue(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the maximum axis value.  This is the value presented at the right or top
         * of the axis.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the maximum axis value.  The value \ref PlotSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        virtual double maximumValue(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the major tick step value.  The value must always be positive.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the major tick step value.  The value \ref PlotSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        virtual double majorStepSize(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the minor tick step value.  The value must always be positive.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the minor tick step value.  The value \ref PlotSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        virtual double minorStepSize(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the major tick style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the major tick style.
         */
        virtual TickStyle majorTickStyle(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the minor tick style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the minor tick style.
         */
        virtual TickStyle minorTickStyle(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the major grid line style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current major grid line style.
         */
        virtual LineStyle majorGridLineStyle(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the major grid line width.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current major grid line width setting.
         */
        virtual float majorGridLineWidth(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the major grid line color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the currently selected major grid line color.
         */
        virtual QColor majorGridLineColor(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the minor grid line style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current minor grid line style.
         */
        virtual LineStyle minorGridLineStyle(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the minor grid line width.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current minor grid line width setting.
         */
        virtual float minorGridLineWidth(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the minor grid line color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the currently selected minor grid line color.
         */
        virtual QColor minorGridLineColor(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to determine if the axis title field is visible or hidden.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns true if the axis title field is visible.  Returns false if the axis title field is hidden.
         */
        virtual bool axisTitleFieldVisible(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to determine if the axis title field is hidden or visible.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns true if the axis title field is hidden.  Returns false if the axis title field is visible.
         */
        virtual bool axisTitleFieldHidden(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the current axis title.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the currently entered axis title.
         */
        virtual QString axisTitle(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the axis title font.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the axis title font.
         */
        virtual QFont titleFont(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the axis title font color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the axis title font color.
         */
        virtual QColor titleFontColor(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the axis title font background color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the axis title font background color.
         */
        virtual QColor titleFontBackgroundColor(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the number font.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the number font.
         */
        virtual QFont numberFont(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the number font color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the number font color.
         */
        virtual QColor numberFontColor(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to obtain the number font background color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the number font background color.
         */
        virtual QColor numberFontBackgroundColor(AxisLocation axisLocation) const = 0;

        /**
         * Method you can use to determine the plot border line style.
         *
         * \return Returns the plot border line style.
         */
        virtual LineStyle borderLineStyle() const = 0;

        /**
         * Method you can use to determine the plot border line width.
         *
         * \return Returns the plot border line width.
         */
        virtual float borderLineWidth() const = 0;

        /**
         * Method you can use to determine the plot border line color.
         *
         * \return Returns the plot border line color.
         */
        virtual QColor borderLineColor() const = 0;

        /**
         * Method you can use to determine the currently selected plot background color.
         *
         * \return Returns the currently selected plot background color.
         */
        virtual QColor backgroundColor() const  = 0;

        /**
         * Method you can use to determine the plot active area border line style.
         *
         * \return Returns the plot active area border line style.
         */
        virtual LineStyle activeAreaBorderLineStyle() const = 0;

        /**
         * Method you can use to determine the plot active area border line width.
         *
         * \return Returns the plot active area border line width.
         */
        virtual float activeAreaBorderLineWidth() const = 0;

        /**
         * Method you can use to determine the plot active area border line color.
         *
         * \return Returns the plot active area border line color.
         */
        virtual QColor activeAreaBorderLineColor() const = 0;

        /**
         * Method you can use to determine the currently selected plotting area color.
         *
         * \return Returns the currently selected plotting area color.
         */
        virtual QColor activeAreaColor() const = 0;

        /**
         * Method you can use to determine the legend location.
         *
         * \return Returns the currently selected legend location (or hidden).
         */
        virtual LegendLocation legendLocation() const = 0;

        /**
         * Method you can use to determine the plot legend line style.
         *
         * \return Returns the plot legend line style.
         */
        virtual LineStyle legendLineStyle() const = 0;

        /**
         * Method you can use to determine the plot legend line width.
         *
         * \return Returns the plot legend line width.
         */
        virtual float legendLineWidth() const = 0;

        /**
         * Method you can use to determine the plot legend line color.
         *
         * \return Returns the plot legend line color.
         */
        virtual QColor legendLineColor() const = 0;

        /**
         * Method you can use to determine the current legend background color.
         *
         * \return Returns the current legend background color.
         */
        virtual QColor legendBackgroundColor() const = 0;

        /**
         * Method you can use to determine the plot legend font.
         *
         * \return Returns the selected plot legend font.
         */
        virtual QFont legendFont() const = 0;

        /**
         * Method you can use to determine the plot legend font color.
         *
         * \return Returns the legend font color.
         */
        virtual QColor legendFontColor() const = 0;

        /**
         * Method you can use to determine the plot tiel font background color.
         *
         * \return Returns the legend font background color.
         */
        virtual QColor legendFontBackgroundColor() const = 0;

        /**
         * Method you can use to determine the plot title text.
         *
         * \return Returns the plot title text.
         */
        virtual QString titleText() const = 0;

        /**
         * Method you can use to determine the plot title font.
         *
         * \return Returns the selected plot title font.
         */
        virtual QFont titleFont() const = 0;

        /**
         * Method you can use to determine the plot title font color.
         *
         * \return Returns the title font color.
         */
        virtual QColor titleFontColor() const = 0;

        /**
         * Method you can use to determine the plot tiel font background color.
         *
         * \return Returns the title font background color.
         */
        virtual QColor titleFontBackgroundColor() const = 0;
};

#endif
