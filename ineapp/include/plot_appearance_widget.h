/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotAppearanceWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_APPEARANCE_WIDGET_H
#define PLOT_APPEARANCE_WIDGET_H

#include <QColor>
#include <QFont>
#include <QString>

#include <ld_chart_line_style.h>
#include <ld_plot_format.h>

#include "app_common.h"
#include "plot_widget_base.h"

class QWidget;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QLabel;
class QTimer;
class QShowEvent;

namespace EQt {
    class DimensionLineEdit;
    class ColorToolButton;
    class FontButton;
}

class LineStyleWidget;

/**
 * Widget you can use to more easily and consistently manage the overall appearance of plots.  This class can also be
 * used as a wizard page.
 */
class APP_PUBLIC_API PlotAppearanceWidget:public PlotWidgetBase {
    Q_OBJECT

    public:
        /**
         * The axis grid line style.
         */
        typedef Ld::ChartLineStyle::LineStyle LineStyle;

        /**
         * The legend location.
         */
        typedef Ld::PlotFormat::LegendLocation LegendLocation;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        PlotAppearanceWidget(QWidget* parent = Q_NULLPTR);

        ~PlotAppearanceWidget() override;

        /**
         * Method you can use to determine if the current settings are valid or invalid.
         *
         * \return Returns true if the settings are valid.  Returns false if the settings are invalid.
         */
        bool isValid() const override;

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void loadSettings(const QString& groupName);

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void saveSettings(const QString& groupName);

        /**
         * Method that provides the currently selected plot width, in points.
         *
         * \return Returns the currently selected plot width, in points.  A zero or negative value indicates that no
         *         plot width has been entered.
         */
        float plotWidth() const;

        /**
         * Method that provides the currently selected plot height, in points.
         *
         * \return Returns the currently selected plot height, in points.  A zero or negative value indicates that no
         *         plot height has been entered.
         */
        float plotHeight() const;

        /**
         * Method that provides the minimum allowed plot width, in points.
         *
         * \return Returns the currently selected minimum plot width, in points.
         */
        float minimumPlotWidth() const;

        /**
         * Method that provides the minimum allowed plot height, in points.
         *
         * \return Returns the currently selected minimum plot height, in points.
         */
        float minimumPlotHeight() const;

        /**
         * Method you can use to determine the plot border line style.
         *
         * \return Returns the plot border line style.
         */
        LineStyle borderLineStyle() const;

        /**
         * Method you can use to determine the plot border line width.
         *
         * \return Returns the plot border line width.
         */
        float borderLineWidth() const;

        /**
         * Method you can use to determine the plot border line color.
         *
         * \return Returns the plot border line color.
         */
        QColor borderLineColor() const;

        /**
         * Method you can use to determine the currently selected plot background color.
         *
         * \return Returns the currently selected plot background color.
         */
        QColor backgroundColor() const;

        /**
         * Method you can use to determine the plot active area border line style.
         *
         * \return Returns the plot active area border line style.
         */
        LineStyle activeAreaBorderLineStyle() const;

        /**
         * Method you can use to determine the plot active area border line width.
         *
         * \return Returns the plot active area border line width.
         */
        float activeAreaBorderLineWidth() const;

        /**
         * Method you can use to determine the plot active area border line color.
         *
         * \return Returns the plot active area border line color.
         */
        QColor activeAreaBorderLineColor() const;

        /**
         * Method you can use to determine the currently selected plotting area color.
         *
         * \return Returns the currently selected plotting area color.
         */
        QColor activeAreaColor() const;

        /**
         * Method you can use to determine the legend location.
         *
         * \return Returns the currently selected legend location (or hidden).
         */
        LegendLocation legendLocation() const;

        /**
         * Method you can use to determine the plot legend line style.
         *
         * \return Returns the plot legend line style.
         */
        LineStyle legendLineStyle() const;

        /**
         * Method you can use to determine the plot legend line width.
         *
         * \return Returns the plot legend line width.
         */
        float legendLineWidth() const;

        /**
         * Method you can use to determine the current legend background color.
         *
         * \return Returns the current legend background color.
         */
        QColor legendBackgroundColor() const;

        /**
         * Method you can use to determine the plot legend line color.
         *
         * \return Returns the plot legend line color.
         */
        QColor legendLineColor() const;

        /**
         * Method you can use to determine the plot legend font.
         *
         * \return Returns the selected plot legend font.
         */
        QFont legendFont() const;

