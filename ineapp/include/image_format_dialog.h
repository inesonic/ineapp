/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImageFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_FORMAT_DIALOG_H
#define IMAGE_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>

#include <util_hash_functions.h>

#include <ld_image_format.h>

#include <eqt_programmatic_dialog.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;

namespace Ld {
    class PageFormat;
};

/**
 * Dialog used to adjust one or more block format instances.
 */
class APP_PUBLIC_API ImageFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Class that extends /ref Ld::ImageFormat::Axis to include a current dimension value, in points.  The current
         * dimension value is used to calculate new default values when the scaling mode is changed.
         */
        class Axis:public Ld::ImageFormat::Axis {
            public:
                Axis();

                /**
                 * Constructor
                 *
                 * \param[in] formatAxisData         The format axis values.
                 *
                 * \param[in] newPrescaledSizePoints The current unscaled size, in points.
                 *
                 * \param[in] newDisplayedSizePoints The current displayed size, in points.
                 */
                Axis(
                    const Ld::ImageFormat::Axis& formatAxisData,
                    float                        newPrescaledSizePoints,
                    float                        newDisplayedSizePoints
                );

                /**
                 * Constructor
                 *
                 * \param[in] formatAxisValue        The scaling mode dependent format size value.
                 *
                 * \param[in] scalingMode            The scaling mode indicating how to interpret the format axis
                 *                                   value.
                 *
                 * \param[in] newPrescaledSizePoints The current unscaled size, in points.
                 *
                 * \param[in] newDisplayedSizePoints The current displayed size, in points.
                 */
                Axis(
                    float                             formatAxisValue,
                    Ld::ImageFormat::ImageScalingMode scalingMode,
                    float                             newPrescaledSizePoints,
                    float                             newDisplayedSizePoints
                );

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied.
                 */
                Axis(const Axis& other);

                ~Axis();

                /**
                 * Method you can use to set the current displayed size, in points.
                 *
                 * \param[in] newDisplayedSizePoints The new displayed size, in points.
                 */
                void setDisplayedSizePoints(float newDisplayedSizePoints);

                /**
                 * Method you can use to obtain the current displayed size, in points.
                 *
                 * \return Returns the current displayed size, in points.
                 */
                float displayedSizePoints() const;

                /**
                 * Method you can use to set the current prescaled size, in points.
                 *
                 * \param[in] newPrescaledSizePoints The new prescaled size, in points.
                 */
                void setPrescaledSizePoints(float newPrescaledSizePoints);

                /**
                 * Method you can use to obtain the current prescaled size, in points.
                 *
                 * \return Returns the current prescaled size, in points.
                 */
                float prescaledSizePoints() const;

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to be copied.
                 *
                 * \return Returns a reference to this instance.
                 */
                Axis& operator=(const Axis& other);

                /**
                 * Comparison operator.
                 *
                 * \param[in] other The instance to be copied.
                 */
                bool operator==(const Axis& other) const;

                /**
                 * Comparison operator.
                 *
                 * \param[in] other The instance to be compared against this instance.
                 */
                bool operator!=(const Axis& other) const;

            private:
                /**
                 * The current prescaled size value, in points.
                 */
                float currentPrescaledSizePoints;

                /**
                 * The current displayed size value, in points.
                 */
                float currentDisplayedSizePoints;
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        ImageFormatDialog(QWidget* parent = Q_NULLPTR);

        ~ImageFormatDialog() override;

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void loadSettings(const QString& groupName) final;

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void saveSettings(const QString& groupName) final;

        /**
         * Method you can use to enable or disable the auto-apply function.
         *
         * \param[in] nowAutoApply If true, auto-apply will be enabled.  If false, auto-apply will be disabled.
         */
        void setAutoApplyEnabled(bool nowAutoApply = true);

        /**
         * Method you can use to disable or enable the auto-apply function.
         *
         * \param[in] nowAutoApplyDisabled If true, auto-apply will be disabled.  If false, auto-apply will be enabled.
         */
        void setAutoApplyDisabled(bool nowAutoApplyDisabled = true);

        /**
         * Method you can use to determine if auto-apply is enabled.
         *
         * \return Returns true if auto-apply is enabled.  Returns false if auto-apply is disabled.
         */
        bool autoApplyEnabled() const;

        /**
         * Method you can use to determine if auto-apply is disabled.
         *
         * \return Returns true if auto-apply is disabled.  Returns false if auto-apply is enabled.
         */
        bool autoApplyDisabled() const;

        /**
         * Method you can use to obtain the current horizontal scaling mode value.
         *
         * \return Returns the current horizontal scaling mode.  The value Ld::ImageFormat::ScalingMode::INVALID is
         *         returned if the value can not be resolved to a single value.
         */
        Ld::ImageFormat::ImageScalingMode horizontalScalingMode() const;

        /**
         * Method you can use to obtain the current horizontal scaling value.
         *
         * \return Returns the current horizontal scaling value.  The meaning of the value depends on the reported
         *         scaling mode but will be one of: a fractional scaling value, or an absolute size, in points.
         */
        float horizontalValue() const;

        /**
         * Method you can use to determine if the currently reported horizontal scaling value is valid.
         *
         * \return Returns true if the horizontal scaling value is valid.  Returns false if a valid horizontal scaling
         *         value can not be determined.
         */
        bool isHorizontalValueSet() const;

        /**
         * Method you can use to determine if the currently reported horizontal scaling value is not valid.
         *
         * \return Returns true if the horizontal scaling value is invalid.  Returns false if the horizontal scaling
         *         value is valid.
         */
        bool isHorizontalValueCleared() const;

        /**
         * Method you can use to obtain the current vertical scaling mode value.
         *
         * \return Returns the current vertical scaling mode.  The value Ld::ImageFormat::ScalingMode::INVALID is
         *         returned if the value can not be resolved to a single value.
         */
        Ld::ImageFormat::ImageScalingMode verticalScalingMode() const;

        /**
         * Method you can use to obtain the current vertical scaling value.
         *
         * \return Returns the current vertical scaling value.  The meaning of the value depends on the reported
         *         scaling mode but will be one of: a fractional scaling value, or an absolute size, in points.
         */
        float verticalValue() const;

        /**
         * Method you can use to determine if the currently reported vertical scaling value is valid.
         *
         * \return Returns true if the vertical scaling value is valid.  Returns false if a valid vertical scaling
         *         value can not be determined.
         */
        bool isVerticalValueSet() const;

        /**
         * Method you can use to determine if the currently reported vertical scaling value is not valid.
         *
         * \return Returns true if the vertical scaling value is invalid.  Returns false if the vertical scaling value
         *         is valid.
         */
        bool isVerticalValueCleared() const;

        /**
         * Method you can use to determine the currently reported rotation setting.
         *
         * \return Returns the currently reported rotation setting.
         */
        Ld::ImageFormat::Rotation rotation() const;

        /**
         * Method you can use to determine if the currently reported rotation setting is valid.
         *
         * \return Returns true if the current rotation value is valid.  Returns false if the current rotation value
         *         is invalid.
         */
        bool isRotationSet() const;

        /**
         * Method you can use to determine if the currently reported rotation setting is invalid.
         *
         * \return Returns true if the current rotation value is invalid.  Returns false if the current rotation value
         *         is valid.
         */
        bool isRotationCleared() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the Apply button is clicked on Windows or when any update occurs on MacOS.
         *
         * \param[out] thisDialog A pointer to this dialog.  You can use this parameter to obtain the current values.
         */
        void apply(ImageFormatDialog* thisDialog);

    public slots:
        /**
         * Slot you can use to set the horizontal scaling mode and values.
         *
         * \param[in] newScaling The new axis scaling values.
         */
        void setHorizontalScaling(const QSet<Axis>& newScaling);

        /**
         * Slot you can use to set the vertical scaling mode and values.
         *
         * \param[in] newScaling The new axis scaling values.
         */
        void setVerticalScaling(const QSet<Axis>& newScaling);

        /**
         * Slot you can use to set the rotations values.
         *
         * \param[in] newRotations The new rotation settings.
         */
        void setRotations(const QSet<Ld::ImageFormat::Rotation>& newRotations);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when the no rotation button is clicked.
         */
        void noRotationButtonClicked();

        /**
         * Slot that is triggered when the rotate CW button is clicked.
         */
        void rotateCWButtonClicked();

        /**
         * Slot that is triggered when the rotate CCW button is clicked.
         */
        void rotateCCWButtonClicked();

        /**
         * Slot that is triggered when the flip button is clicked.
         */
        void flipButtonClicked();

        /**
         * Slot that is triggered when the no width scaling button is clicked.
         */
        void noWidthScalingButtonClicked();

        /**
         * Slot that is triggered when the maintain aspect ratio for width button clicked.
         */
        void widthMaintainAspectRatioButtonClicked();

        /**
         * Slot that is triggered when the width fixed dimension scaling button is clicked.
         */
        void widthDimensionScalingButtonClicked();

        /**
         * Slot that is triggered when the width percentage scaling button is clicked.
         */
        void widthPercentageScalingButtonClicked();

        /**
         * Slot that is triggered when the width dimension line edit value is changed.
         *
         * \param[in] newValue The new reported value.
         */
        void widthDimensionValueChanged(double newValue);

        /**
         * Slot that is triggered when the width percentage spin box is changed.
         *
         * \param[in] newValue The new spin box value.
         */
        void percentageWidthValueChanged(double newValue);

        /**
         * Slot that is triggered when the no height scaling button is clicked.
         */
        void noHeightScalingButtonClicked();

        /**
         * Slot that is triggered when the maintain aspect ratio for height button clicked.
         */
        void heightMaintainAspectRatioButtonClicked();

        /**
         * Slot that is triggered when the height fixed dimension scaling button is clicked.
         */
        void heightDimensionScalingButtonClicked();

        /**
         * Slot that is triggered when the height percentage scaling button is clicked.
         */
        void heightPercentageScalingButtonClicked();

        /**
         * Slot that is triggered when the height dimension line edit value is changed.
         *
         * \param[in] newValue The new reported value.
         */
        void heightDimensionValueChanged(double newValue);

        /**
         * Slot that is triggered when the height percentage spin box is changed.
         *
         * \param[in] newValue The new spin box value.
         */
        void percentageHeightValueChanged(double newValue);

        /**
         * Slot that is triggered when the apply button is clicked.
         */
        void applyClicked();

        /**
         * Slot that is triggered when a value changes.  The slot checks for updates and adjusts the dialog
         * accordingly.
         */
        void updateDialog();

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Method that is called to check if values are different from the input values.
         *
         * \return Returns true if one or more values are different.  Returns false if all values match the inputs.
         */
        bool valuesChanged() const;

        /**
         * Method that is called to set the rotation setting.
         *
         * \param[in] newRotation The new rotation setting.
         */
        void setRotation(Ld::ImageFormat::Rotation newRotation);

        /**
         * Method that is called to update the GUI for the width scaling mode.
         *
         * \param[in] newScalingMode The new width scaling mode to present to the user.
         */
        void setWidthScalingMode(Ld::ImageFormat::ImageScalingMode newScalingMode);

        /**
         * Method that is called to update the GUI for the height scaling mode.
         *
         * \param[in] newScalingMode The new width scaling mode to present to the user.
         */
        void setHeightScalingMode(Ld::ImageFormat::ImageScalingMode newScalingMode);

        /**
         * Method that is called to clear the width value entry controls and disable them.
         */
        void clearAndDisableWidthEntry();

        /**
         * Method that is called to clear the width value entry controls and disable them.
         */
        void clearAndDisableHeightEntry();

        /**
         * Method that sets the step size for the width percentage scaling value based on a provided value.
         *
         * \param[in] newValue The value to calculate the step size for.
         */
        void updateWidthStepSize(double newValue);

        /**
         * Method that sets the step size for the height percentage scaling value based on a provided value.
         *
         * \param[in] newValue The value to calculate the step size for.
         */
        void updateHeightStepSize(double newValue);

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * Current input horizontal scaling values.
         */
        QSet<Axis> inputHorizontalScaling;

        /**
         * Current input vertical scaling values.
         */
        QSet<Axis> inputVerticalScaling;

        /**
         * Current input vertical scaling values.
         */
        QSet<Ld::ImageFormat::Rotation> inputRotation;
};

/**
 * Hash function for the \ref ImageFormatDialog::Axis class.
 *
 * \param[in] axis The axis data instance to be hashed.
 *
 * \param[in] seed An optional seed to apply to the hash operation.
 *
 * \return Returns a hash for the provided axis.
 */
Util::HashResult qHash(const ImageFormatDialog::Axis& axis, Util::HashSeed seed = 0);

#endif
