/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref PlotDataSourcesModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>
#include <QFont>

#include <algorithm>

#include <ld_variable_name.h>
#include <ld_character_format.h>
#include <ld_chart_axis_format.h>
#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_plot_format.h>

#include "metatypes.h"
#include "application.h"
#include "plot_data_sources_model.h"

const Model::Variant PlotDataSourcesModel::dummyVariant;

PlotDataSourcesModel::PlotDataSourcesModel(QObject* parent):QAbstractTableModel(parent) {
    currentFirstRowIsBarLabels            = false;
    currentNumberDataSourcesPerDataSeries = 2;
    currentAxisLocationStyle              = AxisLocationStyle::PLOT_BORDERS;
    currentlySupportedPlotSeriesMode      = PlotSeriesMode::LINES_AND_MARKERS;
}


PlotDataSourcesModel::~PlotDataSourcesModel() {}


bool PlotDataSourcesModel::isValid() const {
    bool valid = (
           numberDataSeries() >= currentMinimumNumberDataSeries
        && numberDataSeries() <= currentMaximumNumberDataSeries
    );

    if (valid) {
        unsigned numberDataSources = static_cast<unsigned>(currentDataSources.size());
        unsigned sourceIndex       = 0;
        while (valid && sourceIndex < numberDataSources) {
            valid = !currentDataSources.at(sourceIndex).text1().isEmpty();
            ++sourceIndex;
        }
    }

    if (valid                                                                 &&
        (currentAxisLocationStyle == AxisLocationStyle::RGB_COLOR_SPACE  ||
         currentAxisLocationStyle == AxisLocationStyle::CMYK_COLOR_SPACE    )    ) {
        unsigned numberDataSeries = PlotDataSourcesModel::numberDataSeries();
        QSet<AxisLocation> allAxisLocations;
        for (unsigned seriesIndex=0 ; seriesIndex<numberDataSeries ; ++seriesIndex) {
            allAxisLocations.insert(axisLocation(seriesIndex, 0));
        }

        valid = (static_cast<unsigned>(allAxisLocations.size()) == numberDataSeries);
    }

    return valid;
}


unsigned PlotDataSourcesModel::columnsPerDataSource() const {
    unsigned columnsPerSource;
    if (currentAxisLocationStyle == AxisLocationStyle::CATEGORIZED   ||
        currentAxisLocationStyle == AxisLocationStyle::UNCATEGORIZED ||
        currentAxisLocationStyle == AxisLocationStyle::FIXED            ) {
        columnsPerSource = 1;
    } else {
        columnsPerSource = 2;
    }

    return columnsPerSource;
}


PlotDataSourcesModel::ContentsType PlotDataSourcesModel::contentsType(unsigned columnIndex) const {
    ContentsType result;

    unsigned columnsPerSource = columnsPerDataSource();
    if (columnIndex < columnsPerSource * currentNumberDataSourcesPerDataSeries) {
        result = (columnIndex % columnsPerSource) == 0 ? ContentsType::DATA_SOURCE_NAME : ContentsType::AXIS_LOCATION;
    } else {
        unsigned offset = columnIndex - columnsPerSource * currentNumberDataSourcesPerDataSeries;
        if (offset == 0) {
            result = ContentsType::LABEL;
        } else if (offset == 1) {
            result = ContentsType::PLOT_SERIES;
        } else {
            result = ContentsType::INVALID;
        }
    }

    return result;
}


PlotDataSourcesModel::ContentsType PlotDataSourcesModel::contentsType(const QModelIndex& index) const {
    return contentsType(static_cast<unsigned>(index.column()));
}


unsigned PlotDataSourcesModel::sourceIndex(unsigned columnIndex) const {
    unsigned result;

    unsigned columnsPerSource = columnsPerDataSource();
    if (columnIndex < columnsPerSource * currentNumberDataSourcesPerDataSeries) {
        result = columnIndex / columnsPerSource;
    } else {
        result = static_cast<unsigned>(-1);
    }

    return result;
}


unsigned PlotDataSourcesModel::sourceIndex(const QModelIndex& index) const {
    return sourceIndex(static_cast<unsigned>(index.column()));
}


