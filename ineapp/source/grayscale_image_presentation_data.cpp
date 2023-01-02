/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref GrayscaleImagePresentationData class.
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
#include "grayscale_image_presentation_data.h"

GrayscaleImagePresentationData::GrayscaleImagePresentationData() {}


GrayscaleImagePresentationData::~GrayscaleImagePresentationData() {}


unsigned GrayscaleImagePresentationData::numberSeries() const {
    return 1;
}


QImage GrayscaleImagePresentationData::render(
        const QMap<GrayscaleImagePresentationData::AxisLocation, Model::MatrixInteger>& matrixByAxisLocation,
        QString&                                                                        errorString
    ) const {
    const Model::MatrixInteger& grayscale     = matrixByAxisLocation.first();
    Model::Integer              numberRows    = grayscale.numberRows();
    Model::Integer              numberColumns = grayscale.numberColumns();

    QImage image(numberColumns, numberRows, QImage::Format::Format_RGB32);

    Model::Integer columnIndex = 0;
    while (errorString.isEmpty() && columnIndex < numberColumns) {
        Model::Integer rowIndex = 0;
        while (errorString.isEmpty() && rowIndex < numberRows) {
            Model::Integer gy = grayscale.at(rowIndex + 1, columnIndex + 1);

            if (gy < 0) {
                errorString = tr("Integer matrices can not have negative values.");
            } else if (gy > 255) {
                errorString = tr("Integer matrices must contain values between 0 and 255, inclusive.");
            } else {
                image.setPixelColor(columnIndex, rowIndex, QColor(gy, gy, gy));
                ++rowIndex;
            }
        }

        ++columnIndex;
    }

    return image;
}


QImage GrayscaleImagePresentationData::render(
        const QMap<GrayscaleImagePresentationData::AxisLocation, Model::MatrixReal>& matrixByAxisLocation,
        QString&                                                                     errorString
    ) const {
    const Model::MatrixReal& grayscale     = matrixByAxisLocation.first();
    Model::Integer           numberRows    = grayscale.numberRows();
    Model::Integer           numberColumns = grayscale.numberColumns();

    QImage image(numberColumns, numberRows, QImage::Format::Format_RGB32);

    Model::Integer columnIndex = 0;
    while (errorString.isEmpty() && columnIndex < numberColumns) {
        Model::Integer rowIndex = 0;
        while (errorString.isEmpty() && rowIndex < numberRows) {
            Model::Real gy = grayscale.at(rowIndex + 1, columnIndex + 1);

            if (gy < 0) {
                errorString = tr("Real matrices can not have negative values.");
            } else if (gy > 1) {
                errorString = tr("Real matrices must contain values between 0 and 1, inclusive.");
            } else {
                int p = std::min(255, static_cast<int>(256 * gy));
                image.setPixelColor(columnIndex, rowIndex, QColor(p, p, p));
                ++rowIndex;
            }
        }

        ++columnIndex;
    }

    return image;
}
