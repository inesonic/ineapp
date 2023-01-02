/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref FunctionBrowserModel class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>
#include <QChar>
#include <QVariant>
#include <QMap>
#include <QList>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractItemModel>

#include <ld_variable_name.h>
#include <ld_data_type.h>
#include <ld_capabilities.h>
#include <ld_function_data.h>
#include <ld_function_database.h>

#include "metatypes.h"
#include "application.h"
#include "function_browser_model.h"

FunctionBrowserModel::FunctionBrowserModel(QObject* parent):QAbstractItemModel(parent) {
    geometryUpdateNeeded = true;
    currentFunctionList  = Ld::FunctionDatabase::search(Ld::Capabilities(), QString(), QList<QString>());
}


FunctionBrowserModel::FunctionBrowserModel(
        const QFont& functionFont,
        const QFont& descriptionFont,
        QObject*     parent
    ):QAbstractItemModel(
        parent
    ) {
    currentFunctionFont    = functionFont;
    currentDescriptionFont = descriptionFont;
    geometryUpdateNeeded   = true;
    currentFunctionList    = Ld::FunctionDatabase::search(Ld::Capabilities(), QString(), QList<QString>());
}


FunctionBrowserModel::~FunctionBrowserModel() {}


const QList<Ld::FunctionData>& FunctionBrowserModel::presentedFunctions() const {
    return currentFunctionList;
}


Ld::Capabilities FunctionBrowserModel::capabilities() const {
    return currentCapabilities;
}


QList<QString> FunctionBrowserModel::functionCategories() const {
    return Ld::FunctionDatabase::categories();
}


const QList<QString>& FunctionBrowserModel::selectedFunctionCategories() const {
    return currentSelectedCategories;
}


const QString& FunctionBrowserModel::searchString() const {
    return currentSearchString;
}


const QFont& FunctionBrowserModel::functionFont() const {
    return currentFunctionFont;
}


const QFont& FunctionBrowserModel::descriptionFont() const {
    return currentDescriptionFont;
}


unsigned FunctionBrowserModel::recommendedColumn1Width() {
    return currentColumn1Width;
}


unsigned FunctionBrowserModel::recommendedColumn2Width() {
    return currentColumn2Width;
}


unsigned FunctionBrowserModel::recommendedRowHeight() {
    return currentRowHeight;
}


bool FunctionBrowserModel::identifyFunction(
        const QModelIndex& modelIndex,
        Ld::FunctionData&  functionData,
        QStringList&       parameterDescriptions
    ) const {
    bool success = false;

    if (modelIndex.isValid()) {
        unsigned functionIndex = static_cast<unsigned>(-1);
        unsigned variantIndex  = static_cast<unsigned>(-1);

        if (modelIndex.internalId() != static_cast<quintptr>(-1)) {
            functionIndex = static_cast<unsigned>(modelIndex.internalId());
            variantIndex  = static_cast<unsigned>(modelIndex.row());
        } else {
            functionIndex = static_cast<unsigned>(modelIndex.row());
            variantIndex  = 0;
        }

        functionData = currentFunctionList.at(functionIndex);

        QList<QStringList> functionVariants = variants(functionData);

        if (variantIndex >= static_cast<unsigned>(functionVariants.size())) {
            variantIndex = 0;
        }

        parameterDescriptions = functionVariants.at(variantIndex);
        success               = true;
    }

    return success;
}


QModelIndex FunctionBrowserModel::index(int row, int column, const QModelIndex& parent) const {
    QModelIndex result;

    if (parent.isValid()) {
        unsigned parentRow = static_cast<unsigned>(parent.row());
        if (parentRow < static_cast<unsigned>(currentFunctionList.size())) {
            const Ld::FunctionData& functionData = currentFunctionList.at(parentRow);
            QList<QStringList> functionVariants = variants(functionData);
            if (row < functionVariants.size()) {
                result = createIndex(row, column, static_cast<quintptr>(parentRow));
            }
        }
    } else {
        if (row < currentFunctionList.size() && column < 2) {
            result = createIndex(row, column, static_cast<quintptr>(-1));
        }
    }

    return result;
}


