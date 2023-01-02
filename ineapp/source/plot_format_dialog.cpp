/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QList>
#include <QStringList>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QTabBar>
#include <QDialogButtonBox>
#include <QPushButton>

#include <cmath>

#include <eqt_programmatic_dialog.h>

#include <ld_plot_series.h>
#include <ld_character_format.h>

#include "application.h"
#include "plot_engine.h"
#include "plot_style_widget.h"
#include "plot_data_sources_widget.h"
#include "plot_axis_settings_widget.h"
#include "plot_appearance_widget.h"
#include "plot_format_dialog.h"

PlotFormatDialog::PlotFormatDialog(QWidget* parent):EQt::ProgrammaticDialog("PlotFormatDialog", parent) {
    configure();
    runBuilders();
}


PlotFormatDialog::~PlotFormatDialog() {}


void PlotFormatDialog::loadSettings(const QString& groupName) {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    appearancesWidget->loadSettings(groupName);
}


void PlotFormatDialog::saveSettings(const QString& groupName) {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    appearancesWidget->saveSettings(groupName);
}


const PlotEngine* PlotFormatDialog::plotStyle() const {
    PlotStyleWidget* plotStyleWidget = widget<PlotStyleWidget>("plot_style_widget");
    return plotStyleWidget->plotStyle();
}


float PlotFormatDialog::plotWidth() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->plotWidth();
}


float PlotFormatDialog::plotHeight() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->plotHeight();
}


unsigned PlotFormatDialog::dataSourcesPerDataSeries() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->dataSourcesPerDataSeries();
}


unsigned PlotFormatDialog::numberDataSeries() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->numberDataSeries();
}


PlotFormatDialog::AxisLocationStyle PlotFormatDialog::axisLocationStyle() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->axisLocationStyle();
}


PlotFormatDialog::PlotSeriesMode PlotFormatDialog::supportedPlotSeriesMode() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->supportedPlotSeriesMode();
}


QList<Ld::VariableName> PlotFormatDialog::dataSources() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->dataSources();
}


Ld::VariableName PlotFormatDialog::dataSource(unsigned seriesIndex, unsigned sourceIndex) const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->dataSource(seriesIndex, sourceIndex);
}


QList<PlotFormatDialog::AxisLocation> PlotFormatDialog::configuredAxisLocations() const {
    return currentTabsByAxisLocation.keys();
}


PlotFormatDialog::AxisLocation PlotFormatDialog::axisLocation(unsigned seriesIndex, unsigned sourceIndex) const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->axisLocation(seriesIndex, sourceIndex);
}


QList<Ld::PlotSeries> PlotFormatDialog::plotSeries() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->plotSeries();
}


Ld::PlotSeries PlotFormatDialog::plotSeries(unsigned seriesIndex) const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->plotSeries(seriesIndex);
}


const QStringList& PlotFormatDialog::seriesLabels() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->seriesLabels();
}


QString PlotFormatDialog::seriesLabel(unsigned seriesIndex) const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->seriesLabel(seriesIndex);
}


QList<PlotFormatDialog::AxisLocation> PlotFormatDialog::axisLocations() const {
    QList<PlotFormatDialog::AxisLocation> result;

    const PlotEngine* plotEngine = plotStyle();
    if (plotEngine != Q_NULLPTR) {
        if (plotEngine->axisLocationStyle() == AxisLocationStyle::FIXED) {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::LEFT_Y_R_RC);
        } else if (plotEngine->axisLocationStyle() != AxisLocationStyle::UNCATEGORIZED) {
            PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");

            QList<PlotFormatDialog::AxisLocation> unculled = dataSourcesWidget->axisLocations();
            QSet<PlotFormatDialog::AxisLocation>  unique(unculled.constBegin(), unculled.constEnd());

            result = QList<PlotFormatDialog::AxisLocation>(unique.constBegin(), unique.constEnd());
        }
    }

    return result;
}


PlotFormatDialog::AxisLocation PlotFormatDialog::baselineAxis() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->baselineAxis();
}


bool PlotFormatDialog::firstDataSeriesIsDataLabels() const {
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    return dataSourcesWidget->firstDataSeriesIsLabels();
}


PlotFormatDialog::AxisScale PlotFormatDialog::axisScale(PlotFormatDialog::AxisLocation axisLocation) const {
    PlotAxisSettingsWidget* axisSettingsWidget = PlotFormatDialog::axisSettingsWidget(axisLocation);
    return axisSettingsWidget != Q_NULLPTR ? axisSettingsWidget->axisScale() : AxisScale::LINEAR;
}


double PlotFormatDialog::minimumValue(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->minimumValue();
}


double PlotFormatDialog::maximumValue(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->maximumValue();
}


double PlotFormatDialog::majorStepSize(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->majorStepSize();
}


double PlotFormatDialog::minorStepSize(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->minorStepSize();
}


PlotFormatDialog::TickStyle PlotFormatDialog::majorTickStyle(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->majorTickStyle();
}


PlotFormatDialog::TickStyle PlotFormatDialog::minorTickStyle(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->minorTickStyle();
}


PlotFormatDialog::LineStyle PlotFormatDialog::majorGridLineStyle(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->majorGridLineStyle();
}


float PlotFormatDialog::majorGridLineWidth(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->majorGridLineWidth();
}


QColor PlotFormatDialog::majorGridLineColor(AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->majorGridLineColor();
}


PlotFormatDialog::LineStyle PlotFormatDialog::minorGridLineStyle(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->minorGridLineStyle();
}


float PlotFormatDialog::minorGridLineWidth(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->minorGridLineWidth();
}


QColor PlotFormatDialog::minorGridLineColor(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->minorGridLineColor();
}


bool PlotFormatDialog::axisTitleFieldVisible(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->axisTitleFieldVisible();
}


bool PlotFormatDialog::axisTitleFieldHidden(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->axisTitleFieldHidden();
}


QString PlotFormatDialog::axisTitle(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->axisTitle();
}


QFont PlotFormatDialog::titleFont(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->titleFont();
}


QColor PlotFormatDialog::titleFontColor(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->titleFontColor();
}


QColor PlotFormatDialog::titleFontBackgroundColor(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->titleFontBackgroundColor();
}


QFont PlotFormatDialog::numberFont(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->numberFont();
}


