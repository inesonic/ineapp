/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotDataSourcesWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_DATA_SOURCES_WIDGET_H
#define PLOT_DATA_SOURCES_WIDGET_H

#include <QString>
#include <QList>
#include <QSet>
#include <QStringList>

#include <ld_variable_name.h>
#include <ld_chart_axis_format.h>
#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_plot_format.h>

#include "app_common.h"
#include "plot_engine.h"
#include "plot_widget_base.h"

class QWidget;
class QCheckBox;
class QComboBox;
class QTableView;
class QItemSelection;
class QToolButton;
class QPushButton;

namespace EQt {
    class DimensionLineEdit;
}

/**
 * Widget you can use to track and enter sources for data for the plot.  This class can also be used as a page in a
 * wizard.
 *
 * Note that this widget does not tie the data sources to the plot series and only adjusts the appearance of each
 * plot series.
 */
class APP_PUBLIC_API PlotDataSourcesWidget:public PlotWidgetBase {
    Q_OBJECT

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
         * Enumeration of supported spline types.
         */
        typedef Ld::PlotSeries::SplineType SplineType;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        PlotDataSourcesWidget(QWidget* parent = Q_NULLPTR);

        ~PlotDataSourcesWidget() override;

        /**
         * Method you can use to determine if the current settings are valid or invalid.
         *
         * \return Returns true if the settings are valid.  Returns false if the settings are invalid.
         */
        bool isValid() const override;

        /**
         * Method you can use to determine the current number of data sources per data series.
         *
         * \return Returns the current number of data sources per data series.
         */
        unsigned dataSourcesPerDataSeries() const;

        /**
         * Method you can use to determine the current number of data series.
         *
         * \return Returns the current number of data series.
         */
        unsigned numberDataSeries() const;

        /**
         * Method you can use to determine the style to use for axis locations.
         *
         * \returns the currently selected axis location style.
         */
        AxisLocationStyle axisLocationStyle() const;

        /**
         * Method you can use to determine the currently supported plot series mode.
         *
         * \return Returns the currently supported plot series mode.
         */
        PlotSeriesMode supportedPlotSeriesMode() const;

        /**
         * Method you can use to determine if the first data source represents series labels.
         *
         * \return Returns true if the first data series represents the data labels.  Returns false if the first data
         *         series is simply a data series.
         */
        bool firstDataSeriesIsLabels() const;

        /**
         * Method you can use to determine if the first data source represents a data value.
         *
         * \return Returns true if the first data series represents a value.  Returns false if the first data series
         *         contains the data labels.
         */
        bool firstDataSeriesIsValue() const;

        /**
         * Method you can use to determine the baseline for bar charts.
         *
         * \return Returns the baseline axis for bar charts.
         */
        AxisLocation baselineAxis() const;

        /**
         * Method you can use to determine the currently supported spline types.
         *
         * \return Returns a set of currently supported spline types.
         */
        QSet<SplineType> supportedSplineTypes() const;

        /**
         * Method you can use to obtain a list of all the data sources.
         *
         * \return Returns a list of all the data sources.  Paired sources will be presented in even/odd pairs.
         */
        QList<Ld::VariableName> dataSources() const;

        /**
         * Method you can use to obtain a specific data source.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \param[in] sourceIndex The zero based index of the source within the data series.
         *
         * \return Returns the name of the requested data source.
         */
        Ld::VariableName dataSource(unsigned seriesIndex, unsigned sourceIndex) const;

        /**
         * Method you can use to determine the axis assigned to each data source.
         *
         * \return Returns a list of axis assigned to each data source.
         */
        QList<AxisLocation> axisLocations() const;

        /**
         * Method you can use to determine the axis location associated with a specific data source.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \param[in] sourceIndex The zero based index of the source within the data series.
         *
         * \return Returns the axis location associated with the data soruce and data series.
         */
        AxisLocation axisLocation(unsigned seriesIndex, unsigned sourceIndex) const;

        /**
         * Method you can use to determine the chart line style and markers.
         *
         * \return Returns the list of chart line styles and markers for each data source or pair of sources.
         */
        QList<Ld::PlotSeries> plotSeries() const;

        /**
         * Method youc an use to determine the chart line style and markers for a specifiec series.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \return Returns the requested plot series data.
         */
        const Ld::PlotSeries& plotSeries(unsigned seriesIndex) const;

        /**
         * Method you can use to determine the full set of plot series labels.
         *
         * \return Returns a string list containing all the plot series labels.
         */
        const QStringList& seriesLabels() const;

        /**
         * Method you can use to obtain a label for a specific plot series.
         *
         * \param[in] seriesIndex The zero based index of the series of interest.
         *
         * \return Returns the label associated with the plot series.
         */
        QString seriesLabel(unsigned seriesIndex) const;

    public slots:
        /**
         * Slot you can use to set the number of data sources per data series.
         *
         * \param[in] newNumberDataSourcesPerDataSeries The new number of data sources per data series.
         */
        void setDataSourcesPerDataSeries(unsigned newNumberDataSourcesPerDataSeries);

