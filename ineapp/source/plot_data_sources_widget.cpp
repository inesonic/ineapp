/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotDataSourcesWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QWizardPage>
#include <QString>
#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QModelIndex>
#include <QModelIndexList>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QPushButton>
#include <QToolButton>

#include <algorithm>

#include <ld_variable_name.h>
#include <ld_chart_axis_format.h>
#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_plot_format.h>

#include "application.h"
#include "metatypes.h"
#include "variable_name_editor_widget.h"
#include "plot_series_dialog.h"
#include "plot_data_sources_model.h"
#include "plot_data_sources_delegate.h"
#include "plot_widget_base.h"
#include "plot_data_sources_widget.h"

PlotDataSourcesWidget::PlotDataSourcesWidget(QWidget* parent):PlotWidgetBase(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    firstSeriesIsLabelsCheckBox = new QCheckBox(tr("First Series Holds Data Labels"));
    mainLayout->addWidget(firstSeriesIsLabelsCheckBox);

    barsBaselineComboBox = new QComboBox;
    mainLayout->addWidget(barsBaselineComboBox);

    barsBaselineComboBox->setEditable(false);
    barsBaselineComboBox->addItem(
        tr("Bars shown from bottom, scale on left"),
        QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM)
    );
    barsBaselineComboBox->addItem(
        tr("Bars shown from left, scale on bottom"),
        QVariant::fromValue(AxisLocation::LEFT_Y_R_RC)
    );

    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    mainLayout->addLayout(horizontalLayout);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QGroupBox* tableGroupBox = new QGroupBox;
        horizontalLayout->addWidget(tableGroupBox);

        QHBoxLayout* tableLayout = new QHBoxLayout;
        tableGroupBox->setLayout(tableLayout);

        dataSeriesTableView = new QTableView;

        tableLayout->addWidget(dataSeriesTableView);

        QVBoxLayout* moveUpDownLayout = new QVBoxLayout;
        tableLayout->addLayout(moveUpDownLayout);

        moveUpButton = new QPushButton(Application::icon("move_up"), QString(""));
        moveUpDownLayout->addWidget(moveUpButton);

        moveUpDownLayout->addStretch(1);

        moveDownButton = new QPushButton(Application::icon("move_down"), QString(""));
        moveUpDownLayout->addWidget(moveDownButton);

        horizontalLayout->addSpacing(10);

        QVBoxLayout* addRemoveButtonLayout = new QVBoxLayout;
        horizontalLayout->addLayout(addRemoveButtonLayout);

        insertButton = new QPushButton(Application::icon("add_entry"), tr("Add Series"));
        addRemoveButtonLayout->addWidget(insertButton);

        removeButton = new QPushButton(Application::icon("remove_entry"), tr("Remove Series"));
        addRemoveButtonLayout->addWidget(removeButton);

        addRemoveButtonLayout->addStretch(1);

    #elif (defined(Q_OS_DARWIN))

        horizontalLayout->setSpacing(0);

        #if (QT_VERSION < 0x060000)

            horizontalLayout->setMargin(0);

        #else

            horizontalLayout->setContentsMargins(0, 0, 0, 0);

        #endif

        dataSeriesTableView = new QTableView;
        horizontalLayout->addWidget(dataSeriesTableView);

        QHBoxLayout* addRemoveButtonLayout = new QHBoxLayout;
        mainLayout->addLayout(addRemoveButtonLayout);

        addRemoveButtonLayout->setSpacing(0);

        #if (QT_VERSION < 0x060000)

            addRemoveButtonLayout->setMargin(0);

        #else

            addRemoveButtonLayout->setContentsMargins(0, 0, 0, 0);

        #endif

        insertButton = new QPushButton(Application::icon("add_entry"), QString(""));
        addRemoveButtonLayout->addWidget(insertButton);

        removeButton = new QPushButton(Application::icon("remove_entry"), QString(""));
        addRemoveButtonLayout->addWidget(removeButton);

        addRemoveButtonLayout->addStretch(1);

        QVBoxLayout* moveUpDownLayout = new QVBoxLayout;
        horizontalLayout->addLayout(moveUpDownLayout);

        moveUpButton = new QPushButton(Application::icon("move_up"), QString(""));
        moveUpDownLayout->addWidget(moveUpButton);

        moveUpDownLayout->addStretch(1);

        moveDownButton = new QPushButton(Application::icon("move_down"), QString(""));
        moveUpDownLayout->addWidget(moveDownButton);

    #else

        #error Unknown platform

    #endif

    dataSeriesTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

    dataSeriesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    dataSeriesTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignRight);
    dataSeriesTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    PlotDataSourcesModel* model = new PlotDataSourcesModel(dataSeriesTableView);
    dataSeriesTableView->setModel(model);

    QAbstractItemDelegate* oldDelegate = dataSeriesTableView->itemDelegate();
    dataSeriesTableView->setItemDelegate(new PlotDataSourcesDelegate(this));
    delete oldDelegate;

    connect(firstSeriesIsLabelsCheckBox, &QCheckBox::clicked, model, &PlotDataSourcesModel::setFirstRowIsBarLabels);
    connect(
        firstSeriesIsLabelsCheckBox,
        &QCheckBox::clicked,
        this,
        &PlotDataSourcesWidget::firstRowIsBarLabelsButtonClicked
    );

    connect(
        barsBaselineComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotDataSourcesWidget::baselineAxisChanged
    );
    connect(dataSeriesTableView, &QTableView::doubleClicked, this, &PlotDataSourcesWidget::cellDoubleClicked);

    QItemSelectionModel* selectionModel = dataSeriesTableView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &PlotDataSourcesWidget::selectionChanged);
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &PlotDataSourcesWidget::currentChanged);

    connect(insertButton, &QPushButton::clicked, this, &PlotDataSourcesWidget::insertButtonClicked);
    connect(removeButton, &QPushButton::clicked, this, &PlotDataSourcesWidget::removeButtonClicked);
    connect(moveUpButton, &QPushButton::clicked, this, &PlotDataSourcesWidget::moveUpButtonClicked);
    connect(moveDownButton, &QPushButton::clicked, this, &PlotDataSourcesWidget::moveDownButtonClicked);

    insertButton->setEnabled(true);
    removeButton->setEnabled(false);
    moveUpButton->setEnabled(false);
    moveDownButton->setEnabled(false);
}


