/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QSet>
#include <QSharedPointer>
#include <QLocale>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>

#include <util_hash_functions.h>

#include <cmath>

#include <util_units.h>

#include <eqt_dimension_line_edit.h>
#include <eqt_spin_box.h>
#include <eqt_tool_button.h>
#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_image_format.h>

#include "application.h"
#include "view_widget.h"
#include "image_presentation.h"
#include "image_format_dialog.h"

/***********************************************************************************************************************
 * ImageFormatDialog::Axis
 */

ImageFormatDialog::Axis::Axis() {}


ImageFormatDialog::Axis::Axis(
        const Ld::ImageFormat::Axis& formatAxisData,
        float                        newPrescaledSizePoints,
        float                        newDisplayedSizePoints
    ):Ld::ImageFormat::Axis(
        formatAxisData
    ) {
    currentPrescaledSizePoints = newPrescaledSizePoints;
    currentDisplayedSizePoints = newDisplayedSizePoints;
}


ImageFormatDialog::Axis::Axis(
        float                             formatAxisValue,
        Ld::ImageFormat::ImageScalingMode scalingMode,
        float                             newPrescaledSizePoints,
        float                             newDisplayedSizePoints
    ):Ld::ImageFormat::Axis(
        formatAxisValue,
        scalingMode
    ) {
    currentPrescaledSizePoints = newPrescaledSizePoints;
    currentDisplayedSizePoints = newDisplayedSizePoints;
}


ImageFormatDialog::Axis::Axis(const ImageFormatDialog::Axis& other):Ld::ImageFormat::Axis(other) {
    currentPrescaledSizePoints = other.currentPrescaledSizePoints;
    currentDisplayedSizePoints = other.currentDisplayedSizePoints;
}


ImageFormatDialog::Axis::~Axis() {}


void ImageFormatDialog::Axis::setDisplayedSizePoints(float newDisplayedSizePoints) {
    currentDisplayedSizePoints = newDisplayedSizePoints;
}


float ImageFormatDialog::Axis::displayedSizePoints() const {
    return currentDisplayedSizePoints;
}


void ImageFormatDialog::Axis::setPrescaledSizePoints(float newPrescaledSizePoints) {
    currentPrescaledSizePoints = newPrescaledSizePoints;
}


float ImageFormatDialog::Axis::prescaledSizePoints() const {
    return currentPrescaledSizePoints;
}


ImageFormatDialog::Axis& ImageFormatDialog::Axis::operator=(const ImageFormatDialog::Axis& other) {
    Ld::ImageFormat::Axis::operator=(other);

    currentPrescaledSizePoints = other.currentPrescaledSizePoints;
    currentDisplayedSizePoints = other.currentDisplayedSizePoints;

    return *this;
}


bool ImageFormatDialog::Axis::operator==(const ImageFormatDialog::Axis& other) const {
    return (
           Ld::ImageFormat::Axis::operator==(other)
        && other.currentDisplayedSizePoints == currentDisplayedSizePoints
        && other.currentPrescaledSizePoints == currentPrescaledSizePoints
    );
}


bool ImageFormatDialog::Axis::operator!=(const ImageFormatDialog::Axis& other) const {
    return (
           Ld::ImageFormat::Axis::operator!=(other)
        || other.currentDisplayedSizePoints != currentDisplayedSizePoints
        || other.currentPrescaledSizePoints != currentPrescaledSizePoints
    );
}

/***********************************************************************************************************************
 * ImageFormatDialog
 */

