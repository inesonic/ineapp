/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotDataSourcesDelegate class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_DATA_SOURCES_DELEGATE_H
#define PLOT_DATA_SOURCES_DELEGATE_H

#include <QFont>
#include <QRect>
#include <QStyledItemDelegate>

#include "plot_data_sources_model.h"

namespace Ld {
    class VariableName;
    class PlotSeries;
}

class QObject;
class QWidget;
class QStringRef;
class QPainter;
class QStyleOptionViewItem;
class QAbstractItemModel;
class QModelIndex;

/**
 * Class that provides a delegate for rendering and editing the plot data sources table contents.
 */
class PlotDataSourcesDelegate:public QStyledItemDelegate {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        PlotDataSourcesDelegate(QObject* parent = Q_NULLPTR);

        ~PlotDataSourcesDelegate() override;

        /**
         * Method that calculates the size for the painted item.
         *
         * \param[in] option  The option used to control the size estimation.  The parameter provides the drawing
         *                    rectangle as well as parameters to indicate if the field is selected or enabled.
         *
         * \param[in] index   The index into the model indicating the location being sized.
         *
         * \return Returns the size needed by the delegate to fully display the item.
         */
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        /**
         * Method that performs painting of content.
         *
         * \param[in] painter The painter that the delegate should paint upon.
         *
         * \param[in] option  The option used to control the painting.  The parameter provides the drawing rectangle
         *                    as well as parameters to indicate if the field is selected or enabled.
         *
         * \param[in] index   The index into the model indicating the location being painted.
         */
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    private:
        /**
         * Method that converts an axis location to a text string.
         *
         * \param[in] axisLocationStyle
         *
         * \param[in] axisLocation
         *
         * \return Returns a string representation of the axis location.
         */
        static QString toString(
            PlotDataSourcesModel::AxisLocationStyle axisLocationStyle,
            PlotDataSourcesModel::AxisLocation      axisLocation
        );

        /**
         * Method that paints a data source name.
         *
         * \param[in] painter        The painter used to paint the cell.
         *
         * \param[in] option         Style options used to control painting.
         *
         * \param[in] index          The index to the cell to contain the name.
         *
         * \param[in] dataSourceName The data source name.
         */
        void paintDataSource(
            QPainter*                   painter,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index,
            const Ld::VariableName&     dataSourceName
        ) const;

        /**
         * Method that paints the axis location.
         *
         * \param[in] painter      The painter used to paint the cell.
         *
         * \param[in] option       Style options used to control painting.
         *
         * \param[in] index        The index to the cell to contain the name.
         *
         * \param[in] axisLocation The data source name.
         */
        void paintAxisLocation(
            QPainter*                          painter,
            const QStyleOptionViewItem&        option,
            const QModelIndex&                 index,
            PlotDataSourcesModel::AxisLocation axisLocation
        ) const;

        /**
         * Method that paints the series labels.
         *
         * \param[in] painter     The painter used to paint the cell.
         *
         * \param[in] option      Style options used to control painting.
         *
         * \param[in] index       The index to the cell to contain the name.
         *
         * \param[in] seriesLabel The series label to be presented.
         */
        void paintSeriesLabels(
            QPainter*                   painter,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index,
            const QString&              seriesLabel
        ) const;

        /**
         * Method that paints the plot series symbol.
         *
         * \param[in] painter    The painter used to paint the cell.
         *
         * \param[in] option     Style options used to control painting.
         *
         * \param[in] index      The index to the cell to contain the name.
         *
         * \param[in] plotSeries The data source name.
         */
        void paintPlotSeries(
            QPainter*                   painter,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index,
            const Ld::PlotSeries&       plotSeries
        ) const;
};

#endif
