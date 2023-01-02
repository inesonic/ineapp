/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref RootPresentation class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QMetaType>
#include <QEventLoop>
#include <QTimer>
#include <QSizeF>
#include <QRectF>
#include <QSharedPointer>
#include <QFileInfo>

#include <ld_handle.h>
#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_data_type.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_diagnostic.h>
#include <ld_format.h>
#include <ld_page_format.h>
#include <ld_character_format.h>
#include <ld_paragraph_format.h>
#include <ld_root_element.h>
#include <ld_paragraph_element.h>
#include <ld_text_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_root_import.h>
#include <ld_plug_in_manager.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_cpp_translation_phase.h>

#include <root_presentation.h>
#include <presentation_with_fixed_children.h>
#include <presentation.h>

#include "test_root_presentation.h"

/***********************************************************************************************************************
 * RootElementWrapper:
 */

class RootElementWrapper:public Ld::RootElement {
    public:
        RootElementWrapper();

        ~RootElementWrapper() override;

        void dataChanged();
};


RootElementWrapper::RootElementWrapper() {}


RootElementWrapper::~RootElementWrapper() {}


void RootElementWrapper::dataChanged() {
    elementDataChanged();
}

/***********************************************************************************************************************
 * ChildElement:
 */

class ChildElement:public Ld::ElementWithFixedChildren {
    public:
        static constexpr unsigned forcedNumberChildren = 3;

        ChildElement();

        ~ChildElement() override;

        QString typeName() const final;

        QString plugInName() const final;

        QString description() const final;

        Ld::DataType::ValueType valueType() const final;

        Ld::Capabilities parentRequires(unsigned long index) const final;

        Ld::Capabilities childProvidesCapabilities() const final;

        void dataChanged();
};


ChildElement::ChildElement() {
    setNumberChildren(forcedNumberChildren, nullptr);
}


ChildElement::~ChildElement() {}


QString ChildElement::typeName() const {
    return QString("ChildElement");
}


QString ChildElement::plugInName() const {
    return QString();
}


QString ChildElement::description() const {
    return QString();
}


Ld::DataType::ValueType ChildElement::valueType() const {
    return Ld::DataType::ValueType::NONE;
}


Ld::Capabilities ChildElement::parentRequires(unsigned long) const {
    return Ld::Capabilities();
}


Ld::Capabilities ChildElement::childProvidesCapabilities() const {
    return Ld::Capabilities();
}


void ChildElement::dataChanged() {
    elementDataChanged();
}

/***********************************************************************************************************************
 * ChildPresentation:
 */

ChildPresentation::ChildPresentation() {}


ChildPresentation::~ChildPresentation() {}


QString ChildPresentation::typeName() const {
    return QString("ChildElement");
}


QString ChildPresentation::plugInName() const {
    return QString();
}


void ChildPresentation::requestRepositioning(Presentation*) {}


void ChildPresentation::recalculatePlacement(
        PlacementTracker*,
        PlacementNegotiator*,
        unsigned long,
        Presentation*,
        bool,
        float,
        float,
        float
    ) {}


void ChildPresentation::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {}


QSizeF ChildPresentation::requestArea(unsigned long, ChildPresentation::SpaceQualifier*) {
    return QSizeF();
}


void ChildPresentation::allocateArea(unsigned long, unsigned long, const QSizeF&, float, bool) {}


void ChildPresentation::areaInsufficient(unsigned long, const QSizeF&) {}


void ChildPresentation::applyStretch(unsigned long, float) {}


QGraphicsItem* ChildPresentation::graphicsItem(unsigned long) const {
    return Q_NULLPTR;
}


void ChildPresentation::removeFromScene() {}

/***********************************************************************************************************************
 * RootPresentationWrapper:
 */

RootPresentationWrapper::RootPresentationWrapper() {
    clearStatus();
}


RootPresentationWrapper::~RootPresentationWrapper() {}


void RootPresentationWrapper::clearStatus() {
    unexpectedValueFound  = false;
    currentPresentation   = nullptr;
    currentFilename       = QString();
    currentErrorMessage   = QString();
    currentMajorVersion   = static_cast<unsigned>(-2);
    currentMinorVersion   = static_cast<unsigned>(-2);
    currentChildIndex     = static_cast<unsigned long>(-1);
    currentPageNumber     = static_cast<unsigned long>(-1);
    currentOldFormat.reset();
    currentNewFormat.reset();

    currentLastSignals.clear();
    currentElement.reset();
    currentDiagnostic.reset();
}