QVariant PlotDataSourcesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant result;

    if (orientation == Qt::Orientation::Horizontal) {
        switch (role) {
            case Qt::ItemDataRole::DisplayRole:
            case Qt::ItemDataRole::EditRole: {
                result = QVariant(headerString(static_cast<unsigned>(section)));
                break;
            }

            case Qt::ItemDataRole::ToolTipRole: {
                ContentsType contentsType = PlotDataSourcesModel::contentsType(section);
                switch (contentsType) {
                    case ContentsType::INVALID: {
                        break;
                    }

                    case ContentsType::DATA_SOURCE_NAME: {
                        result = tr("Data source %1 for the series").arg(sourceIndex(section) + 1);
                        break;

                    }

                    case ContentsType::AXIS_LOCATION: {
                        result = tr("Axis used by data source %1").arg(sourceIndex(section) + 1);
                        break;
                    }

                    case ContentsType::LABEL: {
                        result = tr("Text displayed in the legend for the series");
                        break;
                    }

                    case ContentsType::PLOT_SERIES: {
                        result = tr("The visual appearance for the series in the plot, graph, or chart");
                        break;
                    }

                    default: {
                        Q_ASSERT(false);
                        break;
                    }
                }

                break;
            }

            case Qt::ItemDataRole::TextAlignmentRole: {
                result = Qt::AlignmentFlag::AlignLeft;
                break;
            }

            case Qt::ItemDataRole::SizeHintRole: {
                QString s = headerString(static_cast<unsigned>(section));

                QFontMetrics fontMetrics(Application::font());
                result = QVariant(
                    QSize(
                        fontMetrics.horizontalAdvance(s + tr("MMMMM")),
                        static_cast<unsigned>(2.0 * fontMetrics.height())
                    )
                );

                break;
            }

            default: {
                break;
            }
        }
    }

    return result;
}


QVariant PlotDataSourcesModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    unsigned row = static_cast<unsigned>(index.row());

    if (row < static_cast<unsigned>(currentPlotSeries.size())) {
        switch (role) {
            case Qt::ItemDataRole::EditRole:
            case Qt::ItemDataRole::DisplayRole: {
                unsigned     seriesBase   = currentNumberDataSourcesPerDataSeries * row;
                ContentsType contentsType = PlotDataSourcesModel::contentsType(index);
                switch (contentsType) {
                    case ContentsType::INVALID: {
                        break;
                    }

                    case ContentsType::DATA_SOURCE_NAME: {
                        unsigned sourceIndex = PlotDataSourcesModel::sourceIndex(index);
                        result = QVariant::fromValue(currentDataSources.at(seriesBase + sourceIndex));
                        break;

                    }

                    case ContentsType::AXIS_LOCATION: {
                        unsigned sourceIndex = PlotDataSourcesModel::sourceIndex(index);
                        result = QVariant::fromValue(currentAxisLocations.at(seriesBase + sourceIndex));
                        break;
                    }

                    case ContentsType::LABEL: {
                        result = QVariant::fromValue(currentPlotSeriesLabels.at(row));
                        break;
                    }

                    case ContentsType::PLOT_SERIES: {
                        const Ld::PlotSeries& plotSeries = currentPlotSeries.at(row);
                        result = QVariant::fromValue(plotSeries);
                        break;
                    }

                    default: {
                        Q_ASSERT(false);
                        break;
                    }
                }

                break;
            }

            case Qt::ItemDataRole::FontRole: {
                unsigned column = static_cast<unsigned>(index.column());
                result = QVariant(fontForColumn(static_cast<unsigned>(column)));
                break;
            }

            case Qt::ItemDataRole::ForegroundRole: {
                ContentsType contentsType = PlotDataSourcesModel::contentsType(index);
                if (contentsType == ContentsType::AXIS_LOCATION) {
                    unsigned sourceIndex = PlotDataSourcesModel::sourceIndex(index);
                    if (static_cast<unsigned>(currentAllowedAxisLocations.size()) > sourceIndex) {
                        const QSet<AxisLocation>& allowedAxisLocations = currentAllowedAxisLocations.at(sourceIndex);
                        if (!allowedAxisLocations.isEmpty()) {
                            unsigned seriesBase = currentNumberDataSourcesPerDataSeries * row;
                            AxisLocation currentAxisLocation = currentAxisLocations.at(seriesBase + sourceIndex);
                            if (!allowedAxisLocations.contains(currentAxisLocation)) {
                                result = QVariant(QBrush(QColor(Qt::GlobalColor::red)));
                            }
                        }
                    }
                }

                break;
            }

            case Qt::ItemDataRole::SizeHintRole: {
                unsigned     column       = static_cast<unsigned>(index.column());
                ContentsType contentsType = PlotDataSourcesModel::contentsType(index);

                QFontMetrics fontMetrics(fontForColumn(column));

                switch (contentsType) {
                    case ContentsType::INVALID: {
                        break;
                    }

                    case ContentsType::DATA_SOURCE_NAME: {
                        unsigned seriesBase  = currentNumberDataSourcesPerDataSeries * row;
                        unsigned sourceIndex = PlotDataSourcesModel::sourceIndex(column);

                        const Ld::VariableName variableName = currentDataSources.at(seriesBase + sourceIndex);

                        int width = fontMetrics.horizontalAdvance(
                            variableName.text1() + variableName.text2() + tr("M")
                        );
                        unsigned height = static_cast<unsigned>(fontMetrics.height());

                        if (!variableName.text2().isEmpty()) {
                            height *= 1.25 * height; // Rough estimate
                        }

                        result = QVariant::fromValue(QSize(width, height));
                        break;
                    }

                    case ContentsType::AXIS_LOCATION: {
                        result = QVariant(QSize(fontMetrics.horizontalAdvance(tr("BottomM")), fontMetrics.height()));
                        break;
                    }

                    case ContentsType::LABEL: {
                        const QString& label = currentPlotSeriesLabels.at(row);
                        result = QVariant(
                            QSize(fontMetrics.horizontalAdvance(label + tr("M")), fontMetrics.height())
                        );
                        break;
                    }

                    case ContentsType::PLOT_SERIES: {
                        QString s = headerString(column);
                        result = QVariant(QSize(fontMetrics.horizontalAdvance(s + tr("M")), fontMetrics.height()));
                        break;
                    }

                    default: {
                        Q_ASSERT(false);
                        break;
                    }
                }

                break;
            }

            default: {
                break;
            }
        }
    }

    return result;
}


