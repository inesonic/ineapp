/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BlockFormatBuilderEarly class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BLOCK_FORMAT_BUILDER_EARLY_H
#define BLOCK_FORMAT_BUILDER_EARLY_H

#include <QObject>
#include <QList>
#include <QColor>

#include <util_bit_set.h>

#include <eqt_builder.h>

class QHBoxLayout;
class QVBoxLayout;

namespace EQt {
    class ProgrammaticApplication;
    class ProgrammaticMainWindow;
    class ProgrammaticDockWidget;
    class ProgrammaticMainWindowProxy;
    class ProgrammaticViewProxy;
    class ProgrammaticView;
    class ToolButton;
}

/**
 * Builer for the application's block format user interface functions.  These functions include:
 *
 *     * Basic block formatting dialog menu functions.
 *     * Basic block formatting dock widget functions.
 *
 * This class specifically adds menu items near the top of the format menu.
 */
class BlockFormatBuilderEarly:public EQt::Builder {
    Q_OBJECT

    public:
        /**
         * The priority for this builder.
         */
        static constexpr unsigned builderPriority = 400;

        /**
         * The builder name.
         */
        static const char builderName[];

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit BlockFormatBuilderEarly(QObject* parent = Q_NULLPTR);

        ~BlockFormatBuilderEarly() override;

        /**
         * Method that returns a priority number used to order this builder with respect to other builders.
         * Builders with lower numbers are executed first.
         *
         * \return The builder priority number.
         */
        unsigned priority() const final;

        /**
         * Method that returns a short identifier for builder.
         *
         * \return Returns a string holding the builder name.
         */
        const char* builderIdentifier() const final;

        /**
         * Method that returns the name of the plug-in associated with this builder.
         *
         * \return Returns a string holding the plug-in name.
         */
        const char* plugInName() const final;

        /**
         * Method that is called to build a main window's GUI.  Note that this method will be called each time a
         * new main window needs to be created.
         *
         * This method should build the menus and toolbars, including support for custom tool-tips, icons, etc.
         *
         * \param[in] window The main window to build the GUI on.
         */
        void buildMainWindowGui(EQt::ProgrammaticMainWindow* window) final;
};

#endif
