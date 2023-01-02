/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CompoundStatementOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QList>
#include <QFont>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_compound_statement_operator_element.h>

#include "application.h"
#include "placement_tracker.h"
#include "presentation_area_tracker.h"
#include "list_presentation_base.h"
#include "compound_statement_operator_presentation.h"

CompoundStatementOperatorPresentation::CompoundStatementOperatorPresentation() {}


CompoundStatementOperatorPresentation::~CompoundStatementOperatorPresentation() {}


Ld::Visual* CompoundStatementOperatorPresentation::creator(const QString&) {
    return new CompoundStatementOperatorPresentation();
}


void CompoundStatementOperatorPresentation::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                minimumTopSpacing,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    Ld::ElementPointer element = CompoundStatementOperatorPresentation::element();
    Q_ASSERT(!element.isNull());

    placementTracker->addNewJobs(element->numberChildren());

    QSharedPointer<Ld::FontFormat> format = element->format().dynamicCast<Ld::FontFormat>();
    if (format.isNull()) {
        format = Ld::CharacterFormat::applicationDefaultMathFont();
    }

    QFont   font            = format->toQFont();
    QColor  color           = format->fontColor();
    QColor  backgroundColor = format->fontBackgroundColor();
    float   fontPointSize   = font.pointSizeF();
    float   fontScaleFactor = Application::fontScaleFactor();

    if (fontScaleFactor != 1.0F || relativeScale != 1.0F) {
        font.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale);
    }

    QPen   fontPen(color.isValid() ? color : QColor(Qt::black));
    QBrush fontBackgroundBrush(backgroundColor.isValid() ? backgroundColor : QColor(255, 255, 255, 0));

    QFontMetricsF fontMetrics(font);
    float leftSideMargin    = fontMetrics.horizontalAdvance(tr("M"));
    float verticalSeparator = 0; // fontMetrics.height() / 2.0;
    float fontAscent        = fontMetrics.ascent();

    QList<PresentationAreaTracker>& trackers = populatePresentationAreaTrackers(
        placementTracker,
        minimumTopSpacing,
        relativeScale
    );

    EQt::GraphicsMathGroup* currentItem = graphicsItem();
    if (currentItem == Q_NULLPTR) {
        currentItem = new EQt::GraphicsMathGroup;
        setGraphicsItem(currentItem);
    }

    currentItem->setTextPen(fontPen);
    currentItem->setBackgroundBrush(fontBackgroundBrush);

    unsigned numberPresentations = static_cast<unsigned>(trackers.size());
    float    maximumChildWidth   = 0;
    float    currentY            = 0;

    for (unsigned i=0 ; i<numberPresentations ; ++i) {
        const PresentationAreaTracker& tracker        = trackers.at(i);
        float                          childWidth     = tracker.width();
        float                          maximumAscent  = tracker.maximumAscent();
        float                          maximumDescent = tracker.maximumDescent();
        float                          currentX       = leftSideMargin;

        if (i != 0) {
            currentY += verticalSeparator;
        }

        for (  PresentationAreaTracker::Iterator areaIterator    = tracker.begin(),
                                                 areaEndIterator = tracker.end()
             ; areaIterator != areaEndIterator
             ; ++areaIterator
            ) {
            const PresentationAreaTracker::Entry& area = *areaIterator;
            QGraphicsItem* areaItem   = area.graphicsItem();
            float          areaAscent = area.ascent();

            if (areaItem->parentItem() != currentItem) {
                currentItem->addToGroup(areaItem);
            }

            areaItem->setPos(currentX, currentY + maximumAscent - areaAscent);
            currentX += area.areaSize().width();
        }

        if (childWidth > maximumChildWidth) {
            maximumChildWidth = childWidth;
        }

        currentY += maximumAscent + maximumDescent;
    }

    float leftBorderX = leftSideMargin / 2.0;
    currentItem->setLeftParenthesis(
        EQt::GraphicsMathGroup::ParenthesisStyle::ABSOLUTE_VALUE,
        QRectF(leftBorderX, 0, 0, currentY)
    );
    currentItem->setRightParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);

    allocateSpaceForThisPresentation(
        parent,
        childIdentifier,
        QSizeF(maximumChildWidth + leftSideMargin, currentY + minimumTopSpacing),
        fontAscent + minimumTopSpacing
    );
}
