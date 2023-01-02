/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TupleDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFont>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFontMetricsF>

#include <eqt_graphics_text_item.h>
#include <eqt_graphics_math_group.h>

#include <model_intrinsic_types.h>
#include <model_tuple.h>
#include <model_variant.h>

#include <ld_format_structures.h>
#include <ld_data_type.h>
#include <ld_data_type_format.h>
#include <ld_tuple_data_type_format.h>
#include <ld_character_format.h>
#include <ld_complex_data_type_decoder.h>
#include <ld_value_field_format.h>

#include "application.h"
#include "placement_tracker.h"
#include "data_type_presentation_generator.h"
#include "tuple_data_type_presentation_generator.h"

const TupleDataTypePresentationGenerator TupleDataTypePresentationGenerator::instance;

TupleDataTypePresentationGenerator::TupleDataTypePresentationGenerator() {}


TupleDataTypePresentationGenerator::~TupleDataTypePresentationGenerator() {}


float TupleDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     placementTracker,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float                                result;

    Ld::TupleDataTypeFormat::DisplayMode displayMode;
    unsigned long                        leadingMemberCount;
    unsigned long                        trailingMemberCount;
    unsigned long                        hiddenMemberCount;
    QFont                                font;

    if (!format.isNull()                                                         &&
        (format->capabilities().contains(Ld::TupleDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)     )    ) {
        QSharedPointer<Ld::TupleDataTypeFormat> tupleFormat = format.dynamicCast<Ld::TupleDataTypeFormat>();

        displayMode           = tupleFormat->displayMode();
        leadingMemberCount    = tupleFormat->leadingMemberCount();
        trailingMemberCount   = tupleFormat->trailingMemberCount();
        font                  = tupleFormat->toQFont();
    } else {
        displayMode           = Ld::TupleDataTypeFormat::defaultDisplayMode;
        leadingMemberCount    = Ld::TupleDataTypeFormat::defaultLeadingMemberCount;
        trailingMemberCount   = Ld::TupleDataTypeFormat::defaultTrailingMemberCount;

        QSharedPointer<Ld::CharacterFormat> defaultFont = Ld::CharacterFormat::applicationDefaultMathFont();
        font = defaultFont->toQFont();
    }

    if (displayMode == Ld::TupleDataTypeFormat::DisplayMode::NORMAL  ||
        displayMode == Ld::TupleDataTypeFormat::DisplayMode::INVALID    ) {
        bool         ok;
        Model::Tuple tupleValue = variant.toTuple(&ok);
        Q_ASSERT(ok);

        unsigned long numberTupleMembers = tupleValue.size();
        if (leadingMemberCount == Ld::TupleDataTypeFormat::showAllMembers  ||
            trailingMemberCount == Ld::TupleDataTypeFormat::showAllMembers ||
            leadingMemberCount + trailingMemberCount >= numberTupleMembers    ) {
            leadingMemberCount  = numberTupleMembers;
            hiddenMemberCount   = 0;
            trailingMemberCount = 0;
        } else {
            hiddenMemberCount = numberTupleMembers - leadingMemberCount - trailingMemberCount;
        }

        font.setPointSizeF(font.pointSizeF() * Application::fontScaleFactor() * relativeScale);
        QFontMetricsF fontMetrics(font);
        float maximumAscent  = fontMetrics.ascent();
        float maximumDescent = fontMetrics.descent();

        Model::Tuple::ConstIterator tupleIterator = tupleValue.constBegin();

        placementTracker->addNewJobs(leadingMemberCount + trailingMemberCount);

        calculateMaximumChildHeight(
            placementTracker,
            tupleIterator,
            leadingMemberCount,
            format,
            relativeScale,
            maximumAscent,
            maximumDescent
        );

        tupleIterator.advance(hiddenMemberCount);

        calculateMaximumChildHeight(
            placementTracker,
            tupleIterator,
            trailingMemberCount,
            format,
            relativeScale,
            maximumAscent,
            maximumDescent
        );

        if (ascent != Q_NULLPTR) {
            *ascent = maximumAscent;
        }

        result = maximumAscent + maximumDescent;
    } else {
        font.setPointSizeF(font.pointSizeF() * Application::fontScaleFactor() * relativeScale);
        QFontMetricsF fontMetrics(font);

        if (ascent != Q_NULLPTR) {
            *ascent = fontMetrics.ascent();
        }

        result = fontMetrics.height();
    }

    return result;
}