        /**
         * Method you can use to determine the plot legend font color.
         *
         * \return Returns the legend font color.
         */
        QColor legendFontColor() const;

        /**
         * Method you can use to determine the plot tiel font background color.
         *
         * \return Returns the legend font background color.
         */
        QColor legendFontBackgroundColor() const;

        /**
         * Method you can use to determine the plot title text.
         *
         * \return Returns the plot title text.
         */
        QString titleText() const;

        /**
         * Method you can use to determine the plot title font.
         *
         * \return Returns the selected plot title font.
         */
        QFont titleFont() const;

        /**
         * Method you can use to determine the plot title font color.
         *
         * \return Returns the title font color.
         */
        QColor titleFontColor() const;

        /**
         * Method you can use to determine the plot tiel font background color.
         *
         * \return Returns the title font background color.
         */
        QColor titleFontBackgroundColor() const;

    signals:
        /**
         * Signal that is emitted when the user changes the current plot size.
         *
         * \param[out] newWidth  The new plot width, in points.
         *
         * \param[out] newHeight The new plot height, in points.
         */
        void plotSizeChanged(float newWidth, float newHeight);

        /**
         * Signal that is emitted when the plot border line style changes.
         *
         * \param[out] newBorderLineStyle The new plot border line style.
         */
        void borderLineStyleChanged(LineStyle newBorderLineStyle);

        /**
         * Signal that is emitted when the plot border line width changes.
         *
         * \param[out] newBorderWidth The new plot border line width.
         */
        void borderLineWidthChanged(float newBorderWidth);

        /**
         * Signal that is emitted when the plot border line color changes.
         *
         * \param[out] newBorderColor The new plot border line color.
         */
        void borderLineColorChanged(const QColor& newBorderColor);

        /**
         * Signal that is emitted when the currently selected plot background color changes.
         *
         * \param[out] newBackgroundColor The new plot background color.
         */
        void backgroundColorChanged(const QColor& newBackgroundColor);

        /**
         * Signal that is emitted when the plot active area border line style changes.
         *
         * \param[out] newLineStyle The new plot active area border line style.
         */
        void activeAreaBorderLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is emitted when the plot active area border line width changes.
         *
         * \param[out] newLineWidth The new plot active area border line width.
         */
        void activeAreaBorderLineWidthChanged(float newLineWidth);

        /**
         * Signal that is emitted when the plot active area border line color changes.
         *
         * \param[out] newLineColor The new plot active area border line color.
         */
        void activeAreaBorderLineColorChanged(const QColor& newLineColor);

        /**
         * Signal that is emitted when the currently selected plotting area color changes.
         *
         * \param[out] newPlotAreaColor The new plotting area color.
         */
        void activeAreaColorChanged(const QColor& newPlotAreaColor);

        /**
         * Signal that is emitted when the legend location changes.
         *
         * \param[out] newLocation The new legend location (or hidden).
         */
        void legendLocationChanged(LegendLocation newLocation);

        /**
         * Signal that is emitted when the plot legend line style changes.
         *
         * \param[out] newLineStyle The new plot legend line style.
         */
        void legendLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is emitted when the plot legend line width changes.
         *
         * \param[out] newLineWidth The new plot legend line width.
         */
        void legendLineWidthChanged(float newLineWidth);

        /**
         * Signal that is emitted when the plot legend line color changes.
         *
         * \param[out] newLineColor The new plot legend line color.
         */
        void legendLineColorChanged(const QColor& newLineColor);

        /**
         * Signal that is emitted when the legend background color changes.
         *
         * \param[out] newLegendBackgroundColor The new legend background color.
         */
        void legendBackgroundColorChanged(const QColor& newLegendBackgroundColor);

        /**
         * Signal that is emitted when the plot legend font changes.
         *
         * \param[out] newFont The new selected plot legend font.
         */
        void legendFontChanged(const QFont& newFont);

        /**
         * Signal that is emitted when the plot legend font color changes.
         *
         * \param[out] newFontColor The new legend font color.
         */
        void legendFontColorChanged(const QColor& newFontColor);

        /**
         * Signal that is emitted when the plot tiel font background color changes.
         *
         * \param[out] newFontBackgroundColor The new legend font background color.
         */
        void legendFontBackgroundColorChanged(const QColor& newFontBackgroundColor);

        /**
         * Signal that is emitted when the plot title text changes.
         *
         * \param[out] newText The new plot title text.
         */
        void titleTextChanged(const QString& newText);

