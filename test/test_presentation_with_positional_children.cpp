/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PresentationWithPositionalChildren class.
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
#include <ld_data_type.h>
#include <ld_capabilities.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_diagnostic.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_cpp_translation_phase.h>

#include <presentation.h>
#include <presentation_with_positional_children.h>

#include "test_presentation_with_positional_children.h"

/***********************************************************************************************************************
 * PositionalElement:
 */

class PositionalElement:public Ld::ElementWithPositionalChildren {
    public:
        static constexpr unsigned forcedNumberChildren = 3;

        PositionalElement();

        ~PositionalElement() override;

        QString typeName() const final;

        QString plugInName() const final;

        QString description() const final;

        Ld::DataType::ValueType valueType() const final;

        Ld::Capabilities parentRequires(unsigned long index) const final;

        Ld::Capabilities childProvidesCapabilities() const final;

        void dataChanged();
};


PositionalElement::PositionalElement() {}


PositionalElement::~PositionalElement() {}


QString PositionalElement::typeName() const {
    return QString("PositionalElement");
}


QString PositionalElement::plugInName() const {
    return QString();
}


QString PositionalElement::description() const {
    return QString();
}


Ld::DataType::ValueType PositionalElement::valueType() const {
    return Ld::DataType::ValueType::NONE;
}


Ld::Capabilities PositionalElement::parentRequires(unsigned long) const {
    return Ld::Capabilities();
}


Ld::Capabilities PositionalElement::childProvidesCapabilities() const {
    return Ld::Capabilities();
}

/***********************************************************************************************************************
 * PositionalPresentation:
 */

PositionalPresentation::PositionalPresentation() {}


PositionalPresentation::~PositionalPresentation() {}


QString PositionalPresentation::typeName() const {
    return QString("PositionalElement");
}


QString PositionalPresentation::plugInName() const {
    return QString();
}


bool PositionalPresentation::childPresentationRemoved() const {
    return childPresentationRemovedReported;
}


bool PositionalPresentation::childPresentationInsertedBefore() const {
    return childPresentationInsertedBeforeReported;
}


bool PositionalPresentation::childPresentationInsertedAfter() const {
    return childPresentationInsertedAfterReported;
}


unsigned long PositionalPresentation::childIndex() const {
    return reportedChildIndex;
}


Presentation* PositionalPresentation::childPresentation() const {
    return reportedChildPresentation;
}


void PositionalPresentation::clearStatus() {
    childPresentationRemovedReported        = false;
    childPresentationInsertedBeforeReported = false;
    childPresentationInsertedAfterReported  = false;

    reportedChildIndex               = static_cast<unsigned long>(-1);
    reportedChildPresentation        = nullptr;
}


void PositionalPresentation::requestRepositioning(Presentation*) {}


void PositionalPresentation::recalculatePlacement(
        PlacementTracker*,
        PlacementNegotiator*,
        unsigned long,
        Presentation*,
        bool,
        float,
        float,
        float
    ) {}


void PositionalPresentation::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {}


QSizeF PositionalPresentation::requestArea(unsigned long, PositionalPresentation::SpaceQualifier*) {
    return QSizeF();
}


void PositionalPresentation::allocateArea(unsigned long, unsigned long, const QSizeF&, float, bool) {}


void PositionalPresentation::areaInsufficient(unsigned long, const QSizeF&) {}


void PositionalPresentation::applyStretch(unsigned long, float) {}


QGraphicsItem* PositionalPresentation::graphicsItem(unsigned long) const {
    return Q_NULLPTR;
}


void PositionalPresentation::removeFromScene() {}


void PositionalPresentation::processRemovingChildPresentation(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationRemovedReported = true;
    reportedChildIndex               = childIndex;
    reportedChildPresentation        = childPresentation;
}


void PositionalPresentation::processChildPresentationInsertedBefore(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationInsertedBeforeReported = true;
    reportedChildIndex                      = childIndex;
    reportedChildPresentation               = childPresentation;
}


void PositionalPresentation::processChildPresentationInsertedAfter(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationInsertedAfterReported = true;
    reportedChildIndex                     = childIndex;
    reportedChildPresentation              = childPresentation;
}

/***********************************************************************************************************************
 * Class TestPresentationWithPositionalChildren
 */

TestPresentationWithPositionalChildren::TestPresentationWithPositionalChildren() {
    eventLoop  = new QEventLoop(this);
    eventTimer = new QTimer(this);

    eventTimer->setSingleShot(true);

    connect(eventTimer, SIGNAL(timeout()), this, SLOT(terminateEventLoop()));
}


