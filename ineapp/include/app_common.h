/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides macros shared across the entire library API.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef APP_COMMON_H
#define APP_COMMON_H

#include <QtGlobal>

/** \def APP_PUBLIC_API
 *
 * Macro used to define the interface to the container library.  Resolves to __declspec(dllexport) or
 * __declspec(dllimport) on Windows.
 */
#if (defined(INEAPP_BUILD))

    #define APP_PUBLIC_API Q_DECL_EXPORT

#else

    #define APP_PUBLIC_API Q_DECL_IMPORT

#endif

/** \def APP_PUBLIC_TEMPLATE_METHOD
 * *
 * Macro that circumvents limitations in the Windows DLL implementation for templated class methods.  The macro
 * expands to "__forceinline" on Windows and nothing on other platforms.
 *
 * Note that this solution is an ugly kludge to work around weaknesses in the Windows DLL architecture.
 */
#if (defined(Q_OS_WIN))

    #define APP_PUBLIC_TEMPLATE_METHOD __forceinline

#else

    #define APP_PUBLIC_TEMPLATE_METHOD

#endif

#endif