        /**
         * Signal that is emitted when the plot title font changes.
         *
         * \param[out] newFont The new plot title font.
         */
        void titleFontChanged(const QFont& newFont);

        /**
         * Signal that is emitted when the plot title font color changes.
         *
         * \param[out] newFontColor The new title font color.
         */
        void titleFontColorChanged(const QColor& newFontColor);

        /**
         * Signal that is emitted when the plot tiel font background color changes.
         *
         * \param[out] newFontBackgroundColor The new title font background color.
         */
        void titleFontBackgroundColorChanged(const QColor& newFontBackgroundColor);

    public slots:
        /**
         * Slot you can use to set currently selected plot width, in points.
         *
         * \param[in] newPlotWidth The newly selected plot width.  A zero or negative value indicates that the field
         *                         should be left empty.
         */
        void setPlotWidth(float newPlotWidth);

        /**
         * Slot you can use to set currently selected plot height, in points.
         *
         * \param[in] newPlotHeight The newly selected plot height.  A zero or negative value indicates that the field
         *                          should be left empty.
         */
        void setPlotHeight(float newPlotHeight);

        /**
         * Slot you can use tos et the the minimum allowed plot width, in points.
         *
         * \param[in] newMinimumWidth The new minimum width, in points.
         */
        void setMinimumPlotWidth(float newMinimumWidth);

        /**
         * Slot you can use tos et the the minimum allowed plot height, in points.
         *
         * \param[in] newMinimumHeight The new minimum height, in points.
         */
        void setMinimumPlotHeight(float newMinimumHeight);

        /**
         * Slot you can trigger to change the plot border line style.
         *
         * \param[in] newBorderLineStyle The new plot border line style.
         */
        void setBorderLineStyle(LineStyle newBorderLineStyle);

        /**
         * Slot you can trigger to change the plot border line width.
         *
         * \param[in] newBorderWidth The new plot border line width.
         */
        void setBorderLineWidth(float newBorderWidth);

        /**
         * Slot you can trigger to change the plot border line color.
         *
         * \param[in] newBorderColor The new plot border line color.
         */
        void setBorderLineColor(const QColor& newBorderColor);

        /**
         * Slot you can trigger to change the currently selected plot background color.
         *
         * \param[in] newBackgroundColor The new plot background color.
         */
        void setBackgroundColor(const QColor& newBackgroundColor);

        /**
         * Slot you can trigger to change the plot active area border line style.
         *
         * \param[in] newLineStyle The new plot active area border line style.
         */
        void setActiveAreaBorderLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to change the plot active area border line width.
         *
         * \param[in] newLineWidth The new plot active area border line width.
         */
        void setActiveAreaBorderLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to change the plot active area border line color.
         *
         * \param[in] newLineColor The new plot active area border line color.
         */
        void setActiveAreaBorderLineColor(const QColor& newLineColor);

        /**
         * Slot you can trigger to change the currently selected plotting area color.
         *
         * \param[in] newPlotAreaColor The new plotting area color.
         */
        void setPlotAreaColor(const QColor& newPlotAreaColor);

        /**
         * Slot you can trigger to change the legend location.
         *
         * \param[in] newLocation The new legend location (or hidden).
         */
        void setLegendLocation(LegendLocation newLocation);

        /**
         * Slot you can trigger to change the plot legend line style.
         *
         * \param[in] newLineStyle The new plot legend line style.
         */
        void setLegendLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to change the plot legend line width.
         *
         * \param[in] newLineWidth The new plot legend line width.
         */
        void setLegendLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to change the plot legend line color.
         *
         * \param[in] newLineColor The new plot legend line color.
         */
        void setLegendLineColor(const QColor& newLineColor);

        /**
         * Slot you can trigger to change the plot legend font.
         *
         * \param[in] newFont The new selected plot legend font.
         */
        void setLegendFont(const QFont& newFont);

        /**
         * Slot you can trigger to change the plot legend font color.
         *
         * \param[in] newFontColor The new legend font color.
         */
        void setLegendFontColor(const QColor& newFontColor);

        /**
         * Slot you can trigger to change the plot title font background color.
         *
         * \param[in] newFontBackgroundColor The new legend font background color.
         */
        void setLegendFontBackgroundColor(const QColor& newFontBackgroundColor);

        /**
         * Slot you can trigger to change the legend background color.
         *
         * \param[in] newLegendBackgroundColor The new legend background color.
         */
        void setLegendBackgroundColor(const QColor& newLegendBackgroundColor);

