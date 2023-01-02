/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotSeriesDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_SERIES_DIALOG_H
#define PLOT_SERIES_DIALOG_H

#include <QDialog>
#include <QSize>
#include <QPoint>
#include <QList>
#include <QSet>

#include <ld_variable_name.h>
#include <ld_plot_series.h>
#include <ld_plot_format.h>

#include "app_common.h"
#include "plot_engine.h"

class QWidget;
class QGroupBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;
class QDialogButtonBox;
class QShowEvent;
class QTimer;

namespace EQt {
    class LineEdit;
    class ColorToolButton;
    class ToolButton;
    class ToolButtonArray;
}

class VariableNameFieldsWidget;
class VariableNameSpecialCharactersWidget;
class PlotSeriesDemoWidget;

/**
 * You can use this dialog to add or modify a plot series.
 */
class APP_PUBLIC_API PlotSeriesDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * The axis location.
         */
        typedef Ld::PlotFormat::AxisLocation AxisLocation;

        /**
         * The plot line style.
         */
        typedef Ld::PlotSeries::LineStyle LineStyle;

        /**
         * The marker style.
         */
        typedef Ld::PlotSeries::MarkerStyle MarkerStyle;

        /**
         * The line spline type.
         */
        typedef Ld::PlotSeries::SplineType SplineType;

        /**
         * The gradient type.
         */
        typedef Ld::PlotSeries::GradientType GradientType;

        /**
         * The display mode.
         */
        typedef PlotEngine::PlotSeriesMode PlotSeriesMode;

        /**
         * Enumeration of supported axis location styles.
         */
        typedef PlotEngine::AxisLocationStyle AxisLocationStyle;

        /**
         * The maximum number of supported data sources.
         */
        static constexpr unsigned maximumNumberDataSources = 6;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlotSeriesDialog(QWidget* parent = Q_NULLPTR);

        ~PlotSeriesDialog() override;

        /**
         * Method you can use to determine the current plot series mode.
         *
         * \return Returns the current plot series mode.
         */
        PlotSeriesMode plotSeriesMode() const;

        /**
         * Method you can use to determine the selected axis location style setting.
         *
         * \return Returns the axis location style setting.
         */
        AxisLocationStyle axisLocationStyle() const;

        /**
         * Method you can use to determine the number of displayed data sources.
         *
         * \return Returns the number of displayed data sources.
         */
        unsigned numberDataSources() const;

        /**
         * Method you can use to determine the currently supported spline types.
         *
         * \return Returns a set of currently supported spline types.
         */
        QSet<SplineType> supportedSplineTypes() const;

        /**
         * Method you can use to obtain the current source text1 value.
         *
         * \param[in] sourceIndex The zero based index of the desired source.
         *
         * \return Returns the main variable text.
         */
        QString sourceText1(unsigned sourceIndex) const;

        /**
         * Method you can use to obtain the current source text2 value.
         *
         * \param[in] sourceIndex The zero based index of the desired source.
         *
         * \return Returns the main variable text.
         */
        QString sourceText2(unsigned sourceIndex) const;

        /**
         * Method you can use to obtain the current source 1 variable name.
         *
         * \param[in] sourceIndex The zero based index of the desired source.
         *
         * \return Returns the currently displayed variable name.
         */
        Ld::VariableName sourceVariableName(unsigned sourceIndex) const;

        /**
         * Method you can use to obtain the currently selected axis location.
         *
         * \param[in] sourceIndex The zero based index of the desired source.
         *
         * \return Returns the currently selected axis 1 location.
         */
        AxisLocation sourceAxisLocation(unsigned sourceIndex) const;

        /**
         * Method you can use to obtain the current variable name font.
         *
         * \return Returns the current variable name font.
         */
        QFont variableNameFont() const;

        /**
         * Method you can use to obtain the currently entered plot series label.
         *
         * \return Returns the currently entered plot series label.
         */
        QString label() const;

        /**
         * Method you can use to obtain the plot series line style.
         *
         * \return Returns the current plot series line style.
         */
        LineStyle lineStyle() const;

        /**
         * Method you can use to obtain the current line width.
         *
         * \return Returns the current line width setting.
         */
        float lineWidth() const;

        /**
         * Method you can use to obtain the current line color.
         *
         * \return Returns the currently selected line color.
         */
        QColor lineColor() const;

        /**
         * Method you can use to obtain the marker style.
         *
         * \return Returns the currently selected marker style.
         */
        MarkerStyle markerStyle() const;

        /**
         * Method you can use to obtain the selected spline type.
         *
         * \return Returns the selected spline type.
         */
        SplineType splineType() const;

        /**
         * Method you can use to obtain the selected gradient type.
         *
         * \return Returns the selected gradient type.
         */
        GradientType gradientType() const;

        /**
         * Method you can use to determine if the axis location controls are enabled.
         *
         * \return Returns true if the axis location controls are enabled.  Returns false if the axis location
         *         controls are disabled.
         */
        bool axisLocationControlsEnabled() const;

        /**
         * Method you can use to determine if the axis location controls are disabled.
         *
         * \return Returns true if the axis location controls are disabled.  Returns false if the axis location
         *         controls are enabled.
         */
        bool axisLocationControlsDisabled() const;

        /**
         * Method you can use to determine if the series label text edit field is enabled.
         *
         * \return Returns true if the series label text edit field is enabled.  Returns false if the series label text
         *         edit field is disabled.
         */
        bool seriesLabelFieldEnabled() const;

        /**
         * Method you can use to determine if the series label text edit field is disabled.
         *
         * \return Returns true if the series label text edit field is disabled.  Returns false if the series label
         *         text edit field is enabled.
         */
        bool seriesLabelFieldDisabled() const;

        /**
         * Method you can use to determine if the series visible attributes are enabled.
         *
         * \return Returns true if the series visible attributes are enabled.  Returns false if the series visible
         *         attributes are disabled.
         */
        bool seriesVisibleAttributesEnabled() const;

        /**
         * Method you can use to determine if the series visible attributes are disabled.
         *
         * \return Returns true if the series visible attributes are enabled.  Returns false if the series visible
         *         attributes are disabled.
         */
        bool seriesVisibleAttributesDisabled() const;

    public slots:
        /**
         * Slot you can trigger to change the current plot series mode.
         *
         * \param[in] newPlotSeriesMode The new plot series mode.
         */
        void setPlotSeriesMode(PlotSeriesMode newPlotSeriesMode);

        /**
         * Slot you can tirgger to set the axis location style.
         *
         * \param[in] newAxisLocationStyle The new axis location style.
         */
        void setAxisLocationStyle(AxisLocationStyle newAxisLocationStyle);

        /**
         * Slot you can tirgger to set the axis location style.
         *
         * \param[in] allowedAxisLocations A set holding the allowed axis locations.  An empty set indicates that all
         *                                 axis locations for the current style are allowed.
         */
        void setAllowedAxisLocations(const QSet<AxisLocation>& allowedAxisLocations);

        /**
         * Slot you can trigger to specify the number of displayed data sources.
         *
         * \param[in] newNumberDataSources The new number of displayed data sources.
         */
        void setNumberDataSources(unsigned newNumberDataSources);

        /**
         * Slot you can trigger to set the supported spline types.
         *
         * \param[in] supportedSplineTypes The new set of supported spline types.
         */
        void setSupportedSplineTypes(const QSet<SplineType>& supportedSplineTypes);

        /**
         * Slot you can use to change the source 1 axis location.
         *
         * \param[in] sourceIndex     The zero based index of the desired source.
         *
         * \param[in] newAxisLocation The new axis location value.
         */
        void setSourceAxisLocation(unsigned sourceIndex, AxisLocation newAxisLocation);

        /**
         * Slot you can use to change the source 1 variable name, text1 field.
         *
         * \param[in] sourceIndex The zero based index of the desired source.
         *
         * \param[in] newText     The new text to apply to the field.
         */
        void setSourceText1(unsigned sourceIndex, const QString& newText);

        /**
         * Slot you can use to change the source 1 variable name, text2 field.
         *
         * \param[in] sourceIndex The zero based index of the desired source.
         *
         * \param[in] newText     The new text to apply to the field.
         */
        void setSourceText2(unsigned sourceIndex, const QString& newText);

        /**
         * Slot you can use to change the source 1 variable name.
         *
         * \param[in] sourceIndex     The zero based index of the desired source.
         *
         * \param[in] newVariableName The new variable name.
         */
        void setSourceVariableName(unsigned sourceIndex, const Ld::VariableName& newVariableName);

        /**
         * Slot you can use to set the font used for the variable names.
         *
         * \param[in] newFont The new variable name font.
         */
        void setVariableNameFont(const QFont& newFont);

        /**
         * Slot you can use to update the series label text.
         *
         * \param[in] newText The new series label text.
         */
        void setSeriesLabel(const QString& newText);

        /**
         * Slot you can trigger to set the plot series line style.
         *
         * \param[in] newLineStyle The new plot series line style.
         */
        void setLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to change the current line width.
         *
         * \param[in] newLineWidth The new line width setting.
         */
        void setLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to change the current line color.
         *
         * \param[in] newColor The new line color.
         */
        void setLineColor(const QColor& newColor);

        /**
         * Slot you can trigger to change the marker style.
         *
         * \param[in] newMarkerStyle The new marker style.
         */
        void setMarkerStyle(MarkerStyle newMarkerStyle);

        /**
         * Slot you can trigger to change the selected spline type.
         *
         * \param[in] newSplineType The new spline type.
         */
        void setSplineType(SplineType newSplineType);

        /**
         * Slot you can trigger to change the selected gradient type.
         *
         * \param[in] newGradientType The new gradient type.
         */
        void setGradientType(GradientType newGradientType);

        /**
         * Method you can use to enable or disable the axis location controls.
         *
         * \param[in] nowEnabled If true, the axis location controls will be enabled.  If false, the axis location
         *                       controls will be disabled.
         */
        void setAxisLocationControlsEnabled(bool nowEnabled = true);

        /**
         * Method you can use to disable or enable the axis location controls.
         *
         * \param[in] nowDisabled If true, the axis location controls will be disabled.  If false, the axis location
         *                        controls will be enabled.
         */
        void setAxisLocationControlsDisabled(bool nowDisabled = true);

        /**
         * Method you can use to enable or disable the series label text edit field.
         *
         * \param[in] nowEnabled If true, the series label text exit field will be enabled.  If false, the series label
         *                       text edit field will be disabled.
         */
        void setSeriesLabelFieldEnabled(bool nowEnabled = true);

        /**
         * Method you can use to disable or enable the series label text edit field.
         *
         * \param[in] nowDisabled If true, the series label text exit field will be disabled.  If false, the series
         *                        label text edit field will be enabled.
         */
        void setSeriesLabelFieldDisabled(bool nowDisabled = true);

        /**
         * Method you can use to enable or disable the series visible attributes.
         *
         * \param[in] nowEnabled If true, the series visible attributes will be enabled.  If false, the series
         *                       visible attributes will be disabled.
         */
        void setSeriesVisibleAttributesEnabled(bool nowEnabled = true);

        /**
         * Method you can use to disable or enable the series visible attributes.
         *
         * \param[in] nowDisabled If true, the series visible attributes will be disabled.  If false, the series
         *                        visible attributes will be enabled.
         */
        void setSeriesVisibleAttributesDisabled(bool nowDisabled = true);

    protected:
        /**
         * Method that is triggered when the widget is shown.
         *
         * \param[in] event Event that triggered the call to this method.
         */
        void showEvent(QShowEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the user selects a special character.
         *
         * \param[in] character The newly selected special character.
         */
        void specialCharacterButtonClicked(const QString& character);

        /**
         * Slot that is triggered when the variable name field focus changes.
         */
        void variableNameFieldFocusChanged();

        /**
         * Slot that is triggered when an axis location combo box is updated.
         *
         * \param[in] index The index of the newly selected item.  This value is ignored.
         */
        void axisLocationChanged(int index);

        /**
         * Slot that is triggered whenever a data source is changed.
         *
         * \param[in] text1 The text1 field.  The value is ignored.
         *
         * \param[in] text2 The text2 field.  The value is ignored.
         */
        void variableNameChanged(const QString& text1, const QString& text2);

        /**
         * Slot that is triggered when the color tool button is clicked.
         */
        void colorToolButtonClicked();

        /**
         * Slot that is triggered when the line style is changed.
         *
         * \param[in] index The new index into the combo box.
         */
        void lineStyleChanged(int index);

        /**
         * Slot that is triggered when the line width is changed.
         *
         * \param[in] newValue The new line width value.
         */
        void lineWidthChanged(double newValue);

        /**
         * Slot that is triggered when the line color is changed.
         *
         * \param[in] newColor The new line color.
         */
        void lineColorChanged(const QColor& newColor);

        /**
         * Slot that is triggered when the marker style is changed.
         *
         * \param[in] index The index of the newly selected marker style.
         */
        void markerStyleChanged(int index);

        /**
         * Slot that is triggered when the spline style is changed.
         *
         * \param[in] index The index of the newly selected spline style.
         */
        void splineStyleChanged(int index);

        /**
         * Slot that is triggered when the gradient type is changed.
         *
         * \param[in] index The index of the newly selected gradient type.
         */
        void gradientTypeChanged(int index);

        /**
         * Slot that is triggered after this widget becomes visible.  We use this to conditionally steal the
         * applicaiton focus.
         */
        void nowVisible();

    private:
        /**
         * Called from the constructor to configure this widget.
         */
        void configureWidget();

        /**
         * Method that is called to disconnect all signals.
         */
        void disconnectAll();

        /**
         * Method that is called to connect all signals.
         */
        void connectAll();

        /**
         * Method that updates the demo widget.
         */
        void updateDemoWidget();

        /**
         * Method that returns a set of axis that are incompatible with a specified axis.
         *
         * \param[in] axisLocationStyle The axis location style that is currently being supported.
         *
         * \param[in] axisLocation      The axis location to find compatible axis for.
         *
         * \return Returns a set of axis that are compatible with the specified axis.
         */
        static QSet<AxisLocation> incompatibleAxis(AxisLocationStyle axisLocationStyle, AxisLocation axisLocation);

        /**
         * Method that populates an axis location combo box.
         *
         * \param[in] comboBox             The combo box to be populated.
         *
         * \param[in] sourceIndex          The zero based source index.
         *
         * \param[in] axisLocationStyle    The axis location style used to populate the combo box.
         *
         * \param[in] allowedAxisLocations An optional set of allowed axis locations.
         */
        static void populateAxisLocationComboBox(
            QComboBox*                comboBox,
            unsigned                  sourceIndex,
            AxisLocationStyle         axisLocationStyle,
            const QSet<AxisLocation>& allowedAxisLocations
        );

        /**
         * Method that populates all axis location combo boxes with the current settings.
         */
        void populateAxisLocationComboBoxes();

        /**
         * Method that populates the line style combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateLineStyleComboBox(bool manageSignals = true);

        /**
         * Method that populates the spline type combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateSplineTypeComboBox(bool manageSignals = true);

        /**
         * Method that populates the gradient type combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateGradientTypeComboBox(bool manageSignals = true);

        /**
         * Method that populates the marker style combo box.
         *
         * \param[in] manageSignals If true, signals will be managed when updating the list.  If false, signals will
         *                          not be managed.
         */
        void populateMarkerStyleComboBox(bool manageSignals = true);

        /**
         * Method that updates the OK button.
         */
        void updateOkButton();

        /**
         * The number of supported data sources.
         */
        unsigned currentNumberDataSources;

        /**
         * The input source variable names.
         */
        QList<Ld::VariableName> inputSourceVariableNames;

        /**
         * The input source axis locations.
         */
        QList<AxisLocation> inputSourceAxisLocations;

        /**
         * The input supported spline types.
         */
        QSet<SplineType> inputSupportedSplineTypes;

        /**
         * The input series label.
         */
        QString inputSeriesLabel;

        /**
         * The input line style.
         */
        LineStyle inputLineStyle;

        /**
         * The input line width.
         */
        float inputLineWidth;

        /**
         * The input line color.
         */
        QColor inputLineColor;

        /**
         * The input marker style.
         */
        MarkerStyle inputMarkerStyle;

        /**
         * The input spline type.
         */
        SplineType inputSplineType;

        /**
         * The input gradient type.
         */
        GradientType inputGradientType;

        /**
         * The current plot series mode.  Determines what functions are visible and what functions are hidden.
         */
        PlotSeriesMode currentPlotSeriesMode;

        /**
         * The currently selected axis location style.
         */
        AxisLocationStyle currentAxisLocationStyle;

        /**
         * The set of currently allowed axis locations.
         */
        QSet<AxisLocation> currentAllowedAxisLocations;

        /**
         * Data source group boxs.
         */
        QList<QGroupBox*> dataSourceGroupBoxes;

        /**
         * The source fields widgets.
         */
        QList<VariableNameFieldsWidget*> sourceFieldsWidgets;

        /**
         * The source axis location combo boxs.
         */
        QList<QComboBox*> sourceAxisLocationComboBoxes;

        /**
         * The special characters tool button array.
         */
        VariableNameSpecialCharactersWidget* specialCharactersWidget;

        /**
         * The label for the plot series label.
         */
        QLabel* labelLineEditLabel;

        /**
         * The plot series label.
         */
        QLineEdit* labelLineEdit;

        /**
         * The line style combo box.
         */
        QComboBox* lineStyleComboBox;

        /**
         * The line width spin box.
         */
        QDoubleSpinBox* lineWidthSpinBox;

        /**
         * The right arrow associated with the line style and line width.
         */
        QLabel* rightArrowLabel;

        /**
         * The demo widget.
         */
        PlotSeriesDemoWidget* plotSeriesDemoWidget;

        /**
         * The left arrow associated with the spline type.
         */
        QLabel* leftArrowLabel;

        /**
         * The spline type combo box.
         */
        QComboBox* splineTypeComboBox;

        /**
         * The gradient type combo box.
         */
        QComboBox* gradientTypeComboBox;

        /**
         * The up arrow associated with the marker style.
         */
        QLabel* upArrowLabel;

        /**
         * The marker style.
         */
        QComboBox* markerStyleComboBox;

        /**
         * The color tool button.
         */
        EQt::ColorToolButton* colorToolButton;

        /**
         * The button box holding the OK and Cancel buttons.
         */
        QDialogButtonBox* buttonBox;

        /**
         * Timer used to defer focus changes when this widget is shown.
         */
        QTimer* showTimer;
};

#endif
