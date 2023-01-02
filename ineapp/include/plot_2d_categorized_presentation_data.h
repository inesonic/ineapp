/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Plot2DCategorizedPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_2D_CATEGORIZED_PRESENTATION_DATA_H
#define PLOT_2D_CATEGORIZED_PRESENTATION_DATA_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHash>
#include <QTimer>
#include <QVariant>
#include <QStringList>
#include <QVector>

#include <ld_plot_element.h>
#include <ld_plot_series.h>

#include "app_common.h"
#include "plot_wrapped_data_presentation_data.h"

class QGraphicsItem;

namespace QtCharts {
    class QAbstractAxis;
    class QXYSeries;
    class QVXYModelMapper;
    class QChart;
};

namespace EQt {
    class GraphicsItem;
    class ChartItem;
}

namespace Model {
    class Variant;
    class Complex;
}

namespace Ld {
    class CalculatedValue;
}

/**
 * Pure virtual base class you can use to manage bar, pie, and donut plots.
 */
class APP_PUBLIC_API Plot2DCategorizedPresentationData:public PlotWrappedDataPresentationData {
    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The graphics item to be managed.  This instance must be derived from both
         *                         EQt::GraphicsItem and QtCharts::QChart
         */
        Plot2DCategorizedPresentationData(EQt::GraphicsItem* graphicsItem = Q_NULLPTR);

        ~Plot2DCategorizedPresentationData() override;

    protected:
        /**
         * Class used internally to track series data.
         */
        class SeriesData:public PlotWrappedDataPresentationData::SeriesData {
            public:
                SeriesData();

                /**
                 * Constructor
                 *
                 * \param[in] axisScale      The scaling being applied to this axis.
                 *
                 * \param[in] dataSourceName The name of the data source feeding this series.
                 *
                 * \param[in] seriesLabel    The label associated with this data source.
                 */
                SeriesData(AxisScale axisScale, const Ld::VariableName& dataSourceName, const QString& seriesLabel);

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesData(const SeriesData& other);

                ~SeriesData() override;

                /**
                 * Method you can use to obtain the axis scale.
                 *
                 * \return Returns the axis scale.
                 */
                AxisScale axisScale() const {
                    return currentAxisScale;
                }

                /**
                 * Method you can use to obtain the data source name.
                 *
                 * \return Returns The data source name.
                 */
                const Ld::VariableName dataSourceName() const {
                    return currentDataSourceName;
                }

                /**
                 * Method you can use to obtain the series label.
                 *
                 * \return Returns the series label.
                 */
                const QString& seriesLabel() const {
                    return currentSeriesLabel;
                }

                /**
                 * Method you can use to obtain the current calculated value.
                 *
                 * \return Returns the current calculated value.
                 */
                const Ld::CalculatedValue& calculatedValue() const {
                    return currentCalculatedValue;
                }

                /**
                 * Method you can use to set the calculated value for a data source.
                 *
                 * \param[in] sourceIndex        The zero based source index.
                 *
                 * \param[in] newCalculatedValue The newly calculated value.
                 */
                void setCalculatedValue(unsigned sourceIndex, const Ld::CalculatedValue& newCalculatedValue) override;

                /**
                 * Method you can use to clear the calculated values.
                 */
                void calculatedValuesCleared() override;

                /**
                 * Method you can use to convert a variant to a list of strings.
                 *
                 * \param[in,out] ok An optional pointer to a boolean value.  On success, the value will be set to
                 *                   true.  On failure, the value will be set to false.
                 *
                 * \return Returns a string list representation of the variant.
                 */
                QStringList toStringList(bool* ok = Q_NULLPTR) const;

                /**
                 * Method you can use to convert a variant to a vector of values.
                 *
                 * \param[in]     seriesMinMax An optional \ref PlotPresentationData::SeriesMinMax to be updated.
                 *
                 * \param[in,out] ok           An optional pointer to a boolean value.  On success, the value will be
                 *                             set to true.   On failure, the value will be set to false.
                 */
                QVector<double> toVector(SeriesMinMax* seriesMinMax = Q_NULLPTR, bool* ok = Q_NULLPTR) const;

                /**
                 * Assignment operator.
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesData& operator=(const SeriesData& other);

            private:
                /**
                 * Method you can use to convert a variant to a vector of values.
                 *
                 * \param[in]     variant      The variant to be converted.
                 *
                 * \param[in]     axisScale    The axis scaling.  This setting is used to adjust values to match the
                 *                             selected axis.
                 *
                 * \param[in]     seriesMinMax An optional \ref PlotPresentationData::SeriesMinMax to be updated.
                 *
                 * \param[in,out] ok           An optional pointer to a boolean value.  On success, the value will be
                 *                             set to true.   On failure, the value will be set to false.
                 */
                static QVector<double> variantToVector(
                    const Model::Variant& variant,
                    AxisScale             axisScale,
                    SeriesMinMax*         seriesMinMax = Q_NULLPTR,
                    bool*                 ok = Q_NULLPTR
                );

                /**
                 * Method you can use to convert a variant to a string.
                 *
                 * \param[in]     variant The variant to convert to a string.
                 *
                 * \param[in,out] ok      An optional pointer to a boolean value.  On success, the value will be set to true.
                 *                        On failure, the value will be set to false.
                 *
                 * \return Returns a string representation of the variant.
                 */
                static QString variantToString(const Model::Variant& variant, bool* ok = Q_NULLPTR);

                /**
                 * Method you can use to convert a complex value to a string.
                 *
                 * \param[in] value The value to be converted.
                 *
                 * \return Returns a string representation of the complex value.
                 */
                static QString complexToString(const Model::Complex& value);

                /**
                 * The axis scale.
                 */
                AxisScale currentAxisScale;

                /**
                 * The data source.
                 */
                Ld::VariableName currentDataSourceName;

                /**
                 * The series label.
                 */
                QString currentSeriesLabel;

                /**
                 * The calculated value for the series.
                 */
                Ld::CalculatedValue currentCalculatedValue;
        };

        /**
         * Method you can use to obtain a default color by index.
         *
         * \param[in] index The index to get a default color for.
         *
         * \return Returns the color tied to the index.
         */
        static const QColor& defaultColor(unsigned index);

        /**
         * Method you can use to conditionally add a value to a label.
         *
         * \param[in] label The label to be modified.
         *
         * \param[in] value The value to be added to the label.
         */
        static QString addValueToLabel(const QString& label, double value);

    protected:
        /**
         * The number of default colors.
         */
        static constexpr unsigned numberColors = 9;

        /**
         * Table of default colors.
         */
        static const QColor defaultColors[numberColors];
};

#endif
