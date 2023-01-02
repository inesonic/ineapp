/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file is the main entry point for the ineapp unit tests.
***********************************************************************************************************************/

#include <QDebug>


#include <metatypes.h>

#include "application_wrapper.h"

#include "test_presentation_with_fixed_children.h"
#include "test_presentation_with_positional_children.h"
#include "test_presentation_with_floating_children.h"
#include "test_placement_line_data.h"
#include "test_presentation_area.h"
#include "test_placement_line_start.h"
#include "test_placement_line_start_list.h"
#include "test_presentation_locator.h"
#include "test_root_child_location.h"
#include "test_root_presentation.h"
#include "test_command_container.h"

int main(int argumentCount, char** argumentValues) {
    ApplicationWrapper wrapper(argumentCount, argumentValues);
    registerMetaTypes();

    wrapper.includeTest(new TestPlacementLineData);
    wrapper.includeTest(new TestPresentationWithFixedChildren);
    wrapper.includeTest(new TestPresentationWithPositionalChildren);
    wrapper.includeTest(new TestPresentationWithFloatingChildren);
    wrapper.includeTest(new TestPresentationArea);
    wrapper.includeTest(new TestPlacementLineStart);
    wrapper.includeTest(new TestPlacementLineStartList);
    wrapper.includeTest(new TestPresentationLocator);
    wrapper.includeTest(new TestRootChildLocation);
    wrapper.includeTest(new TestRootPresentation);
    wrapper.includeTest(new TestCommandContainer);

    int status = wrapper.exec();

    return status;
}