bool RootPresentationWrapper::unexpectedValue() const {
    return unexpectedValueFound;
}


QList<LastSignal> RootPresentationWrapper::lastSignals() const {
    return currentLastSignals;
}


Presentation* RootPresentationWrapper::reportedPresentation() const {
    return currentPresentation;
}


QString RootPresentationWrapper::reportedFilename() const {
    return currentFilename;
}


QString RootPresentationWrapper::reportedErrorMessage() const {
    return currentErrorMessage;
}


unsigned RootPresentationWrapper::reportedMajorVersion() const {
    return currentMajorVersion;
}


unsigned RootPresentationWrapper::reportedMinorVersion() const {
    return currentMinorVersion;
}


unsigned long RootPresentationWrapper::reportedChildIndex() const {
    return currentChildIndex;
}


unsigned long RootPresentationWrapper::reportedPageNumber() const {
    return currentPageNumber;
}

Ld::ElementPointer RootPresentationWrapper::reportedElement() const {
    return currentElement;
}


Ld::FormatPointer RootPresentationWrapper::reportedOldFormat() const {
    return currentOldFormat;
}


Ld::FormatPointer RootPresentationWrapper::reportedNewFormat() const {
    return currentNewFormat;
}


Ld::DiagnosticPointer RootPresentationWrapper::reportedDiagnostic() const {
    return currentDiagnostic;
}


