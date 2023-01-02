/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetDataTypePresentationGenerator class.
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
#include <model_set.h>
#include <model_variant.h>

#include <ld_format_structures.h>
#include <ld_data_type.h>
#include <ld_data_type_format.h>
#include <ld_set_data_type_format.h>
#include <ld_character_format.h>
#include <ld_complex_data_type_decoder.h>
#include <ld_value_field_format.h>

#include "application.h"
#include "placement_tracker.h"
#include "data_type_presentation_generator.h"
#include "set_data_type_presentation_generator.h"

const SetDataTypePresentationGenerator SetDataTypePresentationGenerator::instance;

SetDataTypePresentationGenerator::SetDataTypePresentationGenerator() {}


SetDataTypePresentationGenerator::~SetDataTypePresentationGenerator() {}


float SetDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     placementTracker,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    unsigned long leadingMemberCount;
    unsigned long trailingMemberCount;
    unsigned long hiddenMemberCount;
    QFont         font;

    if (!format.isNull()                                                         &&
        (format->capabilities().contains(Ld::SetDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)     )    ) {
        QSharedPointer<Ld::SetDataTypeFormat> setFormat = format.dynamicCast<Ld::SetDataTypeFormat>();

        leadingMemberCount  = setFormat->leadingMemberCount();
        trailingMemberCount = setFormat->trailingMemberCount();
        font                = setFormat->toQFont();
    } else {
        leadingMemberCount  = Ld::SetDataTypeFormat::defaultLeadingMemberCount;
        trailingMemberCount = Ld::SetDataTypeFormat::defaultTrailingMemberCount;

        QSharedPointer<Ld::CharacterFormat> defaultFont = Ld::CharacterFormat::applicationDefaultMathFont();
        font = defaultFont->toQFont();
    }

    bool       ok;
    Model::Set setValue = variant.toSet(&ok);
    Q_ASSERT(ok);

    unsigned long numberSetMembers = setValue.size();
    if (leadingMemberCount == Ld::SetDataTypeFormat::showAllMembers  ||
        trailingMemberCount == Ld::SetDataTypeFormat::showAllMembers ||
        leadingMemberCount + trailingMemberCount >= numberSetMembers    ) {
        leadingMemberCount  = numberSetMembers;
        hiddenMemberCount   = 0;
        trailingMemberCount = 0;
    } else {
        hiddenMemberCount = numberSetMembers - leadingMemberCount - trailingMemberCount;
    }

    font.setPointSizeF(font.pointSizeF() * Application::fontScaleFactor() * relativeScale);
    QFontMetricsF fontMetrics(font);
    float maximumAscent  = fontMetrics.ascent();
    float maximumDescent = fontMetrics.descent();

    Model::Set::ConstIterator setIterator = setValue.constBegin();

    placementTracker->addNewJobs(leadingMemberCount + trailingMemberCount);

    calculateMaximumChildHeight(
        placementTracker,
        setIterator,
        leadingMemberCount,
        format,
        relativeScale,
        maximumAscent,
        maximumDescent
    );

    setIterator.advance(hiddenMemberCount);

    calculateMaximumChildHeight(
        placementTracker,
        setIterator,
        trailingMemberCount,
        format,
        relativeScale,
        maximumAscent,
        maximumDescent
    );

    if (ascent != Q_NULLPTR) {
        *ascent = maximumAscent;
    }

    return maximumAscent + maximumDescent;
}


