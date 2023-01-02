/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MacProxyStyle class.
***********************************************************************************************************************/

#include <QApplication>
#include <QProxyStyle>
#include <QCommonStyle>
#include <QStyle>
#include <QStyleOption>
#include <QStyleHintReturn>
#include <QWidget>

#include <QDebug> // Debug
#include <eqt_tool_button.h> // Debug

#include "mac_proxy_style.h"

MacProxyStyle::MacProxyStyle(const QString& key):QProxyStyle(key) {}


MacProxyStyle::MacProxyStyle(QStyle* style):QProxyStyle(style) {}


MacProxyStyle::~MacProxyStyle() {}


int MacProxyStyle::styleHint(
        QStyle::StyleHint   hint,
        const QStyleOption* option,
        const QWidget*      widget,
        QStyleHintReturn*   returnData
    ) const {
    int result = QProxyStyle::styleHint(hint, option, widget, returnData);

    if (dynamic_cast<const EQt::ToolButton*>(widget) != Q_NULLPTR) {
        qDebug() << "styleHint(" << hint << ", " << option << ", widget, " << returnData << ") = " << result;

        if (returnData != Q_NULLPTR) {
            qDebug() << "    returnData->type = " << returnData->type
                     << "    returnData->version = " << returnData->version;
        }
    }

    return result;
}

