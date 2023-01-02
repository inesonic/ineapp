/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotWizard class.
***********************************************************************************************************************/

#include <QWidget>
#include <QWizard>
#include <QWizardPage>
#include <QString>
#include <QList>
#include <QStringList>
#include <QTimer>

#include <util_algorithm.h>

#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_variable_name.h>

#include "application.h"
#include "plot_engine.h"
#include "plot_style_widget.h"
#include "plot_data_sources_widget.h"
#include "plot_axis_settings_widget.h"
#include "plot_appearance_widget.h"
#include "plot_wizard.h"

const QSize PlotWizard::initialSizeInPixels(1000, 600);

PlotWizard::PlotWizard(QWidget* parent):QWizard(parent) {
    setWindowTitle(tr("Insert New Plot"));
    setOptions(ExtendedWatermarkPixmap | NoBackButtonOnStartPage | NoDefaultButton);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        setPixmap(QWizard::LogoPixmap, QPixmap(":application_icon_256.png"));
        setPixmap(QWizard::WatermarkPixmap, QPixmap(":first_time_use_wizard_watermark.png"));
        setPixmap(QWizard::BannerPixmap, QPixmap(":first_time_use_wizard_banner.png"));
        setWizardStyle(QWizard::WizardStyle::ModernStyle);

    #elif (defined(Q_OS_DARWIN))

        setPixmap(QWizard::BackgroundPixmap, QPixmap(":first_time_use_wizard_background.png"));
        setWizardStyle(QWizard::WizardStyle::MacStyle);

    #endif

    plotStyleWidget = new PlotStyleWidget(this);
    plotStyleWidget->setTitle(tr("Plot Style"));
    plotStyleWidget->setSubTitle(
        tr("You can use this page to specify the desired plot or chart style.")
    );

    plotDataSourcesWidget = new PlotDataSourcesWidget(this);
    plotDataSourcesWidget->setTitle(tr("Data Series"));
    plotDataSourcesWidget->setSubTitle(
        tr("You can use this page to configure each data series in the plot or chart.")
    );

    plotStyleWidgetId = addPage(plotStyleWidget);
    plotDataSourcesWidgetId = addPage(plotDataSourcesWidget);

    for (unsigned axisSettingsIndex=0 ; axisSettingsIndex<4 ; ++axisSettingsIndex) {
        PlotAxisSettingsWidget* plotAxisSettingsWidget = new PlotAxisSettingsWidget(this);
        plotAxisSettingsWidgets.append(plotAxisSettingsWidget);
        plotAxisLocations.append(static_cast<AxisLocation>(axisSettingsIndex));
        plotAxisSettingsWidgetIds.append(addPage(plotAxisSettingsWidget));
    }

    plotAppearanceWidget = new PlotAppearanceWidget(this);
    plotAppearanceWidget->setTitle(tr("Appearance"));
    plotAppearanceWidget->setSubTitle(
        tr("You can use this page to adjust the overall appearance of the plot or chart.")
    );

    plotAppearanceWidget->setPlotWidth(defaultPlotWidth);
    plotAppearanceWidget->setPlotHeight(defaultPlotHeight);

    plotAppearanceWidgetId = addPage(plotAppearanceWidget);

    setStartId(plotStyleWidgetId);
    plotStyleWidget->setNextId(plotDataSourcesWidgetId);
    plotDataSourcesWidget->setNextId(plotAxisSettingsWidgetIds.first());
    plotAppearanceWidget->setNextId(-1);

    QFont defaultFont = Ld::CharacterFormat::applicationDefaultTextFont()->toQFont();

    QFont defaultLargeFont = defaultFont;
    QFont defaultSmallFont = defaultFont;

    defaultLargeFont.setPointSize(defaultFont.pointSize() + 2);
    defaultSmallFont.setPointSize(defaultFont.pointSize() - 2);

    plotAppearanceWidget->setPlotWidth(4.0 * 72.0);
    plotAppearanceWidget->setPlotHeight(3.0 * 72.0);

    plotAppearanceWidget->setTitleFont(defaultLargeFont);
    plotAppearanceWidget->setTitleFontColor(QColor());
    plotAppearanceWidget->setTitleFontBackgroundColor(QColor());

    plotAppearanceWidget->setBackgroundColor(QColor());
    plotAppearanceWidget->setBorderLineColor(QColor(Qt::GlobalColor::black));
    plotAppearanceWidget->setBorderLineWidth(0);
    plotAppearanceWidget->setBorderLineStyle(LineStyle::SOLID_LINE);

    plotAppearanceWidget->setLegendLineColor(QColor(Qt::GlobalColor::black));
    plotAppearanceWidget->setLegendLineWidth(0);
    plotAppearanceWidget->setLegendLineStyle(LineStyle::NO_LINE);
    plotAppearanceWidget->setLegendFont(defaultSmallFont);
    plotAppearanceWidget->setLegendFontColor(QColor());
    plotAppearanceWidget->setLegendFontBackgroundColor(QColor());
    plotAppearanceWidget->setLegendLocation(LegendLocation::RIGHT);

    plotAppearanceWidget->setActiveAreaBorderLineStyle(LineStyle::SOLID_LINE);
    plotAppearanceWidget->setActiveAreaBorderLineWidth(0);
    plotAppearanceWidget->setActiveAreaBorderLineColor(QColor(Qt::GlobalColor::black));

    for (unsigned axisSettingsIndex=0 ; axisSettingsIndex<4 ; ++axisSettingsIndex) {
        PlotAxisSettingsWidget* axisSettingsWidget = plotAxisSettingsWidgets.at(axisSettingsIndex);

        axisSettingsWidget->setTitleFont(defaultFont);
        axisSettingsWidget->setTitleFontColor(QColor());
        axisSettingsWidget->setTitleFontBackgroundColor(QColor());

        axisSettingsWidget->setNumberFont(defaultSmallFont);
        axisSettingsWidget->setNumberFontColor(QColor());
        axisSettingsWidget->setNumberFontBackgroundColor(QColor());
    }

    connect(plotStyleWidget, &PlotStyleWidget::plotStyleChanged, this, &PlotWizard::plotStyleChanged);
    connect(plotDataSourcesWidget, &PlotDataSourcesWidget::changed, this, &PlotWizard::dataSeriesChanged);

    resize(initialSizeInPixels);
}