PlotDataSourcesWidget::~PlotDataSourcesWidget() {}


bool PlotDataSourcesWidget::isValid() const {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->isValid();
}


unsigned PlotDataSourcesWidget::dataSourcesPerDataSeries() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->dataSourcesPerDataSeries();
}


unsigned PlotDataSourcesWidget::numberDataSeries() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->numberDataSeries();
}


PlotDataSourcesWidget::AxisLocationStyle PlotDataSourcesWidget::axisLocationStyle() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->axisLocationStyle();
}


PlotDataSourcesWidget::PlotSeriesMode PlotDataSourcesWidget::supportedPlotSeriesMode() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->supportedPlotSeriesMode();
}


bool PlotDataSourcesWidget::firstDataSeriesIsLabels() const {
    return firstSeriesIsLabelsCheckBox->isEnabled() && firstSeriesIsLabelsCheckBox->isChecked();
}


bool PlotDataSourcesWidget::firstDataSeriesIsValue() const {
    return !firstDataSeriesIsLabels();
}


PlotDataSourcesWidget::AxisLocation PlotDataSourcesWidget::baselineAxis() const {
    return barsBaselineComboBox->currentData().value<AxisLocation>();
}


QSet<PlotDataSourcesWidget::SplineType> PlotDataSourcesWidget::supportedSplineTypes() const {
    return currentSupportedSplineTypes;
}


QList<Ld::VariableName> PlotDataSourcesWidget::dataSources() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->dataSources();
}


Ld::VariableName PlotDataSourcesWidget::dataSource(unsigned seriesIndex, unsigned sourceIndex) const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->dataSource(seriesIndex, sourceIndex);
}


