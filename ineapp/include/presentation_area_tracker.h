/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationAreaTracker class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_AREA_TRACKER_H
#define PRESENTATION_AREA_TRACKER_H

#include <QObject>
#include <QList>
#include <QSizeF>

#include "app_common.h"

class QGraphicsItem;

class Presentation;

/**
 * You can use this class to maintain a list of presentation areas associated with a child presentation.  The class is
 * intended to be used to collect up presentation area information for use in operators that have fixed/immutable
 * size constraints.
 *
 * Presentation areas are assumed to always stack horizontally from left to right.
 */
class APP_PUBLIC_API PresentationAreaTracker {
    public:
        /**
         * Class used to track information tied to each presentation area.
         */
        class APP_PUBLIC_API Entry {
            friend class PresentationAreaTracker;

            private:
                /**
                 * Private constructor
                 *
                 * \param[in] presentation A pointer to the underlying presentation.
                 *
                 * \param[in] areaId       The reported area ID.
                 *
                 * \param[in] areaSize     The reported area size, in scene units.
                 *
                 * \param[in] ascent       The reported area ascent.
                 */
                Entry(
                    Presentation* presentation,
                    unsigned long areaId,
                    const QSizeF& areaSize,
                    float         ascent
                );

            public:
                Entry();

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied.
                 */
                Entry(const Entry& other);

                ~Entry();

                /**
                 * Method you can use to determine if this instance is valid.
                 *
                 * \return Returns true if the instance is valid.  Returns false if the instance is invalid.
                 */
                bool isValid() const;

                /**
                 * Method you can use to determine if this instance is invalid.
                 *
                 * \return Returns true if the instance is invalid.  Returns false if the instance is valid.
                 */
                bool isInvalid() const;

                /**
                 * Method you can use to obtain the presentation.
                 *
                 * \return Returns a pointer to the underlying presentation.
                 */
                Presentation* presentation() const;

                /**
                 * Method you can use to obtain the presentation area ID.
                 *
                 * \return Returns the presentation area ID.
                 */
                unsigned long areaId() const;

                /**
                 * Method you can use to obtain the size for this presentation area.
                 *
                 * \return Returns the size of the presentation area.
                 */
                const QSizeF& areaSize() const;

                /**
                 * Method you can use to obtain the ascent of the presentation area.
                 *
                 * \return Returns the ascent of the presentation area.
                 */
                float ascent() const;

                /**
                 * Method you can use to obtain the graphics item for this presentation area.
                 *
                 * \return Returns the graphics item tied to this presentation area.
                 */
                QGraphicsItem* graphicsItem() const;

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to be copied.
                 *
                 * \return Returns a reference to this instance.
                 */
                Entry& operator=(const Entry& other);

            private:
                /**
                 * The presentation.
                 */
                Presentation* currentPresentation;

                /**
                 * The presentation area ID.
                 */
                unsigned long currentAreaId;

                /**
                 * The presentation area size.
                 */
                QSizeF currentAreaSize;

                /**
                 * The presentation area ascent.
                 */
                float currentAscent;
        };

        /**
         * Iterator
         */
        typedef QList<Entry>::const_iterator Iterator;

        PresentationAreaTracker();

        /**
         * Copy constructor
         *
         * \param[in] other The instance to be copied.
         */
        PresentationAreaTracker(const PresentationAreaTracker& other);

        ~PresentationAreaTracker();

        /**
         * Method you can use to set the child presentation tied to this instance.  Call this before calling
         * \ref PresentationAreaTracker::allocateArea.
         *
         * \param[in] childPresentation The child presentation to be processed.
         */
        void setChildPresentation(Presentation* childPresentation);

        /**
         * Method you can use to clear this tracker.
         */
        void clear();

        /**
         * Method called by a child to a parent to allocate the next available area.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] size               The required space.
         *
         * \param[in] ascent             Ascent used for text to perform vertical alignment.  Non text elements should
         *                               set this value to 0.
         */
        void allocateArea(unsigned long presentationAreaId, const QSizeF& size, float ascent);

