/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PresentationWithFloatingChildren class.  Tests also check the \ref Presentation
* class.
***********************************************************************************************************************/

#ifndef TEST_PRESENTATION_WITH_FLOATING_CHILDREN_H
#define TEST_PRESENTATION_WITH_FLOATING_CHILDREN_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>
#include <QSharedPointer>
#include <QMetaType>
#include <QByteArray>
#include <QBitArray>
#include <QString>
#include <QStringList>
#include <QChar>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QAction>
#include <QPointF>

#include <ld_element.h>
#include <ld_location.h>
#include <ld_visual_with_floating_children.h>
#include <ld_format.h>
#include <ld_diagnostic.h>

#include <presentation.h>
#include <presentation_with_floating_children.h>

class QEventLoop;
class QTimer;

class PlacementNegotiator;
class PlacementTracker;

class TestPresentationWithFloatingChildren:public QObject {
    Q_OBJECT

    public:
        TestPresentationWithFloatingChildren();

        ~TestPresentationWithFloatingChildren() override;

    protected slots: // protected to keep the test framework from thinking these are test cases.
        void reportChildPresentationRemoved(const QPointF& childLocation, Presentation* childPresentation);
        void reportChildPresentationInserted(Presentation* childPresentation, const QPointF& childLocation);
        void reportChildPresentationRelocated(
            Presentation*  childPresentation,
            const QPointF& childOldLocation,
            const QPointF& childNewLocation
        );

        void terminateEventLoop();

    private slots:
        void initTestCase();
        void testChildAddRemoveSignals();

    private:
        void clearStatus();

        bool childPresentationRemovedReported;
        bool childPresentationInsertedReported;
        bool childPresentationRelocatedReported;

        QPointF reportedChildLocation;
        QPointF reportedOldLocation;
        Presentation* reportedChildPresentation;

        static constexpr unsigned signalPropagationTime = 5;

        QEventLoop*        eventLoop;
        QTimer*            eventTimer;
};


class FloatingPresentation:public PresentationWithFloatingChildren {
    Q_OBJECT

    public:
        FloatingPresentation();

        ~FloatingPresentation() override;

        QString typeName() const final;

        QString plugInName() const final;

        bool childPresentationRemoved() const;

        bool childPresentationInserted() const;

        bool childPresentationRelocated() const;

        QPointF childLocation() const;

        QPointF oldChildLocation() const;

        Presentation* childPresentation() const;

        void clearStatus();

        void requestRepositioning(Presentation* childPresentation) final;

        void recalculatePlacement(
            PlacementTracker*    placementTracker,
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            Presentation*        nextSibling,
            bool                 honorLeadingWhitespace,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) final;

        void redoPlacement(
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            unsigned long        firstPresentationAreaId = 0,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) final;

        QSizeF requestArea(unsigned long childIdentifier, SpaceQualifier* spaceQualifier = Q_NULLPTR) final;

        void allocateArea(
            unsigned long childIdentifier,
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0,
            bool          canStretch = false
        ) final;

        void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) final;

        void applyStretch(unsigned long presentationAreaId, float stretchFactor) final;

        QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const final;

        void removeFromScene() final;

    protected slots:
        void processRemovingChildPresentation(const QPointF& location, Presentation* childPresentation) final;

        void processChildPresentationInserted(Presentation* childPresentation, const QPointF& location) final;

        void processChildPresentationRelocated(
            Presentation*  childPresentation,
            const QPointF& oldLocation,
            const QPointF& newLocation
        ) final;

    private:
        bool childPresentationRemovedReported;
        bool childPresentationInsertedReported;
        bool childPresentationRelocatedReported;

        QPointF       reportedChildLocation;
        QPointF       reportedOldChildLocation;
        Presentation* reportedChildPresentation;
};

#endif
