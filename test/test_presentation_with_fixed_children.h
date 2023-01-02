/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PresentationWithFixedChildren class.  Tests also check the \ref Presentation
* class.
***********************************************************************************************************************/

#ifndef TEST_PRESENTATION_WITH_FIXED_CHILDREN_H
#define TEST_PRESENTATION_WITH_FIXED_CHILDREN_H

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

#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_diagnostic.h>

#include <presentation.h>
#include <presentation_with_fixed_children.h>

class QEventLoop;
class QTimer;

class PlacementNegotiator;
class PlacementTracker;

class TestPresentationWithFixedChildren:public QObject {
    Q_OBJECT

    public:
        TestPresentationWithFixedChildren();

        ~TestPresentationWithFixedChildren() override;

    protected slots: // protected to keep the test framework from thinking these are test cases.
        void reportFormatChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat);
        void reportElementDataChanged();
        void reportDiagnosticReported(Ld::DiagnosticPointer diagnostic);
        void reportDiagnosticCleared();

        void reportChildPresentationRemoved(unsigned long childIndex, Presentation* childPresentation);
        void reportPresentationSet(unsigned long childIndex, Presentation* childPresentation);

        void terminateEventLoop();

    private slots:
        void initTestCase();
        void testChildAddRemoveSignals();
        void testFormatChangedSignal();
        void testDataChangedSignal();
        void testDiagnosticReportedSignal();
        void testDiagnosticClearedSignal();

    private:
        void clearStatus();

        bool formatChangedReported;
        Ld::FormatPointer reportedOldFormat;
        Ld::FormatPointer reportedNewFormat;

        bool elementDataChangeReported;

        bool diagnosticReportedReported;
        Ld::DiagnosticPointer reportedDiagnostic;

        bool diagnosticClearedReported;

        bool childPresentationRemovedReported;
        bool childPresentationSetReported;

        unsigned long reportedChildIndex;
        Presentation* reportedChildPresentation;

        static constexpr unsigned signalPropagationTime = 5;

        QEventLoop*        eventLoop;
        QTimer*            eventTimer;
};

class FixedPresentation:public PresentationWithFixedChildren {
    Q_OBJECT

    public:
        FixedPresentation();

        ~FixedPresentation() override;

        QString typeName() const final;

        QString plugInName() const final;

        bool formatWasChanged() const;

        Ld::FormatPointer oldFormat() const;

        Ld::FormatPointer newFormat() const;

        bool elementDataWasChanged() const;

        bool diagnosticWasReported() const;

        Ld::DiagnosticPointer diagnostic() const;

        bool diagnosticWasCleared() const;

        bool childPresentationRemoved() const;

        bool childPresentationSet() const;

        unsigned long childIndex() const;

        Presentation* childPresentation() const;

        void clearStatus();

        static void clearNumberInstances();

        static unsigned numberInstances();

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
        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        void processDataChange() final;

        void processDiagnostic(Ld::DiagnosticPointer diagnostic) final;

        void clearDiagnosticDisplay() final;

        void processPresentationRemoved(unsigned long childIndex, Presentation* childPresentation) final;

        void processChildPresentationSet(unsigned long childIndex, Presentation* childPresentation) final;

    private:
        static unsigned currentNumberInstances;

        bool formatChangedReported;
        Ld::FormatPointer reportedOldFormat;
        Ld::FormatPointer reportedNewFormat;

        bool elementDataChangeReported;

        bool diagnosticReportedReported;
        Ld::DiagnosticPointer reportedDiagnostic;

        bool diagnosticClearedReported;

        bool childPresentationRemovedReported;
        bool childPresentationSetReported;

        unsigned long reportedChildIndex;
        Presentation* reportedChildPresentation;
};

#endif