        /**
         * Method you can use to determine if this presentation area tracker contains child information.
         *
         * \return Returns true if this presentation area tracker has collected up information for a child.  Returns
         *         false if this presentation area tracker has not collected up information for a child.
         */
        bool containsChildData() const;

        /**
         * Method you can use to obtain the tracked child presentation.
         *
         * \return Returns a pointer to the tracked child presentation.
         */
        Presentation* childPresentation() const;

        /**
         * Method you can use to determine a bounding rectangle for the entire child.  The method will take into
         * account the ascent and descent of every presentation area.
         *
         * \return Returns a bounding rectangle, in scene coordinates for a child.
         */
        QSizeF boundingRectangle() const;

        /**
         * Method you can use to determine the width for the entire child.
         *
         * \return Returns the width of the entire child, in scene units.
         */
        float width() const;

        /**
         * Method you can use to determine the maximum ascent across every presentation area.
         *
         * \return Returns the maximum ascent across every presentation area, in scene units.
         */
        float maximumAscent() const;

        /**
         * Method you can use to determine the maximum descent across every presentation area.
         *
         * \return Returns the maximum descent across every presentation area, in scene units.
         */
        float maximumDescent() const;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.  Note that this method must call the
         * \ref PlacementNegotiator::resetPlacement method of every child.
         */
        void resetPlacement();

        /**
         * Method that is called when the presentation tracked by this instance needs to remove itself from the scene.
         */
        void removeFromScene();

        /**
         * Method you can use to obtain the number of tracked presentation areas for a given child.
         *
         * \return Returns the number of presentation areas.
         */
        unsigned long numberPresentationAreas() const;

        /**
         * Method you can use to obtain a presentation area entry.
         *
         * \param[in] index The zero based index of the presentation area.
         *
         * \return Returns a reference to the presentation area entry.  A reference to an invalid entry is returned if
         *         the index is invalid.
         */
        Entry& entry(unsigned index);

        /**
         * Method you can use to obtain a presentation area entry.
         *
         * \param[in] index The zero based index of the presentation area.
         *
         * \return Returns a reference to the presentation area entry.  A reference to an invalid entry is returned if
         *         the index is invalid.
         */
        const Entry& entry(unsigned index) const;

        /**
         * Array operator.
         *
         * \param[in] index The zero based index of the presentation area.
         *
         * \return Returns a reference to the presentation area entry.  A reference to an invalid entry is returned if
         *         the index is invalid.
         */
        Entry& operator[](unsigned index);

        /**
         * Array operator.
         *
         * \param[in] index The zero based index of the presentation area.
         *
         * \return Returns a reference to the presentation area entry.  A reference to an invalid entry is returned if
         *         the index is invalid.
         */
        const Entry& operator[](unsigned index) const;

        /**
         * Method you can use to obtain an iterator to the first presentation entry.
         *
         * \return Returns an iterator to the first presentation entry.
         */
        Iterator begin() const;

        /**
         * Method you can use to obtain an iterator past the last presentation entry.
         *
         * \return Returns an iterator past the last presentation entry.
         */
        Iterator end() const;

        /**
         * Assignment operator
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object.
         */
        PresentationAreaTracker& operator=(const PresentationAreaTracker& other);

    private:
        /**
         * A null entry used when we receive an invalid index.
         */
        static const Entry nullEntry;

        /**
         * Pointer to the presentation tracked by this class.
         */
        Presentation* currentPresentation;

        /**
         * List of presentation area entries.
         */
        QList<Entry> currentEntries;

        /**
         * The current width, in scene units.
         */
        float currentWidth;

        /**
         * The current maximum ascent, in scene units.
         */
        float currentMaximumAscent;

        /**
         * The current maximum descent, in scene units.
         */
        float currentMaximumDescent;
};

#endif
