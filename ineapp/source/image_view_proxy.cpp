/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QPageSize>
#include <QList>
#include <QMap>
#include <QSet>
#include <QIODevice>
#include <QFile>
#include <QMessageBox>

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_spin_box.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_image_element.h>
#include <ld_image_format.h>

#include "application.h"
#include "insert_element_command.h"
#include "update_format_command.h"
#include "image_file_dialog.h"
#include "document.h"
#include "main_window.h"
#include "view_widget.h"
#include "image_presentation.h"
#include "image_builder.h"
#include "image_format_dialog.h"
#include "image_view_proxy.h"

ImageViewProxy::ImageViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {
    currentImageFormatDialog = Q_NULLPTR;
}


ImageViewProxy::~ImageViewProxy() {}


void ImageViewProxy::bindDynamic() {
    ViewWidget* view = ImageViewProxy::view();
    updateImageFormatDockState(view->aggregationsByCapabilityWithoutAncestors());

    connectSignals();
}


void ImageViewProxy::unbindDynamic() {
    disconnectSignals();
}

void ImageViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithAncestors */,
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
    ) {
    disconnectSignals();
    updateImageFormatDockState(aggregationsByCapabilityWithoutAncestors);
    connectSignals();

    if (currentImageFormatDialog != Q_NULLPTR) {
        updateImageFormatDialog(currentImageFormatDialog);
    }
}


void ImageViewProxy::insertImageRequested() {
    ImageFileDialog* fileDialog = Application::imageFileDialog();
    fileDialog->configureMode(ImageFileDialog::DialogMode::IMPORT_IMAGE);

    bool accepted = fileDialog->exec();
    if (accepted) {
        Q_ASSERT(fileDialog->selectedFiles().count() == 1);
        QString filepath = fileDialog->selectedFiles()[0];
        QFile   imageFile(filepath);

        if (imageFile.open(QIODevice::ReadOnly)) {
            unsigned long long fileSize    = static_cast<unsigned long long>(imageFile.size());
            QByteArray         payloadData = imageFile.readAll();

            if (static_cast<unsigned long long>(payloadData.size()) == fileSize) {
                QSharedPointer<Ld::ImageElement> element = Ld::Element::createElement<Ld::ImageElement>();
                QSharedPointer<Ld::ImageFormat> format   = Ld::Format::createFormat<Ld::ImageFormat>();

                format->setHorizontalAxis(0.0F, Ld::ImageFormat::ImageScalingMode::IGNORED);
                format->setVerticalAxis(0.0F, Ld::ImageFormat::ImageScalingMode::IGNORED);

                element->setFormat(format);

                bool success = element->updatePayload(payloadData);
                if (!success) {
                    QMessageBox::warning(
                        window(),
                        tr("Could Not Update Local Store"),
                        tr("Could not update the local program store to hold the image\n%1").arg(filepath)
                    );
                } else {
                    ViewWidget* view = ImageViewProxy::view();
                    view->insertCommand(new InsertElementCommand(element));
                }
            } else {
                QMessageBox::warning(
                    window(),
                    tr("Could Not Load Image"),
                    tr("Could not load image file:\n%1").arg(filepath)
                );
            }
        } else {
            QMessageBox::warning(
                window(),
                tr("Could Not Load Image"),
                tr("Could not open image file:\n%1").arg(filepath)
            );
        }
    }
 }


void ImageViewProxy::formatDialogRequested() {
    MainWindow* window = ImageViewProxy::window();

    ImageFormatDialog imageFormatDialog(window);
    currentImageFormatDialog = &imageFormatDialog;

    imageFormatDialog.loadSettings(QString("ImageFormatDialog"));

    connect(&imageFormatDialog, &ImageFormatDialog::apply, this, &ImageViewProxy::imageFormatDialogApplyUpdates);

    updateImageFormatDialog(&imageFormatDialog);

    int result = imageFormatDialog.exec();
    currentImageFormatDialog = Q_NULLPTR;

    if (result == ImageFormatDialog::Accepted) {
        imageFormatDialogApplyUpdates(&imageFormatDialog);
    }
}


