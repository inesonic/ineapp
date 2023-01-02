/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RulerScene class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RULER_SCENE_H
#define RULER_SCENE_H

#include <QObject>
#include <QSize>
#include <QPaintEvent>
#include <QVector>
#include <QPen>
#include <QList>
#include <QGraphicsScene>

class QGraphicsLineItem;
class QGraphicsRectItem;

/**
 * Class the manages the underlying scene used to display a ruler.  The associated \ref Ruler class provides the view
 * into the scene.
 */
class RulerScene:public QGraphicsScene {
    Q_OBJECT

    public:
        /**
         * Constuctor
         *
         * \param[in] parent  The parent object that owns this widget.
         */
        RulerScene(QObject* parent = Q_NULLPTR);

        ~RulerScene() override;

        /**
         * Method that determines the current length of the rulers, in points.
         *
         * \return Returns the current ruler widths, in pixels.
         */
        QList<double> lengths() const;

        /**
         * Method that determines the current zoom factor for the ruler.
         *
         * Note that, to property manage text, we perform zooming in the scene rather than using the built-in
         * transformation functions provided with QGraphicsView.
         *
         * \return Returns the current ruler zoom factor.  A value of 1 indicates 1:1 scaling.
         */
        double zoomFactor() const;

        /**
         * Determines if imperial or metric units are being used.
         *
         * \return Returns true if imperial units are being used.  Returns false if metric units are being used.
         */
        bool imperialUnits() const;

        /**
         * Determines if metric or imperial units are being used.
         *
         * \return Returns true if metric units are being used.  Returns false if imperial units are being used.
         */
        inline bool metricUnits() const {
            return !imperialUnits();
        }

        /**
         * Determines if the cursor bar is currently visible or hidden.
         *
         * \return Returns true if the cursor bar is visible. Returns false if the cursor bar is hidden.
         */
        bool cursorBarIsVisible() const;

        /**
         * Determines if the cursor bar is currently hidden or visible.
         *
         * \return Returns true if the cursor bar is hidden. Returns false if the cursor bar is visible.
         */
        inline bool cursorBarIsHidden() const {
            return !cursorBarIsVisible();
        }

        /**
         * Determines the ruler's current height, in pixels.
         *
         * \return Returns the ruler's current height, in pixels.
         */
        int height() const;

        /**
         * Determines optional padding to be applied to the right side of the ruler to account for scrollbars in the
         * \ref Editor view.
         *
         * \return Returns the padding to apply to the right side of the ruler to account for scroll bars in the editor
         *         window.
         */
        unsigned rightSidePadding() const;

    signals:
        /**
         * Signal that is emitted when the height of the ruler changes.
         *
         * \param[out] newHeight The new ruler height, in pixels.
         */
        void heightChanged(int newHeight);

    public slots:
        /**
         * Slot that is triggered when the ruler horizontal length needs to be changed.
         *
         * \param[in] newRulerLengths   The length of the ruler associated with the current size, in points
         *
         * \param[in] newRulerPositions The new position for each ruler, in points.
         *
         * \param[in] newLeftEdge       The new scene left edge, in scene units.
         *
         * \param[in] newRightEdge      The new scene right edge, in scene units.
         */
        void setLengthsAndPositions(
            QList<double> newRulerLengths,
            QList<double> newRulerPositions,
            double        newLeftEdge,
            double        newRightEdge
        );

        /**
         * Slot that is triggered when the ruler's zoom factor needs to be changed.
         *
         * \param[in] newZoomFactor The new zoom factor to apply to the ruler.
         */
        void setZoomFactor(double newZoomFactor);

        /**
         * Slot that is triggered when the display units should be changed.
         *
         * \param[in] nowUsingImperialUnits If true, imperial units will be used.  If false, metric units will be used.
         */
        void setImperialUnits(bool nowUsingImperialUnits = true);

        /**
         * Slot that is triggered when the display units should be changed.
         *
         * \param[in] nowUsingMetricUnits If true, metric units will be used.  If false, imperial units will be used.
         */
        inline void setMetricUnits(bool nowUsingMetricUnits = true) {
            setImperialUnits(!nowUsingMetricUnits);
        }