        /**
         * Slot you can use to set the allowed range of number of data series.
         *
         * \param[in] minimumNumberDataSeries The minimum number of allowed data series.
         *
         * \param[in] maximumNumberDataSeries The maximum number of allowed data series.
         */
        void setNumberAllowedDataSeries(unsigned minimumNumberDataSeries, unsigned maximumNumberDataSeries);

        /**
         * Slot you can use to set the desired axis location style.
         *
         * \param[in] newAxisLocationStyle The new axis location style.
         */
        void setAxisLocationStyle(AxisLocationStyle newAxisLocationStyle);

        /**
         * Slot you can use to indicate that the first series represents data labels.
         *
         * \param[in] firstSeriesIsDataLabels If true, the first series represents data labels.  If false, the first
         *                                    series contains data.
         */
        void setFirstSeriesIsDataLabels(bool firstSeriesIsDataLabels = true);

        /**
         * Slot you can use to indicate that the first series represents a value.
         *
         * \param[in] firstSeriesIsValue If true, the first series represents a value.  If false, the first series
         *                               contains data labels.
         */
        void setFirstSeriesIsValue(bool firstSeriesIsValue = true);

        /**
         * Slot you can use to set baseline for bar charts.
         *
         * \param[in] newBaselineAxis The axis used as the basis for the bars.
         */
        void setBaselineAxis(AxisLocation newBaselineAxis);

        /**
         * Slot you can use to specify the supported plot series mode.
         *
         * \param[in] newSupportedPlotSeriesMode The new plot series mode to be supported.
         */
        void setSupportedPlotSeriesMode(PlotSeriesMode newSupportedPlotSeriesMode);

        /**
         * Slot you can trigger to set the supported spline types.
         *
         * \param[in] supportedSplineTypes The new set of supported spline types.
         */
        void setSupportedSplineTypes(const QSet<SplineType>& supportedSplineTypes);

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

    private slots:
        /**
         * Slot that is triggered when the user clicks on the first row is labels check box.
         *
         * \param[in] nowChecked If true, the checkbox is now checked.  If false, the check box is not checked.
         */
        void firstRowIsBarLabelsButtonClicked(bool nowChecked);

        /**
         * Slot that is triggered when the user changes the baseline axis.
         *
         * \param[in] index The index of the newly selected axis.
         */
        void baselineAxisChanged(int index);

        /**
         * Slot that is triggered when a cell is double clicked.
         *
         * \param[in] index The index of the double-clicked cell.
         */
        void cellDoubleClicked(const QModelIndex& index);

        /**
         * Slot that is triggered when the add data series button is clicked.
         */
        void insertButtonClicked();

        /**
         * Slot that is triggered when the remove data series button is clicked.
         */
        void removeButtonClicked();

        /**
         * Slot that is triggered when the move up button is clicked.
         */
        void moveUpButtonClicked();

        /**
         * Slot that is triggered when teh move down button is clicked.
         */
        void moveDownButtonClicked();

        /**
         * Slot that is triggered when the current cell changes.
         *
         * \param[in] current  The current cell.
         *
         * \param[in] previous The previous cell.
         */
        void currentChanged(const QModelIndex& current, const QModelIndex& previous);

        /**
         * Slot that is triggered when the selections are changed.
         *
         * \param[in] selected   The newly selected items.
         *
         * \param[in] deselected The newly deselected items.
         */
        void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    private:
        /**
         * Method that is called when the selection or current item changes.  The method updates button states.
         */
        void updateButtonStates();

        /**
         * Method that returns a reverse sorted list of selected rows.
         *
         * \return Returns a reverse sorted list of currently selected rows.
         */
        QList<unsigned> selectedRows() const;

        /**
         * Method that updates the model based on the allowed axis locations based on the current axis location style
         * and an optional baseline axis location.
         *
         * \param[in] axisLocationStyle The axis location style being imposed.
         *
         * \param[in] baselineAxis      The baseline axis.
         */
        void updateAllowedAxisLocations(AxisLocationStyle axisLocationStyle, AxisLocation baselineAxis);

        /**
         * The list of supported spline types.
         */
        QSet<SplineType> currentSupportedSplineTypes;

        /**
         * Checkbox used to indicate that the first data series holds labels.
         */
        QCheckBox* firstSeriesIsLabelsCheckBox;

        /**
         * Combo box used to indicate the category axis.
         */
        QComboBox* barsBaselineComboBox;

        /**
         * The main table view.
         */
        QTableView* dataSeriesTableView;

        /**
         * Insert button.
         */
        QPushButton* insertButton;

        /**
         * Remove button.
         */
        QPushButton* removeButton;

        /**
         * Move up button.
         */
        QPushButton* moveUpButton;

        /**
         * Move down button.
         */
        QPushButton* moveDownButton;
};

#endif
