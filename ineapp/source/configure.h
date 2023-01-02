/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref configure function that configures the core language and presentation classes.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <cstdint>

class Registrar;

/**
 * Method that is called by the application instance to configure the language and presentation factories.
 *
 * \param[in] registrar The registrar used to register builders and element instances with the application.
 */
void configure(Registrar* registrar);

#endif