QList<PlotDataSourcesWidget::AxisLocation> PlotDataSourcesWidget::axisLocations() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->axisLocations();
}


PlotDataSourcesWidget::AxisLocation PlotDataSourcesWidget::axisLocation(
        unsigned seriesIndex,
        unsigned sourceIndex
    ) const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->axisLocation(seriesIndex, sourceIndex);
}


QList<Ld::PlotSeries> PlotDataSourcesWidget::plotSeries() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->plotSeries();
}


const Ld::PlotSeries& PlotDataSourcesWidget::plotSeries(unsigned seriesIndex) const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->plotSeries(seriesIndex);
}


const QStringList& PlotDataSourcesWidget::seriesLabels() const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->seriesLabels();
}


QString PlotDataSourcesWidget::seriesLabel(unsigned seriesIndex) const {
    const PlotDataSourcesModel* model = dynamic_cast<const PlotDataSourcesModel*>(dataSeriesTableView->model());
    return model->seriesLabel(seriesIndex);
}


void PlotDataSourcesWidget::setDataSourcesPerDataSeries(unsigned newNumberDataSourcesPerDataSeries) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setDataSourcesPerDataSeries(newNumberDataSourcesPerDataSeries);
}


void PlotDataSourcesWidget::setNumberAllowedDataSeries(
        unsigned minimumNumberDataSeries,
        unsigned maximumNumberDataSeries
    ) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setNumberAllowedDataSeries(minimumNumberDataSeries, maximumNumberDataSeries);

    if (model->axisLocationStyle() == AxisLocationStyle::CATEGORIZED   ||
        model->axisLocationStyle() == AxisLocationStyle::UNCATEGORIZED ||
        model->axisLocationStyle() == AxisLocationStyle::FIXED            ) {
        if (maximumNumberDataSeries > 1) {
            firstSeriesIsLabelsCheckBox->setVisible(true);
            firstSeriesIsLabelsCheckBox->setEnabled(true);
        } else {
            firstSeriesIsLabelsCheckBox->setVisible(false);
            firstSeriesIsLabelsCheckBox->setEnabled(false);
        }
    }
}


void PlotDataSourcesWidget::setAxisLocationStyle(AxisLocationStyle newAxisLocationStyle) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());

    if (newAxisLocationStyle == AxisLocationStyle::CATEGORIZED) {
        if (model->maximumNumberDataSeries() > 1) {
            firstSeriesIsLabelsCheckBox->setVisible(true);
            firstSeriesIsLabelsCheckBox->setEnabled(true);
        } else {
            firstSeriesIsLabelsCheckBox->setVisible(false);
            firstSeriesIsLabelsCheckBox->setEnabled(false);
        }

        barsBaselineComboBox->setVisible(true);
    } else {
        firstSeriesIsLabelsCheckBox->setVisible(false);
        firstSeriesIsLabelsCheckBox->setEnabled(false);
        barsBaselineComboBox->setVisible(false);

        model->setFirstRowIsDataSeries();
    }

    AxisLocation baselineAxis = PlotDataSourcesWidget::baselineAxis();
    updateAllowedAxisLocations(newAxisLocationStyle, baselineAxis);

    return model->setAxisLocationStyle(newAxisLocationStyle);
}


void PlotDataSourcesWidget::setFirstSeriesIsDataLabels(bool firstSeriesIsDataLabels) {
    firstSeriesIsLabelsCheckBox->setChecked(firstSeriesIsDataLabels);

    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setFirstRowIsBarLabels(firstSeriesIsDataLabels);
}


void PlotDataSourcesWidget::setFirstSeriesIsValue(bool firstSeriesIsValue) {
    setFirstSeriesIsDataLabels(!firstSeriesIsValue);
}


