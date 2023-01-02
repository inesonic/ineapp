/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref registerMetaTypes function.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QSet>
#include <QObject>
#include <QMetaType>
#include <QSharedPointer>

#include <model_api_types.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_format.h>
#include <ld_translation_phase.h>
#include <ld_cpp_translation_phase.h>
#include <ld_code_generator_output_type_container.h>
#include <ld_diagnostic.h>
#include <ld_variable_name.h>
#include <ld_page_format.h>
#include <ld_chart_line_style.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_plot_format.h>

#include <eqt_unique_application.h>

#include "metatypes.h"

/**
 * Function you can call during application start-up to register metatypes required by the application.
 */
void registerMetaTypes() {
    qRegisterMetaType<Ld::ElementPointer>();
    qRegisterMetaType<Ld::FormatPointer>();
    qRegisterMetaType<Ld::DiagnosticPointer>();
    qRegisterMetaType<QSharedPointer<Ld::RootElement>>();
    qRegisterMetaType<QSharedPointer<Ld::PageFormat>>();
    qRegisterMetaType<Ld::CodeGeneratorOutputTypeContainer>();
    qRegisterMetaType<Ld::VariableName>();
    qRegisterMetaType<Ld::PlotSeries::MarkerStyle>();
    qRegisterMetaType<Ld::PlotSeries::SplineType>();
    qRegisterMetaType<Ld::PlotSeries::GradientType>();
    qRegisterMetaType<Ld::PlotSeries>();
    qRegisterMetaType<Ld::ChartLineStyle::LineStyle>();
    qRegisterMetaType<Ld::ChartAxisFormat::AxisScale>();
    qRegisterMetaType<Ld::ChartAxisFormat::TickStyle>();
    qRegisterMetaType<Ld::PlotFormat::AxisLocation>();
    qRegisterMetaType<Ld::PlotFormat::LegendLocation>();
    qRegisterMetaType<Qt::AlignmentFlag>();
    qRegisterMetaType<Model::AbortReason>();
    qRegisterMetaType<Model::OperationHandle>();
    qRegisterMetaType<Model::IdentifierHandle>();
    qRegisterMetaType<Model::Device>();
}
