/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotEngine class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_ENGINE_H
#define PLOT_ENGINE_H

#include <QCoreApplication>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QSet>
#include <QString>
#include <QIcon>

#include <util_hash_functions.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>
#include <ld_plot_series.h>

#include "app_common.h"
#include "scene_units.h"

class QGraphicsItem;

class PlotPresentationData;

/**
 * This pure virtual class provides an engine use to define new plotting styles.  To define a new plot style, overload
 * this class with the functions necessary to manage the new plotting style and then register the class instance with
 * the static registration methods.
 */
class APP_PUBLIC_API PlotEngine:protected SceneUnits {
    Q_DECLARE_TR_FUNCTIONS(PlotEngine)

    public:
        /**
         * Enumeration of ways plot series can be shown.
         */
        enum class PlotSeriesMode {
            /**
             * Indicates the plot series mode is fixed/unchangeable based on the type of axis and plot.
             */
            FIXED,

            /**
             * Indicates plot series can be shown as lines.
             */
            LINES,

            /**
             * Indicates plot series can be shown as markers.
             */
            MARKERS,

            /**
             * Indicates plot series can be shown as lines and/or markers.
             */
            LINES_AND_MARKERS,

            /**
             * Indicates plot series are solid colors.  This is used for bar charts, pie charts, and donut charts.
             */
            COLOR_REGION,

            /**
             * Indicates plot series are shown as gradients.  This is used for heat charts.
             */
            GRADIENT
        };

        /**
         * Enumeration of scaling control modes.
         */
        enum class ScalingControlsMode {
            /**
             * Indicates scaling can not be controlled.
             */
            NO_SCALING_CONTROLS,

            /**
             * Indicates the plot only supports manual scaling only.
             */
            MANUAL_SCALING_CONTROLS,

            /**
             * Indicates the plot style supports both manual and automatic scaling.
             */
            ALL_SCALING_CONTROLS
        };

        /**
         * Enumeration of axis location types.
         */
        enum class AxisLocationStyle {
            /**
             * Indicates axis are based on plot borders (bottom, left, right, top)
             */
            PLOT_BORDERS,

            /**
             * Indicates axis are based on a category label.  When specified, a global position for the category
             * labels is provided which the determine which axis to use for any given data series.
             */
            CATEGORIZED,

            /**
             * Indicates the plot does not use distinct axis.  Example plots that follow this approach would be
             * pie charts.
             */
            UNCATEGORIZED,

            /**
             * Indicates the plot axis are fixed.
             */
            FIXED,

            /**
             * Indicates axis are based on cartesian coordinates in a 2-space.
             */
            CARTESIAN_2D,

            /**
             * Indicates axis are based on cartesian coordinates in a 3-space.
             */
            CARTESIAN_3D,

            /**
             * Indicates axis are based on polar coordinates in a 2-dimensional space,
             */
            POLAR_2D,

            /**
             * Indicates axis are based on polar coordinates in a 3-dimensionsl space.
             */
            POLAR_3D,

            /**
             * Indicates data sets represent values in an RGB color space.
             */
            RGB_COLOR_SPACE,

            /**
             * Indicates data sets represent values in a CMYK color space.
             */
            CMYK_COLOR_SPACE,

            /**
             * Indicates data sets represent values in a grayscale color space.
             */
            GRAYSCALE_COLOR_SPACE
        };

        /**
         * Enumeration of supported spline types.
         */
        typedef Ld::PlotSeries::SplineType SplineType;

        /**
         * Enumeration of supported axis scaling modes.
         */
        typedef Ld::ChartAxisFormat::AxisScale AxisScale;

        /**
         * Enumeration of supported axis locations.
         */
        typedef Ld::PlotFormat::AxisLocation AxisLocation;

        PlotEngine();

        virtual ~PlotEngine();

        /**
         * Method you should call to register a new plotting style.
         *
         * \param[in] plotEngine The new engine to be registered.  The engine is expected to be statically defined and
         *                       will not be destroyed.
         *
         * \return Returns true on success, returns false if the plotting engine could not be added to the database.
         */
        static bool registerInstance(const PlotEngine* plotEngine);

        /**
         * Method you can use to obtain a plotting engine by name.
         *
         * \param[in] plotStyle The name of the plot style.
         *
         * \return Returns a pointer to the plot engine instance.  A null pointer is returned if the provided name is
         *         invalid.
         */
        static const PlotEngine* byName(const QString& plotStyle);

        /**
         * Method you can use to obtain a list of every plotting engine.
         *
         * \return Returns a reference to the internally managed list of plotting engines.
         */
        static const QList<const PlotEngine*>& engines();

        /**
         * Method that provides an internal name for this plot style.
         *
         * \return Returns the internal name for this plot engine.
         */
        virtual QString name() const = 0;

        /**
         * Method that provides a very short description for this plot style.
         *
         * \return Returns a very short description for this plot style.  The value is intended to be used in dialogs.
         */
        virtual QString shortDescription() const = 0;

        /**
         * Method that provides a description suitable for use in tool-tips.
         *
         * \return Returns a description for this plot style.  The value is intended to be used in dialogs for
         *         tool tips.
         */
        virtual QString toolTipDescription() const = 0;

