/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MacProxyStyle class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MAC_PROXY_STYLE_H
#define MAC_PROXY_STYLE_H

#include <QApplication>
#include <QProxyStyle>
#include <QCommonStyle>
#include <QStyle>
#include <QString>

#include "app_common.h"

class QStyleOption;
class QStyleHintReturn;
class QWidget;

/**
 * A proxy style used to customize rendering of controls on MacOS.
 */
class APP_PUBLIC_API MacProxyStyle:public QProxyStyle {
    public:
        /**
         * Constructor
         *
         * \param[in] key The name of the base class style this proxy should override.
         */
        MacProxyStyle(const QString& key);

        /**
         * Constructor
         *
         * \param[in] style The style we are overriding.
         */
        MacProxyStyle(QStyle* style = Q_NULLPTR);

        virtual ~MacProxyStyle();

        /**
         * Method that returns an integer representing the specified style hint for the specified widget.
         *
         * \param[in] hint
         *
         * \param[in] option
         *
         * \param[in] widget
         *
         * \param[in] returnData
         *
         * \return
         */
        int styleHint(
            QStyle::StyleHint   hint,
            const QStyleOption* option = Q_NULLPTR,
            const QWidget*      widget = Q_NULLPTR,
            QStyleHintReturn*   returnData = Q_NULLPTR
        ) const override;
};

#endif
