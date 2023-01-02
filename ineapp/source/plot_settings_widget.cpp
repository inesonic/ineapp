/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotSettingsWidget class.
***********************************************************************************************************************/

#include <ld_chart_line_style.h>
#include <ld_plot_series.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>
#include <ld_font_format.h>
#include <ld_variable_name.h>

#include <limits>

#include "application.h"
#include "plot_engine.h"
#include "plot_settings_widget.h"

const double PlotSettingsWidget::automaticScaling = std::numeric_limits<double>::infinity();

PlotSettingsWidget::PlotSettingsWidget() {}


PlotSettingsWidget::~PlotSettingsWidget() {}
