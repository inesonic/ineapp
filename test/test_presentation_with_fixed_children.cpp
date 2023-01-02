/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PresentationWithFixedChildren class.  The test also validates the underlying
* \ref Presentation class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QMetaType>
#include <QEventLoop>
#include <QTimer>
#include <QSizeF>
#include <QRectF>

#include <ld_handle.h>
#include <ld_format_structures.h>
#include <ld_data_type.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_diagnostic.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_cpp_translation_phase.h>

#include <presentation.h>
#include <presentation_with_fixed_children.h>

#include "test_presentation_with_fixed_children.h"

/***********************************************************************************************************************
 * FixedElement:
 */

class FixedElement:public Ld::ElementWithFixedChildren {
    public:
        static constexpr unsigned forcedNumberChildren = 3;

        FixedElement();

        ~FixedElement() override;

        QString typeName() const final;

        QString plugInName() const final;

        QString description() const final;

        Ld::DataType::ValueType valueType() const final;

        Ld::Capabilities parentRequires(unsigned long index) const final;

        Ld::Capabilities childProvidesCapabilities() const final;

        void dataChanged();
};


FixedElement::FixedElement() {
    setNumberChildren(forcedNumberChildren, nullptr);
}


FixedElement::~FixedElement() {}


QString FixedElement::typeName() const {
    return QString("FixedElement");
}


QString FixedElement::plugInName() const {
    return QString();
}


QString FixedElement::description() const {
    return QString();
}


Ld::DataType::ValueType FixedElement::valueType() const {
    return Ld::DataType::ValueType::NONE;
}


void FixedElement::dataChanged() {
    elementDataChanged();
}


Ld::Capabilities FixedElement::parentRequires(unsigned long) const {
    return Ld::Capabilities();
}


Ld::Capabilities FixedElement::childProvidesCapabilities() const {
    return Ld::Capabilities();
}

/***********************************************************************************************************************
 * FixedPresentation:
 */

unsigned FixedPresentation::currentNumberInstances = 0;

FixedPresentation::FixedPresentation() {
    ++currentNumberInstances;
}


FixedPresentation::~FixedPresentation() {
    --currentNumberInstances;
}


QString FixedPresentation::typeName() const {
    return QString("FixedElement");
}


QString FixedPresentation::plugInName() const {
    return QString();
}


bool FixedPresentation::formatWasChanged() const {
    return formatChangedReported;
}


Ld::FormatPointer FixedPresentation::oldFormat() const {
    return reportedOldFormat;
}


Ld::FormatPointer FixedPresentation::newFormat() const {
    return reportedNewFormat;
}


bool FixedPresentation::elementDataWasChanged() const {
    return elementDataChangeReported;
}


bool FixedPresentation::diagnosticWasReported() const {
    return diagnosticReportedReported;
}


Ld::DiagnosticPointer FixedPresentation::diagnostic() const {
    return reportedDiagnostic;
}


bool FixedPresentation::diagnosticWasCleared() const {
    return diagnosticClearedReported;
}


bool FixedPresentation::childPresentationRemoved() const {
    return childPresentationRemovedReported;
}


bool FixedPresentation::childPresentationSet() const {
    return childPresentationSetReported;
}


unsigned long FixedPresentation::childIndex() const {
    return reportedChildIndex;
}


Presentation* FixedPresentation::childPresentation() const {
    return reportedChildPresentation;
}


void FixedPresentation::clearStatus() {
    formatChangedReported            = false;
    reportedOldFormat.reset();
    reportedNewFormat.reset();

    elementDataChangeReported        = false;

    diagnosticReportedReported       = false;
    reportedDiagnostic.reset();

    diagnosticClearedReported        = false;

    childPresentationRemovedReported = false;
    childPresentationSetReported     = false;

    reportedChildIndex               = static_cast<unsigned long>(-1);
    reportedChildPresentation        = nullptr;
}


void FixedPresentation::clearNumberInstances() {
    currentNumberInstances = 0;
}


unsigned FixedPresentation::numberInstances() {
    return currentNumberInstances;
}


void FixedPresentation::requestRepositioning(Presentation*) {}


void FixedPresentation::recalculatePlacement(
        PlacementTracker*,
        PlacementNegotiator*,
        unsigned long,
        Presentation*,
        bool,
        float,
        float,
        float
    ) {}


void FixedPresentation::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {}


QSizeF FixedPresentation::requestArea(unsigned long, FixedPresentation::SpaceQualifier*) {
    return QSizeF();
}


void FixedPresentation::allocateArea(unsigned long, unsigned long, const QSizeF&, float, bool) {}


void FixedPresentation::areaInsufficient(unsigned long, const QSizeF&) {}


void FixedPresentation::applyStretch(unsigned long, float) {}


QGraphicsItem* FixedPresentation::graphicsItem(unsigned long) const {
    return Q_NULLPTR;
}


void FixedPresentation::removeFromScene() {}


