/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotAxisSettingsWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_AXIS_SETTINGS_WIDGET_H
#define PLOT_AXIS_SETTINGS_WIDGET_H

#include <QFont>
#include <QColor>
#include <QSet>

#include <ld_chart_line_style.h>
#include <ld_chart_axis_format.h>

#include "app_common.h"
#include "plot_engine.h"
#include "plot_widget_base.h"

class QWidget;
class QComboBox;
class QLabel;
class QDoubleSpinBox;
class QGroupBox;
class QLineEdit;

class DoubleAutomaticLineEdit;
class LineStyleWidget;

namespace EQt {
    class ColorToolButton;
    class FontButton;
}

/**
 * Widget you can use to specify scaling and size for an axis.  This class can also be used as a wizard page.
 */
class APP_PUBLIC_API PlotAxisSettingsWidget:public PlotWidgetBase {
    Q_OBJECT

    public:
        /**
         * The axis grid line style.
         */
        typedef Ld::ChartLineStyle::LineStyle LineStyle;

        /**
         * The supported scaling control mode.
         */
        typedef PlotEngine::ScalingControlsMode ScalingControlsMode;

        /**
         * Enumeration indicating the desired axis scale.
         */
        typedef Ld::ChartAxisFormat::AxisScale AxisScale;

        /**
         * Enumeration indicating the desired axis tick style.
         */
        typedef Ld::ChartAxisFormat::TickStyle TickStyle;

        /**
         * Value used to represent automatic scaling of values.  Maps to infinity.
         */
        static const double automaticScaling;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        PlotAxisSettingsWidget(QWidget* parent = Q_NULLPTR);

        ~PlotAxisSettingsWidget() override;

        /**
         * Method you can use to determine if the current settings are valid or invalid.
         *
         * \return Returns true if the settings are valid.  Returns false if the settings are invalid.
         */
        bool isValid() const override;

        /**
         * Method you can use to obtain a set listing the supported axis scale modes.
         *
         * \return Returns a set containing the supported axis scaling modes.
         */
        QSet<AxisScale> supportedAxisScales() const;

        /**
         * Method you can use to determine the currently configured scaling controls mode.
         *
         * \return Returns the currently configured scaling controls mode.
         */
        ScalingControlsMode scalingControlsMode() const;

        /**
         * Method you can use to obtain the desired axis scaling.
         *
         * \return Returns the desired axis scaling.
         */
        AxisScale axisScale() const;

        /**
         * Method you can use to obtain the minimum axis value.  This is the value presented at the left or bottom
         * of the axis.
         *
         * \return Returns the minimum axis value.  The value \ref PlotAxisSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double minimumValue() const;

        /**
         * Method you can use to obtain the maximum axis value.  This is the value presented at the right or top
         * of the axis.
         *
         * \return Returns the maximum axis value.  The value \ref PlotAxisSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double maximumValue() const;

        /**
         * Method you can use to obtain the major tick step value.  The value must always be positive.
         *
         * \return Returns the major tick step value.  The value \ref PlotAxisSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double majorStepSize() const;

        /**
         * Method you can use to obtain the minor tick step value.  The value must always be positive.
         *
         * \return Returns the minor tick step value.  The value \ref PlotAxisSettingsWidget::automaticScaling will be
         *         returned if automatic scaling should be used.
         */
        double minorStepSize() const;

        /**
         * Method you can use to obtain the major tick style.
         *
         * \return Returns the major tick style.
         */
        TickStyle majorTickStyle() const;

        /**
         * Method you can use to obtain the minor tick style.
         *
         * \return Returns the minor tick style.
         */
        TickStyle minorTickStyle() const;

        /**
         * Method you can use to obtain the major grid line style.
         *
         * \return Returns the current major grid line style.
         */
        LineStyle majorGridLineStyle() const;

        /**
         * Method you can use to obtain the major grid line width.
         *
         * \return Returns the current major grid line width setting.
         */
        float majorGridLineWidth() const;

        /**
         * Method you can use to obtain the major grid line color.
         *
         * \return Returns the currently selected major grid line color.
         */
        QColor majorGridLineColor() const;

        /**
         * Method you can use to obtain the minor grid line style.
         *
         * \return Returns the current minor grid line style.
         */
        LineStyle minorGridLineStyle() const;