void PlotDataSourcesWidget::setBaselineAxis(PlotDataSourcesWidget::AxisLocation newBaselineAxis) {
    unsigned numberItems = barsBaselineComboBox->count();
    unsigned index       = 0;

    while (index < numberItems && barsBaselineComboBox->itemData(index).value<AxisLocation>() != newBaselineAxis) {
        ++index;
    }

    if (index < numberItems) {
        barsBaselineComboBox->setCurrentIndex(index);
    } else {
        barsBaselineComboBox->setCurrentIndex(0);
    }
}


void PlotDataSourcesWidget::setSupportedPlotSeriesMode(
        PlotDataSourcesWidget::PlotSeriesMode newSupportedPlotSeriesMode
    ) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setSupportedPlotSeriesMode(newSupportedPlotSeriesMode);
}


void PlotDataSourcesWidget::setSupportedSplineTypes(const QSet<SplineType>& supportedSplineTypes) {
    currentSupportedSplineTypes = supportedSplineTypes;
}


void PlotDataSourcesWidget::setDataSources(
        const QList<Ld::VariableName>& newDataSources,
        const QList<AxisLocation>&     newAxisLocations,
        const QList<Ld::PlotSeries>&   newPlotSeries,
        const QStringList&             newPlotSeriesLabels
    ) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setDataSources(newDataSources, newAxisLocations, newPlotSeries, newPlotSeriesLabels);
}


void PlotDataSourcesWidget::setDataSource(
        unsigned                seriesIndex,
        unsigned                sourceIndex,
        const Ld::VariableName& newDataSource
    ) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setDataSource(seriesIndex, sourceIndex, newDataSource);
}


void PlotDataSourcesWidget::setAxisLocation(unsigned seriesIndex, unsigned sourceIndex, AxisLocation newAxisLocation) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setAxisLocation(seriesIndex, sourceIndex, newAxisLocation);
}


void PlotDataSourcesWidget::setPlotSeries(unsigned index, const Ld::PlotSeries& newPlotSeries) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setPlotSeries(index, newPlotSeries);
}


void PlotDataSourcesWidget::setPlotSeriesLabel(unsigned seriesIndex, const QString& plotSeriesLabel) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    model->setPlotSeriesLabel(seriesIndex, plotSeriesLabel);
}


void PlotDataSourcesWidget::firstRowIsBarLabelsButtonClicked(bool /* nowChecked */) {
     updateChangedStatus();
}


void PlotDataSourcesWidget::baselineAxisChanged(int index) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    if (model->axisLocationStyle() == AxisLocationStyle::CATEGORIZED) {
        AxisLocation      baselineAxis             = barsBaselineComboBox->itemData(index).value<AxisLocation>();
        AxisLocationStyle currentAxisLocationStyle = axisLocationStyle();

        updateAllowedAxisLocations(currentAxisLocationStyle, baselineAxis);
    }

    updateChangedStatus();
}