DataTypePresenter::PresentationAreas TupleDataTypePresentationGenerator::generateGraphicsItems(
        PlacementTracker*                     placementTracker,
        DataTypePresenter::PresentationAreas& currentPresentationAreas,
        DataTypePresenter*                    presenter,
        const Model::Variant&                 variant,
        Ld::FormatPointer                     format,
        float                                 lineSpacing,
        float                                 relativeScale,
        bool                                  forceAppend
    ) const {
    if (!forceAppend) {
        for (  DataTypePresenter::PresentationAreas::iterator areaIterator    = currentPresentationAreas.begin(),
                                                              areaEndIterator = currentPresentationAreas.end()
             ; areaIterator != areaEndIterator
             ; ++areaIterator
            ) {
            EQt::GraphicsItem* graphicsItem = dynamic_cast<EQt::GraphicsItem*>(areaIterator->graphicsItem());
            Q_ASSERT(graphicsItem != Q_NULLPTR);
            graphicsItem->deleteLater();
        }

        currentPresentationAreas.clear();
    }

    bool         ok;
    Model::Tuple tupleValue = variant.toTuple(&ok);
    Q_ASSERT(ok);

    Ld::TupleDataTypeFormat::DisplayMode displayMode;
    unsigned long                        leadingMemberCount;
    unsigned long                        trailingMemberCount;
    unsigned long                        hiddenMemberCount;
    bool                                 showHiddenMemberCount;
    QFont                                font;
    QColor                               fontColor;
    QColor                               fontBackgroundColor;

    if (!format.isNull()                                                         &&
        (format->capabilities().contains(Ld::TupleDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)     )    ) {
        QSharedPointer<Ld::TupleDataTypeFormat> tupleFormat = format.dynamicCast<Ld::TupleDataTypeFormat>();

        displayMode           = tupleFormat->displayMode();
        leadingMemberCount    = tupleFormat->leadingMemberCount();
        trailingMemberCount   = tupleFormat->trailingMemberCount();
        showHiddenMemberCount = tupleFormat->displayHiddenMemberCount();

        font                  = tupleFormat->toQFont();
        fontColor             = tupleFormat->fontColor();
        fontBackgroundColor   = tupleFormat->fontBackgroundColor();
    } else {
        displayMode           = Ld::TupleDataTypeFormat::defaultDisplayMode;
        leadingMemberCount    = Ld::TupleDataTypeFormat::defaultLeadingMemberCount;
        trailingMemberCount   = Ld::TupleDataTypeFormat::defaultTrailingMemberCount;
        showHiddenMemberCount = Ld::TupleDataTypeFormat::defaultShowHiddenMemberCount;

        QSharedPointer<Ld::CharacterFormat> defaultFont = Ld::CharacterFormat::applicationDefaultMathFont();
        font                = defaultFont->toQFont();
        fontColor           = defaultFont->fontColor();
        fontBackgroundColor = defaultFont->fontBackgroundColor();
    }

    if (displayMode == Ld::TupleDataTypeFormat::DisplayMode::NORMAL  ||
        displayMode == Ld::TupleDataTypeFormat::DisplayMode::INVALID    ) {
        unsigned long numberTupleMembers = tupleValue.size();
        if (leadingMemberCount == Ld::TupleDataTypeFormat::showAllMembers  ||
            trailingMemberCount == Ld::TupleDataTypeFormat::showAllMembers ||
            leadingMemberCount + trailingMemberCount >= numberTupleMembers    ) {
            leadingMemberCount  = numberTupleMembers;
            hiddenMemberCount   = 0;
            trailingMemberCount = 0;
        } else {
            hiddenMemberCount = numberTupleMembers - leadingMemberCount - trailingMemberCount;
        }

        if (!fontColor.isValid()) {
            fontColor = QColor(Qt::black);
        }

        if (!fontBackgroundColor.isValid()) {
            fontBackgroundColor = QColor(255, 255, 255, 0);
        }

        float pointSize = font.pointSizeF() * Application::fontScaleFactor();
        font.setPointSizeF(pointSize * relativeScale);

        float maximumChildHeight = calculateHeight(placementTracker, variant, format, relativeScale);
        currentPresentationAreas = renderWrapped(
            placementTracker,
            currentPresentationAreas,
            presenter,
            tupleValue,
            format,
            font,
            fontColor,
            fontBackgroundColor,
            maximumChildHeight,
            leadingMemberCount,
            trailingMemberCount,
            hiddenMemberCount,
            showHiddenMemberCount,
            lineSpacing,
            relativeScale
        );
    } else {
        char*   s = tupleValue.toString();
        QString str;
        if (s == nullptr) {
            str = tr("*** INVALID TEXT CONTENT ***");
        } else {
            str = QString::fromUtf8(s);
            delete[] s;
        }

        switch (displayMode) {
            case Ld::TupleDataTypeFormat::DisplayMode::INVALID:
            case Ld::TupleDataTypeFormat::DisplayMode::NORMAL: {
                Q_ASSERT(false);
                break;
            }

            case Ld::TupleDataTypeFormat::DisplayMode::STRING_BARE: {
                break;
            }

            case Ld::TupleDataTypeFormat::DisplayMode::STRING_ANGLE_BRACKETS: {
                str = tr("%1%2%3").arg(QChar(0x27E8)).arg(str).arg(QChar(0x27E9));
                break;
            }

            case Ld::TupleDataTypeFormat::DisplayMode::STRING_SINGLE_QUOTES: {
                str = tr("'%1'").arg(str);
                break;
            }

            case Ld::TupleDataTypeFormat::DisplayMode::STRING_DOUBLE_QUOTES: {
                str = tr("\"%1\"").arg(str);
                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        font.setPointSizeF(font.pointSizeF() * Application::fontScaleFactor() * relativeScale);
        QFontMetricsF fontMetrics(font);
        float         height            = fontMetrics.height();
        float         ascent            = fontMetrics.ascent();
        float         additionalSpacing = height * (lineSpacing - 1.0);

        height += additionalSpacing;
        ascent += additionalSpacing;

        unsigned long stringLength = static_cast<unsigned long>(str.length());
        unsigned long index        = 0;

        placementTracker->addNewJobs(1);
        while (index < stringLength && !placementTracker->abortPlacement()) {
            PlacementNegotiator::SpaceQualifier spaceQualifier;
            QSizeF availableArea = presenter->requestDataTypeArea(&spaceQualifier);

            unsigned long endingIndex = 0;
            if (spaceQualifier == PlacementNegotiator::SpaceQualifier::USE_PROVIDED) {
                // We've been told to use the provided space, no matter what.  Place the entire string here.
                endingIndex = stringLength;
            } else if (availableArea.height() >= height                                                ||
                       spaceQualifier == PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT    ) {
                // We have sufficient height or this is the best we're going to get.
                unsigned long maximumIndex = maximumEndingIndex(str, fontMetrics, availableArea.width(), index);
                unsigned long bestIndex    =   maximumIndex < stringLength
                                             ? bestSplitBefore(str, maximumIndex)
                                             : maximumIndex;

                if (bestIndex <= index) {
                    if (spaceQualifier == PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH            ||
                        spaceQualifier == PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT    ) {
                        endingIndex = maximumIndex;
                    }
                } else {
                    endingIndex = bestIndex;
                }
            }

            if (endingIndex > index) {
                // We have some text to render.

                unsigned long          presentationAreaId = static_cast<unsigned long>(currentPresentationAreas.size());
                QString                subString         = str.mid(index, endingIndex - index);
                EQt::GraphicsTextItem* textItem          = new EQt::GraphicsTextItem(subString, font);
                float                  width             = fontMetrics.horizontalAdvance(subString);
                QSizeF                 areaSize(width, height);

                DataTypePresenter::PresentationArea presentationArea(textItem, areaSize, ascent);
                currentPresentationAreas.append(presentationArea);
                presenter->allocateDataTypeArea(presentationAreaId, areaSize, ascent);

                index = endingIndex;
            } else {
                // Insufficient space.
                presenter->dataTypeAreaInsufficient(availableArea);
            }
        }
    }

    return currentPresentationAreas;
}


EQt::GraphicsMultiTextGroup* TupleDataTypePresentationGenerator::toGraphicsItem(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& /* value */,
        Ld::FormatPointer     /* format */,
        DataTypePresenter*    /* presenter */,
        QSizeF&               /* size */,
        float&                /* ascent */
    ) const {
    Q_ASSERT(false);
    return Q_NULLPTR; // To make MSVC happy.
}


Presentation::ReflowHint TupleDataTypePresentationGenerator::reflowHint(bool afterAbort) const {
    return afterAbort ? Presentation::ReflowHint::ALWAYS_REFLOW : Presentation::ReflowHint::SUPPORTS_REFLOW;
}


DataTypePresenter::PresentationAreas TupleDataTypePresentationGenerator::renderWrapped(
        PlacementTracker*                     placementTracker,
        DataTypePresenter::PresentationAreas& currentPresentationAreas,
        DataTypePresenter*                    presenter,
        const Model::Tuple&                   tuple,
        Ld::FormatPointer                     format,
        const QFont&                          font,
        const QColor&                         fontColor,
        const QColor&                         fontBackgroundColor,
        float                                 maximumChildHeight,
        unsigned long                         leadingMemberCount,
        unsigned long                         trailingMemberCount,
        unsigned long                         hiddenMemberCount,
        bool                                  showHiddenMemberCount,
        float                                 lineSpacing,
        float                                 relativeScale
    ) const {
    addLeftParenthesis(
        currentPresentationAreas,
        presenter,
        maximumChildHeight,
        EQt::GraphicsMathGroup::ParenthesisStyle::PARENTHESIS,
        QString("("),
        font,
        fontColor,
        fontBackgroundColor
    );

    Model::Tuple::ConstIterator tupleIterator = tuple.constBegin();

    bool isFirstEntry = true;
    renderTupleMembers(
        placementTracker,
        tupleIterator,
        leadingMemberCount,
        isFirstEntry,
        currentPresentationAreas,
        presenter,
        format,
        font,
        fontColor,
        fontBackgroundColor,
        lineSpacing,
        relativeScale
    );

    if (hiddenMemberCount > 0) {
        QString hiddenEntryString;
        if (!isFirstEntry) {
            hiddenEntryString = tr(", ");
        } else {
            isFirstEntry = false;
        }

        if (showHiddenMemberCount) {
            if (leadingMemberCount > 0) {
                hiddenEntryString += tr("%1").arg(QChar(0x2026));
            }

            hiddenEntryString += tr("%2 entries").arg(hiddenMemberCount);

            if (trailingMemberCount > 0) {
                hiddenEntryString += tr("%1").arg(QChar(0x2026));
            }
        } else {
            hiddenEntryString += tr("%1").arg(QChar(0x2026));
        }

        addText(
            currentPresentationAreas,
            presenter,
            hiddenEntryString,
            font,
            fontColor,
            fontBackgroundColor,
            lineSpacing
        );

        tupleIterator.advance(hiddenMemberCount);
    }

    renderTupleMembers(
        placementTracker,
        tupleIterator,
        trailingMemberCount,
        isFirstEntry,
        currentPresentationAreas,
        presenter,
        format,
        font,
        fontColor,
        fontBackgroundColor,
        lineSpacing,
        relativeScale
    );

    addRightParenthesis(
        currentPresentationAreas,
        presenter,
        maximumChildHeight,
        EQt::GraphicsMathGroup::ParenthesisStyle::PARENTHESIS,
        QString(")"),
        font,
        fontColor,
        fontBackgroundColor
    );

    return currentPresentationAreas;
}


void TupleDataTypePresentationGenerator::calculateMaximumChildHeight(
        PlacementTracker*            placementTracker,
        Model::Tuple::ConstIterator& tupleIterator,
        unsigned long                memberCount,
        Ld::FormatPointer            format,
        float                        relativeScale,
        float&                       maximumAscent,
        float&                       maximumDescent
    ) {
    unsigned long index = 0;
    while (index < memberCount && !placementTracker->abortPlacement()) {
        const Model::Variant& childValue     = tupleIterator.constReference();
        Model::ValueType      childValueType = childValue.valueType();
        Ld::DataType          childDataType  = Ld::DataType::fromValueType(childValueType);

        if (childDataType.isValid()) {
            const DataTypePresentationGenerator*
                generator = dynamic_cast<const DataTypePresentationGenerator*>(childDataType.decoder());

            if (generator != Q_NULLPTR) {
                float childAscent;
                float childHeight  = generator->calculateHeight(
                    placementTracker,
                    childValue,
                    format,
                    relativeScale,
                    &childAscent
                );
                float childDescent = childHeight - childAscent;

                if (childAscent > maximumAscent) {
                    maximumAscent = childAscent;
                }

                if (childDescent > maximumDescent) {
                    maximumDescent = childDescent;
                }
            }
        }

        placementTracker->completedJob();

        ++index;
    }
}


void TupleDataTypePresentationGenerator::renderTupleMembers(
        PlacementTracker*                     placementTracker,
        Model::Tuple::ConstIterator&          tupleIterator,
        unsigned long                         memberCount,
        bool&                                 isFirstEntry,
        DataTypePresenter::PresentationAreas& currentPresentationAreas,
        DataTypePresenter*                    presenter,
        Ld::FormatPointer                     format,
        const QFont&                          font,
        const QColor&                         fontColor,
        const QColor&                         fontBackgroundColor,
        float                                 lineSpacing,
        float                                 relativeScale
    ) {
    placementTracker->addNewJobs(memberCount);

    unsigned long index = 0;
    while (index < memberCount && !placementTracker->abortPlacement()) {
        if (!isFirstEntry) {
            addText(
                currentPresentationAreas,
                presenter,
                tr(", "),
                font,
                fontColor,
                fontBackgroundColor,
                lineSpacing
            );
        } else {
            isFirstEntry = false;
        }

        const Model::Variant& childVariant   = tupleIterator.constReference();
        Model::ValueType      childValueType = childVariant.valueType();
        Ld::DataType          childDataType  = Ld::DataType::fromValueType(childValueType);
        if (childDataType.isValid()) {
            const Ld::DataTypeDecoder* childDecoder = childDataType.decoder();
            if (childDecoder != Q_NULLPTR) {
                const DataTypePresentationGenerator*
                    childGenerator = dynamic_cast<const DataTypePresentationGenerator*>(childDecoder);

                if (childGenerator != Q_NULLPTR) {
                    childGenerator->generateGraphicsItems(
                        placementTracker,
                        currentPresentationAreas,
                        presenter,
                        childVariant,
                        format,
                        lineSpacing,
                        relativeScale,
                        true
                    );
                } else {
                    addText(
                        currentPresentationAreas,
                        presenter,
                        tr("*** MISSING GENERATOR %1 ***").arg(static_cast<unsigned int>(childValueType)),
                        font,
                        fontColor,
                        fontBackgroundColor,
                        lineSpacing
                    );
                }
            } else {
                addText(
                    currentPresentationAreas,
                    presenter,
                    tr("*** MISSING DECODER %1 ***").arg(static_cast<unsigned int>(childValueType)),
                    font,
                    fontColor,
                    fontBackgroundColor,
                    lineSpacing
                );
            }
        } else {
            addText(
                currentPresentationAreas,
                presenter,
                tr("*** UNKNOWN TYPE %1 ***").arg(static_cast<unsigned int>(childValueType)),
                font,
                fontColor,
                fontBackgroundColor,
                lineSpacing
            );
        }

        placementTracker->completedJob();

        ++index;
        ++tupleIterator;
    }
}