QModelIndex FunctionBrowserModel::parent(const QModelIndex& index) const {
    QModelIndex parentIndex;

    if (index.isValid() && index.internalId() != static_cast<quintptr>(-1)) {
        parentIndex = createIndex(index.internalId(), 0, static_cast<quintptr>(-1));
    }

    return parentIndex;
}


QVariant FunctionBrowserModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant variant;

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        variant = tr("Header %1").arg(section + 1);
    }

    return variant;
}


QVariant FunctionBrowserModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    unsigned column = static_cast<unsigned>(index.column());
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole: {
            unsigned row = static_cast<unsigned>(index.row());

            if (index.internalId() == static_cast<quintptr>(-1)) {
                const Ld::FunctionData& functionData = currentFunctionList.at(row);

                if (column == 0) {
                    const Ld::VariableName& variableName = functionData.userVisibleName();

                    if (variableName.text2().isEmpty()) {
                        result = tr("%1  ").arg(variableName.text1());
                    } else {
                        result = tr("%1<sub>%2</sub>  ").arg(variableName.text1(), variableName.text2());
                    }
                } else {
                    result = functionData.description();
                }
            } else {
                if (column == 0) {
                    unsigned functionIndex = static_cast<unsigned>(index.internalId());
                    if (functionIndex < static_cast<unsigned>(currentFunctionList.size())) {
                        const Ld::FunctionData& functionData = currentFunctionList.at(functionIndex);
                        QList<QStringList> functionVariants = variants(functionData);

                        if (row < static_cast<unsigned>(functionVariants.size())) {
                            QString            text1            = functionData.userVisibleName().text1();
                            QString            text2            = functionData.userVisibleName().text2();
                            bool               hasSubscript     = functionData.includeSubscriptParameter();

                            const QStringList& parameters       = functionVariants.at(row);
                            unsigned           numberParameters = static_cast<unsigned>(parameters.size());
                            QString            str              =   text2.isEmpty()
                                                                  ? text1
                                                                  : tr("%1<sub>%2</sub>").arg(text1).arg(text2);

                            unsigned p;
                            bool     includeParenthesis;
                            if (hasSubscript && numberParameters > 0) {
                                if (text2.isEmpty()) {
                                    str += tr("<sub><%1></sub>").arg(parameters.first());
                                } else {
                                    str += tr("<sub>,<%1></sub>").arg(parameters.first());
                                }

                                p = 1;

                                includeParenthesis = (numberParameters > 2);
                            } else {
                                p = 0;
                                includeParenthesis = (numberParameters >= 2);
                            }

                            if (includeParenthesis) {
                                str += tr("(");
                            } else {
                                str += tr("%1").arg(QChar(0x200A));
                            }

                            bool firstParameter = true;
                            while (p < numberParameters) {
                                if (!firstParameter) {
                                    str += tr(", ");
                                } else {
                                    firstParameter = false;
                                }

                                str += tr("<%1>").arg(parameters.at(p));
                                ++p;
                            }

                            if (includeParenthesis) {
                                str += tr(")");
                            }

                            result = str;
                        }
                    }
                }
            }

            break;
        }

        case Qt::FontRole: {
            if (column == 0) {
                result = QVariant::fromValue(currentFunctionFont);
            } else {
                result = QVariant::fromValue(currentDescriptionFont);
            }

            break;
        }

        case Qt::ForegroundRole: {
            result = QVariant::fromValue(QColor(Qt::black));
            break;
        }

        case Qt::TextAlignmentRole: {
            result = QVariant::fromValue(Qt::AlignLeft);
            break;
        }
    }

    return result;
}


int FunctionBrowserModel::columnCount(const QModelIndex& parent) const {
    int result;

    if (parent.isValid()) {
        if (parent.internalId() == static_cast<quintptr>(-1)) {
            result = 2;
        } else {
            result = 1;
        }
    } else {
        result = 2;
    }

    return result;
}


int FunctionBrowserModel::rowCount(const QModelIndex& parent) const {
    int result;

    if (parent.isValid()) {
        if (parent.internalId() == static_cast<quintptr>(-1)) {
            unsigned functionIndex = static_cast<unsigned>(parent.row());
            if (functionIndex < static_cast<unsigned>(currentFunctionList.size())) {
                const Ld::FunctionData& functionData = currentFunctionList.at(functionIndex);
                QList<QStringList> functionVariants = variants(functionData);
                result = functionVariants.size();
            } else {
                result = 0;
            }
        } else {
            result = 0;
        }
    } else {
        result = currentFunctionList.size();
    }

    return result;
}


