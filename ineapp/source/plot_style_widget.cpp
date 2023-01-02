/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotStyleWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QWizardPage>
#include <QMap>
#include <QSet>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QAbstractButton>
#include <QToolButton>
#include <QButtonGroup>

#include "application.h"
#include "plot_engine.h"
#include "plot_widget_base.h"
#include "plot_style_widget.h"

const QSize PlotStyleWidget::styleButtonIconSize(64, 64);

PlotStyleWidget::PlotStyleWidget(QWidget* parent):PlotWidgetBase(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QGroupBox* plotStyleGroupBox = new QGroupBox(tr("Plot Style"));
    mainLayout->addWidget(plotStyleGroupBox);

    QVBoxLayout* styleButtonVerticalLayout = new QVBoxLayout;
    plotStyleGroupBox->setLayout(styleButtonVerticalLayout);

    QHBoxLayout* styleButtonHorizontalLayout = Q_NULLPTR;

    styleButtonGroup = new QButtonGroup(this);
    styleButtonGroup->setExclusive(true);

    const QList<const PlotEngine*>& plottingEngines   = PlotEngine::engines();
    unsigned                        numberPlotEngines = static_cast<unsigned>(plottingEngines.size());
    for (unsigned plotEngineIndex=0 ; plotEngineIndex<numberPlotEngines ; ++plotEngineIndex) {
        const PlotEngine* engine = plottingEngines.at(plotEngineIndex);

        if (styleButtonHorizontalLayout == Q_NULLPTR) {
            styleButtonHorizontalLayout = new QHBoxLayout;

            QWidget* styleButtonLayoutWidget = new QWidget;
            styleButtonLayoutWidget->setLayout(styleButtonHorizontalLayout);

            styleButtonVerticalLayout->addWidget(styleButtonLayoutWidget, 0, Qt::AlignCenter);
        } else if ((plotEngineIndex % maximumStyleButtonsPerRow) == 0) {
            styleButtonHorizontalLayout = new QHBoxLayout;

            QWidget* styleButtonLayoutWidget = new QWidget;
            styleButtonLayoutWidget->setLayout(styleButtonHorizontalLayout);

            styleButtonVerticalLayout->addSpacing(styleButtonSpacing);
            styleButtonVerticalLayout->addWidget(styleButtonLayoutWidget, 0, Qt::AlignCenter);
        } else {
            styleButtonHorizontalLayout->addSpacing(styleButtonSpacing);
        }

        QToolButton* toolButton = new QToolButton;
        toolButton->setIcon(engine->icon());
        toolButton->setIconSize(styleButtonIconSize);
        toolButton->setCheckable(true);

        toolButton->setToolTip(engine->toolTipDescription());
        toolButton->setWhatsThis(engine->whatsThisDescription());

        QLabel* descriptionLabel = new QLabel(engine->shortDescription());

        QVBoxLayout* buttonLayout = new QVBoxLayout;
        buttonLayout->addWidget(toolButton,0, Qt::AlignCenter);
        buttonLayout->addWidget(descriptionLabel, 0, Qt::AlignCenter);

        styleButtonHorizontalLayout->addLayout(buttonLayout);

        plotStylesByButton.insert(toolButton, engine);
        styleButtonGroup->addButton(toolButton);
    }

    connect(
        styleButtonGroup,
        static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
        this,
        &PlotStyleWidget::styleButtonClicked
    );
}


PlotStyleWidget::~PlotStyleWidget() {}


bool PlotStyleWidget::isValid() const {
    return plotStyle() != Q_NULLPTR;
}


const PlotEngine* PlotStyleWidget::plotStyle() const {
    const PlotEngine* foundStyle;
    unsigned          numberClicked = 0;
    for (  QMap<QToolButton*, const PlotEngine*>::const_iterator it  = plotStylesByButton.constBegin(),
                                                                 end = plotStylesByButton.constEnd()
         ; it != end
         ; ++it
        ) {
        QToolButton* button = it.key();
        if (button->isChecked()) {
            if (numberClicked == 0) {
                foundStyle = it.value();
            }

            ++numberClicked;
        }
    }

    return numberClicked == 1 ? foundStyle : Q_NULLPTR;
}


void PlotStyleWidget::setPlotStyle(const PlotEngine* newPlotEngine) {
    for (  QMap<QToolButton*, const PlotEngine*>::const_iterator it  = plotStylesByButton.constBegin(),
                                                                 end = plotStylesByButton.constEnd()
         ; it != end
         ; ++it
        ) {
        QToolButton* button = it.key();
        button->setChecked(it.value() == newPlotEngine);
    }
}


void PlotStyleWidget::setPlotStyles(const QSet<const PlotEngine*>& newPlotEngines) {
    styleButtonGroup->setExclusive(false);

    for (  QMap<QToolButton*, const PlotEngine*>::const_iterator it  = plotStylesByButton.constBegin(),
                                                                 end = plotStylesByButton.constEnd()
         ; it != end
         ; ++it
        ) {
        QToolButton* button = it.key();
        button->setChecked(newPlotEngines.contains(it.value()));
    }

    styleButtonGroup->setExclusive(true);
}


void PlotStyleWidget::styleButtonClicked(QAbstractButton* styleButton) {
    Q_ASSERT(plotStylesByButton.contains(dynamic_cast<QToolButton*>(styleButton)));

    const PlotEngine* plotEngine = plotStylesByButton.value(dynamic_cast<QToolButton*>(styleButton));
    emit plotStyleChanged(plotEngine);
    updateChangedStatus();
}