        /**
         * Slot you can trigger to change the plot title text.
         *
         * \param[in] newText The new plot title text.
         */
        void setTitleText(const QString& newText);

        /**
         * Slot you can trigger to change the plot title font.
         *
         * \param[in] newFont The new plot title font.
         */
        void setTitleFont(const QFont& newFont);

        /**
         * Slot you can trigger to change the plot title font color.
         *
         * \param[in] newFontColor The new title font color.
         */
        void setTitleFontColor(const QColor& newFontColor);

        /**
         * Slot you can trigger to change the plot tiel font background color.
         *
         * \param[in] newFontBackgroundColor The new title font background color.
         */
        void setTitleFontBackgroundColor(const QColor& newFontBackgroundColor);

        /**
         * Slot you can trigger to enable and show or disable and hide the plot legend controls.
         *
         * \param[in] nowEnabled If true, the plot legend controls will be enabled and made visible.  If false, the
         *                       plot legend controls will be disabled and hidden.
         */
        void setLegendControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable and hide or enable and show the plot legend controls.
         *
         * \param[in] nowDisabled If true, the plot legend controls will be disabled and hidden.  If false, the plot
         *                        legend controls will be enabled and made visible.
         */
        void setLegendControlsDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to enable and show or disable and hide the plot active area controls.
         *
         * \param[in] nowEnabled If true, the plot active area controls will be enabled and made visible.  If false,
         *                       the plot active area controls will be disabled and hidden.
         */
        void setActiveAreaControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable and hide or enable and show the plot active area controls.
         *
         * \param[in] nowDisabled If true, the plot active area controls will be disabled and hidden.  If false, the
         *                        plot active area controls will be enabled and made visible.
         */
        void setActiveAreaControlsDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to enable the active area background controls.
         *
         * \param[in] nowEnabled If true, background controls will now be enabled.  If false, background controls will
         *                       be disabled.
         */
        void setActiveAreaBackgroundControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable the active area background controls.
         *
         * \param[in] nowDisabled If true, background controls will now be disabled.  If false, background controls
         *                        will be disabled.
         */
        void setActiveAreaBackgroundControlsDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to enable and show the plot title edit field.
         *
         * \param[in] nowEnabled If true, the plot title area will be enabled and made visible.  If false, the plot
         *                       title area will be disabled and hidden.
         */
        void setPlotTitleEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable and hide the plot title edit field.
         *
         * \param[in] nowDisabled If true, the plot title area will be disabled and made hidden.  If false, the plot
         *                        title area will be enabled and made visible.
         */
        void setPlotTitleDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to enable and show the plot background color control button.
         *
         * \param[in] nowEnabled If true, the plot background color control will be enabled and made visible.  If
         *                       false, the plot background color control will be disabled and hidden.
         */
        void setBackgroundColorButtonEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable and hide the plot background color control button.
         *
         * \param[in] nowDisabled If true, the plot background color control will be disabled and hidden.  If false,
         *                        the plot background color control will be enabled and made visible.
         */
        void setBackgroundColorButtonDisabled(bool nowDisabled = true);