void FixedPresentation::processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    PresentationWithFixedChildren::processFormatChange(oldFormat, newFormat);

    formatChangedReported = true;
    reportedOldFormat     = oldFormat;
    reportedNewFormat     = newFormat;
}


void FixedPresentation::processDataChange() {
    PresentationWithFixedChildren::processDataChange();
    elementDataChangeReported = true;
}


void FixedPresentation::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    PresentationWithFixedChildren::processDiagnostic(diagnostic);
    diagnosticReportedReported = true;
    reportedDiagnostic         = diagnostic;
}


void FixedPresentation::clearDiagnosticDisplay() {
    PresentationWithFixedChildren::clearDiagnosticDisplay();
    diagnosticClearedReported = true;
}


void FixedPresentation::processPresentationRemoved(unsigned long childIndex, Presentation* childPresentation) {
    PresentationWithFixedChildren::processPresentationRemoved(childIndex, childPresentation);
    childPresentationRemovedReported = true;
    reportedChildIndex               = childIndex;
    reportedChildPresentation        = childPresentation;
}


void FixedPresentation::processChildPresentationSet(unsigned long childIndex, Presentation* childPresentation) {
    PresentationWithFixedChildren::processChildPresentationSet(childIndex, childPresentation);
    childPresentationSetReported = true;
    reportedChildIndex           = childIndex;
    reportedChildPresentation    = childPresentation;
}

/***********************************************************************************************************************
 * Class TestPresentationWithFixedChildren
 */

TestPresentationWithFixedChildren::TestPresentationWithFixedChildren() {
    eventLoop  = new QEventLoop(this);
    eventTimer = new QTimer(this);

    eventTimer->setSingleShot(true);

    connect(eventTimer, SIGNAL(timeout()), this, SLOT(terminateEventLoop()));
}


TestPresentationWithFixedChildren::~TestPresentationWithFixedChildren() {}


void TestPresentationWithFixedChildren::reportFormatChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    formatChangedReported = true;
    reportedOldFormat     = oldFormat;
    reportedNewFormat     = newFormat;
}


void TestPresentationWithFixedChildren::reportElementDataChanged() {
    elementDataChangeReported = true;
}


void TestPresentationWithFixedChildren::reportDiagnosticReported(Ld::DiagnosticPointer diagnostic) {
    diagnosticReportedReported = true;
    reportedDiagnostic         = diagnostic;
}


void TestPresentationWithFixedChildren::reportDiagnosticCleared() {
    diagnosticClearedReported = true;
}


void TestPresentationWithFixedChildren::reportChildPresentationRemoved(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationRemovedReported = true;
    reportedChildIndex               = childIndex;
    reportedChildPresentation        = childPresentation;
}


void TestPresentationWithFixedChildren::reportPresentationSet(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationSetReported = true;
    reportedChildIndex           = childIndex;
    reportedChildPresentation    = childPresentation;
}


void TestPresentationWithFixedChildren::terminateEventLoop() {
    eventLoop->quit();
}


void TestPresentationWithFixedChildren::initTestCase() {
    Ld::Element::setAutoDeleteVisuals(false);
    Ld::Handle::initialize(0x123456789ABCDEF0ULL);
}


void TestPresentationWithFixedChildren::testChildAddRemoveSignals() {
    FixedPresentation::clearNumberInstances();

    {
        clearStatus();

        QSharedPointer<FixedElement> parentElement(new FixedElement);
        parentElement->setWeakThis(parentElement.toWeakRef());

        FixedPresentation* parentPresentation = new FixedPresentation;
        parentElement->setVisual(parentPresentation);

        QSharedPointer<FixedElement> childElement(new FixedElement);
        childElement->setWeakThis(childElement.toWeakRef());

        FixedPresentation* childPresentation = new FixedPresentation;
        childElement->setVisual(childPresentation);

        connect(
            parentPresentation,
            SIGNAL(childPresentationSet(ulong,Presentation*)),
            this,
            SLOT(reportPresentationSet(ulong,Presentation*))
        );

        connect(
            parentPresentation,
            SIGNAL(childPresentationRemoved(ulong,Presentation*)),
            this,
            SLOT(reportChildPresentationRemoved(ulong,Presentation*))
        );

        bool success = parentElement->setChild(1, childElement, nullptr);
        QVERIFY(success);

        eventTimer->start(signalPropagationTime);
        eventLoop->exec();

        QVERIFY(childPresentationSetReported == true);
        QVERIFY(reportedChildIndex           == 1);
        QVERIFY(reportedChildPresentation    == childPresentation);

        QVERIFY(parentPresentation->childPresentationSet() == true);
        QVERIFY(parentPresentation->childIndex()           == 1);
        QVERIFY(parentPresentation->childPresentation()    == childPresentation);

        clearStatus();
        parentPresentation->clearStatus();

        parentElement->removeChild(1, nullptr);

        eventTimer->start(signalPropagationTime);
        eventLoop->exec();

        QVERIFY(childPresentationRemovedReported == true);
        QVERIFY(reportedChildIndex               == 1);
        QVERIFY(reportedChildPresentation        == childPresentation);

        QVERIFY(parentPresentation->childPresentationRemoved() == true);
        QVERIFY(parentPresentation->childIndex()               == 1);
        QVERIFY(parentPresentation->childPresentation()        == childPresentation);
    }

    // Confirm that the presentations get removed during the event loop.

    QVERIFY(FixedPresentation::numberInstances() == 2);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(FixedPresentation::numberInstances() == 0);
}


