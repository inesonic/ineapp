/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SimpleTextDataTypePresentationGeneratorBase class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_multi_text_group.h>

#include <model_intrinsic_types.h>

#include <ld_format_structures.h>
#include <ld_data_type_format.h>
#include <ld_integer_data_type_format.h>
#include <ld_character_format.h>
#include <ld_value_field_format.h>

#include "application.h"
#include "placement_tracker.h"
#include "data_type_presentation_generator.h"
#include "simple_text_data_type_presentation_generator_base.h"

SimpleTextDataTypePresentationGeneratorBase::SimpleTextDataTypePresentationGeneratorBase() {}


SimpleTextDataTypePresentationGeneratorBase::~SimpleTextDataTypePresentationGeneratorBase() {}


float SimpleTextDataTypePresentationGeneratorBase::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& /* variant */,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float result;

    QFont font;
    if (!format.isNull()                                                             &&
        (format->capabilities().contains(Ld::IntegerDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)         )    ) {
        QSharedPointer<Ld::IntegerDataTypeFormat> dataTypeFormat = format.dynamicCast<Ld::IntegerDataTypeFormat>();
        font = dataTypeFormat->toQFont();
    } else {
        font = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    }

    font.setPointSizeF(font.pointSizeF() * Application::fontScaleFactor() * relativeScale);

    QFontMetricsF fontMetrics(font);
    result = fontMetrics.height();

    if (ascent != Q_NULLPTR) {
        *ascent = fontMetrics.ascent();
    }

    return result;
}


EQt::GraphicsMultiTextGroup* SimpleTextDataTypePresentationGeneratorBase::toGraphicsItem(
        PlacementTracker*     placementTracker,
        const Model::Variant& value,
        Ld::FormatPointer     format,
        DataTypePresenter*    presenter,
        QSizeF&               size,
        float&                ascent
    ) const {
    return updateGraphicsItem(
        placementTracker,
        new EQt::GraphicsMultiTextGroup,
        value,
        format,
        presenter,
        size,
        ascent
    );
}


EQt::GraphicsMultiTextGroup* SimpleTextDataTypePresentationGeneratorBase::updateGraphicsItem(
        PlacementTracker*            /* placementTracker */,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           /* presenter */,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    QString text = toDisplayedString(value, format);

    QFont font;
    if (!format.isNull()                                                             &&
        (format->capabilities().contains(Ld::IntegerDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)         )    ) {
        QSharedPointer<Ld::IntegerDataTypeFormat> dataTypeFormat = format.dynamicCast<Ld::IntegerDataTypeFormat>();

        font = dataTypeFormat->toQFont();

        QColor backgroundColor = dataTypeFormat->fontBackgroundColor();
        QColor textColor       = dataTypeFormat->fontColor();

        graphicsItem->setTextPen(QPen(textColor.isValid() ? textColor : QColor(Qt::black)));

        QBrush fontBackgroundBrush(backgroundColor.isValid() ? backgroundColor : QColor(255, 255, 255, 0));
        graphicsItem->setBackgroundBrush(fontBackgroundBrush);
    } else {
        font = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    }

    float fontScaleFactor = Application::fontScaleFactor();
    if (fontScaleFactor != 1.0F) {
        font.setPointSizeF(font.pointSizeF() * fontScaleFactor);
    }

    QFontMetricsF fontMetrics(font);
    ascent = fontMetrics.ascent();

    graphicsItem->append(text, font, QPointF(0, ascent));

    size.setWidth(fontMetrics.horizontalAdvance(text));
    size.setHeight(fontMetrics.height());

    return graphicsItem;
}