        /**
         * Method you can use to obtain the minor grid line width.
         *
         * \return Returns the current minor grid line width setting.
         */
        float minorGridLineWidth() const;

        /**
         * Method you can use to obtain the minor grid line color.
         *
         * \return Returns the currently selected minor grid line color.
         */
        QColor minorGridLineColor() const;

        /**
         * Method you can use to determine if the axis title field is visible or hidden.
         *
         * \return Returns true if the axis title field is visible.  Returns false if the axis title field is hidden.
         */
        bool axisTitleFieldVisible() const;

        /**
         * Method you can use to determine if the axis title field is hidden or visible.
         *
         * \return Returns true if the axis title field is hidden.  Returns false if the axis title field is visible.
         */
        bool axisTitleFieldHidden() const;

        /**
         * Method you can use to obtain the current axis title.
         *
         * \return Returns the currently entered axis title.
         */
        QString axisTitle() const;

        /**
         * Method you can use to obtain the axis title font.
         *
         * \return Returns the axis title font.
         */
        QFont titleFont() const;

        /**
         * Method you can use to obtain the axis title font color.
         *
         * \return Returns the axis title font color.
         */
        QColor titleFontColor() const;

        /**
         * Method you can use to obtain the axis title font background color.
         *
         * \return Returns the axis title font background color.
         */
        QColor titleFontBackgroundColor() const;

        /**
         * Method you can use to obtain the number font.
         *
         * \return Returns the number font.
         */
        QFont numberFont() const;

        /**
         * Method you can use to obtain the number font color.
         *
         * \return Returns the number font color.
         */
        QColor numberFontColor() const;

        /**
         * Method you can use to obtain the number font background color.
         *
         * \return Returns the number font background color.
         */
        QColor numberFontBackgroundColor() const;

    signals:
        /**
         * Signal that is emitted when the desired axis scaling changes.
         *
         * \param[out] newAxisScale The new axis scaling.
         */
        void axisScaleChanged(AxisScale newAxisScale);

        /**
         * Signal that is emitted when the major grid line style changes.
         *
         * \param[out] newLineStyle The new line style.
         */
        void majorGridLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is emitted when the major grid line width changes.
         *
         * \param[out] newLineWidth The new line width.
         */
        void majorGridLineWidthChanged(float newLineWidth);

        /**
         * Signal that is emitted when the major grid line color changes.
         *
         * \param[out] newLineColor The new line color.
         */
        void majorGridLineColorChanged(const QColor& newLineColor);

        /**
         * Signal that is emitted when the minor grid line style changes.
         *
         * \param[out] newLineStyle The new line style.
         */
        void minorGridLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is emitted when the minor grid line width changes.
         *
         * \param[out] newLineWidth The new line width.
         */
        void minorGridLineWidthChanged(float newLineWidth);

        /**
         * Signal that is emitted when the minor grid line color changes.
         *
         * \param[out] newLineColor The new line color.
         */
        void minorGridLineColorChanged(const QColor& newLineColor);

        /**
         * Signal that is emitted when the minimum axis value changes.  This is the value presented at the left or
         * bottom of the axis.
         *
         * \param[out] newMinimumValue The new minimum value.  The value \ref PlotAxisSettingsWidget::automaticScaling
         *                             will be indicate automatic scaling is desired.
         */
        void minimumValueChanged(double newMinimumValue);

        /**
         * Signal that is emitted when the maximum axis value changes.  This is the value presented at the right or top
         * of the axis.
         *
         * \param[out] newMaximumValue The new maximum value.  The value \ref PlotAxisSettingsWidget::automaticScaling
         *                             will indicate automatic scaling is desired.
         */
        void maximumValueChanged(double newMaximumValue);

        /**
         * Signal that is emitted when the major tick step value changes.  The value must always be positive.
         *
         * \param[out] newMajorStepSize The new major step size.  The value
         *                              \ref PlotAxisSettingsWidget::automaticScaling will indicate that automatic
         *                              scaling is desired.
         */
        void majorStepSizeChanged(double newMajorStepSize);

        /**
         * Signal that is emitted when the minor tick step value changes.  The value must always be positive.
         *
         * \param[out] newMinorStepSize The new minor tick step value.  The value
         *                              \ref PlotAxisSettingsWidget::automaticScaling indicates that automatic scaling
         *                              is desired.
         */
        void minorStepSizeChanged(double newMinorStepSize);