PlotWizard::~PlotWizard() {}


void PlotWizard::loadSettings(const QString& groupName) {
    plotAppearanceWidget->loadSettings(groupName);
}


void PlotWizard::saveSettings(const QString& groupName) {
    plotAppearanceWidget->saveSettings(groupName);
}


const PlotEngine* PlotWizard::plotStyle() const {
    return plotStyleWidget->plotStyle();
}


float PlotWizard::plotWidth() const {
    return plotAppearanceWidget->plotWidth();
}


float PlotWizard::plotHeight() const {
    return plotAppearanceWidget->plotHeight();
}


unsigned PlotWizard::dataSourcesPerDataSeries() const {
    return plotDataSourcesWidget->dataSourcesPerDataSeries();
}


unsigned PlotWizard::numberDataSeries() const {
    return plotDataSourcesWidget->numberDataSeries();
}


PlotWizard::AxisLocationStyle PlotWizard::axisLocationStyle() const {
    return plotDataSourcesWidget->axisLocationStyle();
}


PlotWizard::PlotSeriesMode PlotWizard::supportedPlotSeriesMode() const {
    return plotDataSourcesWidget->supportedPlotSeriesMode();
}


QList<Ld::VariableName> PlotWizard::dataSources() const {
    return plotDataSourcesWidget->dataSources();
}


Ld::VariableName PlotWizard::dataSource(unsigned seriesIndex, unsigned sourceIndex) const {
    return plotDataSourcesWidget->dataSource(seriesIndex, sourceIndex);
}


QList<PlotWizard::AxisLocation> PlotWizard::configuredAxisLocations() const {
    return plotAxisLocations;
}


PlotWizard::AxisLocation PlotWizard::axisLocation(unsigned seriesIndex, unsigned sourceIndex) const {
    return plotDataSourcesWidget->axisLocation(seriesIndex, sourceIndex);
}


QList<Ld::PlotSeries> PlotWizard::plotSeries() const {
    return plotDataSourcesWidget->plotSeries();
}


Ld::PlotSeries PlotWizard::plotSeries(unsigned seriesIndex) const {
    return plotDataSourcesWidget->plotSeries(seriesIndex);
}


const QStringList& PlotWizard::seriesLabels() const {
    return plotDataSourcesWidget->seriesLabels();
}


