/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PresentationWithFloatingChildren class.
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
#include <ld_element.h>
#include <ld_element_with_floating_children.h>
#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_diagnostic.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_cpp_translation_phase.h>

#include <presentation.h>
#include <presentation_with_floating_children.h>

#include "test_presentation_with_floating_children.h"

/***********************************************************************************************************************
 * FloatingElement:
 */

class FloatingElement:public Ld::ElementWithFloatingChildren {
    public:
        static constexpr unsigned forcedNumberChildren = 3;

        FloatingElement();

        ~FloatingElement() override;

        QString typeName() const final;

        QString plugInName() const final;

        QString description() const final;

        Ld::DataType::ValueType valueType() const final;

        Ld::Capabilities parentRequires(unsigned long index) const final;

        Ld::Capabilities childProvidesCapabilities() const final;

        void dataChanged();
};


FloatingElement::FloatingElement() {}


FloatingElement::~FloatingElement() {}


QString FloatingElement::typeName() const {
    return QString("FloatingElement");
}


QString FloatingElement::plugInName() const {
    return QString();
}


QString FloatingElement::description() const {
    return QString();
}


Ld::DataType::ValueType FloatingElement::valueType() const {
    return Ld::DataType::ValueType::NONE;
}


Ld::Capabilities FloatingElement::parentRequires(unsigned long) const {
    return Ld::Capabilities();
}


Ld::Capabilities FloatingElement::childProvidesCapabilities() const {
    return Ld::Capabilities();
}

/***********************************************************************************************************************
 * FloatingPresentation:
 */

FloatingPresentation::FloatingPresentation() {}


FloatingPresentation::~FloatingPresentation() {}


QString FloatingPresentation::typeName() const {
    return QString("FloatingElement");
}


QString FloatingPresentation::plugInName() const {
    return QString();
}


bool FloatingPresentation::childPresentationRemoved() const {
    return childPresentationRemovedReported;
}


bool FloatingPresentation::childPresentationInserted() const {
    return childPresentationInsertedReported;
}


bool FloatingPresentation::childPresentationRelocated() const {
    return childPresentationRelocatedReported;
}


QPointF FloatingPresentation::childLocation() const {
    return reportedChildLocation;
}


QPointF FloatingPresentation::oldChildLocation() const {
    return reportedOldChildLocation;
}


Presentation* FloatingPresentation::childPresentation() const {
    return reportedChildPresentation;
}


void FloatingPresentation::clearStatus() {
    childPresentationRemovedReported   = false;
    childPresentationInsertedReported  = false;
    childPresentationRelocatedReported = false;

    reportedChildLocation              = QPointF();
    reportedOldChildLocation           = QPointF();
    reportedChildPresentation          = nullptr;
}


void FloatingPresentation::requestRepositioning(Presentation*) {}


void FloatingPresentation::recalculatePlacement(
        PlacementTracker*,
        PlacementNegotiator*,
        unsigned long,
        Presentation*,
        bool,
        float,
        float,
        float
    ) {}


void FloatingPresentation::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {}


QSizeF FloatingPresentation::requestArea(unsigned long, FloatingPresentation::SpaceQualifier*) {
    return QSizeF();
}


void FloatingPresentation::allocateArea(unsigned long, unsigned long, const QSizeF&, float, bool) {}


void FloatingPresentation::areaInsufficient(unsigned long, const QSizeF&) {}


void FloatingPresentation::applyStretch(unsigned long, float) {}


QGraphicsItem* FloatingPresentation::graphicsItem(unsigned long) const {
    return Q_NULLPTR;
}


void FloatingPresentation::removeFromScene() {}


void FloatingPresentation::processRemovingChildPresentation(const QPointF& location, Presentation*  childPresentation) {
    childPresentationRemovedReported = true;
    reportedChildLocation            = location;
    reportedChildPresentation        = childPresentation;
}


void FloatingPresentation::processChildPresentationInserted(Presentation* childPresentation, const QPointF& location) {
    childPresentationInsertedReported = true;
    reportedChildLocation             = location;
    reportedChildPresentation         = childPresentation;
}


void FloatingPresentation::processChildPresentationRelocated(
        Presentation*  childPresentation,
        const QPointF& oldLocation,
        const QPointF& newLocation
    ) {
    childPresentationRelocatedReported = true;
    reportedOldChildLocation           = oldLocation;
    reportedChildLocation              = newLocation;
    reportedChildPresentation          = childPresentation;
}

/***********************************************************************************************************************
 * Class TestPresentationWithFloatingChildren
 */

TestPresentationWithFloatingChildren::TestPresentationWithFloatingChildren() {
    eventLoop  = new QEventLoop(this);
    eventTimer = new QTimer(this);

    eventTimer->setSingleShot(true);

    connect(eventTimer, SIGNAL(timeout()), this, SLOT(terminateEventLoop()));
}