void PlotDataSourcesWidget::cellDoubleClicked(const QModelIndex& /* index */) {
    QItemSelectionModel* selectionModel = dataSeriesTableView->selectionModel();
    if (selectionModel->hasSelection()) {
        unsigned              row   = static_cast<unsigned>(selectionModel->currentIndex().row());
        PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());

        PlotSeriesDialog dialog(this);

        unsigned          numberDataSources = model->dataSourcesPerDataSeries();
        AxisLocationStyle axisLocationStyle = PlotDataSourcesWidget::axisLocationStyle();

        dialog.setAxisLocationStyle(axisLocationStyle);
        dialog.setSupportedSplineTypes(currentSupportedSplineTypes);

        dialog.setNumberDataSources(numberDataSources);
        for (unsigned sourceIndex=0 ; sourceIndex<numberDataSources ; ++sourceIndex) {
            dialog.setSourceVariableName(sourceIndex, model->dataSource(row, sourceIndex));
            dialog.setSourceAxisLocation(sourceIndex, model->axisLocation(row, sourceIndex));
        }

        if (axisLocationStyle == AxisLocationStyle::CATEGORIZED   ||
            axisLocationStyle == AxisLocationStyle::UNCATEGORIZED ||
            axisLocationStyle == AxisLocationStyle::FIXED            ) {
            dialog.setAxisLocationControlsDisabled();
            dialog.setSeriesLabelFieldEnabled();
            dialog.setSeriesVisibleAttributesEnabled();

            bool isNotLabelSeries = (model->firstRowIsDataSeries() || row != 0);
            dialog.setSeriesVisibleAttributesEnabled(isNotLabelSeries);
            dialog.setSeriesLabelFieldEnabled(isNotLabelSeries);
        } else if (axisLocationStyle == AxisLocationStyle::RGB_COLOR_SPACE       ||
                   axisLocationStyle == AxisLocationStyle::CMYK_COLOR_SPACE      ||
                   axisLocationStyle == AxisLocationStyle::GRAYSCALE_COLOR_SPACE    ) {
            dialog.setSeriesLabelFieldDisabled();
            dialog.setSeriesVisibleAttributesDisabled();
        } else {
            dialog.setAxisLocationControlsEnabled(row != 0 || model->firstRowIsDataSeries());
            dialog.setAllowedAxisLocations(QSet<AxisLocation>());
            dialog.setSeriesLabelFieldEnabled();
            dialog.setSeriesVisibleAttributesEnabled();
        }

        dialog.setPlotSeriesMode(model->supportedPlotSeriesMode());

        const Ld::PlotSeries& plotSeries      = model->plotSeries(row);
        QString               plotSeriesLabel = model->seriesLabel(row);

        dialog.setSeriesLabel(plotSeriesLabel);
        dialog.setLineStyle(plotSeries.lineStyle());
        dialog.setLineWidth(plotSeries.lineWidth());
        dialog.setLineColor(plotSeries.lineColor());
        dialog.setMarkerStyle(plotSeries.markerStyle());
        dialog.setSplineType(plotSeries.splineType());
        dialog.setGradientType(plotSeries.gradientType());

        int result = dialog.exec();
        if (result == PlotSeriesDialog::DialogCode::Accepted) {
            for (unsigned sourceIndex=0 ; sourceIndex<numberDataSources ; ++sourceIndex) {
                model->setDataSource(row, sourceIndex, dialog.sourceVariableName(sourceIndex));
                model->setAxisLocation(row, sourceIndex, dialog.sourceAxisLocation(sourceIndex));
            }

            Ld::PlotSeries newPlotSeries = plotSeries;

            newPlotSeries.setLineStyle(dialog.lineStyle());
            newPlotSeries.setLineWidth(dialog.lineWidth());
            newPlotSeries.setLineColor(dialog.lineColor());
            newPlotSeries.setMarkerStyle(dialog.markerStyle());
            newPlotSeries.setSplineType(dialog.splineType());
            newPlotSeries.setGradientType(dialog.gradientType());

            model->setPlotSeriesLabel(row, dialog.label());
            model->setPlotSeries(row, newPlotSeries);

            updateChangedStatus();
        }
    }
}


