/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotWizard class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_WIZARD_H
#define PLOT_WIZARD_H

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
#include "plot_settings_widget.h"

class QWidget;

class PlotStyleWidget;
class PlotDataSourcesWidget;
class PlotAxisSettingsWidget;
class PlotAppearanceWidget;

/**
 * Wizard used to configure a new plot.
 */
class APP_PUBLIC_API PlotWizard:public QWizard, public PlotSettingsWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        PlotWizard(QWidget* parent = Q_NULLPTR);

        ~PlotWizard() override;

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void loadSettings(const QString& groupName) override;

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void saveSettings(const QString& groupName) override;

        /**
         * Method that provides the currently selected plot style.
         *
         * \return Returns the currently selected plotting engine.  A null pointer is returned if a singular plotting
         *         engine has not been selected.
         */
        const PlotEngine* plotStyle() const override;

        /**
         * Method that provides the currently selected plot width, in points.
         *
         * \return Returns the currently selected plot width, in points.  A zero or negative value indicates that no
         *         plot width has been entered.
         */
        float plotWidth() const override;

        /**
         * Method that provides the currently selected plot height, in points.
         *
         * \return Returns the currently selected plot height, in points.  A zero or negative value indicates that no
         *         plot height has been entered.
         */
        float plotHeight() const override;

        /**
         * Method you can use to determine the current number of data sources per data series.
         *
         * \return Returns the current number of data sources per data series.
         */
        unsigned dataSourcesPerDataSeries() const override;

        /**
         * Method you can use to determine the current number of data series.
         */
        unsigned numberDataSeries() const override;

        /**
         * Method you can use to determine the style to use for axis locations.
         *
         * \returns the currently selected axis location style.
         */
        AxisLocationStyle axisLocationStyle() const override;

        /**
         * Method you can use to determine the currently supported plot series mode.
         *
         * \return Returns the currently supported plot series mode.
         */
        PlotSeriesMode supportedPlotSeriesMode() const override;

        /**
         * Method you can use to obtain a list of all the data sources.
         *
         * \return Returns a list of all the data sources.  Sources will be in source order by series order.
         */
        QList<Ld::VariableName> dataSources() const override;

        /**
         * Method you can use to obtain a specific data source.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \param[in] sourceIndex The zero based index of the source within the data series.
         *
         * \return Returns the name of the requested data source.
         */
        Ld::VariableName dataSource(unsigned seriesIndex, unsigned sourceIndex) const override;

        /**
         * Method you can use to determine the axis assigned to each data source.
         *
         * \return Returns a list of axis assigned to each data source.
         */
        QList<AxisLocation> configuredAxisLocations() const override;

        /**
         * Method you can use to determine the axis location associated with a specific data source.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \param[in] sourceIndex The zero based index of the source within the data series.
         *
         * \return Returns the axis location associated with the data soruce and data series.
         */
        AxisLocation axisLocation(unsigned seriesIndex, unsigned sourceIndex) const override;

        /**
         * Method you can use to determine the chart line style and markers.  Note that the plot series do not have the
         * axis for each data source assigned.
         *
         * \return Returns the list of chart line styles and markers for each data source or pair of sources.
         */
        QList<Ld::PlotSeries> plotSeries() const override;

        /**
         * Method youc an use to determine the chart line style and markers for a specifiec series.  Note that the
         * plot series do not have the axis for each data source assigned.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \return Returns the requested plot series data.
         */
        Ld::PlotSeries plotSeries(unsigned seriesIndex) const override;

        /**
         * Method you can use to determine the full set of plot series labels.
         *
         * \return Returns a string list containing all the plot series labels.
         */
        const QStringList& seriesLabels() const override;

        /**
         * Method you can use to obtain a label for a specific plot series.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \return Returns the label associated with the plot series.
         */
        QString seriesLabel(unsigned seriesIndex) const override;

        /**
         * Method you can use to obtain a list of all the supported axis locations.
         *
         * \return Returns a list of every supported axis location based on the user input.
         */
        QList<AxisLocation> axisLocations() const override;

        /**
         * Method you can use to determine the baseline axis for the plot.  This value is intended primarily for use
         * with bar charts.
         *
         * \return Returns the baseline axis location for the plot.
         */
        AxisLocation baselineAxis() const override;

        /**
         * Method you can use to determine if the first data series should be used as a source for data labels.
         *
         * \return Returns true if the first data series should be used as a source of data labels.
         */
        bool firstDataSeriesIsDataLabels() const override;

        /**
         * Method you can use to obtain the desired axis scaling.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the desired axis scaling.
         */
        AxisScale axisScale(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the minimum axis value.  This is the value presented at the left or bottom
         * of the axis.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the minimum axis value.  The value \ref PlotFormatDialog::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double minimumValue(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the maximum axis value.  This is the value presented at the right or top
         * of the axis.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the maximum axis value.  The value \ref PlotFormatDialog::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double maximumValue(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the major tick step value.  The value must always be positive.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the major tick step value.  The value \ref PlotFormatDialog::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double majorStepSize(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the minor tick step value.  The value must always be positive.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the minor tick step value.  The value \ref PlotFormatDialog::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double minorStepSize(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the major tick style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the major tick style.
         */
        TickStyle majorTickStyle(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the minor tick style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the minor tick style.
         */
        TickStyle minorTickStyle(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the major grid line style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current major grid line style.
         */
        LineStyle majorGridLineStyle(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the major grid line width.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current major grid line width setting.
         */
        float majorGridLineWidth(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the major grid line color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the currently selected major grid line color.
         */
        QColor majorGridLineColor(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the minor grid line style.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current minor grid line style.
         */
        LineStyle minorGridLineStyle(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the minor grid line width.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the current minor grid line width setting.
         */
        float minorGridLineWidth(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the minor grid line color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the currently selected minor grid line color.
         */
        QColor minorGridLineColor(AxisLocation axisLocation) const override;

        /**
         * Method you can use to determine if the axis title field is visible or hidden.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns true if the axis title field is visible.  Returns false if the axis title field is hidden.
         */
        bool axisTitleFieldVisible(AxisLocation axisLocation) const override;

        /**
         * Method you can use to determine if the axis title field is hidden or visible.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns true if the axis title field is hidden.  Returns false if the axis title field is visible.
         */
        bool axisTitleFieldHidden(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the current axis title.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the currently entered axis title.
         */
        QString axisTitle(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the axis title font.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the axis title font.
         */
        QFont titleFont(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the axis title font color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the axis title font color.
         */
        QColor titleFontColor(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the axis title font background color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the axis title font background color.
         */
        QColor titleFontBackgroundColor(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the number font.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the number font.
         */
        QFont numberFont(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the number font color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the number font color.
         */
        QColor numberFontColor(AxisLocation axisLocation) const override;

        /**
         * Method you can use to obtain the number font background color.
         *
         * \param[in] axisLocation The axis location to be queried.
         *
         * \return Returns the number font background color.
         */
        QColor numberFontBackgroundColor(AxisLocation axisLocation) const override;

        /**
         * Method you can use to determine the plot border line style.
         *
         * \return Returns the plot border line style.
         */
        LineStyle borderLineStyle() const override;

        /**
         * Method you can use to determine the plot border line width.
         *
         * \return Returns the plot border line width.
         */
        float borderLineWidth() const override;

        /**
         * Method you can use to determine the plot border line color.
         *
         * \return Returns the plot border line color.
         */
        QColor borderLineColor() const override;

        /**
         * Method you can use to determine the currently selected plot background color.
         *
         * \return Returns the currently selected plot background color.
         */
        QColor backgroundColor() const override;

        /**
         * Method you can use to determine the plot active area border line style.
         *
         * \return Returns the plot active area border line style.
         */
        LineStyle activeAreaBorderLineStyle() const override;

        /**
         * Method you can use to determine the plot active area border line width.
         *
         * \return Returns the plot active area border line width.
         */
        float activeAreaBorderLineWidth() const override;

        /**
         * Method you can use to determine the plot active area border line color.
         *
         * \return Returns the plot active area border line color.
         */
        QColor activeAreaBorderLineColor() const override;

        /**
         * Method you can use to determine the currently selected plotting area color.
         *
         * \return Returns the currently selected plotting area color.
         */
        QColor activeAreaColor() const override;

        /**
         * Method you can use to determine the legend location.
         *
         * \return Returns the currently selected legend location (or hidden).
         */
        LegendLocation legendLocation() const override;

        /**
         * Method you can use to determine the plot legend line style.
         *
         * \return Returns the plot legend line style.
         */
        LineStyle legendLineStyle() const override;

        /**
         * Method you can use to determine the plot legend line width.
         *
         * \return Returns the plot legend line width.
         */
        float legendLineWidth() const override;

        /**
         * Method you can use to determine the plot legend line color.
         *
         * \return Returns the plot legend line color.
         */
        QColor legendLineColor() const override;

        /**
         * Method you can use to determine the current legend background color.
         *
         * \return Returns the current legend background color.
         */
        QColor legendBackgroundColor() const override;

        /**
         * Method you can use to determine the plot legend font.
         *
         * \return Returns the selected plot legend font.
         */
        QFont legendFont() const override;

        /**
         * Method you can use to determine the plot legend font color.
         *
         * \return Returns the legend font color.
         */
        QColor legendFontColor() const override;

        /**
         * Method you can use to determine the plot tiel font background color.
         *
         * \return Returns the legend font background color.
         */
        QColor legendFontBackgroundColor() const override;

        /**
         * Method you can use to determine the plot title text.
         *
         * \return Returns the plot title text.
         */
        QString titleText() const override;

        /**
         * Method you can use to determine the plot title font.
         *
         * \return Returns the selected plot title font.
         */
        QFont titleFont() const override;

        /**
         * Method you can use to determine the plot title font color.
         *
         * \return Returns the title font color.
         */
        QColor titleFontColor() const override;

        /**
         * Method you can use to determine the plot tiel font background color.
         *
         * \return Returns the title font background color.
         */
        QColor titleFontBackgroundColor() const override;

    private slots:
        /**
         * Slot that is triggered when the user changes the current plot style.
         *
         * \param[in] newPlotStyle A string holding the newly selected plot style.
         */
        void plotStyleChanged(const PlotEngine* newPlotStyle);

        /**
         * Slot that is triggered when a data series value is modified.
         */
        void dataSeriesChanged();

    private:
        /**
         * The default plot width, in points.
         */
        static constexpr float defaultPlotWidth = 4.0F * 72.0F;

        /**
         * The default plot height, in points.
         */
        static constexpr float defaultPlotHeight = 3.0F * 72.0F;

        /**
         * The initial wizard dialog size in pixels.
         */
        static const QSize initialSizeInPixels;

        /**
         * Method that returns a plot axis settings widget for a given axis location.
         *
         * \param[in] axisLocation The axis location of interest.
         *
         * \return Returns the \ref PlotAxisSettingsWidget associated with the requested location.  A null pointer is
         *         returned if the location is invalid.
         */
        PlotAxisSettingsWidget* plotAxisSettingsWidget(AxisLocation axisLocation) const;

        /**
         * The plot style widget.  This represents the first page.
         */
        PlotStyleWidget* plotStyleWidget;

        /**
         * The ID of the plot style widget.
         */
        int plotStyleWidgetId;

        /**
         * The plot data sources widget.  This represents the second page.
         */
        PlotDataSourcesWidget* plotDataSourcesWidget;

        /**
         * The ID of the data sources widget.
         */
        int plotDataSourcesWidgetId;

        /**
         * The plot axis style pages.  This represents up-to four pages.
         */
        QList<PlotAxisSettingsWidget*> plotAxisSettingsWidgets;

        /**
         * The IDs of the plot axis settings widgets.
         */
        QList<int> plotAxisSettingsWidgetIds;

        /**
         * The list of axis locations for each axis settings widget.
         */
        QList<AxisLocation> plotAxisLocations;

        /**
         * The plot appearances page.  This represents the last page.
         */
        PlotAppearanceWidget* plotAppearanceWidget;

        /**
         * The ID of the plot appearances widget.
         */
        int plotAppearanceWidgetId;
};

#endif