QString PlotWizard::seriesLabel(unsigned seriesIndex) const {
    return plotDataSourcesWidget->seriesLabel(seriesIndex);
}


QList<PlotWizard::AxisLocation> PlotWizard::axisLocations() const {
    return plotDataSourcesWidget->axisLocations();
}


PlotWizard::AxisLocation PlotWizard::baselineAxis() const {
    return plotDataSourcesWidget->baselineAxis();
}


bool PlotWizard::firstDataSeriesIsDataLabels() const {
    return plotDataSourcesWidget->firstDataSeriesIsLabels();
}


PlotWizard::AxisScale PlotWizard::axisScale(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->axisScale();
}


double PlotWizard::minimumValue(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->minimumValue();
}


double PlotWizard::maximumValue(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->maximumValue();
}


double PlotWizard::majorStepSize(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->majorStepSize();
}


double PlotWizard::minorStepSize(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->minorStepSize();
}


PlotWizard::TickStyle PlotWizard::majorTickStyle(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->majorTickStyle();
}


PlotWizard::TickStyle PlotWizard::minorTickStyle(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->minorTickStyle();
}


PlotWizard::LineStyle PlotWizard::majorGridLineStyle(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->majorGridLineStyle();
}


float PlotWizard::majorGridLineWidth(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->majorGridLineWidth();
}


QColor PlotWizard::majorGridLineColor(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->majorGridLineColor();
}


PlotWizard::LineStyle PlotWizard::minorGridLineStyle(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->minorGridLineStyle();
}


float PlotWizard::minorGridLineWidth(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->minorGridLineWidth();
}


QColor PlotWizard::minorGridLineColor(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->minorGridLineColor();
}


bool PlotWizard::axisTitleFieldVisible(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->axisTitleFieldVisible();
}


bool PlotWizard::axisTitleFieldHidden(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->axisTitleFieldHidden();
}


QString PlotWizard::axisTitle(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->axisTitle();
}


QFont PlotWizard::titleFont(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->titleFont();
}


QColor PlotWizard::titleFontColor(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->titleFontColor();
}


QColor PlotWizard::titleFontBackgroundColor(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->titleFontBackgroundColor();
}


QFont PlotWizard::numberFont(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->numberFont();
}


QColor PlotWizard::numberFontColor(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->numberFontColor();
}


QColor PlotWizard::numberFontBackgroundColor(PlotWizard::AxisLocation axisLocation) const {
    return plotAxisSettingsWidget(axisLocation)->numberFontBackgroundColor();
}


PlotWizard::LineStyle PlotWizard::borderLineStyle() const {
    return plotAppearanceWidget->borderLineStyle();
}


float PlotWizard::borderLineWidth() const {
    return plotAppearanceWidget->borderLineWidth();
}


QColor PlotWizard::borderLineColor() const {
    return plotAppearanceWidget->borderLineColor();
}


QColor PlotWizard::backgroundColor() const {
    return plotAppearanceWidget->backgroundColor();
}


PlotWizard::LineStyle PlotWizard::activeAreaBorderLineStyle() const {
    return plotAppearanceWidget->activeAreaBorderLineStyle();
}


float PlotWizard::activeAreaBorderLineWidth() const {
    return plotAppearanceWidget->activeAreaBorderLineWidth();
}


QColor PlotWizard::activeAreaBorderLineColor() const {
    return plotAppearanceWidget->activeAreaBorderLineColor();
}


QColor PlotWizard::activeAreaColor() const {
    return plotAppearanceWidget->activeAreaColor();
}


PlotWizard::LegendLocation PlotWizard::legendLocation() const {
    return plotAppearanceWidget->legendLocation();
}


PlotWizard::LineStyle PlotWizard::legendLineStyle() const {
    return plotAppearanceWidget->legendLineStyle();
}


float PlotWizard::legendLineWidth() const {
    return plotAppearanceWidget->legendLineWidth();
}


QColor PlotWizard::legendLineColor() const {
    return plotAppearanceWidget->legendLineColor();
}


QColor PlotWizard::legendBackgroundColor() const {
    return plotAppearanceWidget->legendBackgroundColor();
}


QFont PlotWizard::legendFont() const {
    return plotAppearanceWidget->legendFont();
}


QColor PlotWizard::legendFontColor() const {
    return plotAppearanceWidget->legendFontColor();
}


