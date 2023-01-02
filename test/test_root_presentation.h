/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref RootPresentation class.
***********************************************************************************************************************/

#ifndef TEST_ROOT_PRESENTATION_H
#define TEST_ROOT_PRESENTATION_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_page_format.h>
#include <ld_diagnostic.h>

#include <presentation.h>
#include <presentation_with_fixed_children.h>
#include <root_presentation.h>

class QEventLoop;
class QTimer;

class PlacementNegotiator;
class PlacementTracker;

enum class LastSignal {
    NONE,
    PRESENTATION_CHANGED,
    PRESENTATION_ADDED,
    PRESENTATION_REMOVED,
    PAGE_FORMAT_CHANGED,
    PROGRAM_NOW_PRISTINE,
    REFERENCES_HAVE_CHANGED,
    PROGRAM_WAS_SAVED,
    PROGRAM_WAS_SAVED_AS,
    PROGRAM_SAVE_HAS_FAILED,
    PROGRAM_WAS_LOADED,
    PROGRAM_LOAD_HAS_FAILED,
    NEW_PROGRAM_WAS_CREATED,
    PROGRAM_CLOSED,
    REMOVING_CHILD_PRESENTATION,
    CHILD_PRESENTATION_NOW_INSERTED_BEFORE,
    CHILD_PRESENTATION_NOW_INSERTED_AFTER,
    NOW_TIED_TO_ELEMENT,
    NOW_UNTIED_FROM_ELEMENT,
    FORMAT_WAS_CHANGED,
    ELEMENT_DATA_WAS_CHANGED,
    DIAGNOSTIC_WAS_REPORTED,
    DIAGNOSTIC_WAS_CLEARED
};

class TestRootPresentation:public QObject {
    Q_OBJECT

    public:
        TestRootPresentation();

        ~TestRootPresentation() override;

    protected slots: // protected to keep the test framework from thinking these are test cases.
        void processPresentationChanged(Presentation* changedPresentation);

        void processPresentationAdded(Presentation* addedPresentation);

        void processPresentationRemoved(Presentation* removedPresentation);

        void processPageFormatChange(
            unsigned long                  pageNumber,
            QSharedPointer<Ld::PageFormat> oldFormat,
            QSharedPointer<Ld::PageFormat> newFormat
        );

        void processNowPristine();

        void processReferenceChanges();

        void processProgramSaved(const QString& filename);

        void processProgramSavedAs(const QString& filename);

        void processProgramSaveFailed(const QString& errorMessage);

        void processProgramLoaded(const QString& filename, unsigned majorVersion, unsigned minorVersion);

        void processProgramLoadFailed(const QString& errorMessage);

        void processNewProgramCreated();

        void processProgramClosed();

        void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation);

        void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation);

        void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation);

        void tiedToElement(Ld::ElementPointer element);

        void untiedFromElement();

        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat);

        void processDataChange();

        void processDiagnostic(Ld::DiagnosticPointer diagnostic);

        void clearDiagnosticDisplay();

        void terminateEventLoop();

    private slots:
        void initTestCase();
        void testTiedToUntiedFromElement();
        void testPresentationAddedRemoved();
        void testPresentationChanged();
        void testElementDataChanged();
        void testNowPristineAndNewProgram();
        void testImportsChanged();
        void testProgramSaveLoadClose();
        void testProgramSaveLoadFailed();
        void testChildInsertionRemoval();
        void testFormatChange();
        void testDiagnosticSupport();
        void testPageFormatChanged();

    private:
        static constexpr unsigned signalPropagationTime = 5; // mSec

        void setupConnectionsToThis(RootPresentation* presentation);
        void runEventLoop();
        void clearStatus();

        QEventLoop*           eventLoop;
        QTimer*               eventTimer;

        bool                  unexpectedValueFound;
        QList<LastSignal>     currentLastSignals;
        Presentation*         currentPresentation;
        QString               currentFilename;
        QString               currentErrorMessage;
        unsigned              currentMajorVersion;
        unsigned              currentMinorVersion;
        unsigned long         currentChildIndex;
        unsigned long         currentPageNumber;
        Ld::ElementPointer    currentElement;
        Ld::FormatPointer     currentOldFormat;
        Ld::FormatPointer     currentNewFormat;
        Ld::DiagnosticPointer currentDiagnostic;

};

class RootPresentationWrapper:public RootPresentation {
    Q_OBJECT

    public:
        RootPresentationWrapper();

        ~RootPresentationWrapper() override;

        void clearStatus();

        bool unexpectedValue() const;

        QList<LastSignal> lastSignals() const;

        Presentation* reportedPresentation() const;

        QString reportedFilename() const;

        QString reportedErrorMessage() const;

        unsigned reportedMajorVersion() const;

        unsigned reportedMinorVersion() const;

        unsigned long reportedChildIndex() const;

        unsigned long reportedPageNumber() const;

        Ld::ElementPointer reportedElement() const;

        Ld::FormatPointer reportedOldFormat() const;

        Ld::FormatPointer reportedNewFormat() const;

        Ld::DiagnosticPointer reportedDiagnostic() const;

    protected slots:
        void processPresentationChanged(Presentation* changedPresentation) final;

        void processPresentationAdded(Presentation* addedPresentation) final;

        void processPresentationRemoved(Presentation* removedPresentation) final;

        void processPageFormatChange(
            unsigned long                  pageNumber,
            QSharedPointer<Ld::PageFormat> oldFormat,
            QSharedPointer<Ld::PageFormat> newFormat
        ) final;

        void processNowPristine() final;

        void processReferenceChanges() final;

        void processProgramSaved(const QString& filename) final;

        void processProgramSavedAs(const QString& filename) final;

        void processProgramSaveFailed(const QString& errorMessage) final;

        void processProgramLoaded(const QString& filename, unsigned majorVersion, unsigned minorVersion) final;

        void processProgramLoadFailed(const QString& errorMessage) final;

        void processNewProgramCreated() final;

        void processProgramClosed() final;

        void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation) final;

        void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation) final;

        void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation) final;

        void tiedToElement(Ld::ElementPointer element) final;

        void untiedFromElement() final;

        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        void processDataChange() final;

        void processDiagnostic(Ld::DiagnosticPointer diagnostic) final;

        void clearDiagnosticDisplay() final;

    private:
        bool                  unexpectedValueFound;
        QList<LastSignal>     currentLastSignals;
        Presentation*         currentPresentation;
        QString               currentFilename;
        QString               currentErrorMessage;
        unsigned              currentMajorVersion;
        unsigned              currentMinorVersion;
        unsigned long         currentChildIndex;
        unsigned long         currentPageNumber;
        Ld::ElementPointer    currentElement;
        Ld::FormatPointer     currentOldFormat;
        Ld::FormatPointer     currentNewFormat;
        Ld::DiagnosticPointer currentDiagnostic;
};

class ChildPresentation:public PresentationWithFixedChildren {
    Q_OBJECT

    public:
        ChildPresentation();

        ~ChildPresentation() override;

        QString typeName() const final;

        QString plugInName() const final;

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
};

#endif