QColor PlotFormatDialog::numberFontColor(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->numberFontColor();
}


QColor PlotFormatDialog::numberFontBackgroundColor(PlotFormatDialog::AxisLocation axisLocation) const {
    return axisSettingsWidget(axisLocation)->numberFontBackgroundColor();
}


PlotFormatDialog::LineStyle PlotFormatDialog::borderLineStyle() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->borderLineStyle();
}


float PlotFormatDialog::borderLineWidth() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->borderLineWidth();
}


QColor PlotFormatDialog::borderLineColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->borderLineColor();
}


QColor PlotFormatDialog::backgroundColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->backgroundColor();
}


PlotFormatDialog::LineStyle PlotFormatDialog::activeAreaBorderLineStyle() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->activeAreaBorderLineStyle();
}


float PlotFormatDialog::activeAreaBorderLineWidth() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->activeAreaBorderLineWidth();
}


QColor PlotFormatDialog::activeAreaBorderLineColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->activeAreaBorderLineColor();
}


QColor PlotFormatDialog::activeAreaColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->activeAreaColor();
}


PlotFormatDialog::LegendLocation PlotFormatDialog::legendLocation() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendLocation();
}


PlotFormatDialog::LineStyle PlotFormatDialog::legendLineStyle() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendLineStyle();
}


float PlotFormatDialog::legendLineWidth() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendLineWidth();
}


QColor PlotFormatDialog::legendLineColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendLineColor();
}


QColor PlotFormatDialog::legendBackgroundColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendBackgroundColor();
}


QFont PlotFormatDialog::legendFont() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendFont();
}


QColor PlotFormatDialog::legendFontColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendFontColor();
}


QColor PlotFormatDialog::legendFontBackgroundColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->legendFontBackgroundColor();
}


QString PlotFormatDialog::titleText() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->titleText();
}


QFont PlotFormatDialog::titleFont() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->titleFont();
}


QColor PlotFormatDialog::titleFontColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->titleFontColor();
}


QColor PlotFormatDialog::titleFontBackgroundColor() const {
    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    return appearancesWidget->titleFontBackgroundColor();
}