DataTypePresenter::PresentationAreas SetDataTypePresentationGenerator::generateGraphicsItems(
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

    bool       ok;
    Model::Set setValue = variant.toSet(&ok);
    Q_ASSERT(ok);

    unsigned long leadingMemberCount;
    unsigned long trailingMemberCount;
    unsigned long hiddenMemberCount;
    bool          showHiddenMemberCount;
    QFont         font;
    QColor        fontColor;
    QColor        fontBackgroundColor;

    if (!format.isNull()                                                         &&
        (format->capabilities().contains(Ld::SetDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)     )    ) {
        QSharedPointer<Ld::SetDataTypeFormat> setFormat = format.dynamicCast<Ld::SetDataTypeFormat>();

        leadingMemberCount    = setFormat->leadingMemberCount();
        trailingMemberCount   = setFormat->trailingMemberCount();
        showHiddenMemberCount = setFormat->displayHiddenMemberCount();

        font                  = setFormat->toQFont();
        fontColor             = setFormat->fontColor();
        fontBackgroundColor   = setFormat->fontBackgroundColor();
    } else {
        leadingMemberCount    = Ld::SetDataTypeFormat::defaultLeadingMemberCount;
        trailingMemberCount   = Ld::SetDataTypeFormat::defaultTrailingMemberCount;
        showHiddenMemberCount = Ld::SetDataTypeFormat::defaultShowHiddenMemberCount;

        QSharedPointer<Ld::CharacterFormat> defaultFont = Ld::CharacterFormat::applicationDefaultMathFont();
        font                = defaultFont->toQFont();
        fontColor           = defaultFont->fontColor();
        fontBackgroundColor = defaultFont->fontBackgroundColor();
    }

    unsigned long numberSetMembers = setValue.size();
    if (leadingMemberCount == Ld::SetDataTypeFormat::showAllMembers  ||
        trailingMemberCount == Ld::SetDataTypeFormat::showAllMembers ||
        leadingMemberCount + trailingMemberCount >= numberSetMembers    ) {
        leadingMemberCount  = numberSetMembers;
        hiddenMemberCount   = 0;
        trailingMemberCount = 0;
    } else {
        hiddenMemberCount = numberSetMembers - leadingMemberCount - trailingMemberCount;
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
        setValue,
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

    return currentPresentationAreas;
}


EQt::GraphicsMultiTextGroup* SetDataTypePresentationGenerator::toGraphicsItem(
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


Presentation::ReflowHint SetDataTypePresentationGenerator::reflowHint(bool afterAbort) const {
    return afterAbort ? Presentation::ReflowHint::ALWAYS_REFLOW : Presentation::ReflowHint::SUPPORTS_REFLOW;
}


DataTypePresenter::PresentationAreas SetDataTypePresentationGenerator::renderWrapped(
        PlacementTracker*                     placementTracker,
        DataTypePresenter::PresentationAreas& currentPresentationAreas,
        DataTypePresenter*                    presenter,
        const Model::Set&                     set,
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
        EQt::GraphicsMathGroup::ParenthesisStyle::BRACES,
        QString("{"),
        font,
        fontColor,
        fontBackgroundColor
    );

    Model::Set::ConstIterator setIterator = set.constBegin();

    bool isFirstEntry = true;
    renderSetMembers(
        placementTracker,
        setIterator,
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

        setIterator.advance(hiddenMemberCount);
    }

    renderSetMembers(
        placementTracker,
        setIterator,
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
        EQt::GraphicsMathGroup::ParenthesisStyle::BRACES,
        QString("}"),
        font,
        fontColor,
        fontBackgroundColor
    );

    return currentPresentationAreas;
}


void SetDataTypePresentationGenerator::calculateMaximumChildHeight(
        PlacementTracker*          placementTracker,
        Model::Set::ConstIterator& setIterator,
        unsigned long              memberCount,
        Ld::FormatPointer          format,
        float                      relativeScale,
        float&                     maximumAscent,
        float&                     maximumDescent
    ) {
    unsigned long index = 0;
    while (index < memberCount && !placementTracker->abortPlacement()) {
        const Model::Variant& childValue     = setIterator.constReference();
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


void SetDataTypePresentationGenerator::renderSetMembers(
        PlacementTracker*                     placementTracker,
        Model::Set::ConstIterator&            setIterator,
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

        const Model::Variant& childVariant   = setIterator.constReference();
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
        ++setIterator;
    }
}

