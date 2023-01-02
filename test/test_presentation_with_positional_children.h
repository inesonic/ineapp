/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PresentationWithPositionalChildren class.
***********************************************************************************************************************/

#ifndef TEST_PRESENTATION_WITH_POSITIONAL_CHILDREN_H
#define TEST_PRESENTATION_WITH_POSITIONAL_CHILDREN_H

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

#include <ld_element.h>
#include <ld_format.h>
#include <ld_diagnostic.h>

#include <presentation.h>
#include <presentation_with_positional_children.h>

class QEventLoop;
class QTimer;

class PlacementNegotiator;

class TestPresentationWithPositionalChildren:public QObject {
    Q_OBJECT

    public:
        TestPresentationWithPositionalChildren();

        ~TestPresentationWithPositionalChildren() override;

    protected slots: // protected to keep the test framework from thinking these are test cases.
        void reportChildPresentationRemoved(unsigned long childIndex, Presentation* childPresentation);
        void reportChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation);
        void reportChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation);

        void terminateEventLoop();

    private slots:
        void initTestCase();
        void testChildAddRemoveSignals();

    private:
        void clearStatus();

        bool childPresentationRemovedReported;
        bool childPresentationInsertedBeforeReported;
        bool childPresentationInsertedAfterReported;

        unsigned long reportedChildIndex;
        Presentation* reportedChildPresentation;

        static constexpr unsigned signalPropagationTime = 5;

        QEventLoop*        eventLoop;
        QTimer*            eventTimer;
};

class PositionalPresentation:public PresentationWithPositionalChildren {
    Q_OBJECT

    public:
        PositionalPresentation();

        ~PositionalPresentation() override;

        QString typeName() const final;

        QString plugInName() const final;

        bool childPresentationRemoved() const;

        bool childPresentationInsertedBefore() const;

        bool childPresentationInsertedAfter() const;

        unsigned long childIndex() const;

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
        void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation) final;

        void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation) final;

        void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation) final;

    private:
        bool childPresentationRemovedReported;
        bool childPresentationInsertedBeforeReported;
        bool childPresentationInsertedAfterReported;

        unsigned long reportedChildIndex;
        Presentation* reportedChildPresentation;
};

#endif