void PlotFormatDialog::populate() {
    PlotStyleWidget*       plotStyleWidget   = widget<PlotStyleWidget>("plot_style_widget");
    PlotDataSourcesWidget* dataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    PlotAppearanceWidget*  appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");

    currentlyPopulatingDialog = true;

    plotStyleWidget->setPlotStyle(inputPlotStyle);

    PlotSeriesMode               supportedPlotSeriesMode = inputPlotStyle->supportedPlotSeriesMode();
    AxisLocationStyle            axisLocationStyle       = inputPlotStyle->axisLocationStyle();
    QSet<PlotEngine::SplineType> supportedSplineTypes    = inputPlotStyle->supportedSplineTypes();
    unsigned                     dataSourcesPerSeries    = inputPlotStyle->dataSourcesPerSeries();
    unsigned                     minimumNumberDataSeries = inputPlotStyle->minimumNumberAllowedDataSeries();
    unsigned                     maximumNumberDataSeries = inputPlotStyle->maximumNumberAllowedDataSeries();

    dataSourcesWidget->setSupportedPlotSeriesMode(supportedPlotSeriesMode);
    dataSourcesWidget->setAxisLocationStyle(axisLocationStyle);
    dataSourcesWidget->setSupportedSplineTypes(supportedSplineTypes);
    dataSourcesWidget->setDataSourcesPerDataSeries(dataSourcesPerSeries);
    dataSourcesWidget->setNumberAllowedDataSeries(minimumNumberDataSeries, maximumNumberDataSeries);
    dataSourcesWidget->setBaselineAxis(inputBaselineAxis);
    dataSourcesWidget->setFirstSeriesIsDataLabels(inputFirstDataSeriesIsDataLabels);

    unsigned numberDataSeries = static_cast<unsigned>(inputPlotSeries.size());
    QSet<AxisLocation> referencedAxisLocations;
    for (unsigned seriesIndex=0 ; seriesIndex<numberDataSeries ; ++seriesIndex) {
        for (unsigned sourceIndex=0 ; sourceIndex<dataSourcesPerSeries ; ++sourceIndex) {
            unsigned                index        = seriesIndex * dataSourcesPerSeries + sourceIndex;
            const Ld::VariableName& source       = inputDataSources.at(index);
            AxisLocation            axisLocation = inputAxisLocations.at(index);

            dataSourcesWidget->setDataSource(seriesIndex, sourceIndex, source);
            dataSourcesWidget->setAxisLocation(seriesIndex, sourceIndex, axisLocation);

            referencedAxisLocations.insert(axisLocation);
        }

        dataSourcesWidget->setPlotSeries(seriesIndex, inputPlotSeries.at(seriesIndex));
        dataSourcesWidget->setPlotSeriesLabel(seriesIndex, inputPlotSeriesLabels.at(seriesIndex));
    }

    if (axisLocationStyle == AxisLocationStyle::CATEGORIZED ||
        axisLocationStyle == AxisLocationStyle::FIXED          ) {
        referencedAxisLocations.clear();
        referencedAxisLocations.insert(AxisLocation::BOTTOM_X_A_GM);
        referencedAxisLocations.insert(AxisLocation::LEFT_Y_R_RC);
    } else if (axisLocationStyle == AxisLocationStyle::UNCATEGORIZED) {
        referencedAxisLocations.clear();
    }

    updateTabs();

    QSharedPointer<Ld::CharacterFormat> applicationTextFont = Ld::CharacterFormat::applicationDefaultTextFont();

    QFont  defaultAxisFont                = applicationTextFont->toQFont();
    QColor defaultAxisFontColor           = applicationTextFont->fontColor();
    QColor defaultAxisFontBackgroundColor = applicationTextFont->fontBackgroundColor();

    QSharedPointer<Ld::CharacterFormat> applicationNumberFont = Ld::CharacterFormat::applicationDefaultMathFont();

    QFont  defaultNumberFont                = applicationNumberFont->toQFont();
    QColor defaultNumberFontColor           = applicationNumberFont->fontColor();
    QColor defaultNumberFontBackgroundColor = applicationNumberFont->fontBackgroundColor();

    QTabWidget* tabWidget = widget<QTabWidget>("tab_widget");
    for (unsigned tabIndex=firstAxisTabIndex ; tabIndex<=lastAxisTabIndex ; ++tabIndex) {
        PlotAxisSettingsWidget* widget = dynamic_cast<PlotAxisSettingsWidget*>(tabWidget->widget(tabIndex));

        widget->setAxisScale(AxisScale::LINEAR);

        widget->setMajorGridLineStyle(LineStyle::SOLID_LINE);
        widget->setMajorGridLineWidth(0);
        widget->setMajorGridLineColor(QColor(Qt::GlobalColor::black));
        widget->setMajorTickStyle(TickStyle::LINE);
        widget->setMajorStepSize(automaticScaling);

        widget->setMinorGridLineStyle(LineStyle::SOLID_LINE);
        widget->setMinorGridLineWidth(0);
        widget->setMinorGridLineColor(QColor(Qt::GlobalColor::black));
        widget->setMinorTickStyle(TickStyle::LINE);
        widget->setMinorStepSize(automaticScaling);

        widget->setMinimumValue(automaticScaling);
        widget->setMaximumValue(automaticScaling);

        widget->setAxisTitle(QString());
        widget->setTitleFont(defaultAxisFont);
        widget->setTitleFontColor(defaultAxisFontColor);
        widget->setTitleFontBackgroundColor(defaultAxisFontBackgroundColor);

        widget->setNumberFont(defaultNumberFont);
        widget->setNumberFontColor(defaultNumberFontColor);
        widget->setNumberFontBackgroundColor(defaultNumberFontBackgroundColor);
    }

    if (axisLocationStyle != AxisLocationStyle::RGB_COLOR_SPACE       &&
        axisLocationStyle != AxisLocationStyle::CMYK_COLOR_SPACE      &&
        axisLocationStyle != AxisLocationStyle::GRAYSCALE_COLOR_SPACE    ) {
        for (  QSet<AxisLocation>::const_iterator
                   axisLocationIterator = referencedAxisLocations.constBegin(),
                   axisLocationEndIterator = referencedAxisLocations.constEnd()
             ; axisLocationIterator != axisLocationEndIterator
             ; ++axisLocationIterator
            ) {
            AxisLocation            axis   = *axisLocationIterator;
            PlotAxisSettingsWidget* widget = axisSettingsWidget(axis);

            widget->setAxisScale(inputAxisScaling.value(axis, AxisScale::LINEAR));
            widget->setSupportedAxisScales(inputPlotStyle->supportedAxisScales(axis));

            widget->setMajorGridLineStyle(inputMajorGridLineStyles.value(axis, LineStyle::SOLID_LINE));
            widget->setMajorGridLineWidth(inputMajorGridLineWidths.value(axis, 0));
            widget->setMajorGridLineColor(inputMajorGridLineColors.value(axis, QColor(Qt::GlobalColor::black)));
            widget->setMajorTickStyle(inputMajorTickStyles.value(axis, TickStyle::LINE));
            widget->setMajorStepSize(inputMajorStepSizes.value(axis, automaticScaling));

            widget->setMinorGridLineStyle(inputMinorGridLineStyles.value(axis, LineStyle::SOLID_LINE));
            widget->setMinorGridLineWidth(inputMinorGridLineWidths.value(axis, 0));
            widget->setMinorGridLineColor(inputMinorGridLineColors.value(axis, QColor(Qt::GlobalColor::black)));
            widget->setMinorTickStyle(inputMinorTickStyles.value(axis, TickStyle::LINE));
            widget->setMinorStepSize(inputMinorStepSizes.value(axis, automaticScaling));

            widget->setMinimumValue(inputMinimumValues.value(axis, automaticScaling));
            widget->setMaximumValue(inputMaximumValues.value(axis, automaticScaling));

            widget->setAxisTitle(inputAxisTitles.value(axis, QString()));
            widget->setTitleFont(inputAxisTitleFonts.value(axis, defaultAxisFont));
            widget->setTitleFontColor(inputAxisTitleFontColors.value(axis, defaultAxisFontColor));
            widget->setTitleFontBackgroundColor(
                inputAxisTitleFontBackgroundColors.value(axis, defaultAxisFontBackgroundColor)
            );

            widget->setNumberFont(inputAxisNumberFonts.value(axis, defaultNumberFont));
            widget->setNumberFontColor(inputAxisNumberFontColors.value(axis, defaultNumberFontColor));
            widget->setNumberFontBackgroundColor(
                inputAxisNumberFontBackgroundColors.value(axis, defaultNumberFontBackgroundColor)
            );
        }
    }

    appearancesWidget->setPlotWidth(inputPlotWidth);
    appearancesWidget->setPlotHeight(inputPlotHeight);

    appearancesWidget->setBorderLineStyle(inputBorderLineStyle);
    appearancesWidget->setBorderLineWidth(inputBorderLineWidth);
    appearancesWidget->setBorderLineColor(inputBorderLineColor);

    appearancesWidget->setBackgroundColor(inputBackgroundColor);

    appearancesWidget->setActiveAreaBorderLineStyle(inputActiveAreaBorderLineStyle);
    appearancesWidget->setActiveAreaBorderLineWidth(inputActiveAreaBorderLineWidth);
    appearancesWidget->setActiveAreaBorderLineColor(inputActiveAreaBorderLineColor);

    appearancesWidget->setPlotAreaColor(inputActiveAreaBackgroundColor);

    appearancesWidget->setLegendLocation(inputLegendLocation);
    appearancesWidget->setLegendLineStyle(inputLegendBorderLineStyle);
    appearancesWidget->setLegendLineWidth(inputLegendBorderLineWidth);
    appearancesWidget->setLegendLineColor(inputLegendBorderLineColor);

    appearancesWidget->setLegendFont(inputLegendFont);
    appearancesWidget->setLegendFontColor(inputLegendFontColor);
    appearancesWidget->setLegendFontBackgroundColor(inputLegendFontBackgroundColor);
    appearancesWidget->setLegendBackgroundColor(inputLegendBackgroundColor);

    appearancesWidget->setTitleText(inputPlotTitleText);
    appearancesWidget->setTitleFont(inputPlotTitleFont);
    appearancesWidget->setTitleFontColor(inputPlotTitleFontColor);
    appearancesWidget->setTitleFontBackgroundColor(inputPlotTitleFontBackgroundColor);

    appearancesWidget->setActiveAreaBackgroundControlsEnabled(inputPlotStyle->activeAreaBackgroundAdjustable());
    appearancesWidget->setLegendControlsEnabled(inputPlotStyle->supportsPlotLegend());
    appearancesWidget->setActiveAreaControlsEnabled(inputPlotStyle->hasDistinctActiveArea());
    appearancesWidget->setPlotTitleEnabled(inputPlotStyle->supportsPlotTitle());
    appearancesWidget->setBackgroundColorButtonEnabled(inputPlotStyle->supportsControllableBackgroundArea());

    currentlyPopulatingDialog = false;

    updateOkButton();
}


