/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_FORMAT_DIALOG_H
#define PLOT_FORMAT_DIALOG_H

#include <QSize>
#include <QString>
#include <QList>
#include <QStringList>
#include <QColor>
#include <QFont>

#include <eqt_programmatic_dialog.h>

#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>
#include <ld_variable_name.h>

#include "app_common.h"
#include "plot_engine.h"
#include "plot_settings_widget.h"

class QWidget;

class PlotAxisSettingsWidget;

/**
 * Dialog used to modify the plot format.
 */
class APP_PUBLIC_API PlotFormatDialog:public EQt::ProgrammaticDialog, public virtual PlotSettingsWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlotFormatDialog(QWidget* parent = Q_NULLPTR);

        ~PlotFormatDialog() override;

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
         * Method you can use to determine the chart line style and markers.
         *
         * \return Returns the list of chart line styles and markers for each data source or pair of sources.
         */
        QList<Ld::PlotSeries> plotSeries() const override;

        /**
         * Method youc an use to determine the chart line style and markers for a specifiec series.
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

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the user accepts the dialog contents.
         *
         * \param[out] dialog Pointer to this dialog.
         */
        void accepted(PlotFormatDialog* dialog);

    public slots:
        /**
         * Slot you can use to set the currently selected plotting engine.
         *
         * \param[in] newPlotEngine The newly selected plotting engine.
         */
        void setPlotStyle(const PlotEngine* newPlotEngine);

        /**
         * Slot you can use to set currently selected plot width, in points.
         *
         * \param[in] newPlotWidth The newly selected plot width.  A zero or negative value indicates that the field
         *                         should be left empty.
         */
        void setPlotWidth(float newPlotWidth);

        /**
         * Slot you can use to set currently selected plot height, in points.
         *
         * \param[in] newPlotHeight The newly selected plot height.  A zero or negative value indicates that the field
         *                          should be left empty.
         */
        void setPlotHeight(float newPlotHeight);

        /**
         * Method you can use to set the data sources and related structures.
         *
         * \param[in] newDataSources      The new list of data sources.
         *
         * \param[in] newAxisLocations    A list of axis locations for each data source.  The number of elements must
         *                                match the number of data sources.
         *
         * \param[in] newPlotSeries       The plot series values for every data source or pair of data sources.  If the
         *                                number of plot series equals the number of data sources then the data wlll be
         *                                marked as singular.  If the number of plot series equals exactly half the
         *                                number of data sources, the the data will be marked as paired.  All other
         *                                values will cause this method to assert.
         *
         * \param[in] newPlotSeriesLabels The labels to associate with each plot series.
         */
        void setDataSources(
            const QList<Ld::VariableName>& newDataSources,
            const QList<AxisLocation>&     newAxisLocations,
            const QList<Ld::PlotSeries>&   newPlotSeries,
            const QStringList&             newPlotSeriesLabels
        );

        /**
         * Method you can use to set a single data source.
         *
         * \param[in] seriesIndex   The zero based index of the data series of interest.
         *
         * \param[in] sourceIndex   The zero based index of the source within the data series.
         *
         * \param[in] newDataSource The new list of data source.
         */
        void setDataSource(unsigned seriesIndex, unsigned sourceIndex, const Ld::VariableName& newDataSource);

        /**
         * Method you can use to set the axis to apply to a particular data source.
         *
         * \param[in] seriesIndex     The zero based index of the data series of interest.
         *
         * \param[in] sourceIndex     The zero based index of the source within the data series.
         *
         * \param[in] newAxisLocation The axis location to apply to the data source.
         */
        void setAxisLocation(unsigned seriesIndex, unsigned sourceIndex, AxisLocation newAxisLocation);

        /**
         * Method you can use to set the plot series for a particular data source or pair of data sources.
         *
         * \param[in] index         The zero based index for the plot series to be updated.
         *
         * \param[in] newPlotSeries The particular plot series to be updated.
         */
        void setPlotSeries(unsigned index, const Ld::PlotSeries& newPlotSeries);

        /**
         * Method you can use to set the plot series label for a particular data series.
         *
         * \param[in] seriesIndex     The zero based index of the plotseries of interest.
         *
         * \param[in] plotSeriesLabel the label to assign to the plot series.
         */
        void setPlotSeriesLabel(unsigned seriesIndex, const QString& plotSeriesLabel);

        /**
         * Slot you can trigger to set the baseline axis for the plot.  This value is intended primarily for use with
         * bar charts.
         *
         * \param[in] newBaselineAxis The new baseline axis.
         */
        void setBaselineAxis(AxisLocation newBaselineAxis);

        /**
         * Slot you can trigger to specify if the first data series should be used for data labels.
         *
         * \param[in] nowFirstDataSeriesIsDataLabels If true, the first data series should be used for data labels.  If
         *                                           false, the first data series is simply a data series.
         */
        void setFirstDataSeriesIsDataLabels(bool nowFirstDataSeriesIsDataLabels);

        /**
         * Slot you can trigger to set the desired axis scaling.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newAxisScale The new axis scaling.
         */
        void setAxisScale(AxisLocation axisLocation, AxisScale newAxisScale);

        /**
         * Slot you can trigger to set the major grid line style.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newLineStyle The new line style.
         */
        void setMajorGridLineStyle(AxisLocation axisLocation, LineStyle newLineStyle);

        /**
         * Slot you can trigger to set the major grid line width.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newLineWidth The new line width.
         */
        void setMajorGridLineWidth(AxisLocation axisLocation, float newLineWidth);

        /**
         * Slot you can trigger to set the major grid line color.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newLineColor The new line color.
         */
        void setMajorGridLineColor(AxisLocation axisLocation, const QColor& newLineColor);

        /**
         * Slot you can trigger to set the minor grid line style.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newLineStyle The new line style.
         */
        void setMinorGridLineStyle(AxisLocation axisLocation, LineStyle newLineStyle);

        /**
         * Slot you can trigger to set the minor grid line width.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newLineWidth The new line width.
         */
        void setMinorGridLineWidth(AxisLocation axisLocation, float newLineWidth);

        /**
         * Slot you can trigger to set the minor grid line color.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newLineColor The new line color.
         */
        void setMinorGridLineColor(AxisLocation axisLocation, const QColor& newLineColor);

        /**
         * Slot you can trigger to set the minimum axis value.  This is the value presented at the left or bottom
         * of the axis.
         *
         * \param[in] axisLocation    The axis location to be updated.
         *
         * \param[in] newMinimumValue The new minimum value.  The value \ref PlotFormatDialog::automaticScaling
         *                            will be indicate automatic scaling is desired.
         */
        void setMinimumValue(AxisLocation axisLocation, double newMinimumValue);

        /**
         * Slot you can trigger to set the maximum axis value.  This is the value presented at the right or top
         * of the axis.
         *
         * \param[in] axisLocation    The axis location to be updated.
         *
         * \param[in] newMaximumValue The new maximum value.  The value \ref PlotFormatDialog::automaticScaling
         *                            will indicate automatic scaling is desired.
         */
        void setMaximumValue(AxisLocation axisLocation, double newMaximumValue);

        /**
         * Slot you can trigger to set the major tick step value.  The value must always be positive.
         *
         * \param[in] axisLocation     The axis location to be updated.
         *
         * \param[in] newMajorStepSize The new major step size.  The value
         *                             \ref PlotFormatDialog::automaticScaling will indicate that automatic
         *                             scaling is desired.
         */
        void setMajorStepSize(AxisLocation axisLocation, double newMajorStepSize);

        /**
         * Slot you can trigger to set the minor tick step value.  The value must always be positive.
         *
         * \param[in] axisLocation     The axis location to be updated.
         *
         * \param[in] newMinorStepSize The new minor tick step value.  The value
         *                             \ref PlotFormatDialog::automaticScaling indicates that automatic scaling
         *                             is desired.
         */
        void setMinorStepSize(AxisLocation axisLocation, double newMinorStepSize);

        /**
         * Slot you can trigger to set the major tick style.
         *
         * \param[in] axisLocation      The axis location to be updated.
         *
         * \param[in] newMajorTickStyle The new major tick style.
         */
        void setMajorTickStyle(AxisLocation axisLocation, TickStyle newMajorTickStyle);

        /**
         * Slot you can trigger to set the minor tick style.
         *
         * \param[in] axisLocation      The axis location to be updated.
         *
         * \param[in] newMinorTickStyle The new minor tick style.
         */
        void setMinorTickStyle(AxisLocation axisLocation, TickStyle newMinorTickStyle);

        /**
         * Slot you can trigger to update the axis title text.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newAxisTitle The new axis title text.
         */
        void setAxisTitle(AxisLocation axisLocation, const QString& newAxisTitle);

        /**
         * Slot you can trigger to set the axis title font.
         *
         * \param[in] axisLocation The axis location to be updated.
         *
         * \param[in] newTitleFont The new title font.
         */
        void setTitleFont(AxisLocation axisLocation, const QFont& newTitleFont);

        /**
         * Slot you can trigger to set the axis title font color.
         *
         * \param[in] axisLocation      The axis location to be updated.
         *
         * \param[in] newTitleFontColor The new title font color.
         */
        void setTitleFontColor(AxisLocation axisLocation, const QColor& newTitleFontColor);

        /**
         * Slot you can trigger to set the axis title font background color.
         *
         * \param[in] axisLocation                The axis location to be updated.
         *
         * \param[in] newTitleFontBackgroundColor The new title font background color.
         */
        void setTitleFontBackgroundColor(AxisLocation axisLocation, const QColor& newTitleFontBackgroundColor);

        /**
         * Slot you can trigger to set the number font.
         *
         * \param[in] axisLocation  The axis location to be updated.
         *
         * \param[in] newNumberFont The new number font.
         */
        void setNumberFont(AxisLocation axisLocation, const QFont& newNumberFont);

        /**
         * Slot you can trigger to set the number font color.
         *
         * \param[in] axisLocation       The axis location to be updated.
         *
         * \param[in] newNumberFontColor The new number font color.
         */
        void setNumberFontColor(AxisLocation axisLocation, const QColor& newNumberFontColor);

        /**
         * Slot you can trigger to set the number font background color.
         *
         * \param[in] axisLocation                 The axis location to be updated.
         *
         * \param[in] newNumberFontBackgroundColor The new number font background color.
         */
        void setNumberFontBackgroundColor(AxisLocation axisLocation, const QColor& newNumberFontBackgroundColor);

        /**
         * Slot you can trigger to change the plot border line style.
         *
         * \param[in] newBorderLineStyle The new plot border line style.
         */
        void setBorderLineStyle(LineStyle newBorderLineStyle);

        /**
         * Slot you can trigger to change the plot border line width.
         *
         * \param[in] newBorderWidth The new plot border line width.
         */
        void setBorderLineWidth(float newBorderWidth);

        /**
         * Slot you can trigger to change the plot border line color.
         *
         * \param[in] newBorderColor The new plot border line color.
         */
        void setBorderLineColor(const QColor& newBorderColor);

        /**
         * Slot you can trigger to change the currently selected plot background color.
         *
         * \param[in] newBackgroundColor The new plot background color.
         */
        void setBackgroundColor(const QColor& newBackgroundColor);

        /**
         * Slot you can trigger to change the plot active area border line style.
         *
         * \param[in] newLineStyle The new plot active area border line style.
         */
        void setActiveAreaBorderLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to change the plot active area border line width.
         *
         * \param[in] newLineWidth The new plot active area border line width.
         */
        void setActiveAreaBorderLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to change the plot active area border line color.
         *
         * \param[in] newLineColor The new plot active area border line color.
         */
        void setActiveAreaBorderLineColor(const QColor& newLineColor);

        /**
         * Slot you can trigger to change the currently selected plotting area color.
         *
         * \param[in] newPlotAreaColor The new plotting area color.
         */
        void setPlotAreaColor(const QColor& newPlotAreaColor);

        /**
         * Slot you can trigger to change the legend location.
         *
         * \param[in] newLocation The new legend location (or hidden).
         */
        void setLegendLocation(LegendLocation newLocation);

        /**
         * Slot you can trigger to change the plot legend line style.
         *
         * \param[in] newLineStyle The new plot legend line style.
         */
        void setLegendLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to change the plot legend line width.
         *
         * \param[in] newLineWidth The new plot legend line width.
         */
        void setLegendLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to change the plot legend line color.
         *
         * \param[in] newLineColor The new plot legend line color.
         */
        void setLegendLineColor(const QColor& newLineColor);

        /**
         * Slot you can trigger to change the plot legend font.
         *
         * \param[in] newFont The new selected plot legend font.
         */
        void setLegendFont(const QFont& newFont);

        /**
         * Slot you can trigger to change the plot legend font color.
         *
         * \param[in] newFontColor The new legend font color.
         */
        void setLegendFontColor(const QColor& newFontColor);

        /**
         * Slot you can trigger to change the plot title font background color.
         *
         * \param[in] newFontBackgroundColor The new legend font background color.
         */
        void setLegendFontBackgroundColor(const QColor& newFontBackgroundColor);

        /**
         * Slot you can trigger to change the legend background color.
         *
         * \param[in] newLegendBackgroundColor The new legend background color.
         */
        void setLegendBackgroundColor(const QColor& newLegendBackgroundColor);

        /**
         * Slot you can trigger to change the plot title text.
         *
         * \param[in] newText The new plot title text.
         */
        void setTitleText(const QString& newText);

        /**
         * Slot you can trigger to change the plot title font.
         *
         * \param[in] newFont The new plot title font.
         */
        void setTitleFont(const QFont& newFont);

        /**
         * Slot you can trigger to change the plot title font color.
         *
         * \param[in] newFontColor The new title font color.
         */
        void setTitleFontColor(const QColor& newFontColor);

        /**
         * Slot you can trigger to change the plot tiel font background color.
         *
         * \param[in] newFontBackgroundColor The new title font background color.
         */
        void setTitleFontBackgroundColor(const QColor& newFontBackgroundColor);

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

        /**
         * Slot that is triggered whn an axis setting is change.
         */
        void axisSettingChanged();

        /**
         * Slot that is triggered when an appearance setting is changed.
         */
        void appearanceSettingChanged();

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private:
        /**
         * The plot style tab index.
         */
        static constexpr unsigned styleTabIndex = 0;

        /**
         * The plot data sources tab index.
         */
        static constexpr unsigned dataSourcesTabIndex = 1;

        /**
         * The first axis tab index.
         */
        static constexpr unsigned firstAxisTabIndex = 2;

        /**
         * The last available axis tab index.
         */
        static constexpr unsigned lastAxisTabIndex = 5;

        /**
         * The appearances tab index.
         */
        static constexpr unsigned appearancesTabIndex = 6;

        /**
         * Method that configures this dialog.
         */
        void configure();

        /**
         * Method that updates the tab settings.
         */
        void updateTabs();

        /**
         * Method that determines if any of the dialog values have been changed.
         *
         * \return Returns true if one or more values have changed.  Returns false if all value represent their
         *         initial state.
         */
        bool valueChanged() const;

        /**
         * Method that updates the OK button after a settings change.
         */
        void updateOkButton();

        /**
         * Method that obtains the plot axis settings widget associated with a given axis location.
         *
         * \param[in] axisLocation The axis location of interest.
         *
         * \return Returns the plot axis settings widget.  A null pointer is returned if the axis location is
         *         invalid.
         */
        PlotAxisSettingsWidget* axisSettingsWidget(AxisLocation axisLocation) const;

        /**
         * Flag used to avoid recursion when populating the dialog.
         */
        bool currentlyPopulatingDialog;

        /**
         * The input plot style.
         */
        const PlotEngine* inputPlotStyle;

        /**
         * The input plot width.
         */
        float inputPlotWidth;

        /**
         * The input plot height.
         */
        float inputPlotHeight;

        /**
         * The input data sources.
         */
        QList<Ld::VariableName> inputDataSources;

        /**
         * The input axis locations.
         */
        QList<AxisLocation> inputAxisLocations;

        /**
         * The input plot series values.
         */
        QList<Ld::PlotSeries> inputPlotSeries;

        /**
         * The input plot series labels.
         */
        QStringList inputPlotSeriesLabels;

        /**
         * The input baseline axis setting.
         */
        AxisLocation inputBaselineAxis;

        /**
         * The input first data series is data labels flag.
         */
        bool inputFirstDataSeriesIsDataLabels;

        /**
         * The input axis scaling for each axis.
         */
        QMap<AxisLocation, AxisScale> inputAxisScaling;

        /**
         * The input major grid line styles.
         */
        QMap<AxisLocation, LineStyle> inputMajorGridLineStyles;

        /**
         * The input major grid line widths.
         */
        QMap<AxisLocation, float> inputMajorGridLineWidths;

        /**
         * The input major grid line color.
         */
        QMap<AxisLocation, QColor> inputMajorGridLineColors;

        /**
         * The input minor grid line styles.
         */
        QMap<AxisLocation, LineStyle> inputMinorGridLineStyles;

        /**
         * The input minor grid line widths.
         */
        QMap<AxisLocation, float> inputMinorGridLineWidths;

        /**
         * The input minor grid line color.
         */
        QMap<AxisLocation, QColor> inputMinorGridLineColors;

        /**
         * The input minimum axis value.
         */
        QMap<AxisLocation, double> inputMinimumValues;

        /**
         * The input maximum axis value.
         */
        QMap<AxisLocation, double> inputMaximumValues;

        /**
         * The input major step size values.
         */
        QMap<AxisLocation, double> inputMajorStepSizes;

        /**
         * The input minor step size values.
         */
        QMap<AxisLocation, double> inputMinorStepSizes;

        /**
         * The input major tick styles.
         */
        QMap<AxisLocation, TickStyle> inputMajorTickStyles;

        /**
         * The input minor tick styles.
         */
        QMap<AxisLocation, TickStyle> inputMinorTickStyles;

        /**
         * The input axis title strings.
         */
        QMap<AxisLocation, QString> inputAxisTitles;

        /**
         * The input axis title fonts.
         */
        QMap<AxisLocation, QFont> inputAxisTitleFonts;

        /**
         * The input axis title font color.
         */
        QMap<AxisLocation, QColor> inputAxisTitleFontColors;

        /**
         * The input axis title font background color.
         */
        QMap<AxisLocation, QColor> inputAxisTitleFontBackgroundColors;

        /**
         * The input axis number fonts.
         */
        QMap<AxisLocation, QFont> inputAxisNumberFonts;

        /**
         * The input axis number font color.
         */
        QMap<AxisLocation, QColor> inputAxisNumberFontColors;

        /**
         * The input axis number font background color.
         */
        QMap<AxisLocation, QColor> inputAxisNumberFontBackgroundColors;

        /**
         * The input plot border line style.
         */
        LineStyle inputBorderLineStyle;

        /**
         * The input plot border line width.
         */
        float inputBorderLineWidth;

        /**
         * The input plot border line color.
         */
        QColor inputBorderLineColor;

        /**
         * The input background color.
         */
        QColor inputBackgroundColor;

        /**
         * The input plot active area border line style.
         */
        LineStyle inputActiveAreaBorderLineStyle;

        /**
         * The input plot active area border line width.
         */
        float inputActiveAreaBorderLineWidth;

        /**
         * The input plot active area border line color.
         */
        QColor inputActiveAreaBorderLineColor;

        /**
         * The input plot area background color.
         */
        QColor inputActiveAreaBackgroundColor;

        /**
         * The input legend location.
         */
        LegendLocation inputLegendLocation;

        /**
         * The legend border line style.
         */
        LineStyle inputLegendBorderLineStyle;

        /**
         * The legend border line width.
         */
        float inputLegendBorderLineWidth;

        /**
         * The legend border line color.
         */
        QColor inputLegendBorderLineColor;

        /**
         * The legend font.
         */
        QFont inputLegendFont;

        /**
         * The legend font color.
         */
        QColor inputLegendFontColor;

        /**
         * The legend font background color.
         */
        QColor inputLegendFontBackgroundColor;

        /**
         * The legend area background color.
         */
        QColor inputLegendBackgroundColor;

        /**
         * The plot title text.
         */
        QString inputPlotTitleText;

        /**
         * The plot title text font.
         */
        QFont inputPlotTitleFont;

        /**
         * The plot title text font color.
         */
        QColor inputPlotTitleFontColor;

        /**
         * The iplot title text font background color.
         */
        QColor inputPlotTitleFontBackgroundColor;

        /**
         * Map of tabs for each axis location.
         */
        QMap<AxisLocation, unsigned> currentTabsByAxisLocation;
};

#endif
