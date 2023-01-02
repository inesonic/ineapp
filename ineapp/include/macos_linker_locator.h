/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MacOSLinkerLocator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MACOS_LINKER_LOCATOR_H
#define MACOS_LINKER_LOCATOR_H

#include <QObject>
#include <QString>
#include <QStringList>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element_cursor.h>

#include "app_common.h"

namespace Ld {
    class CursorStateCollection;
    class ElementWithGridChildren;
}

class QLabel;
class QPushButton;

/**
 * Class that locates the MacOS linker, installing the XCode command-line tools if needed.
 */
class APP_PUBLIC_API MacOSLinkerLocator:public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent widget.
         */
        MacOSLinkerLocator(QObject* parent = Q_NULLPTR);

        ~MacOSLinkerLocator() override;

        /**
         * Method you can use to locate the XCode linker.
         *
         * \return Returns true on success, returns false on failure.
         */
        bool locateLinker();

        /**
         * Method you can use to obtain the path to the XCode linker.
         *
         * \return Returns the path to the XCode linker.
         */
        QString linkerPath() const;

        /**
         * Method you can use to obtain the name of the XCode linker executable.
         *
         * \return Returns the name of the XCode linker executable.
         */
        QString linkerExecutable() const;

        /**
         * Method you can use to query the error status.
         *
         * \return Returns a string describing the error.
         */
        QString errorStatus() const;

    private:
        /**
         * Path to the xcode-select tool.
         */
        static const char xcodeSelectPath[];

        /**
         * Method used to determine if a copy of XCode is present.
         *
         * \return Returns true if a copy of the XCode tools is present.  Returns false if the XCode tools are not
         *         present.
         */
        static bool xcodeToolsAreInstalled();

        /**
         * Method that installs the XCode tools.
         */
        static void installXCodeTools();

        /**
         * The current error message.
         */
        QString currentErrorStatus;
};

#endif