void PlotFormatDialog::setPlotStyle(const PlotEngine* newPlotEngine) {
    inputPlotStyle = newPlotEngine;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setPlotWidth(float newPlotWidth) {
    inputPlotWidth = newPlotWidth;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setPlotHeight(float newPlotHeight) {
    inputPlotHeight = newPlotHeight;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setDataSources(
        const QList<Ld::VariableName>&               newDataSources,
        const QList<PlotFormatDialog::AxisLocation>& newAxisLocations,
        const QList<Ld::PlotSeries>&                 newPlotSeries,
        const QStringList&                           newPlotSeriesLabels
    ) {
    inputDataSources      = newDataSources;
    inputAxisLocations    = newAxisLocations;
    inputPlotSeries       = newPlotSeries;
    inputPlotSeriesLabels = newPlotSeriesLabels;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setDataSource(
        unsigned                seriesIndex,
        unsigned                sourceIndex,
        const Ld::VariableName& newDataSource
    ) {
    unsigned dataSourcesPerDataSeries = inputPlotStyle->dataSourcesPerSeries();
    unsigned index                    = seriesIndex * dataSourcesPerDataSeries + sourceIndex;

    while (static_cast<unsigned>(inputDataSources.size()) <= index) {
        inputDataSources.append(Ld::VariableName());
        inputAxisLocations.append(AxisLocation::BOTTOM_X_A_GM);
    }

    inputDataSources[index] = newDataSource;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setAxisLocation(
        unsigned                       seriesIndex,
        unsigned                       sourceIndex,
        PlotFormatDialog::AxisLocation newAxisLocation
    ) {
    unsigned dataSourcesPerDataSeries = inputPlotStyle->dataSourcesPerSeries();
    unsigned index                    = seriesIndex * dataSourcesPerDataSeries + sourceIndex;

    while (static_cast<unsigned>(inputDataSources.size()) <= index) {
        inputDataSources.append(Ld::VariableName());
        inputAxisLocations.append(AxisLocation::BOTTOM_X_A_GM);
    }

    inputAxisLocations[index] = newAxisLocation;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setPlotSeries(unsigned index, const Ld::PlotSeries& newPlotSeries) {
    while (static_cast<unsigned>(inputPlotSeries.size()) <= index) {
        unsigned defaultIndex = inputPlotSeries.size() % Ld::PlotSeries::numberDefaultPlotSeries;
        inputPlotSeries.append(Ld::PlotSeries::defaultPlotSeries[defaultIndex]);
        inputPlotSeriesLabels.append(tr("Series %1").arg(inputPlotSeries.size() + 1));
    }

    inputPlotSeries[index] = newPlotSeries;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setPlotSeriesLabel(unsigned seriesIndex, const QString& plotSeriesLabel) {
    while (static_cast<unsigned>(inputPlotSeries.size()) <= seriesIndex) {
        unsigned defaultIndex = inputPlotSeries.size() % Ld::PlotSeries::numberDefaultPlotSeries;
        inputPlotSeries.append(Ld::PlotSeries::defaultPlotSeries[defaultIndex]);
        inputPlotSeriesLabels.append(tr("Series %1").arg(inputPlotSeries.size() + 1));
    }

    inputPlotSeriesLabels[seriesIndex] = plotSeriesLabel;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setBaselineAxis(PlotFormatDialog::AxisLocation newBaselineAxis) {
    inputBaselineAxis = newBaselineAxis;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setFirstDataSeriesIsDataLabels(bool nowFirstDataSeriesIsDataLabels) {
    inputFirstDataSeriesIsDataLabels = nowFirstDataSeriesIsDataLabels;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setAxisScale(
        PlotFormatDialog::AxisLocation axisLocation,
        PlotFormatDialog::AxisScale    newAxisScale
    ) {
    inputAxisScaling.insert(axisLocation, newAxisScale);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMajorGridLineStyle(
        PlotFormatDialog::AxisLocation axisLocation,
        PlotFormatDialog::LineStyle    newLineStyle
    ) {
    inputMajorGridLineStyles.insert(axisLocation, newLineStyle);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMajorGridLineWidth(PlotFormatDialog::AxisLocation axisLocation, float newLineWidth) {
    inputMajorGridLineWidths.insert(axisLocation, newLineWidth);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMajorGridLineColor(
        PlotFormatDialog::AxisLocation axisLocation,
        const QColor&                  newLineColor
    ) {
    inputMajorGridLineColors.insert(axisLocation, newLineColor);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMinorGridLineStyle(
        PlotFormatDialog::AxisLocation axisLocation,
        PlotFormatDialog::LineStyle    newLineStyle
    ) {
    inputMinorGridLineStyles.insert(axisLocation, newLineStyle);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMinorGridLineWidth(PlotFormatDialog::AxisLocation axisLocation, float newLineWidth) {
    inputMinorGridLineWidths.insert(axisLocation, newLineWidth);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMinorGridLineColor(
        PlotFormatDialog::AxisLocation axisLocation,
        const QColor&                  newLineColor
    ) {
    inputMinorGridLineColors.insert(axisLocation, newLineColor);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMinimumValue(PlotFormatDialog::AxisLocation axisLocation, double newMinimumValue) {
    inputMinimumValues.insert(axisLocation, newMinimumValue);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMaximumValue(PlotFormatDialog::AxisLocation axisLocation, double newMaximumValue) {
    inputMaximumValues.insert(axisLocation, newMaximumValue);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMajorStepSize(PlotFormatDialog::AxisLocation axisLocation, double newMajorStepSize) {
    inputMajorStepSizes.insert(axisLocation, newMajorStepSize);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMinorStepSize(PlotFormatDialog::AxisLocation axisLocation, double newMinorStepSize) {
    inputMinorStepSizes.insert(axisLocation, newMinorStepSize);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMajorTickStyle(
        PlotFormatDialog::AxisLocation axisLocation,
        PlotFormatDialog::TickStyle    newMajorTickStyle
    ) {
    inputMajorTickStyles.insert(axisLocation, newMajorTickStyle);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setMinorTickStyle(
        PlotFormatDialog::AxisLocation axisLocation,
        PlotFormatDialog::TickStyle    newMinorTickStyle
    ) {
    inputMinorTickStyles.insert(axisLocation, newMinorTickStyle);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setAxisTitle(PlotFormatDialog::AxisLocation axisLocation, const QString& newAxisTitle) {
    inputAxisTitles.insert(axisLocation, newAxisTitle);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setTitleFont(PlotFormatDialog::AxisLocation axisLocation, const QFont& newTitleFont) {
    inputAxisTitleFonts.insert(axisLocation, newTitleFont);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setTitleFontColor(
        PlotFormatDialog::AxisLocation axisLocation,
        const QColor&                  newTitleFontColor
    ) {
    inputAxisTitleFontColors.insert(axisLocation, newTitleFontColor);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setTitleFontBackgroundColor(
        PlotFormatDialog::AxisLocation axisLocation,
        const QColor&                  newTitleFontBackgroundColor
    ) {
    inputAxisTitleFontBackgroundColors.insert(axisLocation, newTitleFontBackgroundColor);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setNumberFont(PlotFormatDialog::AxisLocation axisLocation, const QFont& newNumberFont) {
    inputAxisNumberFonts.insert(axisLocation, newNumberFont);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setNumberFontColor(
        PlotFormatDialog::AxisLocation axisLocation,
        const QColor&                  newNumberFontColor
    ) {
    inputAxisNumberFontColors.insert(axisLocation, newNumberFontColor);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setNumberFontBackgroundColor(
        PlotFormatDialog::AxisLocation axisLocation,
        const QColor&                  newNumberFontBackgroundColor
    ) {
    inputAxisNumberFontBackgroundColors.insert(axisLocation, newNumberFontBackgroundColor);

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setBorderLineStyle(PlotFormatDialog::LineStyle newBorderLineStyle) {
    inputBorderLineStyle = newBorderLineStyle;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setBorderLineWidth(float newBorderWidth) {
    inputBorderLineWidth = newBorderWidth;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setBorderLineColor(const QColor& newBorderColor) {
    inputBorderLineColor = newBorderColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setBackgroundColor(const QColor& newBackgroundColor) {
    inputBackgroundColor = newBackgroundColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setActiveAreaBorderLineStyle(PlotFormatDialog::LineStyle newLineStyle) {
    inputActiveAreaBorderLineStyle = newLineStyle;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setActiveAreaBorderLineWidth(float newLineWidth) {
    inputActiveAreaBorderLineWidth = newLineWidth;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setActiveAreaBorderLineColor(const QColor& newLineColor) {
    inputActiveAreaBorderLineColor = newLineColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setPlotAreaColor(const QColor& newPlotAreaColor) {
    inputActiveAreaBackgroundColor = newPlotAreaColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendLocation(PlotFormatDialog::LegendLocation newLocation) {
    inputLegendLocation = newLocation;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendLineStyle(PlotFormatDialog::LineStyle newLineStyle) {
    inputLegendBorderLineStyle = newLineStyle;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendLineWidth(float newLineWidth) {
    inputLegendBorderLineWidth = newLineWidth;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendLineColor(const QColor& newLineColor) {
    inputLegendBorderLineColor = newLineColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendFont(const QFont& newFont) {
    inputLegendFont = newFont;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendFontColor(const QColor& newFontColor) {
    inputLegendFontColor = newFontColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendFontBackgroundColor(const QColor& newFontBackgroundColor) {
    inputLegendFontBackgroundColor = newFontBackgroundColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setLegendBackgroundColor(const QColor& newLegendBackgroundColor) {
    inputLegendBackgroundColor = newLegendBackgroundColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}

void PlotFormatDialog::setTitleText(const QString& newText) {
    inputPlotTitleText = newText;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setTitleFont(const QFont& newFont) {
    inputPlotTitleFont = newFont;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setTitleFontColor(const QColor& newFontColor) {
    inputPlotTitleFontColor = newFontColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::setTitleFontBackgroundColor(const QColor& newFontBackgroundColor) {
    inputPlotTitleFontBackgroundColor = newFontBackgroundColor;

    if (isVisible() && !currentlyPopulatingDialog) {
        populate();
    }
}


void PlotFormatDialog::plotStyleChanged(const PlotEngine* newPlotStyle) {
    PlotDataSourcesWidget* plotDataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    PlotAppearanceWidget*  plotAppearanceWidget  = widget<PlotAppearanceWidget>("plot_appearance_widget");

    if (newPlotStyle != Q_NULLPTR) {
        PlotSeriesMode               supportedPlotSeriesMode = newPlotStyle->supportedPlotSeriesMode();
        AxisLocationStyle            axisLocationStyle       = newPlotStyle->axisLocationStyle();
        QSet<PlotEngine::SplineType> supportedSplineTypes    = newPlotStyle->supportedSplineTypes();
        unsigned                     dataSourcesPerSeries    = newPlotStyle->dataSourcesPerSeries();
        unsigned                     minimumNumberDataSeries = newPlotStyle->minimumNumberAllowedDataSeries();
        unsigned                     maximumNumberDataSeries = newPlotStyle->maximumNumberAllowedDataSeries();

        plotDataSourcesWidget->setSupportedPlotSeriesMode(supportedPlotSeriesMode);
        plotDataSourcesWidget->setAxisLocationStyle(axisLocationStyle);
        plotDataSourcesWidget->setSupportedSplineTypes(supportedSplineTypes);
        plotDataSourcesWidget->setDataSourcesPerDataSeries(dataSourcesPerSeries);
        plotDataSourcesWidget->setNumberAllowedDataSeries(minimumNumberDataSeries, maximumNumberDataSeries);
        plotAppearanceWidget->setActiveAreaBackgroundControlsEnabled(newPlotStyle->activeAreaBackgroundAdjustable());
    }

    updateTabs();
    updateOkButton();
}


void PlotFormatDialog::dataSeriesChanged() {
    updateTabs();
    updateOkButton();
}


void PlotFormatDialog::axisSettingChanged() {
    updateOkButton();
}


void PlotFormatDialog::appearanceSettingChanged() {
    updateOkButton();
}


void PlotFormatDialog::bind() {
    PlotStyleWidget* plotStyleWidget = widget<PlotStyleWidget>("plot_style_widget");
    connect(plotStyleWidget, &PlotStyleWidget::plotStyleChanged, this, &PlotFormatDialog::plotStyleChanged);

    PlotDataSourcesWidget* plotDataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    connect(plotDataSourcesWidget, &PlotDataSourcesWidget::changed, this, &PlotFormatDialog::dataSeriesChanged);

    for (unsigned axisNumber=0 ; axisNumber<4 ; ++axisNumber) {
        QString widgetName = QString("plot_axis_settings_widget_%1").arg(axisNumber);
        PlotAxisSettingsWidget* axisSettingsWidget = widget<PlotAxisSettingsWidget>(widgetName);
        connect(axisSettingsWidget, &PlotAxisSettingsWidget::changed, this, &PlotFormatDialog::axisSettingChanged);
    }

    PlotAppearanceWidget* appearancesWidget = widget<PlotAppearanceWidget>("plot_appearance_widget");
    connect(appearancesWidget, &PlotAppearanceWidget::changed, this, &PlotFormatDialog::appearanceSettingChanged);

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(okPushButton, &QPushButton::clicked, this, &PlotFormatDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &PlotFormatDialog::reject);
}


void PlotFormatDialog::configure() {
    setWindowTitle(tr("Plot Format"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentlyPopulatingDialog        = false;
    inputPlotStyle                   = Q_NULLPTR;
    inputPlotWidth                   = 4.0F * 72.0F;
    inputPlotHeight                  = 3.0F * 72.0F;
    inputBorderLineStyle             = LineStyle::SOLID_LINE;
    inputBorderLineWidth             = 0;
    inputActiveAreaBorderLineStyle   = LineStyle::SOLID_LINE;
    inputActiveAreaBorderLineWidth   = 0;
    inputLegendLocation              = LegendLocation::RIGHT;
    inputLegendBorderLineStyle       = LineStyle::NO_LINE;
    inputLegendBorderLineWidth       = 0;
    inputBaselineAxis                = AxisLocation::BOTTOM_X_A_GM;
    inputFirstDataSeriesIsDataLabels = false;

    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setLayout(verticalLayout);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setAcceptDrops(false);
    tabWidget->setMovable(false);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setTabsClosable(false);
    tabWidget->setElideMode(Qt::ElideNone);
    registerWidget(tabWidget, "tab_widget");
    registerWidget(tabWidget->tabBar(), "tab_bar");

    verticalLayout->addWidget(tabWidget);

    PlotStyleWidget* plotStyleWidget = new PlotStyleWidget(this);
    registerWidget(plotStyleWidget, "plot_style_widget");
    tabWidget->addTab(plotStyleWidget, tr("Style"));

    unsigned tabIndex = static_cast<unsigned>(tabWidget->count());

    PlotDataSourcesWidget* plotDataSourcesWidget = new PlotDataSourcesWidget(this);
    registerWidget(plotDataSourcesWidget, "plot_data_sources_widget");
    tabWidget->addTab(plotDataSourcesWidget, tr("Data Sources"));
    tabWidget->setTabEnabled(tabIndex++, false);

    for (unsigned axisNumber=0 ; axisNumber<4 ; ++axisNumber) {
        PlotAxisSettingsWidget* plotAxisSettingsWidget = new PlotAxisSettingsWidget(this);
        registerWidget(plotAxisSettingsWidget, QString("plot_axis_settings_widget_%1").arg(axisNumber));

        tabWidget->addTab(plotAxisSettingsWidget, QString("Axis %1").arg(axisNumber + 1));
        tabWidget->setTabVisible(tabIndex, false);
        tabWidget->setTabEnabled(tabIndex++, false);
    }

    PlotAppearanceWidget* plotAppearanceWidget = new PlotAppearanceWidget(this);
    registerWidget(plotAppearanceWidget, "plot_appearance_widget");
    tabWidget->addTab(plotAppearanceWidget, tr("Appearance"));
    tabWidget->setTabEnabled(tabIndex++, false);

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    registerWidget(dialogButtonBox, "dialog_button_box");
    registerWidget(dialogButtonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(dialogButtonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    verticalLayout->addWidget(dialogButtonBox);
}


void PlotFormatDialog::updateTabs() {
    QTabWidget*            tabWidget             = widget<QTabWidget>("tab_widget");
    PlotStyleWidget*       plotStyleWidget       = widget<PlotStyleWidget>("plot_style_widget");
    const PlotEngine*      currentStyle          = plotStyleWidget->plotStyle();
    AxisLocationStyle      axisLocationStyle      = currentStyle->axisLocationStyle();
    PlotDataSourcesWidget* plotDataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    QSet<AxisLocation>     referencedAxisLocations;

    if (axisLocationStyle == AxisLocationStyle::CATEGORIZED) {
        // This path is used for plots such as bar charts that have more constrained axis settings.

        AxisLocation baselineAxis = plotDataSourcesWidget->baselineAxis();
        AxisLocation scaleAxis;
        if (baselineAxis == AxisLocation::BOTTOM_X_A_GM) {
            scaleAxis = AxisLocation::LEFT_Y_R_RC;
        } else {
            scaleAxis = AxisLocation::BOTTOM_X_A_GM;
        }

        referencedAxisLocations.insert(baselineAxis);
        referencedAxisLocations.insert(scaleAxis);
    } else if (axisLocationStyle == AxisLocationStyle::FIXED) {
        referencedAxisLocations.insert(AxisLocation::BOTTOM_X_A_GM);
        referencedAxisLocations.insert(AxisLocation::LEFT_Y_R_RC);
    } else if (axisLocationStyle != AxisLocationStyle::UNCATEGORIZED) {
        // This is the path for traditional plots.

        QList<AxisLocation> axisLocationsList = plotDataSourcesWidget->axisLocations();
        referencedAxisLocations = QSet<AxisLocation>(axisLocationsList.begin(), axisLocationsList.end());
    }

    bool     enableTabs = (currentStyle != Q_NULLPTR);
    unsigned numberTabs = static_cast<unsigned>(tabWidget->count());
    for (unsigned tabIndex=1 ; tabIndex<numberTabs ; ++tabIndex) {
        tabWidget->setTabEnabled(tabIndex, enableTabs);
    }

    QStringList         allAxisNames;
    QList<AxisLocation> allAxisLocations;

    switch (axisLocationStyle) {
        case AxisLocationStyle::CATEGORIZED:
        case AxisLocationStyle::FIXED:
        case AxisLocationStyle::PLOT_BORDERS: {
            allAxisNames << tr("Bottom Axis")
                         << tr("Left Axis")
                         << tr("Right Axis")
                         << tr("Top Axis");
            allAxisLocations << AxisLocation::BOTTOM_X_A_GM
                             << AxisLocation::LEFT_Y_R_RC
                             << AxisLocation::RIGHT_Y_R_RY
                             << AxisLocation::TOP_X_A_GM;

            break;
        }

        case AxisLocationStyle::UNCATEGORIZED: {
            break;
        }

        case AxisLocationStyle::CARTESIAN_2D: {
            allAxisNames << tr("x Axis")
                         << tr("y Axis");
            allAxisLocations << AxisLocation::BOTTOM_X_A_GM
                             << AxisLocation::LEFT_Y_R_RC;

            break;
        }

        case AxisLocationStyle::CARTESIAN_3D: {
            allAxisNames << tr("x Axis")
                         << tr("y Axis")
                         << tr("z Axis");
            allAxisLocations << AxisLocation::BOTTOM_X_A_GM
                             << AxisLocation::LEFT_Y_R_RC
                             << AxisLocation::Z_B_BK;

            break;
        }

        case AxisLocationStyle::POLAR_2D: {
            allAxisNames << tr("r Axis")
                         << tr("%1 Axis").arg(QChar(0x03B8));
            allAxisLocations << AxisLocation::LEFT_Y_R_RC
                             << AxisLocation::BOTTOM_X_A_GM;

            break;
        }

        case AxisLocationStyle::POLAR_3D: {
            allAxisNames << tr("r Axis")
                         << tr("%1 Axis").arg(QChar(0x03B8))
                         << tr("%1 Axis").arg(QChar(0x03C6));
            allAxisLocations << AxisLocation::LEFT_Y_R_RC
                             << AxisLocation::BOTTOM_X_A_GM
                             << AxisLocation::Z_B_BK;

            break;
        }

        case AxisLocationStyle::CMYK_COLOR_SPACE:
        case AxisLocationStyle::RGB_COLOR_SPACE:
        case AxisLocationStyle::GRAYSCALE_COLOR_SPACE: {
            allAxisNames.clear();
            allAxisLocations.clear();
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    unsigned numberPossibleAxis = static_cast<unsigned>(allAxisNames.size());
    unsigned axisTabIndex       = firstAxisTabIndex;

    currentTabsByAxisLocation.clear();
    for (unsigned axisIndex=0 ; axisIndex<numberPossibleAxis ; ++axisIndex) {
        AxisLocation axisLocation = allAxisLocations.at(axisIndex);
        if (referencedAxisLocations.contains(axisLocation)) {
            tabWidget->setTabText(axisTabIndex, allAxisNames.at(axisIndex));
            tabWidget->setTabVisible(axisTabIndex, enableTabs);

            PlotAxisSettingsWidget* widget = dynamic_cast<PlotAxisSettingsWidget*>(
                tabWidget->widget(axisTabIndex)
            );

            QSet<AxisScale> supportedAxisScales = currentStyle->supportedAxisScales(axisLocation);
            if (supportedAxisScales != widget->supportedAxisScales()) {
                widget->setSupportedAxisScales(supportedAxisScales);
            }

            PlotEngine::ScalingControlsMode scalingControlsMode = currentStyle->supportedScalingControlsMode();
            if (scalingControlsMode != widget->scalingControlsMode()) {
                widget->setScalingControlsMode(scalingControlsMode);
            }

            currentTabsByAxisLocation.insert(axisLocation, axisTabIndex);

            ++axisTabIndex;
        }
    }

    while (axisTabIndex <= lastAxisTabIndex) {
        tabWidget->setTabEnabled(axisTabIndex, false);
        tabWidget->setTabVisible(axisTabIndex, false);
        ++axisTabIndex;
    }
}


bool PlotFormatDialog::valueChanged() const {
    bool valueChanged;

    PlotStyleWidget*       plotStyleWidget       = widget<PlotStyleWidget>("plot_style_widget");
    PlotDataSourcesWidget* plotDataSourcesWidget = widget<PlotDataSourcesWidget>("plot_data_sources_widget");
    PlotAppearanceWidget*  appearancesWidget     = widget<PlotAppearanceWidget>("plot_appearance_widget");

    QList<AxisLocation> currentAxisLocations = axisLocations();
    unsigned            numberAxisLocations  =   axisLocationStyle() != AxisLocationStyle::UNCATEGORIZED
                                               ? static_cast<unsigned>(currentAxisLocations.size())
                                               : 0;

    bool isOk  = (
           plotStyleWidget->isValid()
        && plotDataSourcesWidget->isValid()
        && appearancesWidget->isValid()
    );

    if (isOk) {
        AxisLocationStyle axisLocationStyle = PlotFormatDialog::axisLocationStyle();
        if (axisLocationStyle != AxisLocationStyle::RGB_COLOR_SPACE       &&
            axisLocationStyle != AxisLocationStyle::CMYK_COLOR_SPACE      &&
            axisLocationStyle != AxisLocationStyle::GRAYSCALE_COLOR_SPACE    ) {
            unsigned index = 0;
            while (isOk && index < numberAxisLocations) {
                AxisLocation            axisLocation       = currentAxisLocations.at(index);
                PlotAxisSettingsWidget* axisSettingsWidget = PlotFormatDialog::axisSettingsWidget(axisLocation);

                isOk = axisSettingsWidget != Q_NULLPTR && axisSettingsWidget->isValid();
                ++index;
            }
        } else {
            numberAxisLocations = 0;
        }
    }

    if (isOk) {
        valueChanged = (
               plotStyle() != inputPlotStyle
            || plotWidth() != inputPlotWidth
            || plotHeight() != inputPlotHeight
            || titleText() != inputPlotTitleText
            || titleFont() != inputPlotTitleFont
            || titleFontColor() != inputPlotTitleFontColor
            || titleFontBackgroundColor() != inputPlotTitleFontBackgroundColor
            || borderLineStyle() != inputBorderLineStyle
            || borderLineWidth() != inputBorderLineWidth
            || borderLineColor() != inputBorderLineColor
            || backgroundColor() != inputBackgroundColor
            || activeAreaBorderLineStyle() != inputActiveAreaBorderLineStyle
            || activeAreaBorderLineWidth() != inputActiveAreaBorderLineWidth
            || activeAreaBorderLineColor() != inputActiveAreaBorderLineColor
            || activeAreaColor() != inputActiveAreaBackgroundColor
            || legendLocation() != inputLegendLocation
            || legendLineStyle() != inputLegendBorderLineStyle
            || legendLineWidth() != inputLegendBorderLineWidth
            || legendLineColor() != inputLegendBorderLineColor
            || legendFont() != inputLegendFont
            || legendFontColor() != inputLegendFontColor
            || legendFontBackgroundColor() != inputLegendFontBackgroundColor
            || baselineAxis() != inputBaselineAxis
            || firstDataSeriesIsDataLabels() != inputFirstDataSeriesIsDataLabels
        );

        if (!valueChanged) {
            unsigned numberDataSeries = PlotFormatDialog::numberDataSeries();
            valueChanged = (numberDataSeries != static_cast<unsigned>(inputPlotSeries.size()));

            unsigned seriesIndex = 0;
            while (!valueChanged && seriesIndex < numberDataSeries) {
                valueChanged = (
                       plotSeries(seriesIndex) != inputPlotSeries.at(seriesIndex)
                    || seriesLabel(seriesIndex) != inputPlotSeriesLabels.at(seriesIndex)
                );

                unsigned numberDataSourcesPerSeries = inputPlotStyle->dataSourcesPerSeries();
                unsigned sourceIndex                = 0;
                while (!valueChanged && sourceIndex < numberDataSourcesPerSeries) {
                    unsigned index = seriesIndex * numberDataSourcesPerSeries + sourceIndex;
                    valueChanged = (
                           dataSource(seriesIndex, sourceIndex) != inputDataSources.at(index)
                        || axisLocation(seriesIndex, sourceIndex) != inputAxisLocations.at(index)
                    );

                    ++sourceIndex;
                }

                ++seriesIndex;
            }
        }

        // Checking the data series also validates the axis locations.

        unsigned axisIndex = 0;
        while (!valueChanged && axisIndex < numberAxisLocations) {
            AxisLocation axis = currentAxisLocations.at(axisIndex);

            valueChanged = (
                   axisScale(axis) != inputAxisScaling.value(axis)
                || majorGridLineStyle(axis) != inputMajorGridLineStyles.value(axis)
                || majorGridLineWidth(axis) != inputMajorGridLineWidths.value(axis)
                || majorGridLineColor(axis) != inputMajorGridLineColors.value(axis)
                || minorGridLineStyle(axis) != inputMinorGridLineStyles.value(axis)
                || minorGridLineWidth(axis) != inputMinorGridLineWidths.value(axis)
                || minorGridLineColor(axis) != inputMajorGridLineColors.value(axis)
            );

            if (!valueChanged) {
                double minimumValue = PlotFormatDialog::minimumValue(axis);
                valueChanged = minimumValue != inputMinimumValues.value(axis) && !std::isnan(minimumValue);
            }

            if (!valueChanged) {
                double maximumValue = PlotFormatDialog::maximumValue(axis);
                valueChanged = maximumValue != inputMaximumValues.value(axis) && !std::isnan(maximumValue);
            }

            if (!valueChanged) {
                double majorStepSize = PlotFormatDialog::majorStepSize(axis);
                valueChanged = majorStepSize != inputMajorStepSizes.value(axis) && !std::isnan(majorStepSize);
            }

            if (!valueChanged) {
                double minorStepSize = PlotFormatDialog::minorStepSize(axis);
                valueChanged = minorStepSize != inputMinorStepSizes.value(axis) && !std::isnan(minorStepSize);
            }

            if (!valueChanged) {
                valueChanged = (
                       majorTickStyle(axis) != inputMajorTickStyles.value(axis)
                    || minorTickStyle(axis) != inputMinorTickStyles.value(axis)
                    || axisTitle(axis) != inputAxisTitles.value(axis)
                    || titleFont(axis) != inputAxisTitleFonts.value(axis)
                    || titleFontColor(axis) != inputAxisTitleFontColors.value(axis)
                    || titleFontBackgroundColor(axis) != inputAxisTitleFontBackgroundColors.value(axis)
                    || numberFont(axis) != inputAxisNumberFonts.value(axis)
                    || numberFontColor(axis) != inputAxisNumberFontColors.value(axis)
                    || numberFontBackgroundColor(axis) != inputAxisNumberFontBackgroundColors.value(axis)
                );
            }

            ++axisIndex;
        }
    } else {
        valueChanged = false;
    }

    return valueChanged;
}


void PlotFormatDialog::updateOkButton() {
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(!currentlyPopulatingDialog && valueChanged());
}



PlotAxisSettingsWidget* PlotFormatDialog::axisSettingsWidget(AxisLocation axisLocation) const {
    PlotAxisSettingsWidget* axisSettingsWidget;
    unsigned                tabIndex = currentTabsByAxisLocation.value(axisLocation, static_cast<unsigned>(-1));

    if (tabIndex != static_cast<unsigned>(-1)) {
        QTabWidget* tabWidget = widget<QTabWidget>("tab_widget");
        axisSettingsWidget = dynamic_cast<PlotAxisSettingsWidget*>(tabWidget->widget(tabIndex));
    } else {
        axisSettingsWidget = Q_NULLPTR;
    }

    return axisSettingsWidget;
}
