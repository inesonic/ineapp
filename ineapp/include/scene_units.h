/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SceneUnits class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SCENE_UNITS_H
#define SCENE_UNITS_H

#include <QSize>
#include <QSizeF>
#include <QPointF>
#include <QRectF>
#include <QLineF>

#include "app_common.h"

/**
 * Base class that can be used to provide facilities to consistently convert from points to scene units.  A scene unit
 * is a single pixel on the screen when the document is rendered at a resolution of 1:1.
 */
class APP_PUBLIC_API SceneUnits {
    public:
        /**
         * Method that should be called whenever the screen DPI changes to update internally maintained scale factors.
         *
         * \param[in] physicalDpi The screen DPI settings in dots per inch.
         */
        static void update(const QSize& physicalDpi);

        /**
         * Method that calculates the number of pixels per point at a scaling factor of 1:1.  The value is used as a
         * conversion factor between points and scene units.
         *
         * \return Returns a QSizeF instance holding the number of pixels per point.
         */
        static const QSizeF& sceneUnitsPerPoint();

        /**
         * Method that scales a QSizeF structure from points to scene units.
         *
         * \param[in] size The size instance to be converted.
         *
         * \return The converted size instance.
         */
        static QSizeF toScene(const QSizeF& size);

        /**
         * Method that scales a QPointF structure from points to scene units.
         *
         * \param[in] point The point instance to be converted.
         *
         * \return The converted point instance.
         */
        static QPointF toScene(const QPointF& point);

        /**
         * Method that scales a rectangle from points to scene units.
         *
         * \param[in] rect The rectangle to be converted.
         *
         * \return The converted rectangle.
         */
        static QRectF toScene(const QRectF& rect);

        /**
         * Method that scales a QLineF structure from points to scene units.
         *
         * \param[in] line The line instance to be converted.
         *
         * \return The converted line instance.
         */
        static QLineF toScene(const QLineF& line);

        /**
         * Method that scales a QSizeF structure from scene units to points.
         *
         * \param[in] size The size instance to be converted.
         *
         * \return The converted size instance.
         */
        static QSizeF fromScene(const QSizeF& size);

        /**
         * Method that scales a QPointF structure from scene units to points.
         *
         * \param[in] point The point instance to be converted.
         *
         * \return The converted point instance.
         */
        static QPointF fromScene(const QPointF& point);

        /**
         * Method that scales a rectangle from scene units to points.
         *
         * \param[in] rect The rectangle to be converted.
         *
         * \return The converted rectangle.
         */
        static QRectF fromScene(const QRectF& rect);

        /**
         * Method that scales a QLineF structure from scene units to points.
         *
         * \param[in] line The line instance to be converted.
         *
         * \return The converted line instance.
         */
        static QLineF fromScene(const QLineF& line);

    private:
        /**
         * The number of scene units per point.
         */
        static QSizeF currentSceneUnitsPerPoint;
};

#endif