TestPresentationWithPositionalChildren::~TestPresentationWithPositionalChildren() {}


void TestPresentationWithPositionalChildren::reportChildPresentationRemoved(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationRemovedReported = true;
    reportedChildIndex               = childIndex;
    reportedChildPresentation        = childPresentation;
}


void TestPresentationWithPositionalChildren::reportChildPresentationInsertedBefore(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationInsertedBeforeReported = true;
    reportedChildIndex                      = childIndex;
    reportedChildPresentation               = childPresentation;
}


void TestPresentationWithPositionalChildren::reportChildPresentationInsertedAfter(
        unsigned long childIndex,
        Presentation* childPresentation
    ) {
    childPresentationInsertedAfterReported = true;
    reportedChildIndex                     = childIndex;
    reportedChildPresentation              = childPresentation;
}


void TestPresentationWithPositionalChildren::terminateEventLoop() {
    eventLoop->quit();
}


void TestPresentationWithPositionalChildren::initTestCase() {
    Ld::Element::setAutoDeleteVisuals(false);
    Ld::Handle::initialize(0x123456789ABCDEF0ULL);
}


void TestPresentationWithPositionalChildren::testChildAddRemoveSignals() {
    clearStatus();

    QSharedPointer<PositionalElement> parentElement(new PositionalElement);
    parentElement->setWeakThis(parentElement.toWeakRef());

    PositionalPresentation* parentPresentation = new PositionalPresentation;
    parentElement->setVisual(parentPresentation);

    QSharedPointer<PositionalElement> child1Element(new PositionalElement);
    child1Element->setWeakThis(child1Element.toWeakRef());

    PositionalPresentation* child1Presentation = new PositionalPresentation;
    child1Element->setVisual(child1Presentation);

    QSharedPointer<PositionalElement> child2Element(new PositionalElement);
    child2Element->setWeakThis(child2Element.toWeakRef());

    PositionalPresentation* child2Presentation = new PositionalPresentation;
    child2Element->setVisual(child2Presentation);

    connect(
        parentPresentation,
        SIGNAL(removingChildPresentation(unsigned long,Presentation*)),
        this,
        SLOT(reportChildPresentationRemoved(unsigned long,Presentation*))
    );

    connect(
        parentPresentation,
        SIGNAL(childPresentationNowInsertedBefore(unsigned long,Presentation*)),
        this,
        SLOT(reportChildPresentationInsertedBefore(unsigned long,Presentation*))
    );

    connect(
        parentPresentation,
        SIGNAL(childPresentationNowInsertedAfter(unsigned long,Presentation*)),
        this,
        SLOT(reportChildPresentationInsertedAfter(unsigned long,Presentation*))
    );

    parentElement->prepend(child1Element, nullptr);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(childPresentationInsertedBeforeReported == true);
    QVERIFY(reportedChildIndex                      == 0);
    QVERIFY(reportedChildPresentation               == child1Presentation);

    QVERIFY(parentPresentation->childPresentationInsertedBefore() == true);
    QVERIFY(parentPresentation->childIndex()                      == 0);
    QVERIFY(parentPresentation->childPresentation()               == child1Presentation);

    clearStatus();
    parentPresentation->clearStatus();

    parentElement->append(child2Element, nullptr);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(childPresentationInsertedAfterReported == true);
    QVERIFY(reportedChildIndex                     == 0);
    QVERIFY(reportedChildPresentation              == child2Presentation);

    QVERIFY(parentPresentation->childPresentationInsertedAfter() == true);
    QVERIFY(parentPresentation->childIndex()                     == 0);
    QVERIFY(parentPresentation->childPresentation()              == child2Presentation);

    clearStatus();
    parentPresentation->clearStatus();

    parentElement->removeChild(1, nullptr);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(childPresentationRemovedReported == true);
    QVERIFY(reportedChildIndex               == 1);
    QVERIFY(reportedChildPresentation        == child2Presentation);

    QVERIFY(parentPresentation->childPresentationRemoved() == true);
    QVERIFY(parentPresentation->childIndex()               == 1);
    QVERIFY(parentPresentation->childPresentation()        == child2Presentation);
}


void TestPresentationWithPositionalChildren::clearStatus() {
    childPresentationRemovedReported        = false;
    childPresentationInsertedBeforeReported = false;
    childPresentationInsertedAfterReported  = false;

    reportedChildIndex                      = static_cast<unsigned long>(-1);
    reportedChildPresentation               = nullptr;
}