        /**
         * Slot that can be used to show or hide the cursor bar.
         *
         * \param[in] nowVisible If true, the cursor bar will be made visible.  If false, the cursor bar will be hidden.
         */
        void setCursorBarVisible(bool nowVisible = true);

        /**
         * Slot that can be used to hide or show the cursor bar.
         *
         * \param[in] nowHidden If true, the cursor bar will be hidden.  If false, the cursor bar will be make visible.
         */
        inline void setCursorBarHidden(bool nowHidden = true) {
            setCursorBarVisible(!nowHidden);
        }

        /**
         * Slot used to set the cursor position.
         *
         * \param[in] column      The column of interest.
         *
         * \param[in] newPosition The new cursor position, in points.
         */
        void setCursorPointPosition(unsigned column, double newPosition);

        /**
         * Slot used to set the cursor horizontal position, in scene coordinates.
         *
         * \param[in] newHorizontalPosition The new cursor position, in scene coordinates.
         */
        void setCursorSceneCoordinatePosition(double newHorizontalPosition);

    private:
        /**
         * Structure containing vertical position information.
         */
        struct VerticalPositions {
            /**
             * The location of the top of the labels.
             */
            unsigned fontTop;

            /**
             * The location of the bottom of the labels.
             */
            unsigned fontBottom;

            /**
             * Value used to determine which line start value to use.
             *
             * - For imperial units, this value will be set to RulerScene::imperialDivision.
             * - For metric, this value will be set to RulerScene::metricTicks.
             */
            unsigned divisor;

            /**
             * Vector holding line start positions for each tick mark.
             *
             * The index into the array is calculated by taking the remainder of the tick number divided by the divisor.
             */
            QVector<unsigned> lineStarts;
        };

        /**
         * The default ruler length, in points.
         */
        static constexpr double defaultLength = 612;

        /**
         * The default zoom factor.
         */
        static constexpr double defaultZoomFactor = 1;

        /**
         * The maximum allowed label font size.
         */
        static constexpr unsigned maximumFontSize = 10;

        /**
         * The minimum allowed label font size.
         */
        static constexpr unsigned minimumFontSize = 6;

        /**
         * Spacing to apply above and below text, in pixels.
         */
        static constexpr unsigned fontSpacing = 4;

        /**
         * The number of different size ticks used for imperial units.
         */
        static constexpr unsigned numberImperialTicks = 5;

        /**
         * Used to calculate tick sizes.
         */
        static constexpr unsigned imperialDivisor = 1 << (numberImperialTicks - 1);

        /**
         * Indicates the number of minor ticks per major tick with metric units.
         */
        static constexpr unsigned metricDivisor = 10;

        /**
         * Performs configuration after dimensions are updated.
         */
        void redraw();

        /**
         * Calculates an optimal font based on the curent settings.
         *
         * \param[in]  lastHorizontalUnit The value of the highest possible tick on the ruler.
         *
         * \param[in]  pixelsPerUnit      The number of pixels between major units (1's) on the ruler.
         *
         * \param[out] numberSkipInterval Returns the distance that should be applied between labeled major ticks.
         */
        QFont optimalFont(double lastHorizontalUnit, double pixelsPerUnit, unsigned& numberSkipInterval);

        /**
         * Calculates vertical font and line positions for imperial units.
         *
         * \param[in] font The font used as a basis for the calculations.
         */
        VerticalPositions calculateImperialVerticalPositions(const QFont& font);

        /**
         * Calculates vertical font and line positions for metric units.
         *
         * \param[in] font The font used as a basis for the calculations.
         */
        VerticalPositions calculateMetricVerticalPositions(const QFont& font);

        /**
         * The current ruler lengths, in points.
         */
        QList<double> currentLengths;

        /**
         * The current ruler left edge positions, in points.
         */
        QList<double> currentPositions;

        /**
         * The current zoom factor to apply to the X axis of the ruler.
         */
        double currentZoomFactor;

        /**
         * Determines if we're using imperial or metric units.
         */
        bool currentImperialUnits;

        /**
         * The line used to indicate the current cursor position.
         */
        QGraphicsLineItem* cursorLine;

        /**
         * The number of screen pixels per unit.
         */
        double pixelsPerUnit;

        /**
         * The current height that should be enforced for the ruler.
         */
        int currentHeight;
};

#endif