TestPresentationWithFloatingChildren::~TestPresentationWithFloatingChildren() {}


void TestPresentationWithFloatingChildren::reportChildPresentationRemoved(
        const QPointF& location,
        Presentation*  childPresentation
    ) {
    childPresentationRemovedReported = true;
    reportedChildLocation            = location;
    reportedChildPresentation        = childPresentation;
}


void TestPresentationWithFloatingChildren::reportChildPresentationInserted(
        Presentation*  childPresentation,
        const QPointF& location
    ) {
    childPresentationInsertedReported = true;
    reportedChildLocation             = location;
    reportedChildPresentation         = childPresentation;
}


void TestPresentationWithFloatingChildren::reportChildPresentationRelocated(
        Presentation*  childPresentation,
        const QPointF& oldLocation,
        const QPointF& newLocation
    ) {
    childPresentationRelocatedReported = true;
    reportedChildLocation              = newLocation;
    reportedOldLocation                = oldLocation;
    reportedChildPresentation          = childPresentation;
}


void TestPresentationWithFloatingChildren::terminateEventLoop() {
    eventLoop->quit();
}


void TestPresentationWithFloatingChildren::initTestCase() {
    Ld::Element::setAutoDeleteVisuals(false);
    Ld::Handle::initialize(0x123456789ABCDEF0ULL);
}


void TestPresentationWithFloatingChildren::testChildAddRemoveSignals() {
    clearStatus();

    QSharedPointer<FloatingElement> parentElement(new FloatingElement);
    parentElement->setWeakThis(parentElement.toWeakRef());

    FloatingPresentation* parentPresentation = new FloatingPresentation;
    parentElement->setVisual(parentPresentation);

    QSharedPointer<FloatingElement> child1Element(new FloatingElement);
    child1Element->setWeakThis(child1Element.toWeakRef());

    FloatingPresentation* child1Presentation = new FloatingPresentation;
    child1Element->setVisual(child1Presentation);

    connect(
        parentPresentation,
        SIGNAL(removingChildPresentation(const QPointF&, Presentation*)),
        this,
        SLOT(reportChildPresentationRemoved(const QPointF&, Presentation*))
    );

    connect(
        parentPresentation,
        SIGNAL(childPresentationNowInserted(Presentation*, const QPointF&)),
        this,
        SLOT(reportChildPresentationInserted(Presentation*, const QPointF&))
    );

    connect(
        parentPresentation,
        SIGNAL(childPresentationNowRelocated(Presentation*, const QPointF&, const QPointF&)),
        this,
        SLOT(reportChildPresentationRelocated(Presentation*, const QPointF&, const QPointF&))
    );

    parentElement->insertChild(child1Element, 1, 1, nullptr);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(childPresentationInsertedReported == true);
    QVERIFY(reportedChildLocation             == QPointF(1, 1));
    QVERIFY(reportedChildPresentation         == child1Presentation);

    QVERIFY(parentPresentation->childPresentationInserted() == true);
    QVERIFY(parentPresentation->childLocation()             == QPointF(1, 1));
    QVERIFY(parentPresentation->childPresentation()         == child1Presentation);

    clearStatus();
    parentPresentation->clearStatus();

    parentElement->relocateChild(child1Element, 2, 3, nullptr);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(childPresentationRelocatedReported == true);
    QVERIFY(reportedOldLocation                == QPointF(1, 1));
    QVERIFY(reportedChildLocation              == QPointF(2, 3));
    QVERIFY(reportedChildPresentation          == child1Presentation);

    QVERIFY(parentPresentation->childPresentationRelocated() == true);
    QVERIFY(parentPresentation->oldChildLocation()           == QPointF(1, 1));
    QVERIFY(parentPresentation->childLocation()              == QPointF(2, 3));
    QVERIFY(parentPresentation->childPresentation()          == child1Presentation);

    clearStatus();
    parentPresentation->clearStatus();

    parentElement->removeChild(child1Element, nullptr);

    eventTimer->start(signalPropagationTime);
    eventLoop->exec();

    QVERIFY(childPresentationRemovedReported == true);
    QVERIFY(reportedChildLocation            == QPointF(2, 3));
    QVERIFY(reportedChildPresentation        == child1Presentation);

    QVERIFY(parentPresentation->childPresentationRemoved() == true);
    QVERIFY(parentPresentation->childLocation()            == QPointF(2, 3));
    QVERIFY(parentPresentation->childPresentation()        == child1Presentation);
}


void TestPresentationWithFloatingChildren::clearStatus() {
    childPresentationRemovedReported   = false;
    childPresentationInsertedReported  = false;
    childPresentationRelocatedReported = false;

    reportedChildLocation              = QPointF();
    reportedOldLocation                = QPointF();
    reportedChildPresentation          = nullptr;
}