        /**
         * Signal that is emitted when the major tick style changes.
         *
         * \param[out] newMajorTickStyle The new major tick style.
         */
        void majorTickStyleChanged(TickStyle newMajorTickStyle);

        /**
         * Signal that is emitted when the minor tick style changes.
         *
         * \param[out] newMinorTickStyle The new minor tick style.
         */
        void minorTickStyleChanged(TickStyle newMinorTickStyle);

        /**
         * Signal that is emitted when the axis title is changed.
         *
         * \param[out] newAxisTitle The new axis title text.
         */
        void axisTitleChanged(const QString& newAxisTitle);

        /**
         * Signal that is emitted when the axis title font changes.
         *
         * \param[out] newTitleFont The new title font.
         */
        void titleFontChanged(const QFont& newTitleFont);

        /**
         * Signal that is emitted when the axis title font color changes.
         *
         * \param[out] newTitleFontColor The new title font color.
         */
        void titleFontColorChanged(const QColor& newTitleFontColor);

        /**
         * Signal that is emitted when the axis title font background color changes.
         *
         * \param[out] newTitleFontBackgroundColor The new title font background color.
         */
        void titleFontBackgroundColorChanged(const QColor& newTitleFontBackgroundColor);

        /**
         * Signal that is emitted when the number font changes.
         *
         * \param[out] newNumberFont The new number font.
         */
        void numberFontChanged(const QFont& newNumberFont);

        /**
         * Signal that is emitted when the number font color changes.
         *
         * \param[out] newNumberFontColor The new number font color.
         */
        void numberFontColorChanged(const QColor& newNumberFontColor) const;

        /**
         * Signal that is emitted when the number font background color changes.
         *
         * \param[out] newNumberFontBackgroundColor The new number font background color.
         */
        void numberFontBackgroundColorChanged(const QColor& newNumberFontBackgroundColor) const;

    public slots:
        /**
         * Slot you can trigger to change the supported axis scaling modes.
         *
         * \param[in] newSupportedAxisScales The new set of supported axis scales.
         */
        void setSupportedAxisScales(const QSet<AxisScale>& newSupportedAxisScales);

        /**
         * Slot you can use to set the desired scaling controls mode.
         *
         * \param[in] newScalingControlsMode The new scaling controls mode.
         */
        void setScalingControlsMode(ScalingControlsMode newScalingControlsMode);

        /**
         * Slot you can trigger to set the desired axis scaling.
         *
         * \param[in] newAxisScale The new axis scaling.
         */
        void setAxisScale(AxisScale newAxisScale);

        /**
         * Slot you can trigger to set the major grid line style.
         *
         * \param[in] newLineStyle The new line style.
         */
        void setMajorGridLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to set the major grid line width.
         *
         * \param[in] newLineWidth The new line width.
         */
        void setMajorGridLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to set the major grid line color.
         *
         * \param[in] newLineColor The new line color.
         */
        void setMajorGridLineColor(const QColor& newLineColor);

        /**
         * Slot you can trigger to set the minor grid line style.
         *
         * \param[in] newLineStyle The new line style.
         */
        void setMinorGridLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to set the minor grid line width.
         *
         * \param[in] newLineWidth The new line width.
         */
        void setMinorGridLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to set the minor grid line color.
         *
         * \param[in] newLineColor The new line color.
         */
        void setMinorGridLineColor(const QColor& newLineColor);

        /**
         * Slot you can trigger to set the minimum axis value.  This is the value presented at the left or bottom
         * of the axis.
         *
         * \param[in] newMinimumValue The new minimum value.  The value \ref PlotAxisSettingsWidget::automaticScaling
         *                            will be indicate automatic scaling is desired.
         */
        void setMinimumValue(double newMinimumValue);

        /**
         * Slot you can trigger to set the maximum axis value.  This is the value presented at the right or top
         * of the axis.
         *
         * \param[in] newMaximumValue The new maximum value.  The value \ref PlotAxisSettingsWidget::automaticScaling
         *                            will indicate automatic scaling is desired.
         */
        void setMaximumValue(double newMaximumValue);

