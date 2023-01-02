/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotDataSourcesModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_DATA_SOURCES_MODEL_H
#define PLOT_DATA_SOURCES_MODEL_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <ld_variable_name.h>
#include <ld_chart_axis_format.h>
#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_plot_format.h>

#include "app_common.h"
#include "plot_data_sources_widget.h"

/**
 * Class that provides a table model for plot data sources.  The model is designed to be read-only.  The model should
 * be modified by calling the various accessor methods.
 */
class APP_PUBLIC_API PlotDataSourcesModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Enumeration of contents value types.
         */
        enum class ContentsType {
            /**
             * Indicates an invalid or unknown contents type.
             */
            INVALID,

            /**
             * Indicates a data source.
             */
            DATA_SOURCE_NAME,

            /**
             * Indicates an axis location.
             */
            AXIS_LOCATION,

            /**
             * Indicates a text label.
             */
            LABEL,

            /**
             * Indicates a plot series.
             */
            PLOT_SERIES
        };

        /**
         * Enumeration of supported axis positions.
         */
        typedef PlotDataSourcesWidget::AxisLocation AxisLocation;

        /**
         * Enumeration of supported plot series modes.
         */
        typedef PlotDataSourcesWidget::PlotSeriesMode PlotSeriesMode;

        /**
         * Enumeration of supported axis location styles.
         */
        typedef PlotDataSourcesWidget::AxisLocationStyle AxisLocationStyle;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        PlotDataSourcesModel(QObject* parent = Q_NULLPTR);

        ~PlotDataSourcesModel() override;

        /**
         * Method you can use to determine if the model's current contents are valid.
         *
         * \return Returns true if the model's contents are valid.  Returns false if the model's contents are invalid.
         */
        bool isValid() const;

        /**
         * Method that provides the number of columns per source.
         *
         * \return Returns the number of columns per data source.
         */
        unsigned columnsPerDataSource() const;

        /**
         * Method you can use to determine the type of data stored in a given location.
         *
         * \param[in] columnIndex The index to be queried.
         *
         * \return Returns the type of data stored in the requested location.
         */
        ContentsType contentsType(unsigned columnIndex) const;

        /**
         * Method you can use to determine the type of data stored in a given location.
         *
         * \param[in] index The index to be queried.
         *
         * \return Returns the type of data stored in the requested location.
         */
        ContentsType contentsType(const QModelIndex& index) const;

        /**
         * Method you can use to determine the source index referenced by a given column.
         *
         * \param[in] columnIndex The index to be queried.
         *
         * \return Returns the source index.  A value of static_cast<unsigned>(-1) is returned if the index does not
         *         point to a specific data source.
         */
        unsigned sourceIndex(unsigned columnIndex) const;

        /**
         * Method you can use to determine the source index referenced by a given column.
         *
         * \param[in] index The index to be queried.
         *
         * \return Returns the series index.  A value of static_cast<unsigned>(-1) is returned if the index does not
         *         point to a specific data source.
         */
        unsigned sourceIndex(const QModelIndex& index) const;

        /**
         * Method used by the view to obtain the header data to be displayed.
         *
         * \param[in] section     The zero based section number to obtain data for.
         *
         * \param[in] orientation The orientation of the header.
         *
         * \param[in] role        The requested data role.
         *
         * \return Returns a variant holding the requested header data.
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        /**
         * Method used by the view to obtain the data to be displayed.
         *
         * \param[in] index The index into the model used to locate the data.
         *
         * \param[in] role  The display role for the data.
         *
         * \return Returns a variant type holding the data to be displayed.
         */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        /**
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method that is called to insert a row.
         *
         * \param[in] row         The zero based index where the new row should be inserted.
         *
         * \param[in] parentIndex The index into the parent in the tree.  This value is ignored.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertRow(int row, const QModelIndex& parentIndex = QModelIndex());

        /**
         * Method that is called to remove one or more rows.
         *
         * \param[in] row    The row to be removed.
         *
         * \param[in] parent The model index of the parent.
         *
         * \return Returns true on success, returns false on error.
         */
        bool removeRow(int row, const QModelIndex& parent = QModelIndex());

        /**
         * Method that is called to move rows up.
         *
         * \param[in] rows The list of rows to be moved up.
         *
         * \return Returns true on success, returns false on error.
         */
        bool moveRowsUp(const QList<unsigned>& rows);

        /**
         * Method that is called to move rows down.
         *
         * \param[in] rows The list of rows to be moved down.
         *
         * \return Returns true on success, returns false on error.
         */
        bool moveRowsDown(const QList<unsigned>& rows);

        /**
         * Method you can use to determine the current number of data sources per data series.
         *
         * \return Returns the current number of data sources per data series.
         */
        unsigned dataSourcesPerDataSeries() const;

        /**
         * Method you can use to determine the current minimum number of allowed data series.
         *
         * \return Returns the current minimum number of allowed data series.
         */
        unsigned minimumNumberDataSeries() const;

        /**
         * Method you can use to determine the current maximum number of allowed data series.
         *
         * \return Returns the current maximum number of allowed data series.
         */
        unsigned maximumNumberDataSeries() const;

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
         * Method you can use to determine the currently supported plot series modes.
         *
         * \return Returns the currently supported plot series mode.
         */
        PlotSeriesMode supportedPlotSeriesMode() const;

        /**
         * Method you can use to obtain a list of all the data sources.
         *
         * \return Returns a list of all the data sources.  Paired sources will be presented in even/odd pairs.
         */
        const QList<Ld::VariableName>& dataSources() const;

        /**
         * Method you can use to obtain a single data source.
         *
         * \param[in] seriesIndex The zero based data series of interest.
         *
         * \param[in] sourceIndex The zero based data source index within the data series.
         */
        const Ld::VariableName& dataSource(unsigned seriesIndex, unsigned sourceIndex) const;

        /**
         * Method you can use to determine the axis assigned to each data source.
         *
         * \return Returns a list of axis assigned to each data source.
         */
        const QList<AxisLocation>& axisLocations() const;

        /**
         * Method you can use to determine the axis assigned to a specific data source.
         *
         * \param[in] seriesIndex The zero based data series of interest.
         *
         * \param[in] sourceIndex The zero based data source index within the data series.
         *
         * \return Returns a list of axis assigned to each data source.
         */
        AxisLocation axisLocation(unsigned seriesIndex, unsigned sourceIndex) const;

        /**
         * Method you can use to determine the chart line style and markers.
         *
         * \return Returns the list of chart line styles and markers for each data source or pair of sources.
         */
        const QList<Ld::PlotSeries>& plotSeries() const;

        /**
         * Method you can use to determine the chart line style and markers for a specifiec series.
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

        /**
         * Method you can use to generate a list of default axis values for each data source.
         *
         * \param[in]  axisLocationStyle The axis location style.
         *
         * \return Returns a list of axis location values aligned to the axis location style.
         */
        static QList<AxisLocation> defaultAxisLocationsForStyle(AxisLocationStyle axisLocationStyle);

        /**
         * Method you can use to determine if the first row holds the bar labels.
         *
         * \return Returns true if the first row contains bar labels.  Returns false if the first row contains a data
         *         series.
         */
        bool firstRowIsBarLabels() const;

        /**
         * Method you can use to determine if the first row holds a data series.
         *
         * \return Returns true if the first row is a data series.  Returns false if the first row is the bar labels.
         */
        bool firstRowIsDataSeries() const;

        /**
         * Method you can use to obtain a list of allowed axis locations.
         *
         * \param[in] sourceIndex The zero based index of the source.
         *
         * \return Returns the list of allowed axis locations for a given source.
         */
        QSet<AxisLocation> allowedAxisLocations(unsigned sourceIndex) const;

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
         * Slot you can use to specify the supported plot series modes.
         *
         * \param[in] newSupportedPlotSeriesMode The new plot series mode to be supported.
         */
        void setSupportedPlotSeriesMode(PlotSeriesMode newSupportedPlotSeriesMode);

        /**
         * Method you can use to set the data sources and related structures.  This method will also calculate the
         * number of data sources per data series based on the supplied values.
         *
         * \param[in] newDataSources      The new list of data sources.
         *
         * \param[in] newAxisLocations    A list of axis locations for each data source.  The number of elements must
         *                                match the number of data sources.
         *
         * \param[in] newPlotSeries       The plot series values for every data source or pair of data sources.
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
         * \param[in] sourceIndex   The zero based index of the source within the series.
         *
         * \param[in] newDataSource The new list of data source.
         */
        void setDataSource(unsigned seriesIndex, unsigned sourceIndex, const Ld::VariableName& newDataSource);

        /**
         * Method you can use to set the axis to apply to a particular data source.
         *
         * \param[in] seriesIndex   The zero based index of the data series of interest.
         *
         * \param[in] sourceIndex   The zero based index of the source within the series.
         *
         * \param[in] newAxisLocation The axis location to apply to the data source.
         */
        void setAxisLocation(unsigned seriesIndex, unsigned sourceIndex, AxisLocation newAxisLocation);

        /**
         * Method you can use to set the plot series for a particular data source or pair of data sources.
         *
         * \param[in] seriesIndex   The zero based index for the plot series to be updated.
         *
         * \param[in] newPlotSeries The particular plot series to be updated.
         */
        void setPlotSeries(unsigned seriesIndex, const Ld::PlotSeries& newPlotSeries);

        /**
         * Method you can use to set the plot series label for a particular data series.
         *
         * \param[in] seriesIndex     The zero based index of the plotseries of interest.
         *
         * \param[in] plotSeriesLabel the label to assign to the plot series.
         */
        void setPlotSeriesLabel(unsigned seriesIndex, const QString& plotSeriesLabel);

        /**
         * Method you can use to set the first row as the source of plot categories.
         *
         * \param[in] nowFirstRowIsBarLabels If true, the first row will be presented as the source of categories.
         *                                   If false, the first row will be treated as another data series.
         */
        void setFirstRowIsBarLabels(bool nowFirstRowIsBarLabels = true);

        /**
         * Method you can use to set the first row as a normal data series.
         *
         * \param[in] nowFirstRowIsDataSeries If true, the first row will be presented as a normal data series.  If
         *                                    false, the first row will be treated as the source of categories.
         */
        void setFirstRowIsDataSeries(bool nowFirstRowIsDataSeries = true);

        /**
         * Method you can use to specify the list of allowed axis locations for a given source.
         *
         * \param[in] sourceIndex      The zero based index of the source.
         *
         * \param[in] allowedLocations The list of allowed locations.
         */
        void setAllowedAxisLocations(unsigned sourceIndex, const QSet<AxisLocation>& allowedLocations);

    private:
        /**
         * A dummy variant value used when the row index is invalid.
         */
        static const Model::Variant dummyVariant;

        /**
         * Method that calculates the header data string for a specific header section.
         *
         * \param[in] section The section number we need the header string for.
         *
         * \return Returns the header string for the requested section.
         */
        QString headerString(unsigned section) const;

        /**
         * Method that calculates the font used for any given column
         *
         * \param[in] columnIndex The zero based column index.
         *
         * \return Returns the font to be used for the specified column.
         */
        QFont fontForColumn(unsigned columnIndex) const;

        /**
         * Method that removes rows, saving off the row contents.
         *
         * \param[in]     rowList        The list of rows to be removed.
         *
         * \param[in,out] dataSources      The list of data sources to be inserted.
         *
         * \param[in,out] axisLocations    The list of axis locations to be inserted.
         *
         * \param[in,out] plotSeries       The list of plot series to be inserted.
         *
         * \param[in,out] plotSeriesLabels The list of plot series labels to be inserted.
         */
        void removeAndSaveRows(
            const QList<unsigned>&   rowList,
            QList<Ld::VariableName>& dataSources,
            QList<AxisLocation>&     axisLocations,
            QList<Ld::PlotSeries>&   plotSeries,
            QStringList&             plotSeriesLabels
        );

        /**
         * Method that inserts rows starting at a specified index.
         *
         * \param[in] insertionPoint   The row index where the insertions should be performed.
         *
         * \param[in] dataSources      The list of data sources to be inserted.
         *
         * \param[in] axisLocations    The list of axis locations to be inserted.
         *
         * \param[in] plotSeries       The list of plot series to be inserted.
         *
         * \param[in] plotSeriesLabels The list of plot series labels to be inserted.
         */
        void insertRowData(
            unsigned                       insertionPoint,
            const QList<Ld::VariableName>& dataSources,
            const QList<AxisLocation>&     axisLocations,
            const QList<Ld::PlotSeries>&   plotSeries,
            const QStringList&             plotSeriesLabels
        );

        /**
         * Value indicating the number of data sources assigned to any given data series.
         */
        unsigned currentNumberDataSourcesPerDataSeries;

        /**
         * The current minimum number of allowed data sources.
         */
        unsigned currentMinimumNumberDataSeries;

        /**
         * The current maximum number of allowed data sources.
         */
        unsigned currentMaximumNumberDataSeries;

        /**
         * Flag indicating that the first row should contain the bar labels.
         */
        bool currentFirstRowIsBarLabels;

        /**
         * Value indicating the axis location style.
         */
        AxisLocationStyle currentAxisLocationStyle;

        /**
         * The currently supported plot series mode.
         */
        PlotSeriesMode currentlySupportedPlotSeriesMode;

        /**
         * Value holding the current list of data sources.
         */
        QList<Ld::VariableName> currentDataSources;

        /**
         * Value holding the list of axis locations tied to each data source.
         */
        QList<AxisLocation> currentAxisLocations;

        /**
         * Value holding the plot series information for each series.
         */
        QList<Ld::PlotSeries> currentPlotSeries;

        /**
         * List holding the plot series labels.
         */
        QStringList currentPlotSeriesLabels;

        /**
         * List of allowed axis locations by source index.
         */
        QList<QSet<AxisLocation>> currentAllowedAxisLocations;
};

#endif