void PlotDataSourcesWidget::insertButtonClicked() {
    QItemSelectionModel* selectionModel = dataSeriesTableView->selectionModel();
    if (selectionModel->hasSelection()) {
        unsigned              row               = static_cast<unsigned>(selectionModel->currentIndex().row());
        PlotDataSourcesModel* model             = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
        AxisLocationStyle     axisLocationStyle = model->axisLocationStyle();

        if (axisLocationStyle == AxisLocationStyle::CATEGORIZED && firstDataSeriesIsLabels() && row == 0) {
            row = 1;
        }

        PlotSeriesDialog dialog(this);

        unsigned            numberDataSources = model->dataSourcesPerDataSeries();
        PlotSeriesMode      plotSeriesMode    = model->supportedPlotSeriesMode();
        QList<AxisLocation> axisLocations     = PlotDataSourcesModel::defaultAxisLocationsForStyle(axisLocationStyle);

        Q_ASSERT(static_cast<unsigned>(axisLocations.size()) >= numberDataSources);

        dialog.setSupportedSplineTypes(currentSupportedSplineTypes);
        dialog.setPlotSeriesMode(plotSeriesMode);
        dialog.setAxisLocationStyle(axisLocationStyle);
        dialog.setNumberDataSources(numberDataSources);

        if (axisLocationStyle == AxisLocationStyle::CATEGORIZED   ||
            axisLocationStyle == AxisLocationStyle::UNCATEGORIZED ||
            axisLocationStyle == AxisLocationStyle::FIXED            ) {
            dialog.setAxisLocationControlsDisabled();
        } else {
            dialog.setAxisLocationControlsEnabled(row != 0 || model->firstRowIsDataSeries());
            dialog.setAllowedAxisLocations(QSet<AxisLocation>());
        }

        for (unsigned sourceIndex=0 ; sourceIndex<numberDataSources ; ++sourceIndex) {
            dialog.setSourceAxisLocation(sourceIndex, axisLocations.at(sourceIndex));
            dialog.setSourceVariableName(sourceIndex, Ld::VariableName());
        }

        unsigned              defaultPlotSeriesIndex = model->rowCount() % Ld::PlotSeries::numberDefaultPlotSeries;
        const Ld::PlotSeries& defaultPlotSeries      = Ld::PlotSeries::defaultPlotSeries[defaultPlotSeriesIndex];

        dialog.setLineStyle(defaultPlotSeries.lineStyle());
        dialog.setLineWidth(defaultPlotSeries.lineWidth());
        dialog.setLineColor(defaultPlotSeries.lineColor());
        dialog.setMarkerStyle(defaultPlotSeries.markerStyle());
        dialog.setSplineType(defaultPlotSeries.splineType());
        dialog.setGradientType(defaultPlotSeries.gradientType());

        if (model->firstRowIsDataSeries()) {
            dialog.setSeriesLabel(tr("Series %1").arg(row + 1));
        } else {
            dialog.setSeriesLabel(tr("Series %1").arg(row));
        }

        int result = dialog.exec();
        if (result == PlotSeriesDialog::DialogCode::Accepted) {
            model->insertRow(row);

            for (unsigned sourceIndex=0 ; sourceIndex<numberDataSources ; ++sourceIndex) {
                model->setDataSource(row, sourceIndex, dialog.sourceVariableName(sourceIndex));
                model->setAxisLocation(row, sourceIndex, dialog.sourceAxisLocation(sourceIndex));
            }

            Ld::PlotSeries newPlotSeries;

            newPlotSeries.setLineStyle(dialog.lineStyle());
            newPlotSeries.setLineWidth(dialog.lineWidth());
            newPlotSeries.setLineColor(dialog.lineColor());
            newPlotSeries.setMarkerStyle(dialog.markerStyle());
            newPlotSeries.setSplineType(dialog.splineType());
            newPlotSeries.setGradientType(dialog.gradientType());

            model->setPlotSeriesLabel(row, dialog.label());
            model->setPlotSeries(row, newPlotSeries);

            updateChangedStatus();
        }
    }
}


void PlotDataSourcesWidget::removeButtonClicked() {
    QList<unsigned>       sortedRowIndexes = selectedRows();
    PlotDataSourcesModel* model            = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());

    for (  QList<unsigned>::const_iterator rowIterator    = sortedRowIndexes.constBegin(),
                                           rowEndIterator = sortedRowIndexes.constEnd()
         ; rowIterator != rowEndIterator
         ; ++rowIterator
        ) {
        model->removeRow(*rowIterator);
    }

    updateChangedStatus();
}


void PlotDataSourcesWidget::moveUpButtonClicked() {
    QList<unsigned> sortedRowIndexes = selectedRows();

    if (!sortedRowIndexes.isEmpty()) {
        PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
        model->moveRowsUp(sortedRowIndexes);
    }

    updateButtonStates();
    updateChangedStatus();
}


void PlotDataSourcesWidget::moveDownButtonClicked() {
    QList<unsigned> sortedRowIndexes = selectedRows();

    if (!sortedRowIndexes.isEmpty()) {
        PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
        model->moveRowsDown(sortedRowIndexes);
    }

    updateButtonStates();
    updateChangedStatus();
}