    protected:
        /**
         * Method that is triggered when the widget is shown.
         *
         * \param[in] event Event that triggered the call to this method.
         */
        void showEvent(QShowEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the width dimension line edit value is updated.
         *
         * \param[in] newValue The new width value.
         */
        void plotWidthChanged(double newValue);

        /**
         * Slot that is triggered when the height dimension line edit value is updated.
         *
         * \param[in] newValue The new height value.
         */
        void plotHeightChanged(double newValue);

        /**
         * Slot that is triggered when the border line style is changed.
         *
         * \param[in] newLineStyle The new border line style.
         */
        void borderLineStyleWasChanged(LineStyle newLineStyle);

        /**
         * Slot that is triggered when the border line width is changed.
         *
         * \param[in] newLineWidth The new border line width.
         */
        void borderLineWidthWasChanged(float newLineWidth);

        /**
         * Slot that is triggered when the border line color is changed.
         *
         * \param[in] newLineColor The new border line color.
         */
        void borderLineColorWasChanged(const QColor& newLineColor);

        /**
         * Slot that is triggered when the background color button is clicked.
         *
         * \param[in] newColor The new background color.
         */
        void backgroundColorButtonClicked();

        /**
         * Slot that is triggered when the plot area border line style is changed.
         *
         * \param[in] newLineStyle The new plot area border line style.
         */
        void plotAreaBorderLineStyleWasChanged(LineStyle newLineStyle);

        /**
         * Slot that is triggered when the plot area border line width is changed.
         *
         * \param[in] newLineWidth The new plot area border line width.
         */
        void plotAreaBorderLineWidthWasChanged(float newLineWidth);

        /**
         * Slot that is triggered when the plot area border line color is changed.
         *
         * \param[in] newLineColor The new plot area border line color.
         */
        void plotAreaBorderLineColorWasChanged(const QColor& newLineColor);

        /**
         * Slot that is triggered when the plot area background color button is clicked.
         */
        void plotAreaBackgroundColorButtonClicked();

        /**
         * Slot that is triggered when the legend area group box is checked.
         *
         * \param[in] nowChecked If true, the legend area group box is checked.  If false, the legend area group box
         *                       is unchecked.
         */
        void legendAreaGroupBoxClicked(bool nowChecked);

        /**
         * Slot that is triggered when the legend area combo box is changed.
         *
         * \param[in] index The index of the newly selected area.
         */
        void legendAreaComboBoxChanged(int index);

        /**
         * Slot that is triggered when the legend area border line style is changed.
         *
         * \param[in] newLineStyle The new legend area border line style.
         */
        void legendAreaBorderLineStyleWasChanged(LineStyle newLineStyle);

        /**
         * Slot that is triggered when the legend area border line width is changed.
         *
         * \param[in] newLineWidth The new legend area border line width.
         */
        void legendAreaBorderLineWidthWasChanged(float newLineWidth);

        /**
         * Slot that is triggered when the legend area border line color is changed.
         *
         * \param[in] newLineColor The new legend area border line color.
         */
        void legendAreaBorderLineColorWasChanged(const QColor& newLineColor);

        /**
         * Slot that is triggered when the legend area background color button is clicked.
         */
        void legendAreaBackgroundColorButtonClicked();

        /**
         * Slot that is triggered when the legend font button is clicked.
         */
        void legendFontButtonClicked();

        /**
         * Slot that is triggered when the title text is modified.
         *
         * \param[in] newText The new title text.
         */
        void titleTextEdited(const QString& newText);

        /**
         * Slot that is triggered when the title font button is clicked.
         */
        void titleFontButtonClicked();

        /**
         * Slot that is triggered when the dialog becomes visible.
         */
        void nowVisible();

    private:
        /**
         * The line editor used to set the plot width.
         */
        EQt::DimensionLineEdit* plotWidthDimensionLineEdit;

        /**
         * The line editor used to set the plot height.
         */
        EQt::DimensionLineEdit* plotHeightDimensionLineEdit;

        /**
         * The border line style control.
         */
        LineStyleWidget* borderLineStyleWidget;

        /**
         * Label used in conjunction with the background color button.
         */
        QLabel* backgroundColorToolButtonLabel;

        /**
         * Color tool button used to change the plot background color.
         */
        EQt::ColorToolButton* backgroundColorToolButton;

        /**
         * The plot area group box.
         */
        QGroupBox* plotAreaGroupBox;

        /**
         * The plot area border line style.
         */
        LineStyleWidget* plotAreaBorderLineStyleWidget;

        /**
         * Label used for the plot area background color button.
         */
        QLabel* plotAreaBackgroundColorToolButtonLabel;

        /**
         * Color tool button used to change the plot area background color.
         */
        EQt::ColorToolButton* plotAreaBackgroundColorToolButton;

        /**
         * Checkable group box used to display legend data.
         */
        QGroupBox* legendGroupBox;

        /**
         * Combo box used to set the legend location (or hide the legend).
         */
        QComboBox* legendLocationComboBox;

        /**
         * Color tool button used to set the legend background color.
         */
        LineStyleWidget* legendBorderLineStyleWidget;

        /**
         * The legend background color tool button.
         */
        EQt::ColorToolButton* legendBackgroundColorToolButton;

        /**
         * The legend font button.
         */
        EQt::FontButton* legendFontButton;

        /**
         * The title group box.
         */
        QGroupBox* titleGroupBox;

        /**
         * The line editor used to set the plot title.
         */
        QLineEdit* titleTextLineEdit;

        /**
         * The title font button.
         */
        EQt::FontButton* titleFontButton;

        /**
         * Timer used to trigger an event after the dialog is displayed.
         */
        QTimer* showTimer;
};

#endif
