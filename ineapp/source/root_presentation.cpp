/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RootPresentation class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <QRect>
#include <QRectF>
#include <QTimer>

#include <QDebug> // Debug

#include <cmath>

#include <eqt_graphics_scene.h>
#include <eqt_graphics_item_group.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_page_format.h>
#include <ld_element.h>
#include <ld_element_cursor.h>
#include <ld_root_element.h>
#include <ld_visual_with_positional_children.h>
#include <ld_root_visual.h>

#include "root_child_location.h"
#include "container_status.h"
#include "presentation.h"
#include "page_list.h"
#include "placement_tracker.h"
#include "placement_status_notifier.h"
#include "placement_negotiator.h"
#include "root_presentation.h"

RootPresentation::RootPresentation(QObject* parent):EQt::GraphicsScene(parent) {
    connect(
        this,
        SIGNAL(presentationChanged(Presentation*)),
        SLOT(processPresentationChanged(Presentation*)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(presentationAdded(Presentation*)),
        SLOT(processPresentationAdded(Presentation*)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(presentationRemoved(Presentation*)),
        SLOT(processPresentationRemoved(Presentation*)),
        Presentation::connectionType
    );

    connect(this, SIGNAL(programNowPristine()), SLOT(processNowPristine()), Presentation::connectionType);
    connect(this, SIGNAL(programNowModified()), SLOT(processNowModified()), Presentation::connectionType);
    connect(this, SIGNAL(programNowChanged()), SLOT(processNowChanged()), Presentation::connectionType);

    connect(
        this,
        SIGNAL(pageFormatHasChanged(unsigned long,QSharedPointer<Ld::PageFormat>,QSharedPointer<Ld::PageFormat>)),
        SLOT(processPageFormatChange(unsigned long,QSharedPointer<Ld::PageFormat>,QSharedPointer<Ld::PageFormat>)),
        Presentation::connectionType
    );
    connect(this, SIGNAL(referencesHaveChanged()), SLOT(processReferenceChanges()), Presentation::connectionType);

    connect(
        this,
        SIGNAL(programWasSaved(const QString&)),
        SLOT(processProgramSaved(const QString&)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(programWasSavedAs(const QString&)),
        SLOT(processProgramSavedAs(const QString&)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(programSaveHasFailed(const QString&)),
        SLOT(processProgramSaveFailed(const QString&)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(programWasLoaded(const QString&, unsigned, unsigned)),
        SLOT(processProgramLoaded(const QString&, unsigned, unsigned)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(programLoadHasFailed(const QString&)),
        SLOT(processProgramLoadFailed(const QString&)),
        Presentation::connectionType
    );

    connect(this, SIGNAL(newProgramWasCreated()), SLOT(processNewProgramCreated()), Presentation::connectionType);

    connect(this, SIGNAL(programClosed()), SLOT(processProgramClosed()), Presentation::connectionType);

    connect(
        this,
        SIGNAL(removingChildPresentation(unsigned long, Presentation*)),
        SLOT(processRemovingChildPresentation(unsigned long, Presentation*)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationNowInsertedBefore(unsigned long, Presentation*)),
        SLOT(processChildPresentationInsertedBefore(unsigned long ,Presentation*)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationNowInsertedAfter(unsigned long, Presentation*)),
        SLOT(processChildPresentationInsertedAfter(unsigned long, Presentation*)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(nowTiedToElement(Ld::ElementPointer)),
        this,
        SLOT(tiedToElement(Ld::ElementPointer)),
        Presentation::connectionType
    );

    connect(this, SIGNAL(nowUntiedFromElement()), this, SLOT(untiedFromElement()), Presentation::connectionType);

    connect(
        this,
        SIGNAL(formatWasChanged(Ld::FormatPointer,Ld::FormatPointer)),
        SLOT(processFormatChange(Ld::FormatPointer,Ld::FormatPointer)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(descendantFormatWasChanged(Ld::ElementPointer,Ld::FormatPointer)),
        SLOT(processDescendantFormatChange(Ld::ElementPointer,Ld::FormatPointer)),
        Presentation::connectionType
    );

    connect(
        this,
        SIGNAL(descendantFormatWasUpdated(Ld::ElementPointer,Ld::FormatPointer)),
        SLOT(processDescendantFormatUpdate(Ld::ElementPointer,Ld::FormatPointer)),
        Presentation::connectionType
    );

    connect(this, SIGNAL(elementDataWasChanged()), SLOT(processDataChange()), Presentation::connectionType);

    connect(
        this,
        SIGNAL(diagnosticWasReported(Ld::DiagnosticPointer)),
        SLOT(processDiagnostic(Ld::DiagnosticPointer)),
        Presentation::connectionType
    );

    connect(this, SIGNAL(diagnosticWasCleared()), SLOT(clearDiagnosticDisplay()), Presentation::connectionType);

    connect(
        &pageList,
        SIGNAL(maximumExtentsChanged(const QList<double>&, const QList<double>&)),
        this,
        SLOT(processPageMaximumExtentsChanged(const QList<double>&, const QList<double>&))
    );

//    connect(&pageList, SIGNAL(pageFormatChanged(Entry)), this, SIGNAL(pageResized(Entry)));

    repositionTimer = new QTimer(this);
    repositionTimer->setSingleShot(true);
    connect(repositionTimer, SIGNAL(timeout()), SLOT(performRepositioning()));

    placementStatusNotifier = new PlacementStatusNotifier(this);

    firstChildForRepositioning         = static_cast<unsigned long>(-1);
    lastChildForRepositioning          = static_cast<unsigned long>(-1);
    lastFullyRenderedChildPresentation = 0;
    recalculateAllChildPositions       = true;
    repositionInProgress               = false;
    repositionRequestPending           = false;

    currentMaximumHorizontalExtentPoints = 0;
    currentPresentationUpdatesPending    = false;

    // The line below was added to address a regression in the QGraphicsView/QGraphicsScene framework.  When a
    // QGraphicsItem is removed, the BSP tree is not updated properly.  When the tree is re-indexed, stale
    // entries remain and cause a segmentation fault.
    //
    // Since we don't use the BSP tree for search purposes, we simply disable it.

    setItemIndexMethod(QGraphicsScene::NoIndex);
}


RootPresentation::~RootPresentation() {}


Ld::Visual* RootPresentation::creator(const QString&) {
    return new RootPresentation();
}


void RootPresentation::setNumberPageColumns(unsigned newNumberPageColumns) {
    pageList.setMaximumNumberColumns(newNumberPageColumns);
}


unsigned RootPresentation::numberPageColumns() const {
    return pageList.maximumNumberColumns();
}


QList<double> RootPresentation::maximumHorizontalExtentPoints() const {
    return pageList.maximumHorizontalExtentPoints();
}


QList<double> RootPresentation::pageColumnLeftEdgePoints() const {
    return pageList.columnLeftEdgePoints();
}


void RootPresentation::drawPageDecorations(
        const QRectF& sceneArea,
        QPainter*     painter,
        bool          includeMargins,
        bool          includeGuides
    ) {
    pageList.drawDecorations(sceneArea, painter, includeMargins, includeGuides);
}


Ld::Element::ChildPlacement RootPresentation::childPlacement() const {
    return Ld::Element::ChildPlacement::POSITIONAL;
}


void RootPresentation::requestRepositioning(Presentation* childPresentation) {
    QSharedPointer<Ld::RootElement> rootElement    = element();
    Ld::ElementPointer              childElement   = childPresentation->element();
    unsigned long                   childIndex     = 0;
    unsigned long                   numberChildren = rootElement->numberChildren();

    while (childIndex < numberChildren && rootElement->child(childIndex) != childElement) {
        ++childIndex;
    }

    if (childIndex < numberChildren) {
        requestRepositioning(childIndex);
    }
}


void RootPresentation::recalculatePlacement(
        PlacementTracker*,
        PlacementNegotiator*,
        unsigned long,
        Presentation*,
        bool,
        float,
        float,
        float
    ) {
    Q_ASSERT(false);
}


void RootPresentation::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {
    Q_ASSERT(false);
}


QSizeF RootPresentation::requestArea(unsigned long, RootPresentation::SpaceQualifier* spaceQuantifier) {
    if (spaceQuantifier != Q_NULLPTR) {
        *spaceQuantifier =   cursorY == currentActiveArea.top()
                           ? SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT
                           : SpaceQualifier::MAXIMUM_WIDTH;
    }

    return QSizeF(currentActiveArea.width(), currentActiveArea.bottom() - cursorY);
}


void RootPresentation::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         /* ascent */,
        bool          /* canStretch */
    ) {
    Ld::ElementPointer childElement      = element()->child(childIdentifier);
    Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    QGraphicsItem*     graphicsItem      = childPresentation->graphicsItem(presentationAreaId);

    pageList.at(currentPageIndex).activeAreaGroup()->addToGroup(graphicsItem);
    graphicsItem->setPos(currentActiveArea.left(), cursorY);

    areaInsufficient(childIdentifier, size);
}


void RootPresentation::areaInsufficient(unsigned long childIdentifier, const QSizeF& size) {
    cursorY += size.height();

    currentChildLocations[childIdentifier].setBottomLocation(currentPageIndex, cursorY);

    if (cursorY >= currentActiveArea.bottom()) {
        ++currentPageIndex;
        currentActiveArea = activeAreaRectangle(currentPageIndex);
        cursorY = currentActiveArea.top();
    }
}


void RootPresentation::applyStretch(unsigned long, float) {
    Q_ASSERT(false);
}


QGraphicsItem* RootPresentation::graphicsItem(unsigned long) const {
    Q_ASSERT(false);
    return Q_NULLPTR;
}


QSizeF RootPresentation::largestAvailableArea() const {
    QSizeF   result;

    unsigned nextPageIndex = currentPageIndex + 1;
    if (nextPageIndex < pageList.numberPages()) {
        result = pageList.at(nextPageIndex).activeAreaSizeSceneUnits();
    } else {
        QSharedPointer<Ld::RootElement> rootElement = element();
        QSharedPointer<Ld::PageFormat>  pageFormat;
        if (nextPageIndex < rootElement->numberPages()) {
            pageFormat = rootElement->pageFormat(nextPageIndex);
        } else {
            pageFormat = rootElement->defaultPageFormat();
        }

        QSizeF pageSizePoints(pageFormat->horzontalActiveArea(), pageFormat->verticalActiveArea());
        result = toScene(pageSizePoints);
    }

    return result;
}


bool RootPresentation::arePresentationUpdatesPending() const {
    return currentPresentationUpdatesPending;
}


QList<QGraphicsView*> RootPresentation::viewsShowingElement(Ld::ElementPointer element) {
    PlacementNegotiator*  placementNegotiator = dynamic_cast<PlacementNegotiator*>(element->visual());
    QList<QGraphicsView*> allViews            = views();
    QList<QGraphicsView*> overlappingViews;

    for (QList<QGraphicsView*>::const_iterator it=allViews.constBegin(),end=allViews.constEnd(); it!=end ; ++it) {
        QGraphicsView* view           = *it;
        QRect          viewportWindow = view->viewport()->rect();
        QPointF        upperLeft      = view->mapToScene(viewportWindow.topLeft());
        QPointF        lowerRight     = view->mapToScene(viewportWindow.bottomRight());
        QRectF         sceneRectangle(upperLeft, lowerRight);

        unsigned       presentationAreaId = 0;
        bool           itemInViewport     = false;
        QGraphicsItem* item;
        do {
            item = placementNegotiator->graphicsItem(presentationAreaId);
            ++presentationAreaId;

            if (item != nullptr) {
                QRectF sceneBoundingRect = item->sceneBoundingRect();
                itemInViewport = sceneBoundingRect.intersects(sceneRectangle);
            }
        } while (item != nullptr && !itemInViewport);

        if (itemInViewport) {
            overlappingViews << view;
        }
    }

    return overlappingViews;
}


PageList::Index RootPresentation::pageAt(const QPointF& position) const {
    PageList::Index pageIndex;

    PageList::Entry page = pageList.pageAt(position);
    if (page.isValid()) {
        pageIndex = page.index();
    } else {
        pageIndex = PageList::invalidPageIndex;
    }

    return pageIndex;
}


QSet<PageList::Index> RootPresentation::pagesContainingPresentation(Presentation* presentation) const {
    QSet<PageList::Index> result;

    unsigned       presentationAreaId = 0;
    QGraphicsItem* graphicsItem       = presentation->graphicsItem(presentationAreaId);
    while (graphicsItem != Q_NULLPTR) {
        QRectF          boundingRect = graphicsItem->sceneBoundingRect();
        PageList::Entry page         = pageList.pageAt(boundingRect.center());

        if (page.isValid()) {
            result.insert(page.index());
        }

        ++presentationAreaId;
        graphicsItem = presentation->graphicsItem(presentationAreaId);
    }

    return result;
}


QRectF RootPresentation::extentsSceneUnits(PageList::Index pageIndex) const {
    QRectF result;

    if (pageIndex < static_cast<PageList::Index>(pageList.numberPages())) {
        PageList::Entry pageEntry = pageList.at(pageIndex);
        result = pageEntry.extentsSceneUnits().translated(pageEntry.position());
    }

    return result;
}


QRectF RootPresentation::boundarySceneUnits(PageList::Index pageIndex) const {
    QRectF result;

    if (pageIndex < static_cast<PageList::Index>(pageList.numberPages())) {
        PageList::Entry pageEntry = pageList.at(pageIndex);
        result = pageEntry.boundarySceneUnits().translated(pageEntry.position());
    }

    return result;
}


QRectF RootPresentation::activeAreaSceneUnits(PageList::Index pageIndex) const {
    QRectF result;

    if (pageIndex < static_cast<PageList::Index>(pageList.numberPages())) {
        PageList::Entry pageEntry = pageList.at(pageIndex);
        result = pageEntry.activeAreaSceneUnits().translated(pageEntry.position());
    }

    return result;
}


QSharedPointer<Ld::DocumentSetting> RootPresentation::documentSettingByName(const QString& settingName) const {
    QSharedPointer<Ld::RootElement> rootElement = element();
    return rootElement->documentSettings().value(settingName);
}


void RootPresentation::addDocumentSetting(QSharedPointer<Ld::DocumentSetting> newSetting) {
    QSharedPointer<Ld::RootElement> rootElement = element();
    rootElement->documentSettings().insert(newSetting);
}


double RootPresentation::distanceToClosestPresentationArea(
        const QPointF&       location,
        const Presentation** bestPresentation,
        unsigned long*       presentationAreaId,
        QPointF*             closestPoint
    ) const {
    Ld::ElementPointer  rootElement    = element();
    unsigned long       numberChildren = static_cast<unsigned long>(currentChildLocations.size());
    PageList::Index     pageAtLocation = pageAt(location);

    double              closestDistance           = std::numeric_limits<double>::max();
    const Presentation* closestPresentation       = Q_NULLPTR;
    unsigned long       closestPresentationAreaId = std::numeric_limits<unsigned long>::max();
    QPointF             closestPointOnChild;

    if (pageAtLocation != PageList::invalidPageIndex) {
        unsigned long       childIndex;
        if (pageAtLocation > 0) {
            unsigned long   upperChildIndex = lastFullyRenderedChildPresentation;
            if (upperChildIndex >= static_cast<unsigned long>(currentChildLocations.size())) {
                upperChildIndex = static_cast<unsigned long>(currentChildLocations.size()) - 1;
            }

            PageList::Index upperPageIndex  = currentChildLocations.at(upperChildIndex).topPageIndex();

            if (pageAtLocation > upperPageIndex) {
                // This scenario can happen if we abort rendering and then click on an item that's only
                // partially rendered and thus takes significantly less space than was originally calculated.

                childIndex     = upperChildIndex;
                pageAtLocation = upperPageIndex;
            } else {
                unsigned long   lowerChildIndex  = 0;
                PageList::Index lowerPageIndex   = currentChildLocations.at(lowerChildIndex).bottomPageIndex();
                unsigned long   middleChildIndex = (lowerChildIndex + upperChildIndex) / 2;
                PageList::Index middlePageIndex  = currentChildLocations.at(middleChildIndex).bottomPageIndex();

                unsigned long lastLowerPageIndex = lowerPageIndex;

                while (middlePageIndex != pageAtLocation &&
                       lowerPageIndex <= middlePageIndex &&
                       lowerPageIndex <= pageAtLocation     ) {
                    if (middlePageIndex < pageAtLocation) {
                        if (middlePageIndex == lastLowerPageIndex) {
                            ++middleChildIndex;
                            if (middleChildIndex < numberChildren) {
                                lowerChildIndex = middleChildIndex;
                            } else {
                                lowerChildIndex = numberChildren - 1;
                            }

                            lowerPageIndex  = currentChildLocations.at(lowerChildIndex).bottomPageIndex();
                        } else {
                            lowerChildIndex = middleChildIndex;
                            lowerPageIndex  = middlePageIndex;
                        }

                        lastLowerPageIndex = lowerPageIndex;
                    } else {
                        upperChildIndex = middleChildIndex;
                        upperPageIndex  = middlePageIndex;
                    }

                    middleChildIndex = (lowerChildIndex + upperChildIndex) / 2;
                    middlePageIndex  = currentChildLocations.at(middleChildIndex).bottomPageIndex();
                }

                childIndex = middleChildIndex;
            }

            while (childIndex > 0 && currentChildLocations.at(childIndex - 1).bottomPageIndex() == pageAtLocation) {
                --childIndex;
            }
        } else {
            childIndex = 0;
        }

        do {
            Ld::ElementPointer  childElement      = rootElement->child(childIndex);
            const Presentation* childPresentation = dynamic_cast<const Presentation*>(childElement->visual());
            if (childPresentation != Q_NULLPTR) {
                const Presentation* bestChildPresentation;
                unsigned long       bestChildPresentationAreaId;
                QPointF             bestPointOnChild;
                double childDistance = childPresentation->distanceToClosestPresentationArea(
                    location,
                    &bestChildPresentation,
                    &bestChildPresentationAreaId,
                    &bestPointOnChild
                );

                if (childDistance < closestDistance) {
                    closestDistance           = childDistance;
                    closestPresentation       = bestChildPresentation;
                    closestPresentationAreaId = bestChildPresentationAreaId;
                    closestPointOnChild       = bestPointOnChild;
                }
            }

            ++childIndex;
        } while (childIndex <= lastFullyRenderedChildPresentation                      &&
                 childIndex < static_cast<unsigned long>(currentChildLocations.size()) &&
                 closestDistance != 0                                                  &&
                 currentChildLocations.at(childIndex).topPageIndex() == pageAtLocation    );
    }

    if (bestPresentation != Q_NULLPTR) {
        *bestPresentation = closestPresentation;
    }

    if (presentationAreaId != Q_NULLPTR) {
        *presentationAreaId = closestPresentationAreaId;
    }

    if (closestPoint != Q_NULLPTR) {
        *closestPoint = closestPointOnChild;
    }

    return closestDistance;
}


void RootPresentation::addPlacementStatusNotifierReceiver(PlacementStatusNotifier::Receiver* newReceiver) {
    placementStatusNotifier->addReceiver(newReceiver);
}


void RootPresentation::removePlacementStatusNotifierReceiver(PlacementStatusNotifier::Receiver* receiver) {
    placementStatusNotifier->removeReceiver(receiver);
}


bool RootPresentation::isRepositioning() const {
    return repositionInProgress;
}


bool RootPresentation::isDisplayCoherent() const {
    return !(repositionInProgress || repositionRequestPending);
}


void RootPresentation::redraw() {
    resetPlacement();
    requestRepositioning();
}


void RootPresentation::abortRepositioning() {
    if (repositionInProgress) {
        repositionRequestPending = false;
        placementStatusNotifier->requestPlacementAborted();
    }
}


void RootPresentation::processPresentationChanged(Presentation*) {}


void RootPresentation::processPresentationAdded(Presentation*) {}


void RootPresentation::processPresentationRemoved(Presentation*) {}


void RootPresentation::processPageFormatChange(
        unsigned long                  pageIndex,
        QSharedPointer<Ld::PageFormat>,
        QSharedPointer<Ld::PageFormat> newFormat
    ) {
    pageList.setFormat(pageIndex, newFormat);

    // FIXME - Start repositioning at updated page.
    recalculateAllChildPositions = true;
    requestRepositioning();

    QRectF boundingRectangle = pageList.pageBoundingRectangle();
    setSceneRect(boundingRectangle);
}


void RootPresentation::processNowPristine() {}


void RootPresentation::processNowModified() {}


void RootPresentation::processNowChanged() {}


void RootPresentation::processReferenceChanges() {}


void RootPresentation::processProgramSaved(const QString&) {}


void RootPresentation::processProgramSavedAs(const QString&) {}


void RootPresentation::processProgramSaveFailed(const QString&) {}


void RootPresentation::processProgramLoaded(const QString&, unsigned, unsigned) {}


void RootPresentation::processProgramLoadFailed(const QString&) {}


void RootPresentation::processNewProgramCreated() {}


void RootPresentation::processProgramClosed() {}


void RootPresentation::processRemovingChildPresentation(unsigned long childIndex, Presentation*) {
    QSharedPointer<Ld::RootElement> rootElement = element().dynamicCast<Ld::RootElement>();
    if (!rootElement.isNull()) {
        unsigned long numberChildren = rootElement->numberChildren();

        if (childIndex >= numberChildren) {
            childIndex = numberChildren - 1;
        }

        if (childIndex < static_cast<unsigned long>(currentChildLocations.size())) {
            currentChildLocations.removeAt(childIndex);
        }

        requestRepositioning(childIndex);
    }
}


void RootPresentation::processChildPresentationInsertedBefore(unsigned long childIndex, Presentation*) {
    QSharedPointer<Ld::RootElement> rootElement = element().dynamicCast<Ld::RootElement>();
    if (!rootElement.isNull()) {
        currentChildLocations.insert(childIndex, RootChildLocation(0, -1.0, 0.0));
        requestRepositioning(childIndex);
    }
}


void RootPresentation::processChildPresentationInsertedAfter(unsigned long childIndex, Presentation*) {
    QSharedPointer<Ld::RootElement> rootElement = element().dynamicCast<Ld::RootElement>();
    if (!rootElement.isNull()) {
        currentChildLocations.insert(childIndex + 1, RootChildLocation(0, -1.0, 0.0));
        requestRepositioning(childIndex + 1);
    }
}


void RootPresentation::tiedToElement(Ld::ElementPointer) {
    pageList.truncate(0);
    requestRepositioning();
}


void RootPresentation::untiedFromElement() {
    deleteLater();
}


void RootPresentation::processFormatChange(Ld::FormatPointer, Ld::FormatPointer) {}


void RootPresentation::processDescendantFormatChange(Ld::ElementPointer, Ld::FormatPointer) {}


void RootPresentation::processDescendantFormatUpdate(Ld::ElementPointer, Ld::FormatPointer) {}


void RootPresentation::processDataChange() {}


void RootPresentation::processDiagnostic(Ld::DiagnosticPointer) {}


void RootPresentation::clearDiagnosticDisplay() {}


void RootPresentation::processPageMaximumExtentsChanged(
        const QList<double>& newMaximumHorizontalExtentsPoints,
        const QList<double>& newPageLeftEdgePoints
        ) {
    emit maximumExtentsChanged(
        newMaximumHorizontalExtentsPoints,
        newPageLeftEdgePoints,
        sceneRect().left(),
        sceneRect().right()
    );
}


void RootPresentation::performRepositioning() {
    QSharedPointer<Ld::RootElement> rootElement = element();

    if (!rootElement.isNull()) {
        bool abortRequested;

        repositionInProgress = true;

        bool          terminateEarly       = false;
        unsigned long numberChildLocations = static_cast<unsigned>(currentChildLocations.size());

        unsigned long lastChildIndex;
        if (lastChildForRepositioning < rootElement->numberChildren()) {
            lastChildIndex = lastChildForRepositioning;
        } else if (rootElement->numberChildren() > 0) {
            lastChildIndex = rootElement->numberChildren() - 1;
        } else {
            lastChildIndex = 0;
        }

        unsigned long currentChildIndex = 0;

        do {
            abortRequested           = false;
            repositionRequestPending = false;

            if (firstChildForRepositioning < numberChildLocations) {
                currentChildIndex = firstChildForRepositioning;
            } else if (numberChildLocations > 0) {
                currentChildIndex = numberChildLocations - 1;
            } else {
                currentChildIndex = 0;
            }

            double minimumTopSpacing;
            if (currentChildIndex == 0) {
                currentPageIndex  = 0;
                currentActiveArea = activeAreaRectangle(currentPageIndex);
                cursorY           = currentActiveArea.top();
                minimumTopSpacing = imposeNoTopSpacing;
            } else {
                RootChildLocation& previousLocation = currentChildLocations[currentChildIndex - 1];

                currentPageIndex  = previousLocation.bottomPageIndex();
                currentActiveArea = activeAreaRectangle(currentPageIndex);
                cursorY           = previousLocation.bottomY();

                Ld::ElementPointer previousElement      = rootElement->child(currentChildIndex - 1);
                Presentation*      previousPresentation = dynamic_cast<Presentation*>(previousElement->visual());

                minimumTopSpacing = previousPresentation->bottomSpacingSceneUnits();
            }

            Q_ASSERT(currentChildIndex <= lastChildIndex);

            placementStatusNotifier->placementStarted(numberChildLocations - currentChildIndex);
            terminateEarly = false;

            do {
                Ld::ElementPointer childElement      = rootElement->child(currentChildIndex);
                Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());
                RootChildLocation& childLocation     = currentChildLocations[currentChildIndex];

                Presentation::ReflowHint reflowHint = childPresentation->reflowHint();

                if (cursorY == currentActiveArea.top()) {
                    minimumTopSpacing = imposeNoTopSpacing;
                }

                if (childPresentation->pendingRepositioning()              ||
                    reflowHint == Presentation::ReflowHint::ALWAYS_REFLOW  ||
                    childPresentation->graphicsItem(0) == Q_NULLPTR        ||
                    recalculateAllChildPositions                           ||
                    childLocation.minimumTopSpacing() != minimumTopSpacing    ) {
                    // *** Child is updated, never processed, minimum top spacing changed, always requires reflow, or we
                    // *** need to recalculate all positions.
                    //
                    // *** Recalculate placement
                    recalculateChildPlacement(
                        placementStatusNotifier,
                        childLocation,
                        childPresentation,
                        currentChildIndex,
                        minimumTopSpacing
                    );
                } else if (std::abs(childLocation.topY() - cursorY) > maximumAllowedRepositionVerticalError ||
                           childLocation.topPageIndex() != currentPageIndex                                    ) {
                    if (childLocation.topPageIndex() != childLocation.bottomPageIndex()) {
                        // *** Child previous
                        //
                        // *** Recalculate placement.
                        recalculateChildPlacement(
                            placementStatusNotifier,
                            childLocation,
                            childPresentation,
                            currentChildIndex,
                            minimumTopSpacing
                        );
                    } else if (reflowHint != Presentation::ReflowHint::SUPPORTS_REFLOW) {
                        // *** Child can't reflow but is contained on one page.  Move it as is.
                        moveChild(childLocation, childPresentation, minimumTopSpacing);
                    } else if (childLocation.topPageIndex() != childLocation.bottomPageIndex()) {
                        // *** Child previously crossed a page before.  Relative positioning is likely different.
                        // *** Recalculate placement.
                        recalculateChildPlacement(
                            placementStatusNotifier,
                            childLocation,
                            childPresentation,
                            currentChildIndex,
                            minimumTopSpacing
                        );
                    } else {
                        float remainingHeight = currentActiveArea.bottom() - cursorY;
                        float oldTop          = childLocation.topY();
                        float childHeight     = childLocation.bottomY() - oldTop;
                        if (childHeight <= remainingHeight && cursorY != 0 && oldTop != 0) {
                            // *** Child was contained to a page and is still contained to a page.  Simply move it.
                            moveChild(childLocation, childPresentation, minimumTopSpacing);
                        } else {
                            // We must recalculate the child placement due to the child now needing to wrap across a
                            // page boundary.
                            recalculateChildPlacement(
                                placementStatusNotifier,
                                childLocation,
                                childPresentation,
                                currentChildIndex,
                                minimumTopSpacing
                            );
                        }
                    }
                } else {
                    if (currentChildIndex <= lastChildIndex) {
                        // *** Child is fine where it is but we have more work.  Skip it.
                        cursorY = childLocation.bottomY();
                        unsigned newPageIndex = childLocation.bottomPageIndex();
                        if (newPageIndex != currentPageIndex) {
                            currentPageIndex  = newPageIndex;
                            currentActiveArea = activeAreaRectangle(currentPageIndex);
                        }
                    } else {
                        // *** Child is fine where it is and we've processed the last child.  We're done.
                        terminateEarly = true;
                    }
                }

                placementStatusNotifier->completedJob(); // last point that an abort can be requested in this loop
                abortRequested = placementStatusNotifier->abortPlacement();
                if (!abortRequested) {
                    minimumTopSpacing                  = childPresentation->bottomSpacingSceneUnits();
                    lastFullyRenderedChildPresentation = currentChildIndex;

                    ++currentChildIndex;

                    if (firstChildForRepositioning < currentChildIndex) {
                        firstChildForRepositioning = currentChildIndex;
                    }
                }

            } while (!terminateEarly && currentChildIndex < rootElement->numberChildren() && !abortRequested);

            if (!abortRequested) {
                firstChildForRepositioning   = static_cast<unsigned long>(-1);
                lastChildForRepositioning    = static_cast<unsigned long>(-1);
                recalculateAllChildPositions = false;
            } else {
                emit presentationUpdatesRestarted();
                recalculateAllChildPositions = true;
            }

            if (numberChildLocations > rootElement->numberChildren()) {
                currentChildLocations.erase(
                    currentChildLocations.begin() + rootElement->numberChildren(),
                    currentChildLocations.end()
                );
            }

            placementStatusNotifier->placementCompleted();
        } while (repositionRequestPending);

        if (!terminateEarly) {
            pageList.truncate(currentPageIndex + 1);
        } else {
            if (!abortRequested) {
                lastFullyRenderedChildPresentation = rootElement->numberChildren() - 1;
            }
        }

        repositionInProgress = false;

        if (currentPresentationUpdatesPending) {
            currentPresentationUpdatesPending = false;
            emit presentationUpdatesCompleted();
        }
    }
}


void RootPresentation::elementChanged(Ld::ElementPointer changedElement) {
    Presentation* changedPresentation = dynamic_cast<Presentation*>(changedElement->visual());
    emit presentationChanged(changedPresentation);
}


void RootPresentation::elementAdded(Ld::ElementPointer newElement) {
    Presentation* newPresentation = dynamic_cast<Presentation*>(newElement->visual());
    emit presentationAdded(newPresentation);
}


void RootPresentation::elementRemoved(Ld::ElementPointer removedElement) {
    Presentation* removedPresentation = dynamic_cast<Presentation*>(removedElement->visual());
    emit presentationRemoved(removedPresentation);
}


void RootPresentation::pageFormatChanged(
        unsigned long                  pageNumber,
        QSharedPointer<Ld::PageFormat> oldFormat,
        QSharedPointer<Ld::PageFormat> newFormat
    ) {
    emit pageFormatHasChanged(pageNumber, oldFormat, newFormat);
}


void RootPresentation::nowPristine() {
    emit programNowPristine();
}


void RootPresentation::nowModified() {
    emit programNowModified();
}


void RootPresentation::nowChanged() {
    emit programNowChanged();
}


void RootPresentation::referencesChanged() {
    emit referencesHaveChanged();
}


void RootPresentation::programSaved(const QString& filename) {
    emit programWasSaved(filename);
}


void RootPresentation::programSavedAs(const QString& filename) {
    emit programWasSavedAs(filename);
}


void RootPresentation::programSaveFailed(const QString& errorMessage) {
    emit programSaveHasFailed(errorMessage);
}


void RootPresentation::programLoaded(const QString& filename, unsigned majorVersion, unsigned minorVersion) {
    emit programWasLoaded(filename, majorVersion, minorVersion);
}


void RootPresentation::programLoadFailed(const QString& errorMessage) {
    emit programLoadHasFailed(errorMessage);
}


void RootPresentation::newProgramCreated() {
    emit newProgramWasCreated();
}


void RootPresentation::aboutToClose() {
    emit programClosed();
}


void RootPresentation::aboutToRemoveChild(
        unsigned long      childIndex,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit removingChildPresentation(childIndex, childPresentation);
}


void RootPresentation::childInsertedBefore(
        unsigned long      childIndex,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationNowInsertedBefore(childIndex, childPresentation);
}


void RootPresentation::childInsertedAfter(
        unsigned long      childIndex,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationNowInsertedAfter(childIndex, childPresentation);
}


void RootPresentation::coupledToElement(Ld::ElementPointer element) {
    emit nowTiedToElement(element);
}


void RootPresentation::decoupledFromElement() {
    emit nowUntiedFromElement();
}


void RootPresentation::formatChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    emit formatWasChanged(oldFormat, newFormat);
}


void RootPresentation::descendantFormatChanged(Ld::ElementPointer element, Ld::FormatPointer format) {
    emit descendantFormatWasChanged(element, format);
}


void RootPresentation::descendantFormatUpdated(Ld::ElementPointer element, Ld::FormatPointer format) {
    emit descendantFormatWasUpdated(element, format);
}


void RootPresentation::elementDataChanged() {
    emit elementDataWasChanged();
}


void RootPresentation::diagnosticReported(Ld::DiagnosticPointer diagnostic) {
    emit diagnosticWasReported(diagnostic);
}


void RootPresentation::diagnosticCleared() {
    emit diagnosticWasCleared();
}


void RootPresentation::resetPlacement() {
    Ld::ElementPointer     thisElement   = element();
    Ld::ElementPointerList childElements = thisElement->children();

    for (  Ld::ElementPointerList::const_iterator it  = childElements.constBegin(), end = childElements.constEnd()
         ; it != end
         ; ++it
           ) {
        Ld::ElementPointer childElement = *it;
        Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
        childPresentation->resetPlacement();
    }

    pageList.clear();

    QRectF boundingRectangle = pageList.pageBoundingRectangle();
    setSceneRect(boundingRectangle);
}


void RootPresentation::removeFromScene() {
    pageList.clear();
}


QRectF RootPresentation::activeAreaRectangle(unsigned long pageIndex) {
    QSharedPointer<Ld::RootElement> rootElement = element();

    bool addedPage = false;
    while (pageIndex >= pageList.numberPages()) {
        QSharedPointer<Ld::PageFormat>  pageFormat = rootElement->pageFormat(pageList.numberPages());
        pageList.append(pageFormat, this);
        addedPage = true;
    }

    if (addedPage) {
        QRectF boundingRectangle = pageList.pageBoundingRectangle();
        setSceneRect(boundingRectangle);
    }

    return QRectF(QPointF(0, 0), pageList.at(pageIndex).activeAreaSizeSceneUnits());
}


void RootPresentation::requestRepositioning(unsigned long childIndex) {
    currentPresentationUpdatesPending = true;
    emit presentationUpdatesPending();

    if (firstChildForRepositioning > childIndex) {
        firstChildForRepositioning = childIndex;
    }

    if (lastChildForRepositioning == static_cast<unsigned long>(-1) || lastChildForRepositioning < childIndex) {
        lastChildForRepositioning = childIndex;
    }

    repositionRequestPending = true;

    if (!repositionInProgress) {
        if (!repositionTimer->isActive()) {
            repositionTimer->start(0);
        }
    } else {
        placementStatusNotifier->requestPlacementAborted();
    }
}


QList<QRectF> RootPresentation::visibleSceneBoundingRectangles() const {
    QList<QRectF>         regionList;
    QList<QGraphicsView*> views = RootPresentation::views();

    for (  QList<QGraphicsView*>::const_iterator viewIterator    = views.constBegin(),
                                                 viewEndIterator = views.constEnd()
         ; viewIterator != viewEndIterator
         ; ++viewIterator
        ) {
        QGraphicsView* view           = *viewIterator;
        QRect          viewportWindow = view->viewport()->rect();
        QPointF        upperLeft      = view->mapToScene(viewportWindow.topLeft());
        QPointF        lowerRight     = view->mapToScene(viewportWindow.bottomRight());

        QRectF         viewBoundingRectangle(upperLeft, lowerRight);

        QList<QRectF>::iterator regionListIterator    = regionList.begin();
        QList<QRectF>::iterator regionListEndIterator = regionList.end();
        bool                    merged            = false;
        while (!merged && regionListIterator != regionListEndIterator) {
            if (viewBoundingRectangle.intersects(*regionListIterator)) {
                QRectF unitedArea = regionListIterator->united(viewBoundingRectangle);
                *regionListIterator = unitedArea;

                merged = true;
            } else {
                ++regionListIterator;
            }
        }

        if (!merged) {
            regionList.append(viewBoundingRectangle);
        }
    }

    QMap<double, QRectF> regionMap;
    for (  QList<QRectF>::const_iterator regionListIterator    = regionList.constBegin(),
                                         regionListEndIterator = regionList.constEnd()
         ; regionListIterator != regionListEndIterator
         ; ++regionListIterator
        ) {
        const QRectF& rectangle = *regionListIterator;
        regionMap.insert(rectangle.top(), rectangle);
    }

    return regionMap.values();
}


void RootPresentation::recalculateChildPlacement(
        PlacementTracker*  placementTracker,
        RootChildLocation& childLocation,
        Presentation*      childPresentation,
        unsigned long      childIndex,
        float              minimumTopSpacing
    ) {
    childLocation.setTopLocation(currentPageIndex, cursorY, minimumTopSpacing);
    childPresentation->recalculatePlacement(
        placementTracker,
        this,
        childIndex,
        Q_NULLPTR, // nextSibling -- currently unused
        true,      // honorLeadingWhitespace,
        minimumTopSpacing
    );
}


void RootPresentation::moveChild(
        RootChildLocation& childLocation,
        Presentation*      childPresentation,
        float              minimumTopSpacing
    ) {
    QGraphicsItem* graphicsItem = childPresentation->graphicsItem(0);

    unsigned oldTopPageIndex    = childLocation.topPageIndex();
    unsigned oldBottomPageIndex = childLocation.bottomPageIndex();
    float    oldTop             = childLocation.topY();
    float    oldBottom          = childLocation.bottomY();
    float    totalHeight        = oldTopPageIndex == oldBottomPageIndex ? oldBottom - oldTop : oldBottom - 0;
    float    availableSpace     = currentActiveArea.bottom() - cursorY;
    if (totalHeight > availableSpace) {
        ++currentPageIndex;
        currentActiveArea = activeAreaRectangle(currentPageIndex);
        cursorY           = currentActiveArea.top();
        minimumTopSpacing = 0;
    }

    childLocation.setTopLocation(currentPageIndex, cursorY, minimumTopSpacing);

    EQt::GraphicsItemGroup* pageGroup = pageList.at(currentPageIndex).activeAreaGroup();
    float offset = graphicsItem->pos().y() - oldTop;

    pageGroup->addToGroup(graphicsItem);
    graphicsItem->setPos(currentActiveArea.left(), cursorY + offset);

    cursorY += totalHeight;
    childLocation.setBottomLocation(currentPageIndex, cursorY);

    if (cursorY >= currentActiveArea.bottom()) {
        ++currentPageIndex;
        currentActiveArea = activeAreaRectangle(currentPageIndex);
        cursorY           = currentActiveArea.top();
    }
}