int PlotDataSourcesModel::rowCount(const QModelIndex& /* parent */) const {
    return currentPlotSeries.size();
}


int PlotDataSourcesModel::columnCount(const QModelIndex& /* parent */) const {
    unsigned numberAdditionalColumns;
    if (currentAxisLocationStyle == AxisLocationStyle::RGB_COLOR_SPACE       ||
        currentAxisLocationStyle == AxisLocationStyle::CMYK_COLOR_SPACE      ||
        currentAxisLocationStyle == AxisLocationStyle::GRAYSCALE_COLOR_SPACE    ) {
        numberAdditionalColumns = 0;
    } else {
        numberAdditionalColumns = 2;
    }

    return columnsPerDataSource() * currentNumberDataSourcesPerDataSeries + numberAdditionalColumns;
}


bool PlotDataSourcesModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    unsigned numberRows = static_cast<unsigned>(currentPlotSeries.size());
    unsigned index      = static_cast<unsigned>(row);

    if (index > numberRows) {
        index = numberRows;
    }

    unsigned       plotSeriesIndex = numberRows % Ld::PlotSeries::numberDefaultPlotSeries;
    Ld::PlotSeries newPlotSeries   = Ld::PlotSeries::defaultPlotSeries[plotSeriesIndex];

    currentPlotSeries.insert(index, newPlotSeries);
    currentPlotSeriesLabels.insert(index, tr("Series %1").arg(numberRows + 1));

    QList<AxisLocation> axisLocations = defaultAxisLocationsForStyle(currentAxisLocationStyle);
    Q_ASSERT(static_cast<unsigned>(axisLocations.size()) >= currentNumberDataSourcesPerDataSeries);

    for (unsigned sourceIndex=0 ; sourceIndex<currentNumberDataSourcesPerDataSeries ; ++sourceIndex) {
        currentDataSources.insert(currentNumberDataSourcesPerDataSeries * index, Ld::VariableName());
        currentAxisLocations.insert(
            currentNumberDataSourcesPerDataSeries* index + sourceIndex,
            axisLocations.at(sourceIndex)
        );
    }

    endInsertRows();

    return true;
}