        /**
         * Method that provides a description suitable for use in what's this fly-over text.
         *
         * \return Returns a description for this plot style.  The value is intended to be used in dialogs for
         *         "what's this" text.
         */
        virtual QString whatsThisDescription() const = 0;

        /**
         * Method that is called to obtain an icon for this plot style.
         *
         * \return Returns an icon used to define the plotting style.
         */
        virtual QIcon icon() const = 0;

        /**
         * Method that is called to obtain the supported plot series mode.
         *
         * \return Returns the supported plot series mode.
         */
        virtual PlotSeriesMode supportedPlotSeriesMode() const = 0;

        /**
         * Method that is called to obtain a set of supported spline types.
         *
         * \return Returns a set of supported spline types.
         */
        virtual QSet<SplineType> supportedSplineTypes() const = 0;

        /**
         * Method that is called to determine the number of data sources per data series.
         *
         * \return Returns the number of data sources required to represent a single data series.
         */
        virtual unsigned dataSourcesPerSeries() const = 0;

        /**
         * Method that is called to determine the axis location style.
         *
         * \return Returns the axis location style.
         */
        virtual AxisLocationStyle axisLocationStyle() const = 0;

        /**
         * Method that is called to determine the supported axis scaling modes.
         *
         * \param[in] axisLocation The axis location to query the supported styles for.
         *
         * \return Returns a set of supported axis scaling modes.
         */
        virtual QSet<AxisScale> supportedAxisScales(AxisLocation axisLocation) const = 0;

        /**
         * Method that is called to determine the desired scaling controls mode.
         *
         * \return Returns the desired scaling controls mode.
         */
        virtual ScalingControlsMode supportedScalingControlsMode() const = 0;

        /**
         * Method that is called to determine the minimum number of allowed data series.
         *
         * \return Returns the minimum number of allowed data series.  The default implementation returns 1.
         */
        virtual unsigned minimumNumberAllowedDataSeries() const;

        /**
         * Method that is called to determine the maximum number of allowed data series.
         *
         * \return Returns the maximum number of allowed data series.  The default implementation returns the maximum
         *         value that can be represented in a signed integer.
         */
        virtual unsigned maximumNumberAllowedDataSeries() const;

        /**
         * Method that is called to determine if the active area background settings can be controlled.
         *
         * \return Returns true if the active area background settings can be changed.  Returns false if the active
         *         area background settings can-not be changed.  This version returns true.
         */
        virtual bool activeAreaBackgroundAdjustable() const;

        /**
         * Method that is called to determine if this plot style supports a legend.
         *
         * \return Returns true if this plot style supports a legend.  Returns false if this plot style does not
         *         support a legend.  The default implementation always returns true.
         */
        virtual bool supportsPlotLegend() const;

        /**
         * Method that is called to determine if this plot style has a controllable active plot area.
         *
         * \return Returns true if this plot style has a distinct/controllable active plot area.  Returns false if
         *         this plot style does not have a distinct controllable active plot area.  The default implemetnation
         *         returns true.
         */
        virtual bool hasDistinctActiveArea() const;

        /**
         * Method that is called to determine if this plot style includes a plot title.
         *
         * \return Returns true if this plot style has a plot title.  Returns false if this plot style does not have a
         *         plot title.  The default implementation returns true.
         */
        virtual bool supportsPlotTitle() const;

        /**
         * Method that is called to determine if this plot style has a viewable background area that can be controlled.
         *
         * \return Returns true if this plot style has a viewable and controllable background area.  Returns false if
         *         this plot style does not have a viewable and controllable background area.  The default
         *         implementation returns true.
         */
        virtual bool supportsControllableBackgroundArea() const;

        /**
         * Method that creates a new plot presentation data instance which includes the plot graphics item.
         *
         * \param[in] plotElement The plot element used to create the plot.
         *
         * \param[in] plotFormat  The plot format instance used to create the plot.
         *
         * \return Returns a newly created \ref PlotPresentationData
         */
        virtual PlotPresentationData* createPresentationData(
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const = 0;

        /**
         * Method that updates an existing plot presentation data instance.  The default implementation simply calls
         * \ref PlotEngine::createPresentationData, throwing away the current presentation data instance.
         *
         * \param[in] currentPresentationData The existing graphics item to be updated.
         *
         * \param[in] plotElement             The plot element used to create the plot.
         *
         * \param[in] plotFormat              The plot format instance used to create the plot.
         *
         * \return Note that the calling method is expected to destroy the old \ref PlotPresentationData instance if
         *         the new \ref PlotPresentationData instance is different.
         */
        virtual PlotPresentationData* updatePresentationData(
            PlotPresentationData*           currentPresentationData,
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const;

    private:
        /**
         * Internal map of plot engines by name.
         */
        static QMap<QString, const PlotEngine*> plotEnginesByName;

        /**
         * Internal list of plot engines.
         */
        static QList<const PlotEngine*> plotEngines;
};

/**
 * Function that calculates a hash of the PlotEngine::PlotSeriesModes enumeration.
 *
 * \param[in] value The value to be hashed.
 *
 * \param[in] seed  An optional seed to apply to the hash.
 *
 * \return Returns the hash calculated from the value and seed.
 */
Util::HashResult qHash(PlotEngine::PlotSeriesMode value, Util::HashSeed seed = 0);

#endif
