/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QFontMetricsF>
#include <QPen>
#include <QBrush>

#include <eqt_graphics_text_item.h>
#include <eqt_graphics_math_group.h>

#include <model_variant.h>

#include "ld_format_structures.h"
#include "ld_data_type_decoder.h"

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "data_type_presenter.h"
#include "data_type_presentation_generator.h"

DataTypePresentationGenerator::DataTypePresentationGenerator() {}


DataTypePresentationGenerator::~DataTypePresentationGenerator() {}


DataTypePresenter::PresentationAreas DataTypePresentationGenerator::generateGraphicsItems(
        PlacementTracker*                     placementTracker,
        DataTypePresenter::PresentationAreas& currentPresentationAreas,
        DataTypePresenter*                    presenter,
        const Model::Variant&                 variant,
        Ld::FormatPointer                     format,
        float                                 /* lineSpacing */,
        float                                 /* relativeScale */,
        bool                                  forceAppend
    ) const {
    QSizeF        size;
    float         ascent;
    unsigned long presentationAreaId;

    if (forceAppend || currentPresentationAreas.isEmpty()) {
        presentationAreaId = static_cast<unsigned long>(currentPresentationAreas.size());
        EQt::GraphicsMultiTextGroup* graphicsItem = toGraphicsItem(
            placementTracker,
            variant,
            format,
            presenter,
            size,
            ascent
        );

        currentPresentationAreas.append(DataTypePresenter::PresentationArea(graphicsItem, size, ascent));
    } else {
        presentationAreaId = 0;

        DataTypePresenter::PresentationArea& area    = currentPresentationAreas[presentationAreaId];
        EQt::GraphicsMultiTextGroup*         item    = dynamic_cast<EQt::GraphicsMultiTextGroup*>(area.graphicsItem());
        EQt::GraphicsMultiTextGroup*         newItem = updateGraphicsItem(
            placementTracker,
            item,
            variant,
            format,
            presenter,
            size,
            ascent
        );

        if (item != newItem) {
            item->deleteLater();
            area.setGraphicsItem(newItem);
        }

        area.setSize(size);
        area.setAscent(ascent);

        while (currentPresentationAreas.size() > 1) {
            DataTypePresenter::PresentationArea lastPresentationArea = currentPresentationAreas.takeLast();
            dynamic_cast<EQt::GraphicsItem*>(lastPresentationArea.graphicsItem())->deleteLater();
        }
    }

    PlacementNegotiator::SpaceQualifier spaceQualifier;
    (void) locateFreeSpace(presenter, size, &spaceQualifier);
    presenter->allocateDataTypeArea(presentationAreaId, size, ascent);

    return currentPresentationAreas;
}


EQt::GraphicsMultiTextGroup* DataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            placementTracker,
        EQt::GraphicsMultiTextGroup* /* graphicsItem */,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           presenter,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    return toGraphicsItem(placementTracker, value, format, presenter, size, ascent);
}


Presentation::ReflowHint DataTypePresentationGenerator::reflowHint(bool /* afterAbort */) const {
    return Presentation::ReflowHint::REFLOW_NOT_SUPPORTED;
}


QSizeF DataTypePresentationGenerator::locateFreeSpace(
        DataTypePresenter*                   presenter,
        const QSizeF&                        requiredSpace,
        PlacementNegotiator::SpaceQualifier* spaceQualifier
    ) {
    QSizeF                              availableSpace;
    PlacementNegotiator::SpaceQualifier qualifier;
    bool                                foundSpace = false;

    do {
        availableSpace = presenter->requestDataTypeArea(&qualifier);

        if (qualifier == PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
            qualifier == PlacementNegotiator::SpaceQualifier::USE_PROVIDED             ||
            (qualifier == PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH &&
             availableSpace.height() >= requiredSpace.height()                  )      ||
            (availableSpace.width() >= requiredSpace.width()   &&
             availableSpace.height() >= requiredSpace.height()    )                       ) {
            foundSpace = true;
        } else {
            presenter->dataTypeAreaInsufficient(availableSpace);
        }
    } while (!foundSpace);

    if (spaceQualifier != Q_NULLPTR) {
        *spaceQualifier = qualifier;
    }

    return availableSpace;
}


