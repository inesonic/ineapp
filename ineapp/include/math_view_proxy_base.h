/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MathViewProxyBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATH_VIEW_PROXY_BASE_H
#define MATH_VIEW_PROXY_BASE_H

#include <QObject>

#include <ld_element_structures.h>
#include <ld_format_structures.h>

#include "view_widget.h"
#include "insert_element_command.h"
#include "view_proxy.h"

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class InsertElementCommand;

/**
 * Base class for view proxies used to insert mathematical operators.
 */
class MathViewProxyBase:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        MathViewProxyBase(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~MathViewProxyBase() override;

    protected:
        /**
         * Method you can use to create a new character format instance.
         *
         * \return Returns a character format instance suitable for math functions.
         */
        Ld::FormatPointer mathCharacterFormat() const;

        /**
         * Method you can use to create a new character format instance.
         *
         * \return Returns a character format instance suitable for math functions.
         */
        Ld::FormatPointer mathIdentifierCharacterFormat() const;

        /**
         * Method you can use to copy a font format from one class to another.
         *
         * \param[in] outputFormat The format to receive the font format data.
         *
         * \param[in] inputFormat  The input format used as the source of the font data.
         */
        static void copyFontData(Ld::FormatPointer outputFormat, Ld::FormatPointer inputFormat);

        /**
         * Method you can use to create a new font format instance.
         *
         * \return Returns a font format instance suitable for math functions.
         */
        Ld::FormatPointer mathFunctionFontFormat() const;

        /**
         * Method you can use to create a new operator format instance.
         *
         * \return Returns a operator format instance suitable for math functions.
         */
        Ld::FormatPointer operatorFormat() const;

        /**
         * Method you can use to create a new multiplication operator format instance.
         *
         * \return Returns a multiplication operator format instance suitable for math functions.
         */
        Ld::FormatPointer multiplicationOperatorFormat() const;

        /**
         * Method you can use to create a new division operator format instance.
         *
         * \return Returns a division operator format instance suitable for math functions.
         */
        Ld::FormatPointer divisionOperatorFormat() const;

        /**
         * Method you can use to create a new parenthesis format instance.
         *
         * \return Returns a operator format instance suitable for math functions.
         */
        Ld::FormatPointer parenthesisOperatorFormat() const;

        /**
         * Method you can use to create a new function format instance.
         *
         * \return Returns a function format instance suitable for math functions.
         */
        Ld::FormatPointer functionFormat() const;

        /**
         * Method you can call to insert a new element.
         *
         * \param[in] newElement The new element to be inserted.
         */
        void insertElement(Ld::ElementPointer newElement);

        /**
         * Method you can call to insert a new element.  This version allows you to specify where the cursor should
         * be placed after inserting the element.
         *
         * \param[in] newElement          The new element to be inserted.
         *
         * \param[in] finalCursorPosition The desired final cursor position.
         */
        void insertElement(
            Ld::ElementPointer                        newElement,
            InsertElementCommand::FinalCursorPosition finalCursorPosition
        );

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();
};

#endif