bool PlotDataSourcesModel::removeRow(int row, const QModelIndex& parent) {
    bool success;

    unsigned numberRows = static_cast<unsigned>(currentPlotSeries.size());
    unsigned index      = static_cast<unsigned>(row);
    if (index < numberRows) {
        beginRemoveRows(parent, row, row);

        currentPlotSeries.removeAt(index);
        currentPlotSeriesLabels.removeAt(index);

        for (unsigned sourceIndex=0 ; sourceIndex<currentNumberDataSourcesPerDataSeries ; ++sourceIndex) {
            currentDataSources.removeAt(currentNumberDataSourcesPerDataSeries * index);
            currentAxisLocations.removeAt(currentNumberDataSourcesPerDataSeries * index);
        }

        endRemoveRows();

        success = true;
    } else {
        success = false;
    }

    return success;
}


bool PlotDataSourcesModel::moveRowsUp(const QList<unsigned>& rows) {
    bool success;

    QList<unsigned> indexes = rows;
    std::sort(indexes.begin(), indexes.end(), [](unsigned a, unsigned b)->bool{ return a >= b; });

    unsigned firstRow = indexes.last();
    if (firstRow > 0) {
        beginResetModel();

        QList<Ld::VariableName> removedDataSources;
        QList<AxisLocation>     removedAxisLocations;
        QList<Ld::PlotSeries>   removedPlotSeries;
        QStringList             removedPlotSeriesLabels;

        removeAndSaveRows(
            indexes,
            removedDataSources,
            removedAxisLocations,
            removedPlotSeries,
            removedPlotSeriesLabels
        );

        insertRowData(
            firstRow - 1,
            removedDataSources,
            removedAxisLocations,
            removedPlotSeries,
            removedPlotSeriesLabels
        );

        endResetModel();

        success = true;
    } else {
        success = false;
    }

    return success;
}


bool PlotDataSourcesModel::moveRowsDown(const QList<unsigned>& rows) {
    bool success;

    QList<unsigned> indexes = rows;
    std::sort(indexes.begin(), indexes.end(), [](unsigned a, unsigned b)->bool{ return a >= b; });

    unsigned lastRow = indexes.first();
    if (lastRow + 1 < static_cast<unsigned>(currentPlotSeries.size())) {
        beginResetModel();

        QList<Ld::VariableName> removedDataSources;
        QList<AxisLocation>     removedAxisLocations;
        QList<Ld::PlotSeries>   removedPlotSeries;
        QStringList             removedPlotSeriesLabels;

        removeAndSaveRows(
            indexes,
            removedDataSources,
            removedAxisLocations,
            removedPlotSeries,
            removedPlotSeriesLabels
        );

        insertRowData(
            lastRow + 1,
            removedDataSources,
            removedAxisLocations,
            removedPlotSeries,
            removedPlotSeriesLabels
        );

        endResetModel();

        success = true;
    } else {
        success = false;
    }

    return success;
}


unsigned PlotDataSourcesModel::dataSourcesPerDataSeries() const {
    return currentNumberDataSourcesPerDataSeries;
}


unsigned PlotDataSourcesModel::minimumNumberDataSeries() const {
    return currentMinimumNumberDataSeries;
}


unsigned PlotDataSourcesModel::maximumNumberDataSeries() const {
    return currentMaximumNumberDataSeries;
}


unsigned PlotDataSourcesModel::numberDataSeries() const {
    return static_cast<unsigned>(currentPlotSeries.size());
}


PlotDataSourcesModel::AxisLocationStyle PlotDataSourcesModel::axisLocationStyle() const {
    return currentAxisLocationStyle;
}


PlotDataSourcesModel::PlotSeriesMode PlotDataSourcesModel::supportedPlotSeriesMode() const {
    return currentlySupportedPlotSeriesMode;
}


const QList<Ld::VariableName>& PlotDataSourcesModel::dataSources() const {
    return currentDataSources;
}


const Ld::VariableName& PlotDataSourcesModel::dataSource(unsigned seriesIndex, unsigned sourceIndex) const {
    return currentDataSources.at(seriesIndex * currentNumberDataSourcesPerDataSeries + sourceIndex);
}


const QList<PlotDataSourcesModel::AxisLocation>& PlotDataSourcesModel::axisLocations() const {
    return currentAxisLocations;
}


PlotDataSourcesModel::AxisLocation PlotDataSourcesModel::axisLocation(
        unsigned seriesIndex,
        unsigned sourceIndex
    ) const {
    return currentAxisLocations.at(seriesIndex * currentNumberDataSourcesPerDataSeries + sourceIndex);
}