void RootPresentationWrapper::processPresentationChanged(Presentation* changedPresentation) {
    currentLastSignals.append(LastSignal::PRESENTATION_CHANGED);

    if (currentPresentation == nullptr) {
        currentPresentation = changedPresentation;
    } else {
        if (currentPresentation != changedPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processPresentationAdded(Presentation* addedPresentation) {
    currentLastSignals.append(LastSignal::PRESENTATION_ADDED);

    if (currentPresentation == nullptr) {
        currentPresentation = addedPresentation;
    } else {
        if (currentPresentation != addedPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processPresentationRemoved(Presentation* removedPresentation) {
    currentLastSignals.append(LastSignal::PRESENTATION_REMOVED);

    if (currentPresentation == nullptr) {
        currentPresentation = removedPresentation;
    } else {
        if (currentPresentation != removedPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processPageFormatChange(
        unsigned long                  pageNumber,
        QSharedPointer<Ld::PageFormat> oldFormat,
        QSharedPointer<Ld::PageFormat> newFormat
    ) {
    currentLastSignals.append(LastSignal::PAGE_FORMAT_CHANGED);

    currentPageNumber = pageNumber;
    currentOldFormat  = oldFormat;
    currentNewFormat  = newFormat;
}


void RootPresentationWrapper::processNowPristine() {
    currentLastSignals.append(LastSignal::PROGRAM_NOW_PRISTINE);
}


void RootPresentationWrapper::processReferenceChanges() {
    currentLastSignals.append(LastSignal::REFERENCES_HAVE_CHANGED);
}


void RootPresentationWrapper::processProgramSaved(const QString& filename) {
    currentLastSignals.append(LastSignal::PROGRAM_WAS_SAVED);

    if (currentFilename.isNull()) {
        currentFilename = filename;
    } else {
        if (currentFilename != filename) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processProgramSavedAs(const QString& filename) {
    currentLastSignals.append(LastSignal::PROGRAM_WAS_SAVED_AS);

    if (currentFilename.isNull()) {
        currentFilename = filename;
    } else {
        if (currentFilename != filename) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processProgramSaveFailed(const QString& errorMessage) {
    currentLastSignals.append(LastSignal::PROGRAM_SAVE_HAS_FAILED);

    if (currentErrorMessage.isNull()) {
        currentErrorMessage = errorMessage;
    } else {
        if (currentErrorMessage != errorMessage) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processProgramLoaded(
        const QString& filename,
        unsigned       majorVersion,
        unsigned       minorVersion
    ) {
    currentLastSignals.append(LastSignal::PROGRAM_WAS_LOADED);

    if (currentFilename.isNull()) {
        currentFilename = filename;
    } else {
        if (currentFilename != filename) {
            unexpectedValueFound = true;
        }
    }

    if (currentMajorVersion == static_cast<unsigned>(-2) &&  currentMinorVersion == static_cast<unsigned>(-2)) {
        currentMajorVersion = majorVersion;
        currentMinorVersion = minorVersion;
    } else {
        if (currentMajorVersion != majorVersion || currentMinorVersion != minorVersion) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processProgramLoadFailed(const QString& errorMessage) {
    currentLastSignals.append(LastSignal::PROGRAM_LOAD_HAS_FAILED);

    if (currentErrorMessage.isNull()) {
        currentErrorMessage = errorMessage;
    } else {
        if (currentErrorMessage != errorMessage) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processNewProgramCreated() {
    currentLastSignals.append(LastSignal::NEW_PROGRAM_WAS_CREATED);
}


void RootPresentationWrapper::processProgramClosed() {
    currentLastSignals.append(LastSignal::PROGRAM_CLOSED);
}


void RootPresentationWrapper::processRemovingChildPresentation(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    currentLastSignals.append(LastSignal::REMOVING_CHILD_PRESENTATION);

    if (currentChildIndex == static_cast<unsigned long>(-1)) {
        currentChildIndex = childIndex;
    } else {
        if (currentChildIndex != childIndex) {
            unexpectedValueFound = true;
        }
    }

    if (currentPresentation == nullptr) {
        currentPresentation = childPresentation;
    } else {
        if (currentPresentation != childPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processChildPresentationInsertedBefore(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    currentLastSignals.append(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_BEFORE);

    if (currentChildIndex == static_cast<unsigned long>(-1)) {
        currentChildIndex = childIndex;
    } else {
        if (currentChildIndex != childIndex) {
            unexpectedValueFound = true;
        }
    }

    if (currentPresentation == nullptr) {
        currentPresentation = childPresentation;
    } else {
        if (currentPresentation != childPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processChildPresentationInsertedAfter(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    currentLastSignals.append(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_AFTER);

    if (currentChildIndex == static_cast<unsigned long>(-1)) {
        currentChildIndex = childIndex;
    } else {
        if (currentChildIndex != childIndex) {
            unexpectedValueFound = true;
        }
    }

    if (currentPresentation == nullptr) {
        currentPresentation = childPresentation;
    } else {
        if (currentPresentation != childPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::tiedToElement(Ld::ElementPointer element) {
    currentLastSignals.append(LastSignal::NOW_TIED_TO_ELEMENT);

    if (currentElement.isNull()) {
        currentElement = element;
    } else {
        if (currentElement != element) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::untiedFromElement() {
    currentLastSignals.append(LastSignal::NOW_UNTIED_FROM_ELEMENT);
}


void RootPresentationWrapper::processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    currentLastSignals.append(LastSignal::FORMAT_WAS_CHANGED);

    if (currentOldFormat == nullptr) {
        currentOldFormat = oldFormat;
    } else {
        if (currentOldFormat != oldFormat) {
            unexpectedValueFound = true;
        }
    }

    if (currentNewFormat == nullptr) {
        currentNewFormat = newFormat;
    } else {
        if (currentNewFormat != newFormat) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::processDataChange() {
    currentLastSignals.append(LastSignal::ELEMENT_DATA_WAS_CHANGED);
}


void RootPresentationWrapper::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    currentLastSignals.append(LastSignal::DIAGNOSTIC_WAS_REPORTED);

    if (currentDiagnostic.isNull()) {
        currentDiagnostic = diagnostic;
    } else {
        if (currentDiagnostic != diagnostic) {
            unexpectedValueFound = true;
        }
    }
}


void RootPresentationWrapper::clearDiagnosticDisplay() {
    currentLastSignals.append(LastSignal::DIAGNOSTIC_WAS_CLEARED);
}

/***********************************************************************************************************************
 * Class TestRootPresentation:
 */

TestRootPresentation::TestRootPresentation() {
    eventLoop  = new QEventLoop(this);
    eventTimer = new QTimer(this);

    eventTimer->setSingleShot(true);

    connect(eventTimer, SIGNAL(timeout()), this, SLOT(terminateEventLoop()));
}


TestRootPresentation::~TestRootPresentation() {}

void TestRootPresentation::processPresentationChanged(Presentation* changedPresentation) {
    currentLastSignals.append(LastSignal::PRESENTATION_CHANGED);

    if (currentPresentation == nullptr) {
        currentPresentation = changedPresentation;
    } else {
        if (currentPresentation != changedPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processPresentationAdded(Presentation* addedPresentation) {
    currentLastSignals.append(LastSignal::PRESENTATION_ADDED);

    if (currentPresentation == nullptr) {
        currentPresentation = addedPresentation;
    } else {
        if (currentPresentation != addedPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processPresentationRemoved(Presentation* removedPresentation) {
    currentLastSignals.append(LastSignal::PRESENTATION_REMOVED);

    if (currentPresentation == nullptr) {
        currentPresentation = removedPresentation;
    } else {
        if (currentPresentation != removedPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processPageFormatChange(
        unsigned long                  pageNumber,
        QSharedPointer<Ld::PageFormat> oldFormat,
        QSharedPointer<Ld::PageFormat> newFormat
    ) {
    currentLastSignals.append(LastSignal::PAGE_FORMAT_CHANGED);

    currentPageNumber = pageNumber;
    currentOldFormat  = oldFormat;
    currentNewFormat  = newFormat;
}



void TestRootPresentation::processNowPristine() {
    currentLastSignals.append(LastSignal::PROGRAM_NOW_PRISTINE);
}


void TestRootPresentation::processReferenceChanges() {
    currentLastSignals.append(LastSignal::REFERENCES_HAVE_CHANGED);
}


void TestRootPresentation::processProgramSaved(const QString& filename) {
    currentLastSignals.append(LastSignal::PROGRAM_WAS_SAVED);

    if (currentFilename.isNull()) {
        currentFilename = filename;
    } else {
        if (currentFilename != filename) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processProgramSavedAs(const QString& filename) {
    currentLastSignals.append(LastSignal::PROGRAM_WAS_SAVED_AS);

    if (currentFilename.isNull()) {
        currentFilename = filename;
    } else {
        if (currentFilename != filename) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processProgramSaveFailed(const QString& errorMessage) {
    currentLastSignals.append(LastSignal::PROGRAM_SAVE_HAS_FAILED);

    if (currentErrorMessage.isNull()) {
        currentErrorMessage = errorMessage;
    } else {
        if (currentErrorMessage != errorMessage) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processProgramLoaded(
        const QString& filename,
        unsigned       majorVersion,
        unsigned       minorVersion
    ) {
    currentLastSignals.append(LastSignal::PROGRAM_WAS_LOADED);

    if (currentFilename.isNull()) {
        currentFilename = filename;
    } else {
        if (currentFilename != filename) {
            unexpectedValueFound = true;
        }
    }

    if (currentMajorVersion == static_cast<unsigned>(-2) &&  currentMinorVersion == static_cast<unsigned>(-2)) {
        currentMajorVersion = majorVersion;
        currentMinorVersion = minorVersion;
    } else {
        if (currentMajorVersion != majorVersion || currentMinorVersion != minorVersion) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processProgramLoadFailed(const QString& errorMessage) {
    currentLastSignals.append(LastSignal::PROGRAM_LOAD_HAS_FAILED);

    if (currentErrorMessage.isNull()) {
        currentErrorMessage = errorMessage;
    } else {
        if (currentErrorMessage != errorMessage) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processNewProgramCreated() {
    currentLastSignals.append(LastSignal::NEW_PROGRAM_WAS_CREATED);
}


void TestRootPresentation::processProgramClosed() {
    currentLastSignals.append(LastSignal::PROGRAM_CLOSED);
}


void TestRootPresentation::processRemovingChildPresentation(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    currentLastSignals.append(LastSignal::REMOVING_CHILD_PRESENTATION);

    if (currentChildIndex == static_cast<unsigned long>(-1)) {
        currentChildIndex = childIndex;
    } else {
        if (currentChildIndex != childIndex) {
            unexpectedValueFound = true;
        }
    }

    if (currentPresentation == nullptr) {
        currentPresentation = childPresentation;
    } else {
        if (currentPresentation != childPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processChildPresentationInsertedBefore(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    currentLastSignals.append(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_BEFORE);

    if (currentChildIndex == static_cast<unsigned long>(-1)) {
        currentChildIndex = childIndex;
    } else {
        if (currentChildIndex != childIndex) {
            unexpectedValueFound = true;
        }
    }

    if (currentPresentation == nullptr) {
        currentPresentation = childPresentation;
    } else {
        if (currentPresentation != childPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processChildPresentationInsertedAfter(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    currentLastSignals.append(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_AFTER);

    if (currentChildIndex == static_cast<unsigned long>(-1)) {
        currentChildIndex = childIndex;
    } else {
        if (currentChildIndex != childIndex) {
            unexpectedValueFound = true;
        }
    }

    if (currentPresentation == nullptr) {
        currentPresentation = childPresentation;
    } else {
        if (currentPresentation != childPresentation) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::tiedToElement(Ld::ElementPointer element) {
    currentLastSignals.append(LastSignal::NOW_TIED_TO_ELEMENT);

    if (currentElement.isNull()) {
        currentElement = element;
    } else {
        if (currentElement != element) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::untiedFromElement() {
    currentLastSignals.append(LastSignal::NOW_UNTIED_FROM_ELEMENT);
}


void TestRootPresentation::processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    currentLastSignals.append(LastSignal::FORMAT_WAS_CHANGED);

    if (currentOldFormat == nullptr) {
        currentOldFormat = oldFormat;
    } else {
        if (currentOldFormat != oldFormat) {
            unexpectedValueFound = true;
        }
    }

    if (currentNewFormat == nullptr) {
        currentNewFormat = newFormat;
    } else {
        if (currentNewFormat != newFormat) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::processDataChange() {
    currentLastSignals.append(LastSignal::ELEMENT_DATA_WAS_CHANGED);
}


void TestRootPresentation::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    currentLastSignals.append(LastSignal::DIAGNOSTIC_WAS_REPORTED);

    if (currentDiagnostic.isNull()) {
        currentDiagnostic = diagnostic;
    } else {
        if (currentDiagnostic != diagnostic) {
            unexpectedValueFound = true;
        }
    }
}


void TestRootPresentation::clearDiagnosticDisplay() {
    currentLastSignals.append(LastSignal::DIAGNOSTIC_WAS_CLEARED);
}


void TestRootPresentation::terminateEventLoop() {
    eventLoop->quit();
}


void TestRootPresentation::initTestCase() {
    Ld::Element::setAutoDeleteVisuals(false);
    Ld::Handle::initialize(0x123456789ABCDEF0ULL);
    Ld::Format::registerCreator(Ld::PageFormat::formatName, Ld::PageFormat::creator);
    Ld::Format::registerCreator(Ld::ParagraphFormat::formatName, Ld::ParagraphFormat::creator);
    Ld::Format::registerCreator(Ld::CharacterFormat::formatName, Ld::CharacterFormat::creator);

    Ld::Element::registerCreator(Ld::ParagraphElement::elementName, Ld::ParagraphElement::creator);
    Ld::Element::registerCreator(Ld::TextElement::elementName, Ld::TextElement::creator);
}


void TestRootPresentation::testTiedToUntiedFromElement() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    setupConnectionsToThis(rootPresentation);

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->setVisual(rootPresentation);

    runEventLoop();

    QVERIFY(currentLastSignals.contains(LastSignal::NOW_TIED_TO_ELEMENT));
    QVERIFY(currentElement == rootElement);

    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::NOW_TIED_TO_ELEMENT));
    QVERIFY(rootPresentation->reportedElement() == rootElement);

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->setVisual(nullptr);

    runEventLoop();

    QVERIFY(currentLastSignals.contains(LastSignal::NOW_UNTIED_FROM_ELEMENT));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::NOW_UNTIED_FROM_ELEMENT));

    delete rootPresentation;
}


void TestRootPresentation::testPresentationAddedRemoved() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    QSharedPointer<ChildElement> child1Element(new ChildElement());
    child1Element->setWeakThis(child1Element.toWeakRef());

    ChildPresentation* child1Presentation = new ChildPresentation();
    child1Element->setVisual(child1Presentation);

    rootElement->append(child1Element, nullptr);

    QSharedPointer<ChildElement> child2Element(new ChildElement());
    child2Element->setWeakThis(child2Element.toWeakRef());

    ChildPresentation* child2Presentation = new ChildPresentation();
    child2Element->setVisual(child2Presentation);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    child1Element->setChild(0, child2Element, Q_NULLPTR);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_ADDED));
    QVERIFY(currentPresentation == child2Presentation);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_ADDED));
    QVERIFY(rootPresentation->reportedPresentation() == child2Presentation);

    clearStatus();
    rootPresentation->clearStatus();

    child1Element->removeChild(0, nullptr);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_REMOVED));
    QVERIFY(currentPresentation == child2Presentation);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_REMOVED));
    QVERIFY(rootPresentation->reportedPresentation() == child2Presentation);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testPresentationChanged() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    QSharedPointer<ChildElement> child1Element(new ChildElement());
    child1Element->setWeakThis(child1Element.toWeakRef());

    ChildPresentation* child1Presentation = new ChildPresentation();
    child1Element->setVisual(child1Presentation);

    rootElement->append(child1Element, nullptr);

    QSharedPointer<ChildElement> child2Element(new ChildElement());
    child2Element->setWeakThis(child2Element.toWeakRef());

    ChildPresentation* child2Presentation = new ChildPresentation();
    child2Element->setVisual(child2Presentation);

    child1Element->setChild(0, child2Element, Q_NULLPTR);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    child2Element->dataChanged();

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_CHANGED));
    QVERIFY(currentPresentation == child2Presentation);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_CHANGED));
    QVERIFY(rootPresentation->reportedPresentation() == child2Presentation);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testElementDataChanged() {
    QSharedPointer<RootElementWrapper> rootElement(new RootElementWrapper());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->dataChanged();

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::ELEMENT_DATA_WAS_CHANGED));
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_CHANGED));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::ELEMENT_DATA_WAS_CHANGED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_CHANGED));

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testNowPristineAndNewProgram() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    bool success = rootElement->openNew();
    QVERIFY(success);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(currentLastSignals.contains(LastSignal::NEW_PROGRAM_WAS_CREATED));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() >= 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::NEW_PROGRAM_WAS_CREATED));

    success = rootElement->close();
    QVERIFY(success);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testImportsChanged() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    QSharedPointer<Ld::RootElement> root2Element(new Ld::RootElement());
    root2Element->setWeakThis(root2Element.toWeakRef());

    RootPresentationWrapper* root2Presentation = new RootPresentationWrapper();
    root2Element->setVisual(root2Presentation);

    QList<Ld::RootImport> imports;
    imports << Ld::RootImport(root2Element, false);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->setImports(imports);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::REFERENCES_HAVE_CHANGED));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::REFERENCES_HAVE_CHANGED));

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testProgramSaveLoadClose() {
    Ld::PlugInsByName plugInsByName;

    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    bool success = rootElement->openNew();
    QVERIFY(success);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    success = rootElement->saveAs(QString("test.ms"));
    QVERIFY(success);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_WAS_SAVED_AS));
    QVERIFY(QFileInfo(currentFilename).fileName() == QString("test.ms"));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_WAS_SAVED_AS));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(QFileInfo(rootPresentation->reportedFilename()).fileName() == QString("test.ms"));

    clearStatus();
    rootPresentation->clearStatus();

    success = rootElement->save();
    QVERIFY(success);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_WAS_SAVED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(QFileInfo(currentFilename).fileName() == QString("test.ms"));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_WAS_SAVED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(QFileInfo(rootPresentation->reportedFilename()).fileName() == QString("test.ms"));

    success = rootElement->close();
    QVERIFY(success);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    success = rootElement->openExisting(QString("test.ms"), false, plugInsByName);
    QVERIFY(success);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_WAS_LOADED));
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(QFileInfo(currentFilename).fileName() == QString("test.ms"));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() >= 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_WAS_LOADED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_NOW_PRISTINE));
    QVERIFY(QFileInfo(rootPresentation->reportedFilename()).fileName() == QString("test.ms"));

    clearStatus();
    rootPresentation->clearStatus();

    success = rootElement->close();
    QVERIFY(success);

    runEventLoop();

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() >= 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_CLOSED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::REFERENCES_HAVE_CHANGED));

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testProgramSaveLoadFailed() {
    Ld::PlugInsByName plugInsByName;

    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    bool success = rootElement->openNew();
    QVERIFY(success);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    success = rootElement->saveAs(QString("XY:///fubar.ms"));
    QVERIFY(success == false);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_SAVE_HAS_FAILED));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_SAVE_HAS_FAILED));

    success = rootElement->close();
    QVERIFY(success == false);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    success = rootElement->openExisting(QString("XY:///fubar.ms"), false, plugInsByName);
    QVERIFY(success == false);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 1);
    QVERIFY(currentLastSignals.contains(LastSignal::PROGRAM_LOAD_HAS_FAILED));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() >= 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PROGRAM_LOAD_HAS_FAILED));

    success = rootElement->close();
    QVERIFY(success == false);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testChildInsertionRemoval() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    QSharedPointer<ChildElement> child1Element(new ChildElement());
    child1Element->setWeakThis(child1Element.toWeakRef());

    ChildPresentation* child1Presentation = new ChildPresentation();
    child1Element->setVisual(child1Presentation);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->append(child1Element, nullptr);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_ADDED));
    QVERIFY(currentLastSignals.contains(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_AFTER));
    QVERIFY(currentPresentation == child1Presentation);
    QVERIFY(currentChildIndex == 0);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_ADDED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_AFTER));
    QVERIFY(rootPresentation->reportedPresentation() == child1Presentation);
    QVERIFY(rootPresentation->reportedChildIndex() == 0);

    clearStatus();
    rootPresentation->clearStatus();

    QSharedPointer<ChildElement> child2Element(new ChildElement());
    child2Element->setWeakThis(child2Element.toWeakRef());

    ChildPresentation* child2Presentation = new ChildPresentation();
    child2Element->setVisual(child2Presentation);

    rootElement->prepend(child2Element, nullptr);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_ADDED));
    QVERIFY(currentLastSignals.contains(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_BEFORE));
    QVERIFY(currentPresentation == child2Presentation);
    QVERIFY(currentChildIndex == 0);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_ADDED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::CHILD_PRESENTATION_NOW_INSERTED_BEFORE));
    QVERIFY(rootPresentation->reportedPresentation() == child2Presentation);
    QVERIFY(rootPresentation->reportedChildIndex() == 0);

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->removeChild(0, nullptr);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() >= 2);
    QVERIFY(currentLastSignals.contains(LastSignal::PRESENTATION_REMOVED));
    QVERIFY(currentLastSignals.contains(LastSignal::REMOVING_CHILD_PRESENTATION));
    QVERIFY(currentPresentation == child2Presentation);
    QVERIFY(currentChildIndex == 0);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 2);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PRESENTATION_REMOVED));
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::REMOVING_CHILD_PRESENTATION));
    QVERIFY(rootPresentation->reportedPresentation() == child2Presentation);
    QVERIFY(rootPresentation->reportedChildIndex() == 0);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testFormatChange() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    Ld::CharacterFormat* testFormat1 = new Ld::CharacterFormat("Courier", 10);
    rootElement->setFormat(testFormat1);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    Ld::CharacterFormat* testFormat2 = new Ld::CharacterFormat("Helvetica", 12);
    rootElement->setFormat(testFormat2);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::FORMAT_WAS_CHANGED));
    QVERIFY(currentNewFormat == testFormat2);
    QVERIFY(currentOldFormat == testFormat1);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::FORMAT_WAS_CHANGED));
    QVERIFY(rootPresentation->reportedNewFormat() == testFormat2);
    QVERIFY(rootPresentation->reportedOldFormat() == testFormat1);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testDiagnosticSupport() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    Ld::CppTranslationPhase translationPhase;
    QSharedPointer<Ld::CppCodeGeneratorDiagnostic> diagnostic(new Ld::CppCodeGeneratorDiagnostic(
        rootElement,
        Ld::CppCodeGeneratorDiagnostic::Type::INTERNAL_ERROR,
        translationPhase,
        Ld::CppCodeGeneratorDiagnostic::Code::MISSING_TRANSLATOR
    ));

    rootElement->flagDiagnostic(diagnostic);
    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::DIAGNOSTIC_WAS_REPORTED));
    QVERIFY(currentDiagnostic == diagnostic);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::DIAGNOSTIC_WAS_REPORTED));
    QVERIFY(rootPresentation->reportedDiagnostic() == diagnostic);

    clearStatus();
    rootPresentation->clearStatus();

    rootElement->clearDiagnostic();
    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::DIAGNOSTIC_WAS_CLEARED));

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::DIAGNOSTIC_WAS_CLEARED));

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::testPageFormatChanged() {
    QSharedPointer<Ld::RootElement> rootElement(new Ld::RootElement());
    rootElement->setWeakThis(rootElement.toWeakRef());

    RootPresentationWrapper* rootPresentation = new RootPresentationWrapper();
    rootElement->setVisual(rootPresentation);

    setupConnectionsToThis(rootPresentation);

    runEventLoop();

    clearStatus();
    rootPresentation->clearStatus();

    QSharedPointer<Ld::PageFormat> format(new Ld::PageFormat(8.5 * 72.0, 11.0 * 72.0));
    format->setLeftMargin(0.75 * 72.0);
    format->setRightMargin(0.75 * 72.0);
    format->setTopMargin(0.75 * 72.0);
    format->setBottomMargin(0.75 * 72.0);

    rootElement->setPageFormat(2, format);

    runEventLoop();

    QVERIFY(unexpectedValueFound == false);
    QVERIFY(currentLastSignals.size() == 1);
    QVERIFY(currentLastSignals.contains(LastSignal::PAGE_FORMAT_CHANGED));
    QVERIFY(currentNewFormat == format);

    QVERIFY(rootPresentation->unexpectedValue() == false);
    QVERIFY(rootPresentation->lastSignals().size() == 1);
    QVERIFY(rootPresentation->lastSignals().contains(LastSignal::PAGE_FORMAT_CHANGED));
    QVERIFY(rootPresentation->reportedNewFormat() == format);

    rootElement->setVisual(nullptr);
    delete rootPresentation;
}