void TestPresentationWithFixedChildren::testFormatChangedSignal() {
    clearStatus();

    QSharedPointer<FixedElement> element(new FixedElement);
    element->setWeakThis(element.toWeakRef());

    FixedPresentation* presentation = new FixedPresentation;
    element->setVisual(presentation);

    connect(
        presentation,
        SIGNAL(formatWasChanged(Ld::FormatPointer, Ld::FormatPointer)),
        this,
        SLOT(reportFormatChanged(Ld::FormatPointer, Ld::FormatPointer))
    );

    Ld::CharacterFormat* format1 = new Ld::CharacterFormat;
    element->setFormat(format1);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(formatChangedReported == true);
    QVERIFY(reportedOldFormat     == nullptr);
    QVERIFY(reportedNewFormat     == format1);

    QVERIFY(presentation->formatWasChanged());
    QVERIFY(presentation->oldFormat() == nullptr);
    QVERIFY(presentation->newFormat() == format1);

    clearStatus();

    Ld::CharacterFormat* format2 = new Ld::CharacterFormat;
    element->setFormat(format2);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(formatChangedReported == true);
    QVERIFY(reportedOldFormat     == format1);
    QVERIFY(reportedNewFormat     == format2);

    QVERIFY(presentation->formatWasChanged());
    QVERIFY(presentation->oldFormat() == format1);
    QVERIFY(presentation->newFormat() == format2);
}


void TestPresentationWithFixedChildren::testDataChangedSignal() {
    clearStatus();

    QSharedPointer<FixedElement> element(new FixedElement);
    element->setWeakThis(element.toWeakRef());

    FixedPresentation* presentation = new FixedPresentation;
    element->setVisual(presentation);

    connect(
        presentation,
        SIGNAL(elementDataWasChanged()),
        this,
        SLOT(reportElementDataChanged())
    );

    element->dataChanged();

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(elementDataChangeReported == true);

    QVERIFY(presentation->elementDataWasChanged());
}


void TestPresentationWithFixedChildren::testDiagnosticReportedSignal() {
    clearStatus();

    QSharedPointer<FixedElement> element(new FixedElement);
    element->setWeakThis(element.toWeakRef());

    FixedPresentation* presentation = new FixedPresentation;
    element->setVisual(presentation);

    connect(
        presentation,
        SIGNAL(diagnosticWasReported(Ld::DiagnosticPointer)),
        this,
        SLOT(reportDiagnosticReported(Ld::DiagnosticPointer))
    );

    Ld::CppTranslationPhase translationPhase;
    QSharedPointer<Ld::CppCodeGeneratorDiagnostic> diagnostic(
        new Ld::CppCodeGeneratorDiagnostic(
            element,
            Ld::Diagnostic::Type::INTERNAL_ERROR,
            translationPhase,
            Ld::CppCodeGeneratorDiagnostic::Code::COMPILER_DIAGNOSTIC
        )
    );

    element->flagDiagnostic(diagnostic);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(diagnosticReportedReported == true);
    QVERIFY(reportedDiagnostic == diagnostic);

    QVERIFY(presentation->diagnosticWasReported() == true);
    QVERIFY(presentation->diagnostic() == diagnostic);
}


void TestPresentationWithFixedChildren::testDiagnosticClearedSignal() {
    clearStatus();

    QSharedPointer<FixedElement> element(new FixedElement);
    element->setWeakThis(element.toWeakRef());

    FixedPresentation* presentation = new FixedPresentation;
    element->setVisual(presentation);

    connect(presentation, SIGNAL(diagnosticWasCleared()), this, SLOT(reportDiagnosticCleared()));

    element->clearDiagnostic();

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(diagnosticClearedReported == true);
    QVERIFY(presentation->diagnosticWasCleared() == true);
}


void TestPresentationWithFixedChildren::clearStatus() {
    formatChangedReported            = false;
    reportedOldFormat                = nullptr;
    reportedNewFormat                = nullptr;

    elementDataChangeReported        = false;

    diagnosticReportedReported       = false;
    reportedDiagnostic.reset();

    diagnosticClearedReported        = false;

    childPresentationRemovedReported = false;
    childPresentationSetReported     = false;

    reportedChildIndex               = static_cast<unsigned long>(-1);
    reportedChildPresentation        = nullptr;
}