const QList<Ld::PlotSeries>& PlotDataSourcesModel::plotSeries() const {
    return currentPlotSeries;
}


const Ld::PlotSeries& PlotDataSourcesModel::plotSeries(unsigned seriesIndex) const {
    return currentPlotSeries.at(seriesIndex);
}


const QStringList& PlotDataSourcesModel::seriesLabels() const {
    return currentPlotSeriesLabels;
}


QString PlotDataSourcesModel::seriesLabel(unsigned seriesIndex) const {
    return currentPlotSeriesLabels.at(seriesIndex);
}


QList<PlotDataSourcesModel::AxisLocation> PlotDataSourcesModel::defaultAxisLocationsForStyle(
        PlotDataSourcesModel::AxisLocationStyle axisLocationStyle
    ) {
    QList<AxisLocation> result;

    switch (axisLocationStyle) {
        case AxisLocationStyle::PLOT_BORDERS: {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::LEFT_Y_R_RC);

            break;
        }

        case AxisLocationStyle::CATEGORIZED: {
            result.append(AxisLocation::LEFT_Y_R_RC);
            break;
        }

        case AxisLocationStyle::UNCATEGORIZED:
        case AxisLocationStyle::FIXED: {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            break;
        }

        case AxisLocationStyle::CARTESIAN_2D: {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::LEFT_Y_R_RC);

            break;
        }

        case AxisLocationStyle::CARTESIAN_3D: {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::LEFT_Y_R_RC);
            result.append(AxisLocation::Z_B_BK);

            break;
        }

        case AxisLocationStyle::POLAR_2D: {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::LEFT_Y_R_RC);

            break;
        }

        case AxisLocationStyle::POLAR_3D: {
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::Z_B_BK);
            result.append(AxisLocation::LEFT_Y_R_RC);

            break;
        }

        case AxisLocationStyle::RGB_COLOR_SPACE: {
            result.append(AxisLocation::LEFT_Y_R_RC);
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::Z_B_BK);

            break;
        }

        case AxisLocationStyle::CMYK_COLOR_SPACE: {
            result.append(AxisLocation::LEFT_Y_R_RC);
            result.append(AxisLocation::BOTTOM_X_A_GM);
            result.append(AxisLocation::RIGHT_Y_R_RY);
            result.append(AxisLocation::Z_B_BK);

            break;
        }

        case AxisLocationStyle::GRAYSCALE_COLOR_SPACE: {
            result.append(AxisLocation::Z_B_BK);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


bool PlotDataSourcesModel::firstRowIsBarLabels() const {
    return currentFirstRowIsBarLabels;
}


bool PlotDataSourcesModel::firstRowIsDataSeries() const {
    return !firstRowIsBarLabels();
}


QSet<PlotDataSourcesModel::AxisLocation> PlotDataSourcesModel::allowedAxisLocations(unsigned sourceIndex) const {
    QSet<AxisLocation> result;

    if (sourceIndex < static_cast<unsigned>(currentAllowedAxisLocations.size())) {
        result = currentAllowedAxisLocations.at(sourceIndex);
    }

    return result;
}


void PlotDataSourcesModel::setDataSourcesPerDataSeries(unsigned newNumberDataSourcesPerDataSeries) {
    beginResetModel();

    QList<Ld::VariableName> newDataSources;
    QList<AxisLocation>     newAxisLocations;

    QList<AxisLocation> axisLocations = defaultAxisLocationsForStyle(currentAxisLocationStyle);
    Q_ASSERT(static_cast<unsigned>(axisLocations.size()) >= newNumberDataSourcesPerDataSeries);

    unsigned numberSeries = numberDataSeries();
    for (unsigned dataSeries=0 ; dataSeries < numberSeries ; ++dataSeries) {
        for (unsigned source=0 ; source<newNumberDataSourcesPerDataSeries ; ++source) {
            if (source < currentNumberDataSourcesPerDataSeries) {
                unsigned oldIndex = dataSeries * currentNumberDataSourcesPerDataSeries + source;
                newDataSources.append(currentDataSources.at(oldIndex));
                newAxisLocations.append(currentAxisLocations.at(oldIndex));
            } else {
                newDataSources.append(Ld::VariableName());
                newAxisLocations.append(axisLocations.at(source));
            }
        }
    }

    currentDataSources   = newDataSources;
    currentAxisLocations = newAxisLocations;

    currentNumberDataSourcesPerDataSeries = newNumberDataSourcesPerDataSeries;

    endResetModel();
}


void PlotDataSourcesModel::setNumberAllowedDataSeries(
        unsigned minimumNumberDataSources,
        unsigned maximumNumberDataSources
    ) {
    beginResetModel();

    currentMinimumNumberDataSeries = minimumNumberDataSources;
    currentMaximumNumberDataSeries = maximumNumberDataSources;

    while (numberDataSeries() < minimumNumberDataSources) {
        insertRow(numberDataSeries());
    }

    endResetModel();
}


void PlotDataSourcesModel::setAxisLocationStyle(AxisLocationStyle newAxisLocationStyle) {
    beginResetModel();
    currentAxisLocationStyle = newAxisLocationStyle;
    endResetModel();
}


void PlotDataSourcesModel::setSupportedPlotSeriesMode(
        PlotDataSourcesModel::PlotSeriesMode newSupportedPlotSeriesMode
    ) {
    beginResetModel();
    currentlySupportedPlotSeriesMode = newSupportedPlotSeriesMode;
    endResetModel();
}


void PlotDataSourcesModel::setDataSources(
        const QList<Ld::VariableName>&                   newDataSources,
        const QList<PlotDataSourcesModel::AxisLocation>& newAxisLocations,
        const QList<Ld::PlotSeries>&                     newPlotSeries,
        const QStringList&                               newPlotSeriesLabels
    ) {
    Q_ASSERT(newDataSources.size() == newAxisLocations.size());
    Q_ASSERT(newDataSources.size() == newPlotSeriesLabels.size());
    Q_ASSERT((newDataSources.size() % newPlotSeries.size()) == 0);

    beginResetModel();

    currentNumberDataSourcesPerDataSeries = newDataSources.size() / newPlotSeries.size();
    currentDataSources                    = newDataSources;
    currentAxisLocations                  = newAxisLocations;
    currentPlotSeries                     = newPlotSeries;
    currentPlotSeriesLabels               = newPlotSeriesLabels;

    endResetModel();
}


void PlotDataSourcesModel::setDataSource(
        unsigned seriesIndex,
        unsigned sourceIndex,
        const Ld::VariableName& newDataSource
    ) {
    beginResetModel();

    unsigned numberSeries = numberDataSeries();
    while (numberSeries <= seriesIndex) {
        insertRow(numberSeries);
        ++numberSeries;
    }

    unsigned index = currentNumberDataSourcesPerDataSeries * seriesIndex + sourceIndex;
    currentDataSources[index] = newDataSource;

    endResetModel();
}


void PlotDataSourcesModel::setAxisLocation(
        unsigned                           seriesIndex,
        unsigned                           sourceIndex,
        PlotDataSourcesModel::AxisLocation newAxisLocation
    ) {
    beginResetModel();

    unsigned numberSeries = numberDataSeries();
    while (numberSeries <= seriesIndex) {
        insertRow(numberSeries);
        ++numberSeries;
    }

    unsigned index = currentNumberDataSourcesPerDataSeries * seriesIndex + sourceIndex;
    currentAxisLocations[index] = newAxisLocation;

    endResetModel();
}


void PlotDataSourcesModel::setPlotSeries(unsigned seriesIndex, const Ld::PlotSeries& newPlotSeries) {
    beginResetModel();

    unsigned numberSeries = numberDataSeries();
    while (numberSeries <= seriesIndex) {
        insertRow(numberSeries);
        ++numberSeries;
    }

    currentPlotSeries[seriesIndex] = newPlotSeries;

    endResetModel();
}


void PlotDataSourcesModel::setPlotSeriesLabel(unsigned seriesIndex, const QString& plotSeriesLabel) {
    beginResetModel();

    unsigned numberSeries = numberDataSeries();
    while (numberSeries <= seriesIndex) {
        insertRow(numberSeries);
        ++numberSeries;
    }

    currentPlotSeriesLabels[seriesIndex] = plotSeriesLabel;

    endResetModel();
}


void PlotDataSourcesModel::setFirstRowIsBarLabels(bool nowFirstRowIsBarLabels) {
    beginResetModel();
    currentFirstRowIsBarLabels = nowFirstRowIsBarLabels;
    endResetModel();
}


void PlotDataSourcesModel::setFirstRowIsDataSeries(bool nowFirstRowIsDataSeries) {
    setFirstRowIsBarLabels(!nowFirstRowIsDataSeries);
}


void PlotDataSourcesModel::setAllowedAxisLocations(unsigned sourceIndex, const QSet<AxisLocation>& allowedLocations) {
    beginResetModel();

    while (static_cast<unsigned>(currentAllowedAxisLocations.size()) <= sourceIndex) {
        currentAllowedAxisLocations.append(QSet<AxisLocation>());
    }

    currentAllowedAxisLocations[sourceIndex] = allowedLocations;

    endResetModel();
}


QString PlotDataSourcesModel::headerString(unsigned section) const {
    QString result;

    ContentsType contentsType = PlotDataSourcesModel::contentsType(section);
    switch (contentsType) {
        case ContentsType::INVALID: {
            break;
        }

        case ContentsType::DATA_SOURCE_NAME: {
            result = tr("Source %1").arg(PlotDataSourcesModel::sourceIndex(section) + 1);
            break;

        }

        case ContentsType::AXIS_LOCATION: {
            result = tr("Source %1 Axis").arg(PlotDataSourcesModel::sourceIndex(section) + 1);
            break;
        }

        case ContentsType::LABEL: {
            result = tr("Label");
            break;
        }

        case ContentsType::PLOT_SERIES: {
            result = tr("Appearance");
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


QFont PlotDataSourcesModel::fontForColumn(unsigned int columnIndex) const {
    QFont font;

    if (columnIndex < 2 * currentNumberDataSourcesPerDataSeries && (columnIndex % 2) == 0) {
        font = Ld::CharacterFormat::applicationDefaultMathIdentifierFont()->toQFont();
    } else {
        font = Application::font();
    }

    return font;
}


void PlotDataSourcesModel::removeAndSaveRows(
        const QList<unsigned>&   rowList,
        QList<Ld::VariableName>& dataSources,
        QList<AxisLocation>&     axisLocations,
        QList<Ld::PlotSeries>&   plotSeries,
        QStringList&             plotSeriesLabels

    ) {
    for (  QList<unsigned>::const_iterator rowIterator = rowList.constBegin(), rowEndIterator = rowList.constEnd()
         ; rowIterator != rowEndIterator
         ; ++rowIterator
        ) {
        unsigned index = *rowIterator;

        plotSeries.prepend(currentPlotSeries.at(index));
        currentPlotSeries.removeAt(index);

        plotSeriesLabels.prepend(currentPlotSeriesLabels.at(index));
        currentPlotSeriesLabels.removeAt(index);

        for (unsigned source=0 ; source<currentNumberDataSourcesPerDataSeries ; ++source) {
            unsigned reverseIndex = (
                  currentNumberDataSourcesPerDataSeries * index
                + currentNumberDataSourcesPerDataSeries - 1
                - source
            );

            dataSources.prepend(currentDataSources.at(reverseIndex));
            axisLocations.prepend(currentAxisLocations.at(reverseIndex));
        }

        unsigned removeIndex = currentNumberDataSourcesPerDataSeries * index;
        for (unsigned source=0 ; source<currentNumberDataSourcesPerDataSeries ; ++source) {
            currentDataSources.removeAt(removeIndex);
            currentAxisLocations.removeAt(removeIndex);
        }
    }
}


void PlotDataSourcesModel::insertRowData(
        unsigned                       insertionPoint,
        const QList<Ld::VariableName>& dataSources,
        const QList<AxisLocation>&     axisLocations,
        const QList<Ld::PlotSeries>&   plotSeries,
        const QStringList&             plotSeriesLabels
    ) {
    unsigned numberRows     = static_cast<unsigned>(plotSeries.size());
    unsigned insertionIndex = currentNumberDataSourcesPerDataSeries * insertionPoint;

    for (unsigned i=0 ; i<numberRows ; ++i) {
        currentPlotSeries.insert(insertionPoint, plotSeries.at(i));
        currentPlotSeriesLabels.insert(insertionPoint, plotSeriesLabels.at(i));

        unsigned extractionPoint = currentNumberDataSourcesPerDataSeries * i;
        for (unsigned source=0 ; source<currentNumberDataSourcesPerDataSeries ; ++source) {
            currentDataSources.insert(insertionIndex + source, dataSources.at(extractionPoint + source));
            currentAxisLocations.insert(insertionIndex + source, axisLocations.at(extractionPoint + source));
        }

        ++insertionPoint;
    }
}
