/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref registerMetaTypes function as well as other features used to register and manage various
* application specific metatypes.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef METATYPES_H
#define METATYPES_H

#include <QMetaType>
#include <QSharedPointer>

#include <util_string.h>

#include <model_api_types.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_root_import.h>
#include <ld_format.h>
#include <ld_code_generator_output_type_container.h>
#include <ld_diagnostic.h>
#include <ld_page_format.h>
#include <ld_variable_name.h>
#include <ld_plot_format.h>
#include <ld_chart_line_style.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>

#include "app_common.h"

/**
 * Function you can call during application start-up to register metatypes required by the application.
 */
void APP_PUBLIC_API registerMetaTypes();

/*
 * Declare the various metatypes below.
 */

Q_DECLARE_METATYPE(Util::IntegerNumberStyle)
Q_DECLARE_METATYPE(Util::RealNumberStyle)
Q_DECLARE_METATYPE(Ld::ElementPointer)
Q_DECLARE_METATYPE(Ld::FormatPointer)
Q_DECLARE_METATYPE(Ld::DiagnosticPointer)
Q_DECLARE_METATYPE(QSharedPointer<Ld::RootElement>)
Q_DECLARE_METATYPE(Ld::RootImport)
Q_DECLARE_METATYPE(QSharedPointer<Ld::PageFormat>)
Q_DECLARE_METATYPE(Ld::CodeGeneratorOutputTypeContainer)
Q_DECLARE_METATYPE(Ld::VariableName)
Q_DECLARE_METATYPE(Ld::PlotSeries::MarkerStyle)
Q_DECLARE_METATYPE(Ld::PlotSeries::SplineType)
Q_DECLARE_METATYPE(Ld::PlotSeries::GradientType)
Q_DECLARE_METATYPE(Ld::PlotSeries)
Q_DECLARE_METATYPE(Ld::ChartLineStyle::LineStyle)
Q_DECLARE_METATYPE(Ld::ChartAxisFormat::AxisScale)
Q_DECLARE_METATYPE(Ld::ChartAxisFormat::TickStyle)
Q_DECLARE_METATYPE(Ld::PlotFormat::AxisLocation)
Q_DECLARE_METATYPE(Ld::PlotFormat::LegendLocation)
Q_DECLARE_METATYPE(Qt::AlignmentFlag)
Q_DECLARE_METATYPE(Model::AbortReason)
Q_DECLARE_METATYPE(Model::OperationHandle)
Q_DECLARE_METATYPE(Model::IdentifierHandle)
Q_DECLARE_METATYPE(Model::Device)

#endif