void DataTypePresentationGenerator::addText(
        DataTypePresenter::PresentationAreas& currentPresentationAreas,
        DataTypePresenter*                    presenter,
        const QString&                        text,
        const QFont&                          font,
        const QColor&                         fontColor,
        const QColor&                         fontBackgroundColor,
        float                                 lineSpacing
    ) {
    QFontMetricsF fontMetrics(font);
    float baseFontHeight    = fontMetrics.height();
    float baseFontAscent    = fontMetrics.ascent();
    float width             = fontMetrics.horizontalAdvance(text);
    float additionalSpacing = baseFontHeight * (lineSpacing - 1.0);

    float fontHeight = baseFontHeight + additionalSpacing;
    float fontAscent = baseFontAscent + additionalSpacing;

    QSizeF                              requiredSpace(width, fontHeight);
    PlacementNegotiator::SpaceQualifier spaceQualifier;
    (void) locateFreeSpace(presenter, requiredSpace, &spaceQualifier);

    EQt::GraphicsTextItem* textItem = new EQt::GraphicsTextItem(text, font);
    textItem->setBrush(QBrush(fontColor));
    textItem->setBackgroundBrush(QBrush(fontBackgroundColor));

    presenter->allocateDataTypeArea(
        static_cast<unsigned long>(currentPresentationAreas.size()),
        requiredSpace,
        fontAscent
    );

    currentPresentationAreas.append(DataTypePresenter::PresentationArea(textItem, requiredSpace, fontAscent));
}


void DataTypePresentationGenerator::addLeftParenthesis(
        DataTypePresenter::PresentationAreas&    currentPresentationAreas,
        DataTypePresenter*                       presenter,
        float                                    contentsHeight,
        EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
        const QString&                           leftParenthesisString,
        const QFont&                             font,
        const QColor&                            fontColor,
        const QColor&                            fontBackgroundColor
    ) {
    QFontMetricsF fontMetrics(font);
    float         fontHeight = fontMetrics.height();
    float         fontAscent = fontMetrics.ascent();

    QSizeF leftParenthesisSize;
    if (contentsHeight <= fontHeight * (1.0F + Presentation::parenthesisHeightMargin)) {
        EQt::GraphicsTextItem* leftParenthesisItem;

        if (contentsHeight != fontHeight) {
            QFont parenthesisFont = font;
            parenthesisFont.setPointSizeF(font.pointSizeF() * contentsHeight / fontHeight);

            QFontMetrics parenthesisFontMetrics(parenthesisFont);
            leftParenthesisSize = QSizeF(
                parenthesisFontMetrics.horizontalAdvance(leftParenthesisString),
                parenthesisFontMetrics.height()
            );

            leftParenthesisItem = new EQt::GraphicsTextItem(leftParenthesisString, parenthesisFont);
        } else {
            leftParenthesisSize = QSizeF(
                fontMetrics.horizontalAdvance(leftParenthesisString),
                fontMetrics.height()
            );

            leftParenthesisItem = new EQt::GraphicsTextItem(leftParenthesisString, font);
        }

        leftParenthesisItem->setBrush(QBrush(fontColor));
        leftParenthesisItem->setBackgroundBrush(QBrush(fontBackgroundColor));

        PlacementNegotiator::SpaceQualifier spaceQualifier;
        (void) locateFreeSpace(presenter, leftParenthesisSize, &spaceQualifier);
        presenter->allocateDataTypeArea(
            static_cast<unsigned long>(currentPresentationAreas.size()),
            leftParenthesisSize,
            fontAscent
        );

        currentPresentationAreas.append(
            DataTypePresenter::PresentationArea(leftParenthesisItem, leftParenthesisSize, fontAscent)
        );
    } else {
        EQt::GraphicsMathGroup* leftParenthesisItem = new EQt::GraphicsMathGroup;
        leftParenthesisItem->setRightParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);

        float descent        = fontMetrics.descent();
        float fontBasedWidth = fontMetrics.horizontalAdvance(QString("M")) / 2.0F;

        float leftParenthesisTop = fontMetrics.ascent() - fontMetrics.capHeight();
        float leftInternalHeight = contentsHeight - leftParenthesisTop - descent / 2.0F;

        leftParenthesisSize = QSizeF(
            std::min(fontBasedWidth, static_cast<float>(leftInternalHeight / 4.0F)),
            leftParenthesisTop + leftInternalHeight
        );

        leftParenthesisItem->setLeftParenthesis(
            parenthesisStyle,
            QRectF(QPointF(0, leftParenthesisTop), leftParenthesisSize)
        );

        leftParenthesisItem->setParenthesisPen(QPen(fontColor));
        leftParenthesisItem->setBackgroundBrush(QBrush(fontBackgroundColor));

        PlacementNegotiator::SpaceQualifier spaceQualifier;
        (void) locateFreeSpace(presenter, leftParenthesisSize, &spaceQualifier);
        presenter->allocateDataTypeArea(
            static_cast<unsigned long>(currentPresentationAreas.size()),
            leftParenthesisSize,
            0.0F
        );

        currentPresentationAreas.append(
            DataTypePresenter::PresentationArea(leftParenthesisItem, leftParenthesisSize, 0.0F)
        );
    }
}


