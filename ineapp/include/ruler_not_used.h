/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Ruler class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RULER_H
#define RULER_H

#include <QWidget>
#include <QGraphicsView>
#include <QList>

class RulerScene;

/**
 * Class that displays a ruler.  Dimensions can be in US imperial or metric units depending on the locale and user
 * settings.
 */
class Ruler:public QGraphicsView {
    Q_OBJECT

    public:
        /**
         * Constuctor
         *
         * \param[in] parent The parent object that owns this widget.
         */
        explicit Ruler(QWidget* parent = Q_NULLPTR);

        ~Ruler() override;

        /**
         * Method that determines the current length of the rulers, in points.
         *
         * \return Returns the current ruler widths, in pixels.
         */
        QList<double> lengths() const;

        /**
         * Method that determines the ruler's current zoom factor.
         *
         * \return Returns the current zoom factor.   A value of 1 indicates 100%.  Objects on the screen should be
         *         scaled 1:1 with objects as printed on paper.
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

    public slots:
        /**
         * Slot that is triggered when the ruler horizontal length needs to be changed.
         *
         * \param[in] newRulerLengths   The length of the ruler associated with the current size, in points
         *
         * \param[in] newRulerPositions The new position for each ruler, in points.
         *
         * \param[in] newLeftEdge       The new left edge for the scene.
         *
         * \param[in] newRightEdge      The new right edge for the scene.
         */
        void setLengthsAndPositions(
            QList<double> newRulerLengths,
            QList<double> newRulerPositions,
            double        newLeftEdge,
            double        newRightEdge
        );

        /**
         * Method that sets the ruler's zoom factor.
         *
         * \param[in] newZoomFactor The desired zoom factor for the ruler.  A value of 1 indicates 100%.  Objects on the
         *                          screen should be scaled identically to paper.
         */
        void setZoomFactor(double newZoomFactor);

        /**
         * Method that adjusts the ruler's horizontal position.
         *
         * \param[in] newPosition A value indicating the new position to set the ruler to.
         */
        void setHorizontalPosition(int newPosition);

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

        /**
         * Slot used to make the cursor visible or invisible.
         *
         * \param[in] nowVisible If true, the cursor bar should be visible.  If false, the cursor bar should be
         *                       invisible.
         */
        void setCursorBarVisible(bool nowVisible);

        /**
         * Signal that can be used to force the extents of the underlying ruler scene.
         *
         * \param[out] leftEdge The new scene's left edge, in scene units.
         *
         * \param[out] rightEdge The new scene's right edge, in scene units.
         */
        void setSceneWidth(double leftEdge, double rightEdge);

    private slots:
        /**
         * Slot that is triggered whenever the ruler's height changes.
         *
         * \param[in] newHeight The new ruler height.
         */
        void setHeight(int newHeight);

    private:
        /**
         * Performs configuration needed by all constructors.
         */
        void configureWidget();

        /**
         * The underlying scene used to render the ruler.
         */
        RulerScene* rulerScene;
};

#endif