ImageFormatDialog::ImageFormatDialog(QWidget* parent):ProgrammaticDialog("ImageFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


ImageFormatDialog::~ImageFormatDialog() {}


void ImageFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    widthDimensionLineEdit->loadSettings(
        settings,
        "width_dimension_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    heightDimensionLineEdit->loadSettings(
        settings,
        "height_dimension_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void ImageFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    widthDimensionLineEdit->saveSettings(settings, "width_dimension_line_edit");

    EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    heightDimensionLineEdit->saveSettings(settings, "height_dimension_line_edit");

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void ImageFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void ImageFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool ImageFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool ImageFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


Ld::ImageFormat::ImageScalingMode ImageFormatDialog::horizontalScalingMode() const {
    QToolButton* widthNoScalingToolButton   = widget<QToolButton>("width_no_scaling_tool_button");
    QToolButton* widthAspectRatioToolButton = widget<QToolButton>("width_aspect_ratio_tool_button");
    QToolButton* widthDimensionToolButton   = widget<QToolButton>("width_dimension_tool_button");
    QToolButton* widthPercentageToolButton  = widget<QToolButton>("width_percentage_tool_button");

    bool widthNoScaling          = widthNoScalingToolButton->isChecked();
    bool widthAspectRatioScaling = widthAspectRatioToolButton->isChecked();
    bool widthDimensionScaling   = widthDimensionToolButton->isChecked();
    bool widthPercentageScaling  = widthPercentageToolButton->isChecked();

    Ld::ImageFormat::ImageScalingMode result;

    if        ( widthNoScaling && !widthAspectRatioScaling && !widthDimensionScaling && !widthPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::IGNORED;
    } else if (!widthNoScaling &&  widthAspectRatioScaling && !widthDimensionScaling && !widthPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO;
    } else if (!widthNoScaling && !widthAspectRatioScaling &&  widthDimensionScaling && !widthPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::FIXED_POINTS;
    } else if (!widthNoScaling && !widthAspectRatioScaling && !widthDimensionScaling &&  widthPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::FRACTIONAL;
    } else {
        result = Ld::ImageFormat::ImageScalingMode::INVALID;
    }

    return result;
}


float ImageFormatDialog::horizontalValue() const {
    float result;

    Ld::ImageFormat::ImageScalingMode scalingMode = horizontalScalingMode();
    if (scalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS) {
        EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
        if (widthDimensionLineEdit->valueOk()) {
            result = widthDimensionLineEdit->value();
        } else {
            result = 0;
        }
    } else if (scalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL) {
        QDoubleSpinBox* widthSpinBox = widget<QDoubleSpinBox>("width_spin_box");
        if (widthSpinBox->text().isEmpty()) {
            result = 0;
        } else {
            result = widthSpinBox->value() / 100.0F;
        }
    } else {
        result = 0;
    }

    return result;
}


bool ImageFormatDialog::isHorizontalValueSet() const {
    bool isSet;

    Ld::ImageFormat::ImageScalingMode scalingMode = horizontalScalingMode();
    if (scalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS) {
        EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
        if (widthDimensionLineEdit->valueOk()) {
            isSet = true;
        } else {
            isSet = false;
        }
    } else if (scalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL) {
        QDoubleSpinBox* widthSpinBox = widget<QDoubleSpinBox>("width_spin_box");
        if (!widthSpinBox->text().isEmpty()) {
            isSet = true;
        } else {
            isSet = false;
        }
    } else {
        isSet = false;
    }

    return isSet;
}

bool ImageFormatDialog::isHorizontalValueCleared() const {
    return !isHorizontalValueSet();
}


Ld::ImageFormat::ImageScalingMode ImageFormatDialog::verticalScalingMode() const {
    QToolButton* heightNoScalingToolButton   = widget<QToolButton>("height_no_scaling_tool_button");
    QToolButton* heightAspectRatioToolButton = widget<QToolButton>("height_aspect_ratio_tool_button");
    QToolButton* heightDimensionToolButton   = widget<QToolButton>("height_dimension_tool_button");
    QToolButton* heightPercentageToolButton  = widget<QToolButton>("height_percentage_tool_button");

    bool heightNoScaling          = heightNoScalingToolButton->isChecked();
    bool heightAspectRatioScaling = heightAspectRatioToolButton->isChecked();
    bool heightDimensionScaling   = heightDimensionToolButton->isChecked();
    bool heightPercentageScaling  = heightPercentageToolButton->isChecked();

    Ld::ImageFormat::ImageScalingMode result;

    if        ( heightNoScaling && !heightAspectRatioScaling && !heightDimensionScaling && !heightPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::IGNORED;
    } else if (!heightNoScaling &&  heightAspectRatioScaling && !heightDimensionScaling && !heightPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO;
    } else if (!heightNoScaling && !heightAspectRatioScaling &&  heightDimensionScaling && !heightPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::FIXED_POINTS;
    } else if (!heightNoScaling && !heightAspectRatioScaling && !heightDimensionScaling &&  heightPercentageScaling) {
        result = Ld::ImageFormat::ImageScalingMode::FRACTIONAL;
    } else {
        result = Ld::ImageFormat::ImageScalingMode::INVALID;
    }

    return result;
}


float ImageFormatDialog::verticalValue() const {
    float result;

    Ld::ImageFormat::ImageScalingMode scalingMode = verticalScalingMode();
    if (scalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS) {
        EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
        if (heightDimensionLineEdit->valueOk()) {
            result = heightDimensionLineEdit->value();
        } else {
            result = 0;
        }
    } else if (scalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL) {
        QDoubleSpinBox* heightSpinBox = widget<QDoubleSpinBox>("height_spin_box");
        if (heightSpinBox->text().isEmpty()) {
            result = 0;
        } else {
            result = heightSpinBox->value() / 100.0F;
        }
    } else {
        result = 0;
    }

    return result;
}


bool ImageFormatDialog::isVerticalValueSet() const {
    bool isSet;

    Ld::ImageFormat::ImageScalingMode scalingMode = verticalScalingMode();
    if (scalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS) {
        EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
        if (heightDimensionLineEdit->valueOk()) {
            isSet = true;
        } else {
            isSet = false;
        }
    } else if (scalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL) {
        QDoubleSpinBox* heightSpinBox = widget<QDoubleSpinBox>("height_spin_box");
        if (!heightSpinBox->text().isEmpty()) {
            isSet = true;
        } else {
            isSet = false;
        }
    } else {
        isSet = false;
    }

    return isSet;
}


bool ImageFormatDialog::isVerticalValueCleared() const {
    return !isVerticalValueSet();
}


Ld::ImageFormat::Rotation ImageFormatDialog::rotation() const {
    QToolButton* noRotationToolButton = widget<QToolButton>("no_rotation_tool_button");
    QToolButton* rotateCWToolButton   = widget<QToolButton>("rotate_cw_tool_button");
    QToolButton* flipToolButton       = widget<QToolButton>("flip_tool_button");
    QToolButton* rotateCCWToolButton  = widget<QToolButton>("rotate_ccw_tool_button");

    bool noRotation = noRotationToolButton->isChecked();
    bool rotateCW   = rotateCWToolButton->isChecked();
    bool flip       = flipToolButton->isChecked();
    bool rotateCCW  = rotateCCWToolButton->isChecked();

    Ld::ImageFormat::Rotation result;
    if (noRotation && !rotateCW && !flip && !rotateCCW) {
        result = Ld::ImageFormat::Rotation::NO_ROTATION;
    } else if (rotateCW && !flip && !rotateCCW) {
        result = Ld::ImageFormat::Rotation::ROTATE_CW_90;
    } else if (flip && !rotateCCW) {
        result = Ld::ImageFormat::Rotation::FLIP;
    } else if (rotateCCW) {
        result = Ld::ImageFormat::Rotation::ROTATE_CCW_90;
    } else {
        result = Ld::ImageFormat::Rotation::NO_ROTATION;
    }

    return result;
}


bool ImageFormatDialog::isRotationSet() const {
    QToolButton* noRotationToolButton = widget<QToolButton>("no_rotation_tool_button");
    QToolButton* rotateCWToolButton   = widget<QToolButton>("rotate_cw_tool_button");
    QToolButton* flipToolButton       = widget<QToolButton>("flip_tool_button");
    QToolButton* rotateCCWToolButton  = widget<QToolButton>("rotate_ccw_tool_button");

    bool noRotation = noRotationToolButton->isChecked();
    bool rotateCW   = rotateCWToolButton->isChecked();
    bool flip       = flipToolButton->isChecked();
    bool rotateCCW  = rotateCCWToolButton->isChecked();

    bool isSet;
    if ((noRotation && !rotateCW && !flip && !rotateCCW) ||
        (               rotateCW && !flip && !rotateCCW) ||
        (                            flip && !rotateCCW) ||
        (                                     rotateCCW)    ) {
        isSet = true;
    } else {
        isSet = false;
    }

    return isSet;
}


bool ImageFormatDialog::isRotationCleared() const {
    return !isRotationSet();
}


void ImageFormatDialog::populate() {
    QToolButton*            noRotationToolButton        = widget<QToolButton>("no_rotation_tool_button");
    QToolButton*            rotateCWToolButton          = widget<QToolButton>("rotate_cw_tool_button");
    QToolButton*            flipToolButton              = widget<QToolButton>("flip_tool_button");
    QToolButton*            rotateCCWToolButton         = widget<QToolButton>("rotate_ccw_tool_button");

    QToolButton*            widthNoScalingToolButton    = widget<QToolButton>("width_no_scaling_tool_button");
    QToolButton*            widthAspectRatioToolButton  = widget<QToolButton>("width_aspect_ratio_tool_button");
    QToolButton*            widthDimensionToolButton    = widget<QToolButton>("width_dimension_tool_button");
    QToolButton*            widthPercentageToolButton   = widget<QToolButton>("width_percentage_tool_button");
    EQt::DimensionLineEdit* widthDimensionLineEdit      = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    QDoubleSpinBox*         widthSpinBox                = widget<QDoubleSpinBox>("width_spin_box");

    QToolButton*            heightNoScalingToolButton   = widget<QToolButton>("height_no_scaling_tool_button");
    QToolButton*            heightAspectRatioToolButton = widget<QToolButton>("height_aspect_ratio_tool_button");
    QToolButton*            heightDimensionToolButton   = widget<QToolButton>("height_dimension_tool_button");
    QToolButton*            heightPercentageToolButton  = widget<QToolButton>("height_percentage_tool_button");
    EQt::DimensionLineEdit* heightDimensionLineEdit     = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    QDoubleSpinBox*         heightSpinBox               = widget<QDoubleSpinBox>("height_spin_box");

    noRotationToolButton->setChecked(inputRotation.contains(Ld::ImageFormat::Rotation::NO_ROTATION));
    rotateCWToolButton->setChecked(inputRotation.contains(Ld::ImageFormat::Rotation::ROTATE_CW_90));
    flipToolButton->setChecked(inputRotation.contains(Ld::ImageFormat::Rotation::FLIP));
    rotateCCWToolButton->setChecked(inputRotation.contains(Ld::ImageFormat::Rotation::ROTATE_CCW_90));

    QSet<Ld::ImageFormat::ImageScalingMode> widthScalingModes;
    QSet<float>                             widthValues;
    for (  QSet<Axis>::const_iterator widthAxisIterator    = inputHorizontalScaling.constBegin(),
                                      widthAxisEndIterator = inputHorizontalScaling.constEnd()
         ; widthAxisIterator != widthAxisEndIterator
         ; ++widthAxisIterator
        ) {
        const Axis& axis = *widthAxisIterator;
        widthScalingModes << axis.scalingMode();
        widthValues       << axis.value();
    }

    QSet<Ld::ImageFormat::ImageScalingMode> heightScalingModes;
    QSet<float>                             heightValues;
    for (  QSet<Axis>::const_iterator heightAxisIterator    = inputVerticalScaling.constBegin(),
                                      heightAxisEndIterator = inputVerticalScaling.constEnd()
         ; heightAxisIterator != heightAxisEndIterator
         ; ++heightAxisIterator
        ) {
        const Axis& axis = *heightAxisIterator;
        heightScalingModes << axis.scalingMode();
        heightValues       << axis.value();
    }

    bool widthNoScalingMode           = widthScalingModes.contains(Ld::ImageFormat::ImageScalingMode::IGNORED);
    bool widthAspectRatioScalingMode  = widthScalingModes.contains(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    bool widthDimensionScalingMode    = widthScalingModes.contains(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);
    bool widthPercentageScalingMode   = widthScalingModes.contains(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);

    bool heightNoScalingMode          = heightScalingModes.contains(Ld::ImageFormat::ImageScalingMode::IGNORED);
    bool heightAspectRatioScalingMode = heightScalingModes.contains(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    bool heightDimensionScalingMode   = heightScalingModes.contains(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);
    bool heightPercentageScalingMode  = heightScalingModes.contains(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);

    widthNoScalingToolButton->setChecked(widthNoScalingMode);
    widthAspectRatioToolButton->setChecked(widthAspectRatioScalingMode);
    widthDimensionToolButton->setChecked(widthDimensionScalingMode);
    widthPercentageToolButton->setChecked(widthPercentageScalingMode);

    heightNoScalingToolButton->setChecked(heightNoScalingMode);
    heightAspectRatioToolButton->setChecked(heightAspectRatioScalingMode);
    heightDimensionToolButton->setChecked(heightDimensionScalingMode);
    heightPercentageToolButton->setChecked(heightPercentageScalingMode);

    widthAspectRatioToolButton->setEnabled(!heightAspectRatioScalingMode);
    heightAspectRatioToolButton->setEnabled(!widthAspectRatioScalingMode);

    if (widthScalingModes.count() == 1 && widthPercentageScalingMode) {
        widthSpinBox->setVisible(true);
        widthDimensionLineEdit->setVisible(false);

        if (widthValues.count() == 1) {
            float widthValue = *widthValues.begin();
            widthSpinBox->setValue(widthValue * 100.0);
            updateWidthStepSize(widthValue * 100.0);
        } else {
            widthSpinBox->clear();
        }
    } else {
        widthSpinBox->setVisible(false);
        widthDimensionLineEdit->setVisible(true);

        if (widthScalingModes.count() == 1 && widthDimensionScalingMode && widthValues.count() == 1) {
            float value = *widthValues.begin();
            widthDimensionLineEdit->setValue(value);
        } else {
            widthDimensionLineEdit->clear();
        }
    }

    if (heightScalingModes.count() == 1 && heightPercentageScalingMode) {
        heightSpinBox->setVisible(true);
        heightDimensionLineEdit->setVisible(false);

        if (heightValues.count() == 1) {
            float heightValue = *heightValues.begin();
            heightSpinBox->setValue(heightValue * 100.0);
            updateHeightStepSize(heightValue * 100.0);
        } else {
            heightSpinBox->clear();
        }
    } else {
        heightSpinBox->setVisible(false);
        heightDimensionLineEdit->setVisible(true);

        if (heightScalingModes.count() == 1 && heightDimensionScalingMode && heightValues.count() == 1) {
            float value = *heightValues.begin();
            heightDimensionLineEdit->setValue(value);
        } else {
            heightDimensionLineEdit->clear();
        }
    }

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(false);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void ImageFormatDialog::setHorizontalScaling(const QSet<ImageFormatDialog::Axis>& newScaling) {
    inputHorizontalScaling = newScaling;
}


void ImageFormatDialog::setVerticalScaling(const QSet<ImageFormatDialog::Axis>& newScaling) {
    inputVerticalScaling = newScaling;
}


void ImageFormatDialog::setRotations(const QSet<Ld::ImageFormat::Rotation>& newRotations) {
    inputRotation = newRotations;
}


void ImageFormatDialog::bind() {
    QToolButton*            noRotationToolButton        = widget<QToolButton>("no_rotation_tool_button");
    QToolButton*            rotateCWToolButton          = widget<QToolButton>("rotate_cw_tool_button");
    QToolButton*            flipToolButton              = widget<QToolButton>("flip_tool_button");
    QToolButton*            rotateCCWToolButton         = widget<QToolButton>("rotate_ccw_tool_button");

    QToolButton*            widthNoScalingToolButton    = widget<QToolButton>("width_no_scaling_tool_button");
    QToolButton*            widthAspectRatioToolButton  = widget<QToolButton>("width_aspect_ratio_tool_button");
    QToolButton*            widthDimensionToolButton    = widget<QToolButton>("width_dimension_tool_button");
    QToolButton*            widthPercentageToolButton   = widget<QToolButton>("width_percentage_tool_button");
    EQt::DimensionLineEdit* widthDimensionLineEdit      = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    QDoubleSpinBox*         widthSpinBox                = widget<QDoubleSpinBox>("width_spin_box");

    QToolButton*            heightNoScalingToolButton   = widget<QToolButton>("height_no_scaling_tool_button");
    QToolButton*            heightAspectRatioToolButton = widget<QToolButton>("height_aspect_ratio_tool_button");
    QToolButton*            heightDimensionToolButton   = widget<QToolButton>("height_dimension_tool_button");
    QToolButton*            heightPercentageToolButton  = widget<QToolButton>("height_percentage_tool_button");
    EQt::DimensionLineEdit* heightDimensionLineEdit     = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    QDoubleSpinBox*         heightSpinBox               = widget<QDoubleSpinBox>("height_spin_box");

    connect(noRotationToolButton, &QToolButton::clicked, this, &ImageFormatDialog::noRotationButtonClicked);
    connect(rotateCWToolButton, &QToolButton::clicked, this, &ImageFormatDialog::rotateCWButtonClicked);
    connect(flipToolButton, &QToolButton::clicked, this, &ImageFormatDialog::flipButtonClicked);
    connect(rotateCCWToolButton, &QToolButton::clicked, this, &ImageFormatDialog::rotateCCWButtonClicked);

    connect(widthNoScalingToolButton, &QToolButton::clicked, this, &ImageFormatDialog::noWidthScalingButtonClicked);
    connect(
        widthAspectRatioToolButton,
        &QToolButton::clicked,
        this,
        &ImageFormatDialog::widthMaintainAspectRatioButtonClicked
    );
    connect(
        widthDimensionToolButton,
        &QToolButton::clicked,
        this,
        &ImageFormatDialog::widthDimensionScalingButtonClicked
    );
    connect(
        widthPercentageToolButton,
        &QToolButton::clicked,
        this,
        &ImageFormatDialog::widthPercentageScalingButtonClicked
    );
    connect(
        widthDimensionLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &ImageFormatDialog::widthDimensionValueChanged
    );
    connect(
        widthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &ImageFormatDialog::percentageWidthValueChanged
    );

    connect(heightNoScalingToolButton, &QToolButton::clicked, this, &ImageFormatDialog::noHeightScalingButtonClicked);
    connect(
        heightAspectRatioToolButton,
        &QToolButton::clicked,
        this,
        &ImageFormatDialog::heightMaintainAspectRatioButtonClicked
    );
    connect(
        heightDimensionToolButton,
        &QToolButton::clicked,
        this,
        &ImageFormatDialog::heightDimensionScalingButtonClicked
    );
    connect(
        heightPercentageToolButton,
        &QToolButton::clicked,
        this,
        &ImageFormatDialog::heightPercentageScalingButtonClicked
    );
    connect(
        heightDimensionLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &ImageFormatDialog::heightDimensionValueChanged
    );
    connect(
        heightSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &ImageFormatDialog::percentageHeightValueChanged
    );

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &ImageFormatDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &ImageFormatDialog::reject);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &ImageFormatDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void ImageFormatDialog::noRotationButtonClicked() {
    setRotation(Ld::ImageFormat::Rotation::NO_ROTATION);
    updateDialog();
}


void ImageFormatDialog::rotateCWButtonClicked() {
    setRotation(Ld::ImageFormat::Rotation::ROTATE_CW_90);
    updateDialog();
}


void ImageFormatDialog::rotateCCWButtonClicked() {
    setRotation(Ld::ImageFormat::Rotation::ROTATE_CCW_90);
    updateDialog();
}


void ImageFormatDialog::flipButtonClicked() {
    setRotation(Ld::ImageFormat::Rotation::FLIP);
    updateDialog();
}


void ImageFormatDialog::noWidthScalingButtonClicked() {
    setWidthScalingMode(Ld::ImageFormat::ImageScalingMode::IGNORED);
    clearAndDisableWidthEntry();
    updateDialog();
}


void ImageFormatDialog::widthMaintainAspectRatioButtonClicked() {
    setWidthScalingMode(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    clearAndDisableWidthEntry();
    updateDialog();
}


void ImageFormatDialog::widthDimensionScalingButtonClicked() {
    setWidthScalingMode(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);

    EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    QDoubleSpinBox*         widthSpinBox           = widget<QDoubleSpinBox>("width_spin_box");

    widthSpinBox->setVisible(false);
    widthSpinBox->setEnabled(false);
    widthSpinBox->clear();

    widthDimensionLineEdit->setVisible(true);
    widthDimensionLineEdit->setEnabled(true);

    QSet<float> displayedValues;
    for (  QSet<Axis>::const_iterator it  = inputHorizontalScaling.constBegin(),
                                      end = inputHorizontalScaling.constEnd()
         ; it != end
         ; ++it
        ) {
        displayedValues << it->displayedSizePoints();
    }

    if (displayedValues.count() == 1) {
        float value = *displayedValues.begin();
        widthDimensionLineEdit->setValue(value);
    } else {
        widthDimensionLineEdit->clear();
    }

    updateDialog();
}


void ImageFormatDialog::widthPercentageScalingButtonClicked() {
    setWidthScalingMode(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);

    EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    QDoubleSpinBox*         widthSpinBox           = widget<QDoubleSpinBox>("width_spin_box");

    widthDimensionLineEdit->setVisible(false);
    widthDimensionLineEdit->setEnabled(true);
    widthDimensionLineEdit->clear();

    widthSpinBox->setVisible(true);
    widthSpinBox->setEnabled(true);

    QSet<float> displayedValues;
    for (  QSet<Axis>::const_iterator it  = inputHorizontalScaling.constBegin(),
                                      end = inputHorizontalScaling.constEnd()
         ; it != end
         ; ++it
        ) {
        displayedValues << it->displayedSizePoints() / it->prescaledSizePoints();
    }

    if (displayedValues.count() == 1) {
        float value = *displayedValues.begin();
        widthSpinBox->setValue(value * 100.0);
    } else {
        widthSpinBox->clear();
    }

    updateDialog();
}


void ImageFormatDialog::widthDimensionValueChanged(double) {
    updateDialog();
}


void ImageFormatDialog::percentageWidthValueChanged(double newValue) {
    updateWidthStepSize(newValue);
    updateDialog();
}


void ImageFormatDialog::noHeightScalingButtonClicked() {
    setHeightScalingMode(Ld::ImageFormat::ImageScalingMode::IGNORED);
    clearAndDisableHeightEntry();
    updateDialog();
}


void ImageFormatDialog::heightMaintainAspectRatioButtonClicked() {
    setHeightScalingMode(Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    clearAndDisableHeightEntry();
    updateDialog();
}


void ImageFormatDialog::heightDimensionScalingButtonClicked() {
    setHeightScalingMode(Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);

    EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    QDoubleSpinBox*         heightSpinBox           = widget<QDoubleSpinBox>("height_spin_box");

    heightSpinBox->setVisible(false);
    heightSpinBox->setEnabled(false);
    heightSpinBox->clear();

    heightDimensionLineEdit->setVisible(true);
    heightDimensionLineEdit->setEnabled(true);

    QSet<float> displayedValues;
    for (  QSet<Axis>::const_iterator it  = inputVerticalScaling.constBegin(),
                                      end = inputVerticalScaling.constEnd()
         ; it != end
         ; ++it
        ) {
        displayedValues << it->displayedSizePoints();
    }

    if (displayedValues.count() == 1) {
        float value = *displayedValues.begin();
        heightDimensionLineEdit->setValue(value);
    } else {
        heightDimensionLineEdit->clear();
    }

    updateDialog();
}


void ImageFormatDialog::heightPercentageScalingButtonClicked() {
    setHeightScalingMode(Ld::ImageFormat::ImageScalingMode::FRACTIONAL);

    EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    QDoubleSpinBox*         heightSpinBox           = widget<QDoubleSpinBox>("height_spin_box");

    heightDimensionLineEdit->setVisible(false);
    heightDimensionLineEdit->setEnabled(true);
    heightDimensionLineEdit->clear();

    heightSpinBox->setVisible(true);
    heightSpinBox->setEnabled(true);

    QSet<float> displayedValues;
    for (  QSet<Axis>::const_iterator it  = inputVerticalScaling.constBegin(),
                                      end = inputVerticalScaling.constEnd()
         ; it != end
         ; ++it
        ) {
        displayedValues << it->displayedSizePoints() / it->prescaledSizePoints();
    }

    if (displayedValues.count() == 1) {
        float value = *displayedValues.begin();
        heightSpinBox->setValue(value * 100.0);
    } else {
        heightSpinBox->clear();
    }

    updateDialog();
}


void ImageFormatDialog::heightDimensionValueChanged(double) {
    updateDialog();
}


void ImageFormatDialog::percentageHeightValueChanged(double newValue) {
    updateHeightStepSize(newValue);
    updateDialog();
}


void ImageFormatDialog::applyClicked() {
//    if (justification() != Ld::JustifiedBlockFormat::Justification::NONE) {
//        inputJustifications.clear();
//        inputJustifications << justification();
//    }

//    if (isLeftIndentationSet()) {
//        inputLeftIndentation = leftIndentation();
//    }

//    if (isRightIndentationSet()) {
//        inputRightIndentation = rightIndentation();
//    }

//    if (isTopSpacingSet()) {
//        inputTopSpacing = topSpacing();
//    }

//    if (isBottomSpacingSet()) {
//        inputBottomSpacing = bottomSpacing();
//    }

//    if (isFirstLineIndentationSet()) {
//        inputFirstLineIndentation = firstLineIndentation();
//    }

//    if (isLineSpacingSet()) {
//        inputLineSpacing = lineSpacing();
//    }

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
        okPushButton->setEnabled(false);

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    emit apply(this);
}


void ImageFormatDialog::updateDialog() {
    bool valuesHaveChanged = valuesChanged();

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(valuesHaveChanged);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(valuesHaveChanged);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    if (currentAutoApplyEnabled) {
        applyClicked();
    }
}


void ImageFormatDialog::configureWidget() {
    setWindowTitle(tr("Image Format"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentAutoApplyEnabled   = false;

    inputHorizontalScaling.clear();
    inputVerticalScaling.clear();
    inputRotation.clear();

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* horizontalLayout = newHBoxLayout("horizontal_layout");
    mainLayout->addLayout(horizontalLayout);

    QGroupBox* rotationGroupBox = new QGroupBox(tr("Rotation"));
    registerWidget(rotationGroupBox, "rotation_group_box");
    horizontalLayout->addWidget(rotationGroupBox);

    QGridLayout* rotationLayout = newGridLayout(("rotation_layout"));
    rotationGroupBox->setLayout(rotationLayout);

    QToolButton* noRotationToolButton = new QToolButton;
    noRotationToolButton->setIcon(Application::icon("format_image_no_rotation"));
    noRotationToolButton->setIconSize(QSize(64, 64));
    noRotationToolButton->setCheckable(true);
    noRotationToolButton->setToolTip(tr("Removes any image rotation."));
    noRotationToolButton->setWhatsThis(
        tr("You can use this button to remove any rotation on images.  Images will be presented as stored.")
    );
    registerWidget(noRotationToolButton, "no_rotation_tool_button");
    rotationLayout->addWidget(noRotationToolButton, 0, 1);

    QToolButton* rotateCWToolButton = new QToolButton;
    rotateCWToolButton->setIcon(Application::icon("format_image_rotate_cw"));
    rotateCWToolButton->setIconSize(QSize(64, 64));
    rotateCWToolButton->setCheckable(true);
    rotateCWToolButton->setToolTip(
        tr("Rotates images clockwise 90 degrees from their original orientation.")
    );
    rotateCWToolButton->setWhatsThis(
        tr("You can use this button to rotate images clockwise 90 degrees from their original (stored) orientation.")
    );
    registerWidget(rotateCWToolButton, "rotate_cw_tool_button");
    rotationLayout->addWidget(rotateCWToolButton, 1, 2);

    QToolButton* flipToolButton = new QToolButton;
    flipToolButton->setIcon(Application::icon("format_image_rotate_flip"));
    flipToolButton->setIconSize(QSize(64, 64));
    flipToolButton->setCheckable(true);
    flipToolButton->setToolTip("Flips an image upside down.");
    flipToolButton->setWhatsThis(
        tr(
            "You can use this button to flip images upside down by rotating it 180 degrees from their original "
            "(stored) position."
          )
    );
    registerWidget(flipToolButton, "flip_tool_button");
    rotationLayout->addWidget(flipToolButton, 2, 1);

    QToolButton* rotateCCWToolButton = new QToolButton;
    rotateCCWToolButton->setIcon(Application::icon("format_image_rotate_ccw"));
    rotateCCWToolButton->setIconSize(QSize(64, 64));
    rotateCCWToolButton->setCheckable(true);
    rotateCCWToolButton->setToolTip(
        tr("Roates images counter-clockwise 90 degrees from their original orientation.")
    );
    rotateCCWToolButton->setWhatsThis(
        tr(
           "You can use this button to rotate images counter-clockwise 90 degrees from their original (stored) "
           "orientation."
          )
    );
    registerWidget(rotateCCWToolButton, "rotate_ccw_tool_button");
    rotationLayout->addWidget(rotateCCWToolButton, 1, 0);

    QVBoxLayout* scalingLayout = newVBoxLayout("scaling_layout");
    horizontalLayout->addLayout(scalingLayout);

    scalingLayout->addStretch();

    QGroupBox* widthScalingGroupBox = new QGroupBox("Width");
    registerWidget(widthScalingGroupBox, "width_scaling_group_box");
    scalingLayout->addWidget(widthScalingGroupBox);

    QHBoxLayout* widthScalingLayout = newHBoxLayout("width_scaling_layout");
    widthScalingGroupBox->setLayout(widthScalingLayout);

    EQt::DimensionLineEdit* widthDimensionLineEdit = new EQt::DimensionLineEdit;
    widthDimensionLineEdit->setDynamicUnits(true);
    widthDimensionLineEdit->setToolTip("Enter the desired image width here.");
    widthDimensionLineEdit->setWhatsThis(
        tr(
           "You can enter the desired image width in this field.  Values can be in inches (\"), points (pt), picas "
           "(pc), centimeters (cm), or millimeters (mm)."
        )
    );
    registerWidget(widthDimensionLineEdit, "width_dimension_line_edit");
    widthScalingLayout->addWidget(widthDimensionLineEdit);

    QDoubleSpinBox* widthSpinBox = new QDoubleSpinBox;
    widthSpinBox->setSuffix(tr("%"));
    widthSpinBox->setMinimum(10.0);
    widthSpinBox->setMaximum(1000.0);
    widthSpinBox->setDecimals(2);
    widthSpinBox->setToolTip("Enter the desired image width percentage scaling here.");
    widthSpinBox->setWhatsThis(
        tr(
           "You can enter the desired image width scaling in this field.  A value of 200% will increase the image "
           "width by a factor of 2.  A value of 33% will reduce the image width by a factor of 3."
          )
    );
    registerWidget(widthSpinBox, "width_spin_box");
    widthScalingLayout->addWidget(widthSpinBox);

    QToolButton* widthNoScalingToolButton = new QToolButton;
    widthNoScalingToolButton->setIcon(Application::icon("format_image_no_horizontal_scaling"));
    widthNoScalingToolButton->setIconSize(QSize(32, 32));
    widthNoScalingToolButton->setCheckable(true);
    widthNoScalingToolButton->setToolTip(tr("Click to use the original image width."));
    widthNoScalingToolButton->setWhatsThis(tr("You can click this button to use the original image width."));
    registerWidget(widthNoScalingToolButton, "width_no_scaling_tool_button");
    widthScalingLayout->addWidget(widthNoScalingToolButton);

    QToolButton* widthAspectRatioToolButton = new QToolButton;
    widthAspectRatioToolButton->setIcon(Application::icon("format_image_aspect_ratio"));
    widthAspectRatioToolButton->setIconSize(QSize(32, 32));
    widthAspectRatioToolButton->setCheckable(true);
    widthAspectRatioToolButton->setToolTip(tr("Click to have the image width scale with the image height."));
    widthAspectRatioToolButton->setWhatsThis(
        tr("You can use this button to have the image width scale proportionally with the image height.")
    );
    registerWidget(widthAspectRatioToolButton, "width_aspect_ratio_tool_button");
    widthScalingLayout->addWidget(widthAspectRatioToolButton);

    QToolButton* widthDimensionToolButton = new QToolButton;
    widthDimensionToolButton->setIcon(Application::icon("format_image_scale_horizontal_dimension"));
    widthDimensionToolButton->setIconSize(QSize(32, 32));
    widthDimensionToolButton->setCheckable(true);
    widthDimensionToolButton->setToolTip(tr("Click to set the image width."));
    widthDimensionToolButton->setWhatsThis(
        tr(
           "You can use this button to display and enable a field you can use to enter the desired image width."
        )
    );
    registerWidget(widthDimensionToolButton, "width_dimension_tool_button");
    widthScalingLayout->addWidget(widthDimensionToolButton);

    QToolButton* widthPercentageToolButton = new QToolButton;
    widthPercentageToolButton->setIcon(Application::icon("format_image_scale_percentage"));
    widthPercentageToolButton->setIconSize(QSize(32, 32));
    widthPercentageToolButton->setCheckable(true);
    widthPercentageToolButton->setToolTip(tr("Click to set a percentage increase or reduction to the image width."));
    widthPercentageToolButton->setWhatsThis(
        tr(
           "You can use this button to display a field you can use to enter a percentage scaling factor for the image "
           "width.  A value of 200% will double the width of the image.  A value of 33% will reduce the image width "
           "by a factor of 3."
          )
    );
    registerWidget(widthPercentageToolButton, "width_percentage_tool_button");
    widthScalingLayout->addWidget(widthPercentageToolButton);

    scalingLayout->addStretch();

    QGroupBox* heightScalingGroupBox = new QGroupBox("Height");
    registerWidget(heightScalingGroupBox, "height_scaling_group_box");
    scalingLayout->addWidget(heightScalingGroupBox);

    QHBoxLayout* heightScalingLayout = newHBoxLayout("height_scaling_layout");
    heightScalingGroupBox->setLayout(heightScalingLayout);

    EQt::DimensionLineEdit* heightDimensionLineEdit = new EQt::DimensionLineEdit;
    heightDimensionLineEdit->setDynamicUnits(true);
    heightDimensionLineEdit->setToolTip("Enter the desired image height here.");
    heightDimensionLineEdit->setWhatsThis(
        tr(
           "You can enter the desired image height in this field.  Values can be in inches (\"), points (pt), picas "
           "(pc), centimeters (cm), or millimeters (mm)."
        )
    );
    registerWidget(heightDimensionLineEdit, "height_dimension_line_edit");
    heightScalingLayout->addWidget(heightDimensionLineEdit);

    QDoubleSpinBox* heightSpinBox = new QDoubleSpinBox;
    heightSpinBox->setSuffix(tr("%"));
    heightSpinBox->setMinimum(10.0);
    heightSpinBox->setMaximum(1000.0);
    heightSpinBox->setDecimals(2);
    heightSpinBox->setToolTip("Enter the desired image height percentage scaling here.");
    heightSpinBox->setWhatsThis(
        tr(
           "You can enter the desired image height scaling in this field.  A value of 200% will increase the image "
           "height by a factor of 2.  A value of 33% will reduce the image height by a factor of 3."
          )
    );
    registerWidget(heightSpinBox, "height_spin_box");
    heightScalingLayout->addWidget(heightSpinBox);

    QToolButton* heightNoScalingToolButton = new QToolButton;
    heightNoScalingToolButton->setIcon(Application::icon("format_image_no_vertical_scaling"));
    heightNoScalingToolButton->setIconSize(QSize(32, 32));
    heightNoScalingToolButton->setCheckable(true);
    heightNoScalingToolButton->setToolTip(tr("Click to use the original image height."));
    heightNoScalingToolButton->setWhatsThis(tr("You can click this button to use the original image height."));
    registerWidget(heightNoScalingToolButton, "height_no_scaling_tool_button");
    heightScalingLayout->addWidget(heightNoScalingToolButton);

    QToolButton* heightAspectRatioToolButton = new QToolButton;
    heightAspectRatioToolButton->setIcon(Application::icon("format_image_aspect_ratio"));
    heightAspectRatioToolButton->setIconSize(QSize(32, 32));
    heightAspectRatioToolButton->setCheckable(true);
    heightAspectRatioToolButton->setToolTip(tr("Click to have the height width scale with the image width."));
    heightAspectRatioToolButton->setWhatsThis(
        tr("You can use this button to have the image height scale proportionally with the image width.")
    );
    registerWidget(heightAspectRatioToolButton, "height_aspect_ratio_tool_button");
    heightScalingLayout->addWidget(heightAspectRatioToolButton);

    QToolButton* heightDimensionToolButton = new QToolButton;
    heightDimensionToolButton->setIcon(Application::icon("format_image_scale_vertical_dimension"));
    heightDimensionToolButton->setIconSize(QSize(32, 32));
    heightDimensionToolButton->setCheckable(true);
    heightDimensionToolButton->setToolTip(tr("Click to set the image height."));
    heightDimensionToolButton->setWhatsThis(
        tr(
           "You can use this button to display and enable a field you can use to enter the desired image height."
        )
    );
    registerWidget(heightDimensionToolButton, "height_dimension_tool_button");
    heightScalingLayout->addWidget(heightDimensionToolButton);

    QToolButton* heightPercentageToolButton = new QToolButton;
    heightPercentageToolButton->setIcon(Application::icon("format_image_scale_percentage"));
    heightPercentageToolButton->setIconSize(QSize(32, 32));
    heightPercentageToolButton->setCheckable(true);
    heightPercentageToolButton->setToolTip(tr("Click to set a percentage increase or reduction to the image height."));
    heightPercentageToolButton->setWhatsThis(
        tr(
           "You can use this button to display a field you can use to enter a percentage scaling factor for the image "
           "width.  A value of 200% will double the width of the image.  A value of 33% will reduce the image width "
           "by a factor of 3."
          )
    );
    registerWidget(heightPercentageToolButton, "height_percentage_tool_button");
    heightScalingLayout->addWidget(heightPercentageToolButton);

    scalingLayout->addStretch();

    #if (defined(Q_OS_DARWIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    #elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

    #else

        #error Unknown platform

    #endif

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    mainLayout->addWidget(buttonBox);
}


bool ImageFormatDialog::valuesChanged() const {
    bool valuesHaveChanged = false;

    if (inputRotation.count() != 1) {
        if (isRotationSet()) {
            valuesHaveChanged = true;
        }
    } else /* if (inputRotation.count() == 1) */ {
        if (*inputRotation.begin() != rotation()) {
            valuesHaveChanged = true;
        }
    }

    if (valuesHaveChanged == false) {
        QSet<Ld::ImageFormat::ImageScalingMode> horizontalScalingModes;
        QSet<float>                        horizontalValues;
        for (  QSet<Axis>::const_iterator horizontalAxisIterator    = inputHorizontalScaling.constBegin(),
                                          horizontalAxisEndIterator = inputHorizontalScaling.constEnd()
             ; horizontalAxisIterator != horizontalAxisEndIterator
             ; ++horizontalAxisIterator
            ) {
            horizontalScalingModes << horizontalAxisIterator->scalingMode();
            horizontalValues       << horizontalAxisIterator->value();
        }

        Ld::ImageFormat::ImageScalingMode selectedScalingMode = horizontalScalingMode();
        if (selectedScalingMode != Ld::ImageFormat::ImageScalingMode::INVALID) {
            if (horizontalScalingModes.count() != 1) {
                valuesHaveChanged = true;
            } else if (selectedScalingMode != *horizontalScalingModes.begin()) {
                valuesHaveChanged = true;
            } else if (selectedScalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS ||
                       selectedScalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL      ) {
                if (horizontalValues.count() != 1) {
                    if (isHorizontalValueSet()) {
                        valuesHaveChanged = true;
                    }
                } else /* if (horizontalValues.count == 1) */ {
                    if (*horizontalValues.begin() != horizontalValue()) {
                        valuesHaveChanged = true;
                    }
                }
            }
        }
    }

    if (valuesHaveChanged == false) {
        QSet<Ld::ImageFormat::ImageScalingMode> verticalScalingModes;
        QSet<float>                             verticalValues;
        for (  QSet<Axis>::const_iterator verticalAxisIterator    = inputVerticalScaling.constBegin(),
                                          verticalAxisEndIterator = inputVerticalScaling.constEnd()
             ; verticalAxisIterator != verticalAxisEndIterator
             ; ++verticalAxisIterator
            ) {
            verticalScalingModes << verticalAxisIterator->scalingMode();
            verticalValues       << verticalAxisIterator->value();
        }

        Ld::ImageFormat::ImageScalingMode selectedScalingMode = verticalScalingMode();
        if (selectedScalingMode != Ld::ImageFormat::ImageScalingMode::INVALID) {
            if (verticalScalingModes.count() != 1) {
                valuesHaveChanged = true;
            } else if (selectedScalingMode != *verticalScalingModes.begin()) {
                valuesHaveChanged = true;
            } else if (selectedScalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS ||
                       selectedScalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL      ) {
                if (verticalValues.count() != 1) {
                    if (isVerticalValueSet()) {
                        valuesHaveChanged = true;
                    }
                } else /* if (verticalValues.count == 1) */ {
                    if (*verticalValues.begin() != verticalValue()) {
                        valuesHaveChanged = true;
                    }
                }
            }
        }
    }

    return valuesHaveChanged;
}


void ImageFormatDialog::setRotation(Ld::ImageFormat::Rotation newRotation) {
    QToolButton* noRotationToolButton = widget<QToolButton>("no_rotation_tool_button");
    QToolButton* rotateCWToolButton   = widget<QToolButton>("rotate_cw_tool_button");
    QToolButton* flipToolButton       = widget<QToolButton>("flip_tool_button");
    QToolButton* rotateCCWToolButton  = widget<QToolButton>("rotate_ccw_tool_button");

    noRotationToolButton->setChecked(newRotation == Ld::ImageFormat::Rotation::NO_ROTATION);
    rotateCWToolButton->setChecked(newRotation == Ld::ImageFormat::Rotation::ROTATE_CW_90);
    flipToolButton->setChecked(newRotation == Ld::ImageFormat::Rotation::FLIP);
    rotateCCWToolButton->setChecked(newRotation == Ld::ImageFormat::Rotation::ROTATE_CCW_90);
}


void ImageFormatDialog::setWidthScalingMode(Ld::ImageFormat::ImageScalingMode newScalingMode) {
    QToolButton* widthNoScalingToolButton   = widget<QToolButton>("width_no_scaling_tool_button");
    QToolButton* widthAspectRatioToolButton = widget<QToolButton>("width_aspect_ratio_tool_button");
    QToolButton* widthDimensionToolButton   = widget<QToolButton>("width_dimension_tool_button");
    QToolButton* widthPercentageToolButton  = widget<QToolButton>("width_percentage_tool_button");

    widthNoScalingToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::IGNORED);
    widthAspectRatioToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    widthDimensionToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);
    widthPercentageToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL);

    QToolButton* heightAspectRatioToolButton = widget<QToolButton>("height_aspect_ratio_tool_button");
    heightAspectRatioToolButton->setEnabled(newScalingMode != Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
}


void ImageFormatDialog::setHeightScalingMode(Ld::ImageFormat::ImageScalingMode newScalingMode) {
    QToolButton* heightNoScalingToolButton   = widget<QToolButton>("height_no_scaling_tool_button");
    QToolButton* heightAspectRatioToolButton = widget<QToolButton>("height_aspect_ratio_tool_button");
    QToolButton* heightDimensionToolButton   = widget<QToolButton>("height_dimension_tool_button");
    QToolButton* heightPercentageToolButton  = widget<QToolButton>("height_percentage_tool_button");

    heightNoScalingToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::IGNORED);
    heightAspectRatioToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
    heightDimensionToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::FIXED_POINTS);
    heightPercentageToolButton->setChecked(newScalingMode == Ld::ImageFormat::ImageScalingMode::FRACTIONAL);

    QToolButton* widthAspectRatioToolButton = widget<QToolButton>("width_aspect_ratio_tool_button");
    widthAspectRatioToolButton->setEnabled(newScalingMode != Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);
}


void ImageFormatDialog::clearAndDisableWidthEntry() {
    EQt::DimensionLineEdit* widthDimensionLineEdit = widget<EQt::DimensionLineEdit>("width_dimension_line_edit");
    QDoubleSpinBox*         widthSpinBox           = widget<QDoubleSpinBox>("width_spin_box");

    widthSpinBox->setVisible(false);
    widthSpinBox->setEnabled(false);
    widthSpinBox->clear();

    widthDimensionLineEdit->setVisible(true);
    widthDimensionLineEdit->setEnabled(false);
    widthDimensionLineEdit->clear();
}


void ImageFormatDialog::clearAndDisableHeightEntry() {
    EQt::DimensionLineEdit* heightDimensionLineEdit = widget<EQt::DimensionLineEdit>("height_dimension_line_edit");
    QDoubleSpinBox*         heightSpinBox           = widget<QDoubleSpinBox>("height_spin_box");

    heightSpinBox->setVisible(false);
    heightSpinBox->setEnabled(false);
    heightSpinBox->clear();

    heightDimensionLineEdit->setVisible(true);
    heightDimensionLineEdit->setEnabled(false);
    heightDimensionLineEdit->clear();
}


void ImageFormatDialog::updateWidthStepSize(double newValue) {
    // The + 0.000001 is included because the log function will sometimes incur a tiny error which then causes the
    // floor method to truncate the result downward by a whole decimal place.  By adding this offset, we can still
    // obtain a reasonable result even with a tiny round-off error in the log10 function.

    double newStepSize = 2.0 * std::pow(10, std::floor(std::log10(newValue) + 0.000001) - 2);

    QDoubleSpinBox* widthSpinBox = widget<QDoubleSpinBox>("width_spin_box");
    if (widthSpinBox->singleStep() != newStepSize) {
        widthSpinBox->setSingleStep(newStepSize);
    }
}


void ImageFormatDialog::updateHeightStepSize(double newValue) {
    // The + 0.000001 is included because the log function will sometimes incur a tiny error which then causes the
    // floor method to truncate the result downward by a whole decimal place.  By adding this offset, we can still
    // obtain a reasonable result even with a tiny round-off error in the log10 function.

    double newStepSize = 2.0 * std::pow(10, std::floor(std::log10(newValue) + 0.000001) - 2);

    QDoubleSpinBox* heightSpinBox = widget<QDoubleSpinBox>("height_spin_box");
    if (heightSpinBox->singleStep() != newStepSize) {
        heightSpinBox->setSingleStep(newStepSize);
    }
}


Util::HashResult qHash(const ImageFormatDialog::Axis& axis, Util::HashSeed seed) {
    return (
          qHash(static_cast<const Ld::ImageFormat::Axis&>(axis), seed)
        ^ qHash(axis.displayedSizePoints(), seed)
        ^ qHash(axis.prescaledSizePoints(), seed)
    );
}
