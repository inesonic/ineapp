/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CmykImagePresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QList>
#include <QRectF>
#include <QFont>
#include <QFontMetricsF>
#include <QScatterSeries>
#include <QAbstractAxis>
#include <QValueAxis>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QPen>

#include <cmath>

#include <model_matrix_integer.h>
#include <model_matrix_real.h>

#include <eqt_chart_item.h>
#include <eqt_graphics_pixmap_item.h>
#include <eqt_graphics_item_group.h>

#include "presentation.h"
#include "plot_wrapped_presentation_data.h"
#include "image_render_presentation_data.h"
#include "cmyk_image_presentation_data.h"

CmykImagePresentationData::CmykImagePresentationData() {}


CmykImagePresentationData::~CmykImagePresentationData() {}


unsigned CmykImagePresentationData::numberSeries() const {
    return 4;
}


QImage CmykImagePresentationData::render(
        const QMap<CmykImagePresentationData::AxisLocation, Model::MatrixInteger>& matrixByAxisLocation,
        QString&                                                                   errorString
    ) const {
    const Model::MatrixInteger& cyan    = matrixByAxisLocation.value(AxisLocation::LEFT_Y_R_RC);
    const Model::MatrixInteger& magenta = matrixByAxisLocation.value(AxisLocation::BOTTOM_X_A_GM);
    const Model::MatrixInteger& yellow  = matrixByAxisLocation.value(AxisLocation::RIGHT_Y_R_RY);
    const Model::MatrixInteger& black   = matrixByAxisLocation.value(AxisLocation::Z_B_BK);

    Model::Integer numberRows    = cyan.numberRows();
    Model::Integer numberColumns = cyan.numberColumns();

    QImage image(numberColumns, numberRows, QImage::Format::Format_ARGB32);

    Model::Integer columnIndex = 0;
    while (errorString.isEmpty() && columnIndex < numberColumns) {
        Model::Integer rowIndex = 0;
        while (errorString.isEmpty() && rowIndex < numberRows) {
            Model::Integer c = cyan.at(rowIndex + 1, columnIndex + 1);
            Model::Integer m = magenta.at(rowIndex + 1, columnIndex + 1);
            Model::Integer y = yellow.at(rowIndex + 1, columnIndex + 1);
            Model::Integer k = black.at(rowIndex + 1, columnIndex + 1);

            if (c < 0 || m < 0 || y < 0 || k < 0) {
                errorString = tr("Integer matrices can not have negative values.");
            } else if (c > 255 || m > 255 || y > 255 || k > 255) {
                errorString = tr("Integer matrices must contain values between 0 and 255, inclusive.");
            } else {
                image.setPixelColor(
                    columnIndex,
                    rowIndex,
                    QColor::fromCmyk(
                        static_cast<int>(c),
                        static_cast<int>(m),
                        static_cast<int>(y),
                        static_cast<int>(k)
                    )
                );

                ++rowIndex;
            }
        }

        ++columnIndex;
    }

    return image;
}


QImage CmykImagePresentationData::render(
        const QMap<CmykImagePresentationData::AxisLocation, Model::MatrixReal>& matrixByAxisLocation,
        QString&                                                                errorString
    ) const {
    const Model::MatrixReal& cyan    = matrixByAxisLocation.value(AxisLocation::LEFT_Y_R_RC);
    const Model::MatrixReal& magenta = matrixByAxisLocation.value(AxisLocation::BOTTOM_X_A_GM);
    const Model::MatrixReal& yellow  = matrixByAxisLocation.value(AxisLocation::RIGHT_Y_R_RY);
    const Model::MatrixReal& black   = matrixByAxisLocation.value(AxisLocation::Z_B_BK);

    Model::Integer numberRows    = cyan.numberRows();
    Model::Integer numberColumns = cyan.numberColumns();

    QImage image(numberColumns, numberRows, QImage::Format::Format_ARGB32);

    Model::Integer columnIndex = 0;
    while (errorString.isEmpty() && columnIndex < numberColumns) {
        Model::Integer rowIndex = 0;
        while (errorString.isEmpty() && rowIndex < numberRows) {
            Model::Real c = cyan.at(rowIndex + 1, columnIndex + 1);
            Model::Real m = magenta.at(rowIndex + 1, columnIndex + 1);
            Model::Real y = yellow.at(rowIndex + 1, columnIndex + 1);
            Model::Real k = black.at(rowIndex + 1, columnIndex + 1);

            if (c < 0 || m < 0 || y < 0 || k < 0) {
                errorString = tr("Real matrices can not have negative values.");
            } else if (c > 1 || m > 1 || y > 1 || k > 1) {
                errorString = tr("Real matrices must contain values between 0 and 1, inclusive.");
            } else {
                image.setPixelColor(columnIndex, rowIndex, QColor::fromCmykF(c, m, y, k));
                ++rowIndex;
            }
        }

        ++columnIndex;
    }

    return image;
}