void PlotDataSourcesWidget::currentChanged(const QModelIndex& /* current */, const QModelIndex& /* previous */) {
    updateButtonStates();
}


void PlotDataSourcesWidget::selectionChanged(
        const QItemSelection& /* selected */,
        const QItemSelection& /* deselected */
    ) {
    updateButtonStates();
}


void PlotDataSourcesWidget::updateButtonStates() {
    QModelIndexList             indexes = dataSeriesTableView->selectionModel()->selection().indexes();
    QMap<unsigned, char>        rows;

    for (QModelIndexList::const_iterator it=indexes.constBegin(),end=indexes.constEnd() ; it!=end ; ++it) {
        rows.insert(it->row(), 1);
    }

    const PlotDataSourcesModel* model      = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());
    unsigned long               numberRows = static_cast<unsigned long>(model->rowCount());

    AxisLocationStyle axisLocationStyle = PlotDataSourcesWidget::axisLocationStyle();
    if ((rows.isEmpty() || numberRows <= 1)                       ||
        (axisLocationStyle == AxisLocationStyle::CATEGORIZED &&
         firstDataSeriesIsLabels()                           &&
         (numberRows <= 2 || rows.contains(0))                  )    ) {
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
        removeButton->setEnabled(false);
    } else if (numberRows == static_cast<unsigned long>(rows.size())) {
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
        removeButton->setEnabled(false);
    } else {
        moveUpButton->setEnabled(!rows.contains(0));
        moveDownButton->setEnabled(!rows.contains(numberRows - 1));
        removeButton->setEnabled(model->numberDataSeries() > model->minimumNumberDataSeries());
    }

    insertButton->setEnabled(rows.size() == 1 && model->numberDataSeries() < model->maximumNumberDataSeries());
}


QList<unsigned> PlotDataSourcesWidget::selectedRows() const {
    QModelIndexList indexes = dataSeriesTableView->selectionModel()->selection().indexes();

    QSet<unsigned> rowIndexes;
    for (QModelIndexList::const_iterator it=indexes.constBegin(),end=indexes.constEnd() ; it!=end ; ++it) {
        rowIndexes << it->row();
    }

    QList<unsigned> sortedRowIndexes(rowIndexes.constBegin(), rowIndexes.constEnd());
    std::sort(
        sortedRowIndexes.begin(),
        sortedRowIndexes.end(),
        [](unsigned a, unsigned b)->bool{ return a >= b; }
    );

    return sortedRowIndexes;
}