        /**
         * Slot you can trigger to set the major tick step value.  The value must always be positive.
         *
         * \param[in] newMajorStepSize The new major step size.  The value
         *                             \ref PlotAxisSettingsWidget::automaticScaling will indicate that automatic
         *                             scaling is desired.
         */
        void setMajorStepSize(double newMajorStepSize);

        /**
         * Slot you can trigger to set the minor tick step value.  The value must always be positive.
         *
         * \param[in] newMinorStepSize The new minor tick step value.  The value
         *                             \ref PlotAxisSettingsWidget::automaticScaling indicates that automatic scaling
         *                             is desired.
         */
        void setMinorStepSize(double newMinorStepSize);

        /**
         * Slot you can trigger to set the major tick style.
         *
         * \param[in] newMajorTickStyle The new major tick style.
         */
        void setMajorTickStyle(TickStyle newMajorTickStyle);

        /**
         * Slot you can trigger to set the minor tick style.
         *
         * \param[in] newMinorTickStyle The new minor tick style.
         */
        void setMinorTickStyle(TickStyle newMinorTickStyle);

        /**
         * Slot you can trigger to show or hide the axis title and font buttons.
         *
         * \param[in] nowVisible If true, the axis title text and font buttons will be visible.  If false, the axis
         *                       title text and font buttons will be hidden.
         */
        void setAxisTitleVisible(bool nowVisible);

        /**
         * Slot you can trigger to hide or show the axis title and font buttons.
         *
         * \param[in] nowHidden If true, the axis title text and font buttons will be hidden.  If false, the axis
         *                      title text and font buttons will be visible.
         */
        void setAxisTitleHidden(bool nowHidden);

        /**
         * Slot you can trigger to update the axis title text.
         *
         * \param[in] newAxisTitle The new axis title text.
         */
        void setAxisTitle(const QString& newAxisTitle);

        /**
         * Slot you can trigger to set the axis title font.
         *
         * \param[in] newTitleFont The new title font.
         */
        void setTitleFont(const QFont& newTitleFont);

        /**
         * Slot you can trigger to set the axis title font color.
         *
         * \param[in] newTitleFontColor The new title font color.
         */
        void setTitleFontColor(const QColor& newTitleFontColor);

        /**
         * Slot you can trigger to set the axis title font background color.
         *
         * \param[in] newTitleFontBackgroundColor The new title font background color.
         */
        void setTitleFontBackgroundColor(const QColor& newTitleFontBackgroundColor);

        /**
         * Slot you can trigger to set the number font.
         *
         * \param[in] newNumberFont The new number font.
         */
        void setNumberFont(const QFont& newNumberFont);

        /**
         * Slot you can trigger to set the number font color.
         *
         * \param[in] newNumberFontColor The new number font color.
         */
        void setNumberFontColor(const QColor& newNumberFontColor) const;

        /**
         * Slot you can trigger to set the number font background color.
         *
         * \param[in] newNumberFontBackgroundColor The new number font background color.
         */
        void setNumberFontBackgroundColor(const QColor& newNumberFontBackgroundColor) const;

    private slots:
        /**
         * Slot that is triggered when the axis setting combo box is changed.
         *
         * \param[in] index The index of the newly selected entry.
         */
        void axisSettingChanged(int index);

        /**
         * Slot that is triggered when the minimum value setting is changed.
         *
         * \param[in] value     The currently entered value.
         *
         * \param[in] automatic Holds true if "automatic" is selected.
         */
        void minimumValueChanged(double value, bool automatic);

        /**
         * Slot that is triggered when the maximum value setting is changed.
         *
         * \param[in] value     The currently entered value.
         *
         * \param[in] automatic Holds true if "automatic" is selected.
         */
        void maximumValueChanged(double value, bool automatic);

        /**
         * Slot that is triggered when the major tick mark value is changed.
         *
         * \param[in] value     The currently entered value.
         *
         * \param[in] automatic Holds true if "automatic" is selected.
         */
        void majorTickMarkValueChanged(double value, bool automatic);

        /**
         * Slot that is triggered when the minor tick mark value is changed.
         *
         * \param[in] value     The currently entered value.
         *
         * \param[in] automatic Holds true if "automatic" is selected.
         */
        void minorTickMarkValueChanged(double value, bool automatic);