QColor PlotWizard::legendFontBackgroundColor() const {
    return plotAppearanceWidget->legendFontBackgroundColor();
}


QString PlotWizard::titleText() const {
    return plotAppearanceWidget->titleText();
}


QFont PlotWizard::titleFont() const {
    return plotAppearanceWidget->titleFont();
}


QColor PlotWizard::titleFontColor() const {
    return plotAppearanceWidget->titleFontColor();
}


QColor PlotWizard::titleFontBackgroundColor() const {
    return plotAppearanceWidget->titleFontBackgroundColor();
}


void PlotWizard::plotStyleChanged(const PlotEngine* newPlotStyle) {
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
        plotAppearanceWidget->setLegendControlsEnabled(newPlotStyle->supportsPlotLegend());
        plotAppearanceWidget->setActiveAreaControlsEnabled(newPlotStyle->hasDistinctActiveArea());
        plotAppearanceWidget->setPlotTitleEnabled(newPlotStyle->supportsPlotTitle());
        plotAppearanceWidget->setBackgroundColorButtonEnabled(newPlotStyle->supportsControllableBackgroundArea());
    }

    dataSeriesChanged();
}


void PlotWizard::dataSeriesChanged() {
    const PlotEngine*  plotEngine        = PlotWizard::plotStyle();
    AxisLocationStyle  axisLocationStyle = PlotWizard::axisLocationStyle();
    QSet<AxisLocation> referencedAxisLocations;

    if (axisLocationStyle == AxisLocationStyle::UNCATEGORIZED         ||
        axisLocationStyle == AxisLocationStyle::RGB_COLOR_SPACE       ||
        axisLocationStyle == AxisLocationStyle::CMYK_COLOR_SPACE      ||
        axisLocationStyle == AxisLocationStyle::GRAYSCALE_COLOR_SPACE    ) {
        plotDataSourcesWidget->setNextId(plotAppearanceWidgetId);
    } else {
        if (axisLocationStyle == AxisLocationStyle::CATEGORIZED) {
            // This path is used for plots such as bar charts that have more constrained axis settings.

            AxisLocation baselineAxis = PlotWizard::baselineAxis();
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
        } else {
            // This is the path for traditional plots.

            QList<AxisLocation> axisLocationsList = axisLocations();
            referencedAxisLocations = QSet<AxisLocation>(axisLocationsList.begin(), axisLocationsList.end());
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
        unsigned pageIndex          = 0;

        plotAxisLocations.clear();
        for (unsigned axisIndex=0 ; axisIndex<numberPossibleAxis ; ++axisIndex) {
            AxisLocation axisLocation = allAxisLocations.at(axisIndex);
            if (referencedAxisLocations.contains(axisLocation)) {
                PlotAxisSettingsWidget* plotAxisSettingsWidget   = plotAxisSettingsWidgets.at(pageIndex);

                const QString& axisName = allAxisNames.at(axisIndex);
                plotAxisSettingsWidget->setTitle(tr("%1 Settings").arg(axisName));
                plotAxisSettingsWidget->setSubTitle(
                    tr("You can use this page to set the %1 settings.").arg(axisName.toLower())
                );
                plotAxisSettingsWidget->setSupportedAxisScales(plotEngine->supportedAxisScales(axisLocation));
                plotAxisSettingsWidget->setScalingControlsMode(plotEngine->supportedScalingControlsMode());

                plotAxisLocations.append(axisLocation);

                ++pageIndex;
            }
        }

        plotDataSourcesWidget->setNextId(plotAxisSettingsWidgetIds.first());
        unsigned numberPageIndex = pageIndex;
        if (numberPageIndex > 0) {
            unsigned lastPageIndex = numberPageIndex - 1;
            for (pageIndex=0 ; pageIndex<lastPageIndex ; ++pageIndex) {
                plotAxisSettingsWidgets[pageIndex]->setNextId(plotAxisSettingsWidgetIds.at(pageIndex + 1));
            }

            plotAxisSettingsWidgets[lastPageIndex]->setNextId(plotAppearanceWidgetId);
        }
    }
}


PlotAxisSettingsWidget* PlotWizard::plotAxisSettingsWidget(PlotWizard::AxisLocation axisLocation) const {
    int index = plotAxisLocations.indexOf(axisLocation);
    return index >= 0 ? plotAxisSettingsWidgets.at(index) : Q_NULLPTR;
}