void ImageViewProxy::imageFormatDialogApplyUpdates(ImageFormatDialog* dialog) {
    dialog->saveSettings("ImageFormatDialog");

    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> newFormat     = element->format()->clone().dynamicCast<Ld::ImageFormat>();
            bool                            formatUpdated = false;

            if (dialog->isRotationSet()) {
                Ld::ImageFormat::Rotation newRotation = dialog->rotation();
                if (newRotation != newFormat->rotation()) {
                    newFormat->setRotation(newRotation);
                    formatUpdated = true;
                }
            }

            Ld::ImageFormat::ImageScalingMode newHorizontalScalingMode = dialog->horizontalScalingMode();
            if (newHorizontalScalingMode != Ld::ImageFormat::ImageScalingMode::INVALID) {
                Ld::ImageFormat::Axis axis        = newFormat->horizontalAxis();
                bool                  axisUpdated = false;

                if (axis.scalingMode() != newHorizontalScalingMode) {
                    axis.setScalingMode(newHorizontalScalingMode);
                    axisUpdated = true;
                }

                if (dialog->isHorizontalValueSet()) {
                    float newValue = dialog->horizontalValue();
                    if (axis.value() != newValue) {
                        axis.setValue(newValue);
                        axisUpdated = true;
                    }
                }

                if (axisUpdated) {
                    newFormat->setHorizontalAxis(axis);
                    formatUpdated = true;
                }
            }

            Ld::ImageFormat::ImageScalingMode newVerticalScalingMode = dialog->verticalScalingMode();
            if (newVerticalScalingMode != Ld::ImageFormat::ImageScalingMode::INVALID) {
                Ld::ImageFormat::Axis axis        = newFormat->verticalAxis();
                bool                  axisUpdated = false;

                if (axis.scalingMode() != newVerticalScalingMode) {
                    axis.setScalingMode(newVerticalScalingMode);
                    axisUpdated = true;
                }

                if (dialog->isVerticalValueSet()) {
                    float newValue = dialog->verticalValue();
                    if (axis.value() != newValue) {
                        axis.setValue(newValue);
                        axisUpdated = true;
                    }
                }

                if (axisUpdated) {
                    newFormat->setVerticalAxis(axis);
                    formatUpdated = true;
                }
            }

            if (formatUpdated) {
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void ImageViewProxy::rotateImageRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            Ld::ImageFormat::Rotation rotation = format->rotation();
            Ld::ImageFormat::Rotation newRotation;
            switch (rotation) {
                case Ld::ImageFormat::Rotation::NO_ROTATION: {
                    newRotation = Ld::ImageFormat::Rotation::ROTATE_CW_90;
                    break;
                }

                case Ld::ImageFormat::Rotation::ROTATE_CW_90: {
                    newRotation = Ld::ImageFormat::Rotation::FLIP;
                    break;
                }

                case Ld::ImageFormat::Rotation::FLIP: {
                    newRotation = Ld::ImageFormat::Rotation::ROTATE_CCW_90;
                    break;
                }

                case Ld::ImageFormat::Rotation::ROTATE_CCW_90: {
                    newRotation = Ld::ImageFormat::Rotation::NO_ROTATION;
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    newRotation = Ld::ImageFormat::Rotation::NO_ROTATION;
                    break;
                }
            }

            QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();
            newFormat->setRotation(newRotation);
            formatsByElement.insert(element, newFormat);
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::widthNoScalingModeRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->horizontalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::IGNORED) {
                QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData  = format->horizontalAxis();
                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::IGNORED);
                newFormat->setHorizontalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::widthMaintainAspectRatioRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->horizontalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO) {
                QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData  = format->horizontalAxis();
                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
                newFormat->setHorizontalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::widthPercentageScalingModeRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format       = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->horizontalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::FRACTIONAL) {
                ImagePresentation*              presentation  = dynamic_cast<ImagePresentation*>(element->visual());
                QSizeF                          prescaledSize = presentation->prescaledImageSizePoints();
                QSizeF                          displayedSize = presentation->displayedImageSizePoints();

                QSharedPointer<Ld::ImageFormat> newFormat     = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData      = format->horizontalAxis();

                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);
                axisData.setValue(displayedSize.width() / prescaledSize.width());

                newFormat->setHorizontalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::widthDimensionScalingModeRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format       = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->horizontalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::FIXED_POINTS) {
                ImagePresentation*              presentation  = dynamic_cast<ImagePresentation*>(element->visual());
                QSizeF                          displayedSize = presentation->displayedImageSizePoints();

                QSharedPointer<Ld::ImageFormat> newFormat     = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData      = format->horizontalAxis();

                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);
                axisData.setValue(displayedSize.width());

                newFormat->setHorizontalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::heightNoScalingModeRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->verticalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::IGNORED) {
                QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData  = format->verticalAxis();
                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::IGNORED);
                newFormat->setVerticalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::heightMaintainAspectRatioRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->verticalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO) {
                QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData  = format->verticalAxis();
                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
                newFormat->setVerticalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::heightPercentageScalingModeRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format       = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->verticalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::FRACTIONAL) {
                ImagePresentation*              presentation  = dynamic_cast<ImagePresentation*>(element->visual());
                QSizeF                          prescaledSize = presentation->prescaledImageSizePoints();
                QSizeF                          displayedSize = presentation->displayedImageSizePoints();

                QSharedPointer<Ld::ImageFormat> newFormat     = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData      = format->verticalAxis();

                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);
                axisData.setValue(displayedSize.height() / prescaledSize.height());

                newFormat->setVerticalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::heightDimensionScalingModeRequested() {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            if (format->verticalAxis().scalingMode() != Ld::ImageFormat::ImageScalingMode::FIXED_POINTS) {
                ImagePresentation*              presentation  = dynamic_cast<ImagePresentation*>(element->visual());
                QSizeF                          displayedSize = presentation->displayedImageSizePoints();

                QSharedPointer<Ld::ImageFormat> newFormat     = format->clone().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData      = format->verticalAxis();

                axisData.setScalingMode(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);
                axisData.setValue(displayedSize.height());

                newFormat->setVerticalAxis(axisData);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void ImageViewProxy::widthPercentageScalingChanged() {
    bool        ok;
    MainWindow* window                     = ImageViewProxy::window();
    QAction*    formatImagePercentageWidth = window->action("format_image_percentage_width");
    float       newValue                   = formatImagePercentageWidth->data().toFloat(&ok) / 100.0F;

    if (ok) {
        const Ld::ImageFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::ImageFormat> format   = element->format().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData = format->horizontalAxis();
                if (axisData.scalingMode() == Ld::ImageFormat::ImageScalingMode::FRACTIONAL &&
                    axisData.value()       != newValue                                         ) {
                    QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();

                    axisData.setValue(newValue);

                    newFormat->setHorizontalAxis(axisData);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void ImageViewProxy::widthDimensionScalingChanged() {
    bool        ok;
    MainWindow* window                    = ImageViewProxy::window();
    QAction*    formatImageDimensionWidth = window->action("format_image_dimension_width");
    float       newValue                  = formatImageDimensionWidth->data().toFloat(&ok);

    if (ok) {
        const Ld::ImageFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::ImageFormat> format   = element->format().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData = format->horizontalAxis();
                if (axisData.scalingMode() == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS &&
                    axisData.value()       != newValue                                           ) {
                    QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();

                    axisData.setValue(newValue);

                    newFormat->setHorizontalAxis(axisData);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void ImageViewProxy::heightPercentageScalingChanged() {
    bool        ok;
    MainWindow* window                      = ImageViewProxy::window();
    QAction*    formatImagePercentageHeight = window->action("format_image_percentage_height");
    float       newValue                    = formatImagePercentageHeight->data().toFloat(&ok) / 100.0F;

    if (ok) {
        const Ld::ImageFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::ImageFormat> format   = element->format().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData = format->verticalAxis();
                if (axisData.scalingMode() == Ld::ImageFormat::ImageScalingMode::FRACTIONAL &&
                    axisData.value()       != newValue                                         ) {
                    QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();

                    axisData.setValue(newValue);

                    newFormat->setVerticalAxis(axisData);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void ImageViewProxy::heightDimensionScalingChanged() {
    bool        ok;
    MainWindow* window                     = ImageViewProxy::window();
    QAction*    formatImageDimensionHeight = window->action("format_image_dimension_height");
    float       newValue                   = formatImageDimensionHeight->data().toFloat(&ok);

    if (ok) {
        const Ld::ImageFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::ImageFormat> format   = element->format().dynamicCast<Ld::ImageFormat>();
                Ld::ImageFormat::Axis           axisData = format->verticalAxis();
                if (axisData.scalingMode() == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS &&
                    axisData.value()       != newValue                                           ) {
                    QSharedPointer<Ld::ImageFormat> newFormat = format->clone().dynamicCast<Ld::ImageFormat>();

                    axisData.setValue(newValue);

                    newFormat->setVerticalAxis(axisData);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void ImageViewProxy::updateImageFormatDockState(const Ld::AggregationsByCapability& aggregationsByCapability) {
    currentAggregationsByCapability = aggregationsByCapability;

    if (aggregationsByCapability.contains(Ld::ImageFormat::formatName)) {
        const Ld::ImageFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

        MainWindow* window = ImageViewProxy::window();

        QAction* formatImageDialogAction = window->action("format_image_dialog");
        QAction* formatImageRotateAction = window->action("format_image_rotate");

        formatImageDialogAction->setEnabled(true);
        formatImageRotateAction->setEnabled(true);

        QSet<Ld::ImageFormat::ImageScalingMode> horizontalScalingModes = aggregation.horizontalScalingModes();
        QSet<float>                             horizontalValues       = aggregation.horizontalValues();
        QSet<Ld::ImageFormat::ImageScalingMode> verticalScalingModes   = aggregation.verticalScalingModes();
        QSet<float>                             verticalValues         = aggregation.verticalValues();

        updateScalingControl(
            horizontalScalingModes,
            horizontalValues,
            true,
            verticalScalingModes.contains(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO)
        );

        updateScalingControl(
            verticalScalingModes,
            verticalValues,
            false,
            horizontalScalingModes.contains(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO)
        );
    } else {
        disableFormatControls();
    }
}


void ImageViewProxy::issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement) {
    ViewWidget* view = ImageViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement));
}


void ImageViewProxy::connectSignals() {
    MainWindow* window = ImageViewProxy::window();
    ViewWidget* view   = ImageViewProxy::view();

    sessionConnect(view, &ViewWidget::formatsAtCursorChanged, &ImageViewProxy::formatsUnderCursorChanged);

    QAction* insertImageDialogAction = window->action("insert_image_dialog");
    sessionConnect(insertImageDialogAction, &QAction::triggered, &ImageViewProxy::insertImageRequested);

    QAction* formatImageDialogAction = window->action("format_image_dialog");
    sessionConnect(formatImageDialogAction, &QAction::triggered, &ImageViewProxy::formatDialogRequested);

    QAction* formatImageRotateAction = window->action("format_image_rotate");
    sessionConnect(formatImageRotateAction, &QAction::triggered, &ImageViewProxy::rotateImageRequested);

    QAction* formatImagePercentageWidth = window->action("format_image_percentage_width");
    sessionConnect(formatImagePercentageWidth, &QAction::triggered, &ImageViewProxy::widthPercentageScalingChanged);

    QAction* formatImageDimensionWidth  = window->action("format_image_dimension_width");
    sessionConnect(formatImageDimensionWidth, &QAction::triggered, &ImageViewProxy::widthDimensionScalingChanged);

    QAction* formatImagePercentageHeight = window->action("format_image_percentage_height");
    sessionConnect(formatImagePercentageHeight, &QAction::triggered, &ImageViewProxy::heightPercentageScalingChanged);

    QAction* formatImageDimensionHeight = window->action("format_image_dimension_height");
    sessionConnect(formatImageDimensionHeight, &QAction::triggered, &ImageViewProxy::heightDimensionScalingChanged);

    QAction* formatImageHorizontalNoScaling = window->action("format_image_width_no_scaling");
    sessionConnect(formatImageHorizontalNoScaling, &QAction::triggered, &ImageViewProxy::widthNoScalingModeRequested);

    QAction* formatImageHorizontalHonorAspectRatio = window->action("format_image_width_honor_aspect_ratio");
    sessionConnect(
        formatImageHorizontalHonorAspectRatio,
        &QAction::triggered,
        &ImageViewProxy::widthMaintainAspectRatioRequested
    );

    QAction* formatImageHorizontalPercentageScaling = window->action("format_image_width_percentage_scaling");
    sessionConnect(
        formatImageHorizontalPercentageScaling,
        &QAction::triggered,
        &ImageViewProxy::widthPercentageScalingModeRequested
    );

    QAction* formatImageHorizontalDimensionScaling = window->action("format_image_width_dimension_scaling");
    sessionConnect(
        formatImageHorizontalDimensionScaling,
        &QAction::triggered,
        &ImageViewProxy::widthDimensionScalingModeRequested
    );

    QAction* formatImageVerticalNoScaling = window->action("format_image_height_no_scaling");
    sessionConnect(formatImageVerticalNoScaling, &QAction::triggered, &ImageViewProxy::heightNoScalingModeRequested);

    QAction* formatImageVerticalHonorAspectRatio = window->action("format_image_height_honor_aspect_ratio");
    sessionConnect(
        formatImageVerticalHonorAspectRatio,
        &QAction::triggered,
        &ImageViewProxy::heightMaintainAspectRatioRequested
    );

    QAction* formatImageVerticalPercentageScaling = window->action("format_image_height_percentage_scaling");
    sessionConnect(
        formatImageVerticalPercentageScaling,
        &QAction::triggered,
        &ImageViewProxy::heightPercentageScalingModeRequested
    );

    QAction* formatImageVerticalDimensionScaling = window->action("format_image_height_dimension_scaling");
    sessionConnect(
        formatImageVerticalDimensionScaling,
        &QAction::triggered,
        &ImageViewProxy::heightDimensionScalingModeRequested
    );
}


void ImageViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void ImageViewProxy::updateScalingControl(
        const QSet<Ld::ImageFormat::ImageScalingMode>& scalingModes,
        const QSet<float>&                             values,
        bool                                           horizontal,
        bool                                           disableAspectRatioScalingMode
    ) {
    MainWindow*                  window = ImageViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(ImageBuilder::imageFormatDockName);

    QAction* percentageAction;
    QAction* dimensionAction;
    QAction* noScalingAction;
    QAction* aspectRatioScalingAction;
    QAction* percentageScalingAction;
    QAction* dimensionScalingAction;
    QAction* scalingModeAction;

    EQt::SpinBox*           horizontalSpinBox;
    EQt::SpinBox*           verticalSpinBox;
    EQt::DimensionLineEdit* horizontalLineEdit;
    EQt::DimensionLineEdit* verticalLineEdit;

    if (horizontal) {
        percentageAction         = window->action("format_image_percentage_width");
        dimensionAction          = window->action("format_image_dimension_width");

        noScalingAction          = window->action("format_image_width_no_scaling");
        aspectRatioScalingAction = window->action("format_image_width_honor_aspect_ratio");
        percentageScalingAction  = window->action("format_image_width_percentage_scaling");
        dimensionScalingAction   = window->action("format_image_width_dimension_scaling");
        scalingModeAction        = window->action("format_image_width_mode");

        horizontalSpinBox  = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>("horizontal_width_spin_box");
        horizontalLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
            "horizontal_width_line_edit"
        );
        verticalSpinBox    = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>("vertical_width_spin_box");
        verticalLineEdit   = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("vertical_width_line_edit");
    } else {
        percentageAction         = window->action("format_image_percentage_height");
        dimensionAction          = window->action("format_image_dimension_height");

        noScalingAction          = window->action("format_image_height_no_scaling");
        aspectRatioScalingAction = window->action("format_image_height_honor_aspect_ratio");
        percentageScalingAction  = window->action("format_image_height_percentage_scaling");
        dimensionScalingAction   = window->action("format_image_height_dimension_scaling");
        scalingModeAction        = window->action("format_image_height_mode");

        horizontalSpinBox  = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>("horizontal_height_spin_box");
        horizontalLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
            "horizontal_height_line_edit"
        );
        verticalSpinBox    = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>("vertical_height_spin_box");
        verticalLineEdit   = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
            "vertical_height_line_edit"
        );
    }

    noScalingAction->setEnabled(true);
    aspectRatioScalingAction->setEnabled(!disableAspectRatioScalingMode);
    percentageScalingAction->setEnabled(true);
    dimensionScalingAction->setEnabled(true);
    scalingModeAction->setEnabled(true);

    bool noScalingMode          = scalingModes.contains(Ld::ImageFormat::ImageScalingMode::IGNORED);
    bool aspectRatioScalingMode = scalingModes.contains(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    bool percentageScalingMode  = scalingModes.contains(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);
    bool dimensionScalingMode   = scalingModes.contains(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);

    noScalingAction->setChecked(noScalingMode);
    aspectRatioScalingAction->setChecked(aspectRatioScalingMode);
    percentageScalingAction->setChecked(percentageScalingMode);
    dimensionScalingAction->setChecked(dimensionScalingMode);

    if (scalingModes.count() == 1) {
        if (percentageScalingMode) {
            scalingModeAction->setIcon(EQt::Application::icon("format_image_scale_percentage"));

            horizontalSpinBox->setEnabled(true);
            verticalSpinBox->setEnabled(true);
            horizontalLineEdit->setEnabled(false);
            verticalLineEdit->setEnabled(false);

            horizontalLineEdit->setVisible(false);
            verticalLineEdit->setVisible(false);
            horizontalSpinBox->setVisible(true);
            verticalSpinBox->setVisible(true);

            if (values.count() == 1) {
                float value = *values.begin();
                percentageAction->setData(QVariant(static_cast<unsigned>(100.0F * value + 0.5F)));
            } else {
                percentageAction->setData(QVariant());
            }
        } else {
            horizontalSpinBox->setEnabled(false);
            verticalSpinBox->setEnabled(false);
            horizontalLineEdit->setEnabled(dimensionScalingMode);
            verticalLineEdit->setEnabled(dimensionScalingMode);

            horizontalSpinBox->setVisible(false);
            verticalSpinBox->setVisible(false);
            horizontalLineEdit->setVisible(true);
            verticalLineEdit->setVisible(true);

            if (noScalingMode) {
                if (horizontal) {
                    scalingModeAction->setIcon(EQt::Application::icon("format_image_no_horizontal_scaling"));
                } else {
                    scalingModeAction->setIcon(EQt::Application::icon("format_image_no_vertical_scaling"));
                }
                dimensionAction->setData(QVariant());
                dimensionAction->setEnabled(false);
            } else if (aspectRatioScalingMode) {
                scalingModeAction->setIcon(EQt::Application::icon("format_image_aspect_ratio"));
                dimensionAction->setData(QVariant());
                dimensionAction->setEnabled(false);
            } else {
                Q_ASSERT(dimensionScalingMode);

                if (horizontal) {
                    scalingModeAction->setIcon(EQt::Application::icon("format_image_scale_horizontal_dimension"));
                } else {
                    scalingModeAction->setIcon(EQt::Application::icon("format_image_scale_vertical_dimension"));
                }

                dimensionAction->setEnabled(true);

                if (values.count() == 1) {
                    float value = *values.begin();
                    dimensionAction->setData(QVariant(value));
                } else {
                    dimensionAction->setData(QVariant());
                }
            }
        }
    } else {
        scalingModeAction->setIcon(Application::icon("format_image_scale_unknown"));

        horizontalSpinBox->setEnabled(false);
        verticalSpinBox->setEnabled(false);
        horizontalLineEdit->setEnabled(false);
        verticalLineEdit->setEnabled(false);

        horizontalSpinBox->setVisible(false);
        verticalSpinBox->setVisible(false);
        horizontalLineEdit->setVisible(true);
        verticalLineEdit->setVisible(true);
    }

    dimensionAction->trigger();
    percentageAction->trigger();
}


void ImageViewProxy::updateImageFormatDialog(ImageFormatDialog* dialog) {
    const Ld::ImageFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ImageFormat>();

    QSet<ImageFormatDialog::Axis> widthAxisData;
    QSet<ImageFormatDialog::Axis> heightAxisData;

    for (  Ld::ImageFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::ImageFormat> format = element->format().dynamicCast<Ld::ImageFormat>();
            Q_ASSERT(!format.isNull());

            ImagePresentation* presentation = dynamic_cast<ImagePresentation*>(element->visual());
            Q_ASSERT(presentation != Q_NULLPTR);

            QSizeF prescaledSizePoints = presentation->prescaledImageSizePoints();
            QSizeF displayedSizePoints = presentation->displayedImageSizePoints();

            widthAxisData << ImageFormatDialog::Axis(
                format->horizontalAxis(),
                prescaledSizePoints.width(),
                displayedSizePoints.width()
            );

            heightAxisData << ImageFormatDialog::Axis(
                format->verticalAxis(),
                prescaledSizePoints.height(),
                displayedSizePoints.height()
            );
        }
    }

    dialog->setHorizontalScaling(widthAxisData);
    dialog->setVerticalScaling(heightAxisData);
    dialog->setRotations(aggregation.rotations());
}


void ImageViewProxy::disableFormatControls() {
    MainWindow*                  window = ImageViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(ImageBuilder::imageFormatDockName);

    QAction* formatImageDialogAction = window->action("format_image_dialog");
    QAction* formatImageRotateAction = window->action("format_image_rotate");

    formatImageDialogAction->setEnabled(false);
    formatImageRotateAction->setEnabled(false);

    QAction* formatImageHorizontalMode = window->action("format_image_width_mode");
    QAction* formatImageVerticalMode   = window->action("format_image_height_mode");

    formatImageHorizontalMode->setIcon(Application::icon("format_image_scale_unknown"));
    formatImageVerticalMode->setIcon(Application::icon("format_image_scale_unknown"));

    QAction* formatImagePercentageWidth             = window->action("format_image_percentage_width");
    QAction* formatImageDimensionWidth              = window->action("format_image_dimension_width");
    QAction* formatImagePercentageHeight            = window->action("format_image_percentage_height");
    QAction* formatImageDimensionHeight             = window->action("format_image_dimension_height");
    QAction* formatImageHorizontalNoScaling         = window->action("format_image_width_no_scaling");
    QAction* formatImageHorizontalHonorAspectRatio  = window->action("format_image_width_honor_aspect_ratio");
    QAction* formatImageHorizontalPercentageScaling = window->action("format_image_width_percentage_scaling");
    QAction* formatImageHorizontalDimensionScaling  = window->action("format_image_width_dimension_scaling");
    QAction* formatImageVerticalNoScaling           = window->action("format_image_height_no_scaling");
    QAction* formatImageVerticalHonorAspectRatio    = window->action("format_image_height_honor_aspect_ratio");
    QAction* formatImageVerticalPercentageScaling   = window->action("format_image_height_percentage_scaling");
    QAction* formatImageVerticalDimensionScaling    = window->action("format_image_height_dimension_scaling");

    formatImageHorizontalMode->setEnabled(false);
    formatImageHorizontalNoScaling->setEnabled(false);
    formatImageHorizontalHonorAspectRatio->setEnabled(false);
    formatImageHorizontalPercentageScaling->setEnabled(false);
    formatImageHorizontalDimensionScaling->setEnabled(false);
    formatImageVerticalMode->setEnabled(false);
    formatImageVerticalNoScaling->setEnabled(false);
    formatImageVerticalHonorAspectRatio->setEnabled(false);
    formatImageVerticalPercentageScaling->setEnabled(false);
    formatImageVerticalDimensionScaling->setEnabled(false);

    formatImagePercentageWidth->setData(QVariant());
    formatImageDimensionWidth->setData(QVariant());
    formatImagePercentageHeight->setData(QVariant());
    formatImageDimensionHeight->setData(QVariant());

    formatImagePercentageWidth->trigger();
    formatImageDimensionWidth->trigger();
    formatImagePercentageHeight->trigger();
    formatImageDimensionHeight->trigger();

    EQt::SpinBox* horizontalWidthSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
        "horizontal_width_spin_box"
    );
    EQt::DimensionLineEdit* horizontalWidthLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("horizontal_width_line_edit");
    EQt::SpinBox* verticalWidthSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
        "vertical_width_spin_box"
    );
    EQt::DimensionLineEdit* verticalWidthLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_width_line_edit"
    );

    EQt::SpinBox* horizontalHeightSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
        "horizontal_height_spin_box"
    );
    EQt::DimensionLineEdit* horizontalHeightLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("horizontal_height_line_edit");
    EQt::SpinBox* verticalHeightSpinBox = dock->EQt::ProgrammaticWidget::widget<EQt::SpinBox>(
        "vertical_height_spin_box"
    );
    EQt::DimensionLineEdit* verticalHeightLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_height_line_edit"
    );

    horizontalWidthSpinBox->setVisible(false);
    horizontalWidthLineEdit->setVisible(true);
    verticalWidthSpinBox->setVisible(false);
    verticalWidthLineEdit->setVisible(true);

    horizontalHeightSpinBox->setVisible(false);
    horizontalHeightLineEdit->setVisible(true);
    verticalHeightSpinBox->setVisible(false);
    verticalHeightLineEdit->setVisible(true);

    horizontalWidthSpinBox->setEnabled(false);
    horizontalWidthLineEdit->setEnabled(false);
    verticalWidthSpinBox->setEnabled(false);
    verticalWidthLineEdit->setEnabled(false);

    horizontalHeightSpinBox->setEnabled(false);
    horizontalHeightLineEdit->setEnabled(false);
    verticalHeightSpinBox->setEnabled(false);
    verticalHeightLineEdit->setEnabled(false);
}


void ImageViewProxy::restoreFocus() {
    view()->restoreFocus();
}