void DataTypePresentationGenerator::addRightParenthesis(
        DataTypePresenter::PresentationAreas&    currentPresentationAreas,
        DataTypePresenter*                       presenter,
        float                                    contentsHeight,
        EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
        const QString&                           rightParenthesisString,
        const QFont&                             font,
        const QColor&                            fontColor,
        const QColor&                            fontBackgroundColor
    ) {
    QFontMetricsF fontMetrics(font);
    float         fontHeight = fontMetrics.height();
    float         fontAscent = fontMetrics.ascent();

    QSizeF rightParenthesisSize;
    if (contentsHeight <= fontHeight * (1.0F + Presentation::parenthesisHeightMargin)) {
        EQt::GraphicsTextItem* rightParenthesisItem;

        if (contentsHeight != fontHeight) {
            QFont parenthesisFont = font;
            parenthesisFont.setPointSizeF(font.pointSizeF() * contentsHeight / fontHeight);

            QFontMetrics parenthesisFontMetrics(parenthesisFont);
            rightParenthesisSize = QSizeF(
                parenthesisFontMetrics.horizontalAdvance(rightParenthesisString),
                parenthesisFontMetrics.height()
            );

            rightParenthesisItem = new EQt::GraphicsTextItem(rightParenthesisString, parenthesisFont);
        } else {
            rightParenthesisSize = QSizeF(
                fontMetrics.horizontalAdvance(rightParenthesisString),
                fontMetrics.height()
            );

            rightParenthesisItem = new EQt::GraphicsTextItem(rightParenthesisString, font);
        }

        rightParenthesisItem->setBrush(QBrush(fontColor));
        rightParenthesisItem->setBackgroundBrush(QBrush(fontBackgroundColor));

        PlacementNegotiator::SpaceQualifier spaceQualifier;
        (void) locateFreeSpace(presenter, rightParenthesisSize, &spaceQualifier);
        presenter->allocateDataTypeArea(
            static_cast<unsigned long>(currentPresentationAreas.size()),
            rightParenthesisSize,
            fontAscent
        );

        currentPresentationAreas.append(
            DataTypePresenter::PresentationArea(rightParenthesisItem, rightParenthesisSize, fontAscent)
        );
    } else {
        EQt::GraphicsMathGroup* rightParenthesisItem = new EQt::GraphicsMathGroup;
        rightParenthesisItem->setLeftParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);

        float descent        = fontMetrics.descent();
        float fontBasedWidth = fontMetrics.horizontalAdvance(QString("M")) / 2.0F;

        float rightParenthesisTop = fontMetrics.ascent() - fontMetrics.capHeight();
        float rightInternalHeight = contentsHeight - rightParenthesisTop - descent / 2.0F;

        rightParenthesisSize = QSizeF(
            std::min(fontBasedWidth, static_cast<float>(rightInternalHeight / 4.0F)),
            rightParenthesisTop + rightInternalHeight
        );

        rightParenthesisItem->setRightParenthesis(
            parenthesisStyle,
            QRectF(QPointF(0, rightParenthesisTop), rightParenthesisSize)
        );

        rightParenthesisItem->setParenthesisPen(QPen(fontColor));
        rightParenthesisItem->setBackgroundBrush(QBrush(fontBackgroundColor));

        PlacementNegotiator::SpaceQualifier spaceQualifier;
        (void) locateFreeSpace(presenter, rightParenthesisSize, &spaceQualifier);
        presenter->allocateDataTypeArea(
            static_cast<unsigned long>(currentPresentationAreas.size()),
            rightParenthesisSize,
            0.0F
        );

        currentPresentationAreas.append(
            DataTypePresenter::PresentationArea(rightParenthesisItem, rightParenthesisSize, 0.0F)
        );
    }
}


void DataTypePresentationGenerator::addText(
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const QString&               text,
        const QFont&                 font,
        const QPointF&               position,
        unsigned&                    index
    ) {
    if (index < graphicsItem->numberTextEntries()) {
        EQt::GraphicsMultiTextGroup::TextEntry& entry = graphicsItem->entry(index);

        entry.setText(text);
        entry.setFont(font);
        entry.setPosition(position);
    } else {
        graphicsItem->append(text, font, position);
    }

    ++index;
}