void PlotDataSourcesWidget::updateAllowedAxisLocations(
        PlotDataSourcesWidget::AxisLocationStyle axisLocationStyle,
        PlotDataSourcesWidget::AxisLocation      baselineAxis
    ) {
    PlotDataSourcesModel* model = dynamic_cast<PlotDataSourcesModel*>(dataSeriesTableView->model());

    switch (axisLocationStyle) {
        case AxisLocationStyle::PLOT_BORDERS: {
            QSet<AxisLocation> allowedLocations;
            allowedLocations.insert(AxisLocation::BOTTOM_X_A_GM);
            allowedLocations.insert(AxisLocation::LEFT_Y_R_RC);
            allowedLocations.insert(AxisLocation::RIGHT_Y_R_RY);
            allowedLocations.insert(AxisLocation::TOP_X_A_GM);

            for (unsigned sourceIndex=0 ; sourceIndex<4 ; ++sourceIndex) {
                model->setAllowedAxisLocations(sourceIndex, allowedLocations);
            }

            break;
        }

        case AxisLocationStyle::CATEGORIZED: {
            QSet<AxisLocation> allowedLocations;
            if (baselineAxis == AxisLocation::BOTTOM_X_A_GM || baselineAxis == AxisLocation::TOP_X_A_GM) {
                allowedLocations.insert(AxisLocation::LEFT_Y_R_RC);
                allowedLocations.insert(AxisLocation::RIGHT_Y_R_RY);
            } else {
                allowedLocations.insert(AxisLocation::BOTTOM_X_A_GM);
                allowedLocations.insert(AxisLocation::TOP_X_A_GM);
            }

            model->setAllowedAxisLocations(0, allowedLocations);

            break;
        }

        case AxisLocationStyle::UNCATEGORIZED:
        case AxisLocationStyle::FIXED: {
            // We stuff a value here so things don't break.  What we use doesn't really matter.
            QSet<AxisLocation> allowedLocations;
            allowedLocations.insert(AxisLocation::BOTTOM_X_A_GM);
            model->setAllowedAxisLocations(0, allowedLocations);

            break;
        }

        case AxisLocationStyle::CARTESIAN_2D: {
            QSet<AxisLocation> source0;
            source0.insert(AxisLocation::LEFT_Y_R_RC);
            source0.insert(AxisLocation::RIGHT_Y_R_RY);

            QSet<AxisLocation> source1;
            source1.insert(AxisLocation::BOTTOM_X_A_GM);
            source1.insert(AxisLocation::TOP_X_A_GM);

            model->setAllowedAxisLocations(0, source0);
            model->setAllowedAxisLocations(1, source1);

            break;
        }

        case AxisLocationStyle::POLAR_2D: {
            QSet<AxisLocation> source0;
            source0.insert(AxisLocation::BOTTOM_X_A_GM);
            source0.insert(AxisLocation::TOP_X_A_GM);

            QSet<AxisLocation> source1;
            source1.insert(AxisLocation::LEFT_Y_R_RC);
            source1.insert(AxisLocation::RIGHT_Y_R_RY);

            model->setAllowedAxisLocations(0, source0);
            model->setAllowedAxisLocations(1, source1);

            break;
        }

        case AxisLocationStyle::CARTESIAN_3D: {
            QSet<AxisLocation> source0;
            source0.insert(AxisLocation::BOTTOM_X_A_GM);
            source0.insert(AxisLocation::TOP_X_A_GM);

            QSet<AxisLocation> source1;
            source1.insert(AxisLocation::LEFT_Y_R_RC);
            source1.insert(AxisLocation::RIGHT_Y_R_RY);

            QSet<AxisLocation> source2;
            source2.insert(AxisLocation::Z_B_BK);

            model->setAllowedAxisLocations(0, source0);
            model->setAllowedAxisLocations(1, source1);
            model->setAllowedAxisLocations(2, source2);

            break;
        }

        case AxisLocationStyle::POLAR_3D: {
            QSet<AxisLocation> source0;
            source0.insert(AxisLocation::BOTTOM_X_A_GM);
            source0.insert(AxisLocation::TOP_X_A_GM);

            QSet<AxisLocation> source1;
            source1.insert(AxisLocation::Z_B_BK);

            QSet<AxisLocation> source2;
            source2.insert(AxisLocation::LEFT_Y_R_RC);
            source2.insert(AxisLocation::RIGHT_Y_R_RY);

            model->setAllowedAxisLocations(0, source0);
            model->setAllowedAxisLocations(1, source1);
            model->setAllowedAxisLocations(2, source2);

            break;
        }

        case AxisLocationStyle::RGB_COLOR_SPACE:
        case AxisLocationStyle::CMYK_COLOR_SPACE: {
            QSet<AxisLocation> source0;

            source0.insert(AxisLocation::LEFT_Y_R_RC);
            source0.insert(AxisLocation::RIGHT_Y_R_RY);
            source0.insert(AxisLocation::BOTTOM_X_A_GM);
            source0.insert(AxisLocation::TOP_X_A_GM);
            source0.insert(AxisLocation::Z_B_BK);

            model->setAllowedAxisLocations(0, source0);

            break;
        }

        case AxisLocationStyle::GRAYSCALE_COLOR_SPACE: {
            model->setAllowedAxisLocations(0, QSet<AxisLocation>());
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}