void FunctionBrowserModel::setCapabilities(const Ld::Capabilities& newCapabilities) {
    beginResetModel();

    currentCapabilities = newCapabilities;
    currentFunctionList = Ld::FunctionDatabase::search(
        newCapabilities,
        currentSearchString,
        currentSelectedCategories
    );

    geometryUpdateNeeded = true;

    endResetModel();
}


void FunctionBrowserModel::setSelectedCategories(const QList<QString>& categoryList) {
    beginResetModel();

    currentSelectedCategories = categoryList;
    currentFunctionList       = Ld::FunctionDatabase::search(
        currentCapabilities,
        currentSearchString,
        categoryList
    );

    geometryUpdateNeeded = true;

    endResetModel();
}


void FunctionBrowserModel::setSelectionString(const QString& selectionString) {
    beginResetModel();

    currentSearchString = selectionString;
    currentFunctionList = Ld::FunctionDatabase::search(
        currentCapabilities,
        selectionString,
        currentSelectedCategories
    );

    geometryUpdateNeeded = true;

    endResetModel();
}


void FunctionBrowserModel::calculateRecommendedGeometryData() {
    QFontMetrics functionFontMetrics(currentFunctionFont);
    QFontMetrics descriptionFontMetrics(currentDescriptionFont);

    unsigned column1Width = 0;
    unsigned column2Width = 0;

    for (  QList<Ld::FunctionData>::const_iterator it  = currentFunctionList.constBegin(),
                                                   end = currentFunctionList.constEnd()
         ; it != end
         ; ++it
        ) {
        const Ld::FunctionData& functionData      = *it;
        const Ld::VariableName& variableName      = functionData.userVisibleName();
        unsigned functionNameWidth = (
              functionFontMetrics.horizontalAdvance(variableName.text1())
            + functionFontMetrics.horizontalAdvance(variableName.text2())
        );
        unsigned descriptionWidth = descriptionFontMetrics.horizontalAdvance(functionData.description());

        if (functionNameWidth > column1Width) {
            column1Width = functionNameWidth;
        }

        if (descriptionWidth > column2Width) {
            column2Width = descriptionWidth;
        }
    }

    column1Width += functionFontMetrics.horizontalAdvance(tr("  "));
    currentRowHeight = std::max(functionFontMetrics.height(), descriptionFontMetrics.height());
}


QList<QStringList> FunctionBrowserModel::variants(const Ld::FunctionData& functionData) {
    QMap<unsigned, QMap<QStringList, unsigned>> resultMap;

    QList<Ld::FunctionVariant> functionVariants = functionData.variants();
    for (  QList<Ld::FunctionVariant>::const_iterator variantIterator    = functionVariants.constBegin(),
                                                      variantEndIterator = functionVariants.constEnd()
         ; variantIterator != variantEndIterator
         ; ++variantIterator
        ) {
        const Ld::FunctionVariant&            variant          = *variantIterator;
        unsigned                              numberParameters = variant.numberParameters();
        QList<QString>                        descriptions     = variant.parameterDescriptions();
        const QList<Ld::DataType::ValueType>& parameterTypes   = variant.parameterTypes();

        Q_ASSERT(numberParameters == static_cast<unsigned>(descriptions.size()));
        Q_ASSERT(numberParameters == static_cast<unsigned>(parameterTypes.size()));

        if (variant.allowsVariadicParameters()) {
            descriptions.append(tr("%1").arg(QChar(0x2026)));
        }

        QMap<QStringList, unsigned>& resultMap2 = resultMap[numberParameters];
        resultMap2.insert(descriptions, 1);
    }

    QList<QStringList> result;
    for (  QMap<unsigned, QMap<QStringList, unsigned>>::const_iterator resultIterator    = resultMap.constBegin(),
                                                                       resultEndIterator = resultMap.constEnd()
         ; resultIterator != resultEndIterator
         ; ++resultIterator
        ) {
        const QMap<QStringList, unsigned>& resultMap2 = resultIterator.value();
        result.append(resultMap2.keys());
    }

    return result;
}