        /**
         * Slot that is triggered when the major tick mark style combo box is changed.
         *
         * \param[in] index The index of the newly selected value.
         */
        void majorTickMarkStyleChanged(int index);

        /**
         * Slot that is triggered when the minor tick mark style combo box is changed.
         *
         * \param[in] index The index of the newly selected value.
         */
        void minorTickMarkStyleChanged(int index);

        /**
         * Slot that is triggered when the major grid line style combo box is changed.
         *
         * \param[in] lineStyle The new line style.
         */
        void majorGridLineStyleHasChanged(LineStyle lineStyle);

        /**
         * Slot that is triggered when the minor grid line style combo box is changed.
         *
         * \param[in] lineStyle The new line style.
         */
        void minorGridLineStyleHasChanged(LineStyle lineStyle);

        /**
         * Slot that is triggered when the major grid line width spin box is changed.
         *
         * \param[in] value The newly entered value.
         */
        void majorGridLineWidthHasChanged(double value);

        /**
         * Slot that is triggered when the minor grid line width spin box is changed.
         *
         * \param[in] value The newly entered value.
         */
        void minorGridLineWidthHasChanged(double value);

        /**
         * Slot that is triggered when the major grid line color is changed.
         *
         * \param[in] newColor The new line color.
         */
        void majorGridLineColorHasChanged(const QColor& newColor);

        /**
         * Slot that is triggered when the minor grid line color is changed.
         *
         * \param[in] newColor The new line color.
         */
        void minorGridLineColorHasChanged(const QColor& newColor);

        /**
         * Slot that is triggered  when the axis title is changed.
         *
         * \param[in] newAxisTitle The new axis title text.
         */
        void axisTitleHasChanged(const QString& newAxisTitle);

        /**
         * Slot that is triggered when the title font button is clicked.
         */
        void titleFontButtonClicked();

        /**
         * Slot that is triggered when the number font button is clicked.
         */
        void numberFontButtonClicked();

    private:
        /**
         * Method that populates a axis scaling combo box.
         *
         * \param[in] comboBox            The combo box to be populated.
         *
         * \param[in] supportedAxisScales The set of supported axis scales.
         */
        static void populateAxisScalingComboBox(QComboBox* comboBox, const QSet<AxisScale>& supportedAxisScales);

        /**
         * Method that populates a tick mark style combo box.
         *
         * \param[in] comboBox The combo box to be populated.
         */
        static void populateTickMarkStyleComboBox(QComboBox* comboBox);

        /**
         * The current set of supported axis scales.
         */
        QSet<AxisScale> currentSupportedAxisScales;

        /**
         * Combo box indicating the desired axis scaling.
         */
        QComboBox* axisScalingComboBox;

        /**
         * The label used for the minimum value row.
         */
        QLabel* minimumValueLabel;

        /**
         * The label used for the maximum value.
         */
        QLabel* maximumValueLabel;

        /**
         * The minimum value double automatic line editor.
         */
        DoubleAutomaticLineEdit* minimumValueDoubleAutomaticLineEditor;

        /**
         * The maximum value double automatic line editor.
         */
        DoubleAutomaticLineEdit* maximumValueDoubleAutomaticLineEditor;

        /**
         * The major tick mark increment double automatic line editor.
         */
        DoubleAutomaticLineEdit* majorTickMarkDoubleAutomaticLineEditor;

        /**
         * The minor tick mark increment double automatic line editor.
         */
        DoubleAutomaticLineEdit* minorTickMarkDoubleAutomaticLineEditor;

        /**
         * Major tick mark style combo box.
         */
        QComboBox* majorTickMarkStyleComboBox;

        /**
         * Major grid line style widget.
         */
        LineStyleWidget* majorGridLineStyleWidget;

        /**
         * Minor tick mark style combo box.
         */
        QComboBox* minorTickMarkStyleComboBox;

        /**
         * Minor grid line style widget.
         */
        LineStyleWidget* minorGridLineStyleWidget;

        /**
         * The axis title group box.
         */
        QGroupBox* axisTitleGroupBox;

        /**
         * The axis title line editor.
         */
        QLineEdit* axisTitleLineEdit;

        /**
         * Number font push button.
         */
        EQt::FontButton* numberFontButton;

        /**
         * Title font push button.
         */
        EQt::FontButton* titleFontButton;
};

#endif
