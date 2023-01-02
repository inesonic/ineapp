/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotStyleWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_STYLE_WIDGET_H
#define PLOT_STYLE_WIDGET_H

#include <QSize>
#include <QString>
#include <QSet>
#include <QMap>

#include "app_common.h"
#include "plot_widget_base.h"
#include "plot_engine.h"

class QWidget;
class QPixmap;
class QVBoxLayout;
class QHBoxLayout;
class QToolButton;
class QButtonGroup;
class QAbstractButton;

namespace EQt {
    class DimensionLineEdit;
}

/**
 * Widget you can use to more easily and consistently manage settings for plot style.  This widget can also be used as
 * a wizard page.
 */
class APP_PUBLIC_API PlotStyleWidget:public PlotWidgetBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        PlotStyleWidget(QWidget* parent = Q_NULLPTR);

        ~PlotStyleWidget() override;

        /**
         * Method you can use to determine if the current settings are valid or invalid.
         *
         * \return Returns true if the settings are valid.  Returns false if the settings are invalid.
         */
        bool isValid() const override;

        /**
         * Method that provides the currently selected plot style.
         *
         * \return Returns the currently selected plot style.  A null pointer is returned if there is not a singular
         *         plot style selected.
         */
        const PlotEngine* plotStyle() const;

    signals:
        /**
         * Signal that is emitted when the the user changes the current plot style.
         *
         * \param[out] newPlotEngine A pointer to the newly selected plotting engine.
         */
        void plotStyleChanged(const PlotEngine* newPlotEngine);

    public slots:
        /**
         * Slot you can use to set the currently selected plotting engine.
         *
         * \param[in] newPlotEngine The newly selected plotting engine.
         */
        void setPlotStyle(const PlotEngine* newPlotEngine);

        /**
         * Slot you can use to set the currently selected plotting engines.
         *
         * \param[in] newPlotStyles The names for the newly selected plot styles.
         */
        void setPlotStyles(const QSet<const PlotEngine*>& newPlotStyles);

    private slots:
        /**
         * Slot that is triggered when the user changes the currently selected plot style.
         *
         * \param[in] styleButton The style button that was clicked.
         */
        void styleButtonClicked(QAbstractButton* styleButton);

    private:
        /**
         * The number of plot style buttons per row.
         */
        static constexpr unsigned maximumStyleButtonsPerRow = 5;

        /**
         * The size of the style button icons.
         */
        static const QSize styleButtonIconSize;

        /**
         * The spacing to apply between style buttons.
         */
        static constexpr unsigned styleButtonSpacing = 5;

        /**
         * Map used to translate buttons into style names.
         */
        QMap<QToolButton*, const PlotEngine*> plotStylesByButton;

        /**
         * Button group used to manage exclusivity of plot styles.
         */
        QButtonGroup* styleButtonGroup;
};

#endif