void TestRootPresentation::setupConnectionsToThis(RootPresentation* presentation) {
    connect(
        presentation,
        SIGNAL(presentationChanged(Presentation*)),
        this,
        SLOT(processPresentationChanged(Presentation*)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(presentationAdded(Presentation*)),
        this,
        SLOT(processPresentationAdded(Presentation*)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(presentationRemoved(Presentation*)),
        this,
        SLOT(processPresentationRemoved(Presentation*)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(programNowPristine()),
        this,
        SLOT(processNowPristine()),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(pageFormatHasChanged(unsigned long, QSharedPointer<Ld::PageFormat>, QSharedPointer<Ld::PageFormat>)),
        this,
        SLOT(processPageFormatChange(unsigned long, QSharedPointer<Ld::PageFormat>, QSharedPointer<Ld::PageFormat>)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(referencesHaveChanged()),
        this,
        SLOT(processReferenceChanges()),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(programWasSaved(const QString&)),
        this,
        SLOT(processProgramSaved(const QString&)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(programWasSavedAs(const QString&)),
        this,
        SLOT(processProgramSavedAs(const QString&)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(programSaveHasFailed(const QString&)),
        this,
        SLOT(processProgramSaveFailed(const QString&)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(programWasLoaded(const QString&, unsigned, unsigned)),
        this,
        SLOT(processProgramLoaded(const QString&, unsigned, unsigned)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(programLoadHasFailed(const QString&)),
        this,
        SLOT(processProgramLoadFailed(const QString&)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(newProgramWasCreated()),
        this,
        SLOT(processNewProgramCreated()),
        Presentation::connectionType
    );

    connect(presentation, SIGNAL(programClosed()), this, SLOT(processProgramClosed()), Presentation::connectionType);

    connect(
        presentation,
        SIGNAL(removingChildPresentation(unsigned long, Presentation*)),
        this,
        SLOT(processRemovingChildPresentation(unsigned long, Presentation*)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(childPresentationNowInsertedBefore(unsigned long, Presentation*)),
        this,
        SLOT(processChildPresentationInsertedBefore(unsigned long ,Presentation*)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(childPresentationNowInsertedAfter(unsigned long, Presentation*)),
        this,
        SLOT(processChildPresentationInsertedAfter(unsigned long, Presentation*)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(nowTiedToElement(Ld::ElementPointer)),
        this,
        SLOT(tiedToElement(Ld::ElementPointer)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(nowUntiedFromElement()),
        this,
        SLOT(untiedFromElement()),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(formatWasChanged(Ld::FormatPointer, Ld::FormatPointer)),
        this,
        SLOT(processFormatChange(Ld::FormatPointer,Ld::FormatPointer)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(elementDataWasChanged()),
        this,
        SLOT(processDataChange()),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(diagnosticWasReported(Ld::DiagnosticPointer)),
        this,
        SLOT(processDiagnostic(Ld::DiagnosticPointer)),
        Presentation::connectionType
    );

    connect(
        presentation,
        SIGNAL(diagnosticWasCleared()),
        this,
        SLOT(clearDiagnosticDisplay()),
        Presentation::connectionType
    );
}


void TestRootPresentation::runEventLoop() {
    eventTimer->start(signalPropagationTime);
    eventLoop->exec();
}


void TestRootPresentation::clearStatus() {
    unexpectedValueFound  = false;
    currentPresentation   = nullptr;
    currentFilename       = QString();
    currentErrorMessage   = QString();
    currentMajorVersion   = static_cast<unsigned>(-2);
    currentMinorVersion   = static_cast<unsigned>(-2);
    currentChildIndex     = static_cast<unsigned long>(-1);
    currentPageNumber     = static_cast<unsigned long>(-1);
    currentOldFormat.reset();
    currentNewFormat.reset();

    currentLastSignals.clear();
    currentElement.reset();
    currentDiagnostic.reset();
}




