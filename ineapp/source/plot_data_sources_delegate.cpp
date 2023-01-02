/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref PlotDataSourcesDelegate class.
***********************************************************************************************************************/

#include <QApplication>
#include <QObject>
#include <QString>
#include <QStyle>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QStyleOption>
#include <QStyle>
#include <QComboBox>
#include <QLineEdit>
#include <QPalette>
#include <QFont>
#include <QFontMetrics>

#include <algorithm>

#include "application.h"
#include "metatypes.h"
#include "presentation.h"
#include "plot_series_demo_widget.h"
#include "plot_data_sources_model.h"
#include "plot_data_sources_delegate.h"

PlotDataSourcesDelegate::PlotDataSourcesDelegate(QObject* parent):QStyledItemDelegate(parent) {}


PlotDataSourcesDelegate::~PlotDataSourcesDelegate() {}


QSize PlotDataSourcesDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QStyledItemDelegate::sizeHint(option, index);
}


void PlotDataSourcesDelegate::paint(
        QPainter*                   painter,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index
    ) const {
    const PlotDataSourcesModel*        model        = dynamic_cast<const PlotDataSourcesModel*>(index.model());
    PlotDataSourcesModel::ContentsType contentsType = model->contentsType(index);
    QVariant                           cellContents = index.data();

    switch (contentsType) {
        case PlotDataSourcesModel::ContentsType::INVALID: {
            break;
        }

        case PlotDataSourcesModel::ContentsType::DATA_SOURCE_NAME: {
            paintDataSource(painter, option, index, cellContents.value<Ld::VariableName>());
            break;
        }

        case PlotDataSourcesModel::ContentsType::AXIS_LOCATION: {
            paintAxisLocation(painter, option, index, cellContents.value<PlotDataSourcesModel::AxisLocation>());
            break;
        }

        case PlotDataSourcesModel::ContentsType::LABEL: {
            paintSeriesLabels(painter, option, index, cellContents.toString());
            break;
        }

        case PlotDataSourcesModel::ContentsType::PLOT_SERIES: {
            paintPlotSeries(painter, option, index, cellContents.value<Ld::PlotSeries>());
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


QString PlotDataSourcesDelegate::toString(
        PlotDataSourcesModel::AxisLocationStyle axisLocationStyle,
        PlotDataSourcesModel::AxisLocation      axisLocation
    ) {
    QString rslt;
    switch (axisLocationStyle) {
        case PlotDataSourcesModel::AxisLocationStyle::CATEGORIZED: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("Left");        break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("Bottom");      break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("Right");       break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("Top");         break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("Z Axis");      break; }
                default:                                                { rslt = tr("???");         break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::PLOT_BORDERS: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("Left");        break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("Bottom");      break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("Right");       break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("Top");         break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("Z Axis");      break; }
                default:                                                { rslt = tr("???");         break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::UNCATEGORIZED:
        case PlotDataSourcesModel::AxisLocationStyle::FIXED: {
            Q_ASSERT(false); // What do we need to do here ?
            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::CARTESIAN_2D:
        case PlotDataSourcesModel::AxisLocationStyle::CARTESIAN_3D: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("y");    break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("x");    break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("y");    break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("x");    break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("z");    break; }
                default:                                                { rslt = tr("???");  break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::POLAR_2D: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("r");                      break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("%1").arg(QChar(0x03B8));  break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("r");                      break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("%1").arg(QChar(0x03B8));  break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("%1").arg(QChar(0x03B8));  break; }
                default:                                                { rslt = tr("???");                    break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::POLAR_3D: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("r");                      break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("%1").arg(QChar(0x03B8));  break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("r");                      break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("%1").arg(QChar(0x03B8));  break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("%1").arg(QChar(0x03C6));  break; }
                default:                                                { rslt = tr("???");                    break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::RGB_COLOR_SPACE: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("Red");    break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("Green");  break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("Red");    break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("Green");  break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("Blue");   break; }
                default:                                                { rslt = tr("???");    break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::CMYK_COLOR_SPACE: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("Cyan");     break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("Magenta");  break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("Yellow");   break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("Magenta");  break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("Black");    break; }
                default:                                                { rslt = tr("???");      break; }
            }

            break;
        }

        case PlotDataSourcesModel::AxisLocationStyle::GRAYSCALE_COLOR_SPACE: {
            switch (axisLocation) {
                case PlotDataSourcesModel::AxisLocation::LEFT_Y_R_RC:   { rslt = tr("Gray");  break; }
                case PlotDataSourcesModel::AxisLocation::BOTTOM_X_A_GM: { rslt = tr("Gray");  break; }
                case PlotDataSourcesModel::AxisLocation::RIGHT_Y_R_RY:  { rslt = tr("Gray");  break; }
                case PlotDataSourcesModel::AxisLocation::TOP_X_A_GM:    { rslt = tr("Gray");  break; }
                case PlotDataSourcesModel::AxisLocation::Z_B_BK:        { rslt = tr("Gray");  break; }
                default:                                                { rslt = tr("???");   break; }
            }

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return rslt;
}


void PlotDataSourcesDelegate::paintDataSource(
        QPainter*                   painter,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index,
        const Ld::VariableName&     dataSourceName
    ) const {
    QStyleOptionViewItem styleOption = option;
    initStyleOption(&styleOption, index);

    QStyle* style = styleOption.widget != Q_NULLPTR ? styleOption.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter, styleOption.widget);

    QPalette::ColorGroup colorGroup = styleOption.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (colorGroup == QPalette::Normal && (styleOption.state & QStyle::State_Active) == 0) {
        colorGroup = QPalette::Inactive;
    }

    painter->setPen(
        styleOption.palette.color(
            colorGroup,
            (styleOption.state & QStyle::State_Selected) != 0 ? QPalette::HighlightedText : QPalette::Text
        )
    );

    QString text1 = QString(" ") + dataSourceName.text1();
    QString text2 = dataSourceName.text2();

    const QFont&  baseFont          = styleOption.font;
    const QRectF& boundingRectangle = styleOption.rect;

    unsigned weight       = static_cast<unsigned>(baseFont.weight());
    int      weightOffset = static_cast<int>(
          (static_cast<int>(QFont::Black) - weight)
        * Presentation::subscriptWeightAdjustment
    );

    QFont subscriptFont = baseFont;
    subscriptFont.setPointSizeF(baseFont.pointSize() * Presentation::subscriptSizeAdjustment);
    subscriptFont.setWeight(static_cast<QFont::Weight>(weight + weightOffset));

    QFontMetrics baseFontMetrics(baseFont);

    int baseline   = baseFontMetrics.ascent();
    int text1Width = baseFontMetrics.horizontalAdvance(text1);

    QFontMetrics subscriptFontMetrics(subscriptFont);

    int subscriptPositionAdjustment = baseFontMetrics.height() * Presentation::subscriptBaseline;
    int subscriptBaseline           = baseline - subscriptPositionAdjustment;

    unsigned totalTextHeight = subscriptBaseline + subscriptFontMetrics.descent();
    int      yOffset         = (boundingRectangle.height() - totalTextHeight) / 2.0;

    painter->setFont(baseFont);
    painter->drawText(boundingRectangle.left(), boundingRectangle.top() + baseline + yOffset, text1);

    painter->setFont(subscriptFont);
    painter->drawText(
        boundingRectangle.left() + text1Width,
        boundingRectangle.top() + subscriptBaseline + yOffset,
        text2
    );
}


void PlotDataSourcesDelegate::paintAxisLocation(
        QPainter*                          painter,
        const QStyleOptionViewItem&        option,
        const QModelIndex&                 index,
        PlotDataSourcesModel::AxisLocation axisLocation
    ) const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(index.model());

    QString text;
    if (index.row() == 0) {
        if (model != Q_NULLPTR && model->firstRowIsBarLabels()) {
            text = tr("Data Labels");
        }
    }

    if (text.isEmpty()) {
        PlotDataSourcesModel::AxisLocationStyle axisStyle = model->axisLocationStyle();
        text = toString(axisStyle, axisLocation);
    }

    QStyleOptionViewItem styleOption = option;
    initStyleOption(&styleOption, index);

    QPalette::ColorGroup colorGroup = styleOption.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (colorGroup == QPalette::Normal && (styleOption.state & QStyle::State_Active) == 0) {
        colorGroup = QPalette::Inactive;
    }

    if (option.state & QStyle::StateFlag::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    painter->setPen(
        styleOption.palette.color(
            colorGroup,
            (styleOption.state & QStyle::State_Selected) != 0 ? QPalette::HighlightedText : QPalette::Text
        )
    );

    const QFont& baseFont = styleOption.font;
    QFontMetrics baseFontMetrics(baseFont);

    int baseline   = baseFontMetrics.ascent();
    int textHeight = baseFontMetrics.height();
    int yOffset    = (styleOption.rect.height() - textHeight) / 2;

    painter->setFont(baseFont);
    painter->drawText(styleOption.rect.left(), styleOption.rect.top() + baseline + yOffset, QString(" ") + text);
}


void PlotDataSourcesDelegate::paintSeriesLabels(
        QPainter*                   painter,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index,
        const QString&              seriesLabel
    ) const {
    QString text;
    if (index.row() == 0) {
        const PlotDataSourcesModel* dataSourcesModel = dynamic_cast<const PlotDataSourcesModel*>(index.model());
        if (dataSourcesModel != Q_NULLPTR && dataSourcesModel->firstRowIsBarLabels()) {
            text = tr("---");
        } else {
            text = seriesLabel;
        }
    } else {
        text = seriesLabel;
    }

    QStyleOptionViewItem styleOption = option;
    initStyleOption(&styleOption, index);

    QPalette::ColorGroup colorGroup = styleOption.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (colorGroup == QPalette::Normal && (styleOption.state & QStyle::State_Active) == 0) {
        colorGroup = QPalette::Inactive;
    }

    if (option.state & QStyle::StateFlag::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    painter->setPen(
        styleOption.palette.color(
            colorGroup,
            (styleOption.state & QStyle::State_Selected) != 0 ? QPalette::HighlightedText : QPalette::Text
        )
    );

    const QFont& baseFont = styleOption.font;
    QFontMetrics baseFontMetrics(baseFont);

    int baseline   = baseFontMetrics.ascent();
    int textHeight = baseFontMetrics.height();
    int yOffset    = (styleOption.rect.height() - textHeight) / 2;

    painter->setFont(baseFont);
    painter->drawText(styleOption.rect.left(), styleOption.rect.top() + baseline + yOffset, QString(" ") + text);
}


void PlotDataSourcesDelegate::paintPlotSeries(
        QPainter*                   painter,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index,
        const Ld::PlotSeries&       plotSeries
    ) const {
    const PlotDataSourcesModel* model      = dynamic_cast<const PlotDataSourcesModel*>(index.model());
    bool                        drawSeries = true;
    if (index.row() == 0) {
        const PlotDataSourcesModel* dataSourcesModel = dynamic_cast<const PlotDataSourcesModel*>(index.model());
        if (dataSourcesModel != Q_NULLPTR && dataSourcesModel->firstRowIsBarLabels()) {
            drawSeries = false;
        }
    }

    QStyleOptionViewItem styleOption = option;
    initStyleOption(&styleOption, index);

    const QRect& boundingRect = styleOption.rect;

    if (option.state & QStyle::StateFlag::State_Selected) {
        painter->fillRect(boundingRect, option.palette.highlight());
    }

    if (drawSeries) {
        PlotDataSourcesModel::PlotSeriesMode plotSeriesMode = model->supportedPlotSeriesMode();

        Ld::PlotSeries::LineStyle    lineStyle     = Ld::PlotSeries::LineStyle::NO_LINE;
        float                        lineWidth     = 0;
        QColor                       lineColor     = QColor(Qt::GlobalColor::white);
        Ld::PlotSeries::MarkerStyle  markerStyle   = Ld::PlotSeries::MarkerStyle::NONE;
        Ld::PlotSeries::SplineType   splineType    = Ld::PlotSeries::SplineType::NONE;
        Ld::PlotSeries::GradientType gradientType = Ld::PlotSeries::GradientType::NONE;

        switch (plotSeriesMode) {
            case PlotDataSourcesModel::PlotSeriesMode::FIXED: {
                break;
            }

            case PlotDataSourcesModel::PlotSeriesMode::LINES: {
                lineStyle  = plotSeries.lineStyle();
                lineWidth  = plotSeries.lineWidth();
                lineColor  = plotSeries.lineColor();
                splineType = plotSeries.splineType();

                break;
            }

            case PlotDataSourcesModel::PlotSeriesMode::MARKERS: {
                lineColor   = plotSeries.lineColor();
                markerStyle = plotSeries.markerStyle();

                break;
            }

            case PlotDataSourcesModel::PlotSeriesMode::LINES_AND_MARKERS: {
                lineStyle   = plotSeries.lineStyle();
                lineWidth   = plotSeries.lineWidth();
                lineColor   = plotSeries.lineColor();
                markerStyle = plotSeries.markerStyle();
                splineType  = plotSeries.splineType();

                break;
            }

            case PlotDataSourcesModel::PlotSeriesMode::COLOR_REGION: {
                lineColor = plotSeries.lineColor();
                break;

            }

            case PlotDataSourcesModel::PlotSeriesMode::GRADIENT: {
                lineColor    = plotSeries.lineColor();
                gradientType = plotSeries.gradientType();

                if (gradientType == Ld::PlotSeries::GradientType::NONE) {
                    gradientType = Ld::PlotSeries::GradientType::INTENSITY;
                }

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        PlotSeriesDemoWidget::paint(
            painter,
            boundingRect,
            lineStyle,
            lineWidth,
            lineColor,
            markerStyle,
            splineType,
            gradientType
        );
    }
}
